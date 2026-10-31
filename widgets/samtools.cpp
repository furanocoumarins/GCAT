#include "samtools.h"

bool smt::SamToBam(const std::string& Sam, const std::string& Bam, int threads){
    std::filesystem::path path = Utils::getHomeDirectory() / ".gcat";

    Utils::create_folder(path/"bams");

    std::string fullOutputPath = (path/"bams"/Bam).string();

    std::string command = "samtools view -b -@ " + std::to_string(threads) + " -o " + fullOutputPath + " " + Sam;

    int result = std::system(command.c_str());

    std::filesystem::current_path(path/"bams");

    return (result == 0);
}

bool smt::checkType(const std::string& gff_type, const std::vector<Ui::placeTYPE>& flags) {
    std::string type = gff_type;
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);

    for (auto flag : flags) {
        if (flag == Ui::GEN && (type == "gene" || type == "pseudogene")) return true;
        if (flag == Ui::EXON && type == "exon") return true;
        if (flag == Ui::CDS && type == "cds") return true;
        if (flag == Ui::OTHER && type != "gene" && type != "pseudogene" && type != "exon" && type != "cds") return true;
    }
    return false;
}

int smt::VcfChrom2i(const std::string& vcf_chrom) {
    if (vcf_chrom == "CM000663.2" || vcf_chrom == "NC_000001.11") return 1;
    if (vcf_chrom == "CM000664.2" || vcf_chrom == "NC_000002.12") return 2;
    if (vcf_chrom == "CM000665.2" || vcf_chrom == "NC_000003.12") return 3;
    if (vcf_chrom == "CM000666.2" || vcf_chrom == "NC_000004.12") return 4;
    if (vcf_chrom == "CM000667.2" || vcf_chrom == "NC_000005.12") return 5;
    if (vcf_chrom == "CM000668.2" || vcf_chrom == "NC_000006.12") return 6;
    if (vcf_chrom == "CM000669.2" || vcf_chrom == "NC_000007.12") return 7;
    if (vcf_chrom == "CM000670.2" || vcf_chrom == "NC_000008.12") return 8;
    if (vcf_chrom == "CM000671.2" || vcf_chrom == "NC_000009.12") return 9;
    if (vcf_chrom == "CM000672.2" || vcf_chrom == "NC_000010.11") return 10;
    if (vcf_chrom == "CM000673.2" || vcf_chrom == "NC_000011.10") return 11;
    if (vcf_chrom == "CM000674.2" || vcf_chrom == "NC_000012.12") return 12;
    if (vcf_chrom == "CM000675.2" || vcf_chrom == "NC_000013.11") return 13;
    if (vcf_chrom == "CM000676.2" || vcf_chrom == "NC_000014.9")  return 14;
    if (vcf_chrom == "CM000677.2" || vcf_chrom == "NC_000015.10") return 15;
    if (vcf_chrom == "CM000678.2" || vcf_chrom == "NC_000016.10") return 16;
    if (vcf_chrom == "CM000679.2" || vcf_chrom == "NC_000017.11") return 17;
    if (vcf_chrom == "CM000680.2" || vcf_chrom == "NC_000018.10") return 18;
    if (vcf_chrom == "CM000681.2" || vcf_chrom == "NC_000019.10") return 19;
    if (vcf_chrom == "CM000682.2" || vcf_chrom == "NC_000020.11") return 20;
    if (vcf_chrom == "CM000683.2" || vcf_chrom == "NC_000021.9")  return 21;
    if (vcf_chrom == "CM000684.2" || vcf_chrom == "NC_000022.11") return 22;
    if (vcf_chrom == "CM000685.2" || vcf_chrom == "NC_000023.11") return 23;
    if (vcf_chrom == "CM000686.2" || vcf_chrom == "NC_000024.10") return 24;
    return -1;
}


std::string smt::FilterVcf(int targetChr,int startPOS,int endPOS,const std::vector<Ui::placeTYPE>& ptypeFLAGS,const std::string& vcfInputPath,const std::string& vcfOutputname)
{
    // gff procces
    std::vector<MaskInterval> mask;

    std::ifstream gffFile(Utils::get_gff_path(targetChr));
    if (!gffFile.is_open()) {
        throw Utils::get_gff_path(targetChr);
    }

    std::string line;
    while (std::getline(gffFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string chrom, source, type;
        int start, end;

        if (!(ss >> chrom >> source >> type >> start >> end)) continue;

        if (smt::VcfChrom2i(chrom) != targetChr) continue;

        if (!smt::checkType(type, ptypeFLAGS)) continue;

        int gffStart = start - 1;
        int gffEnd = end;

        int intersectStart = std::max(gffStart, startPOS);
        int intersectEnd = std::min(gffEnd, endPOS);

        if (intersectStart < intersectEnd) {
            mask.push_back({intersectStart, intersectEnd});
        }
    }
    gffFile.close();

    std::sort(mask.begin(), mask.end(), [](const MaskInterval& a, const MaskInterval& b) {
        return a.start < b.start;
    });

    // vcf procces
    htsFile* inf = hts_open(vcfInputPath.c_str(), "r");
    if (!inf) {
        throw "vcf open error";
    }

    bcf_hdr_t* hdr = bcf_hdr_read(inf);
    if (!hdr) {
        hts_close(inf);
        throw "header vcf open error";
    }

    std::filesystem::current_path(Utils::getHomeDirectory() / ".gcat");
    Utils::create_folder("results");
    std::filesystem::current_path(Utils::getHomeDirectory() / ".gcat"/ "results");

    time_t tmt = time(nullptr);
    struct tm* tm = localtime(&tmt);
    char tbuf[80];
    strftime(tbuf, sizeof(tbuf), " - %H_%M %Y-%m-%d", tm);
    std::string newName = vcfOutputname + tbuf + ".vcf";

    htsFile* outf = hts_open(newName.c_str(), "w");
    if (!outf) {
        bcf_hdr_destroy(hdr);
        hts_close(inf);
        throw "output open error";
    }

    if (bcf_hdr_write(outf, hdr) < 0) {
        bcf_hdr_destroy(hdr);
        hts_close(inf);
        hts_close(outf);
        throw "vcfunc open error";
    }

    bcf1_t* rec = bcf_init();

    // filtr
    while (bcf_read(inf, hdr, rec) == 0) {
        std::string vcfChromStr = bcf_hdr_id2name(hdr, rec->rid);
        int vcfChromNum = smt::VcfChrom2i(vcfChromStr);

        if (vcfChromNum == targetChr) {
            int vcfPos = rec->pos;

            bool inMask = false;
            for (const auto& interval : mask) {
                if (vcfPos >= interval.start && vcfPos < interval.end) {
                    inMask = true;
                    break;
                }
                if (interval.start > vcfPos) break;
            }
            if (inMask) {
                bcf_write(outf, hdr, rec);
            }
        }
    }
    bcf_destroy(rec);
    bcf_hdr_destroy(hdr);
    hts_close(inf);
    hts_close(outf);

    return newName;
}

