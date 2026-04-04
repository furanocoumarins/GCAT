#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <deque>

#include <ranges>
#include <string_view>

using namespace std;


int main(int argc, char** argv){
    if(argc < 3){
        cerr << "Usage: " << argv[0] << " file1.vcf file2.sff3 [REGION] [-t TYPES]\n";
        return 1;
    }

    string vcfPath = argv[1];
    string gffPath = argv[2];

    string region_arg;
    string types_arg = "all,";

    int idx = 3;
    while(idx < argc){
        string a = argv[idx];
        if(a == "-t" || a == "--types"){
            if(idx+1 >= argc){ cerr<<"-t requires an argument\n"; return 1; }
            types_arg = argv[idx+1];
            idx += 2;
        } else if(a.size() > 0 && a[0] == '-'){
            cerr << "Unknown flag: " << a << "\n"; return 1;
        } else {
            if(region_arg.empty() && a.find(':')!=string::npos && a.find('-')!=string::npos){
                region_arg = a;
            }
            idx++;
        }
    }

    bool useRegion = false;
    string regionChr; long regionStart, regionEnd;
    if(argc >= 4){
        if(!parseRegion(argv[3], regionChr, regionStart, regionEnd)){
            cerr << "Bad region format. Use CHR:START-END\n"; return 1;
        }
        useRegion = true;
    }

    auto wantedTypes = std::views::split(types_arg, ',');
    if(wantedTypes.empty()){
        cerr << "No type was specified\n"; return 1;
    }


    ifstream vcf(vcfPath);
    if(!vcf){ cerr << "Cannot open " << vcfPath << "\n"; return 1; }
    ifstream gff(gffPath);
    if(!gff){ cerr << "Cannot open " << gffPath << "\n"; return 1; }

    string vline, gline;
    // skip #-lines
    skipLines(vcf, vline);
    skipLines(gff, gline);


    deque<Feature> window;
    bool gffEOF = false;
    auto readNext_gff = [&](string &line)->bool{
        while(getline(gff, line)){
            if(line.empty()) continue;
            if(line[0]=='#') continue;
            return true;
        }
        return false;
    };
    if(!readNext_gff(gline)) gffEOF = true;

    while(getline(vcf, vline)){
        auto vcols = std::views::split(vline, '\t');
        string vchr = vcols[0];
        long vpos = 0;
        try { vpos = stol(vcols[1]); } catch(...) { continue; }

        if(useRegion){
            if(vchr != regionChr) continue;
            if(vpos < regionStart || vpos > regionEnd) continue;
        }


        while(!gffEOF){
            auto gcols = std::views::split(gline, '\t');
            if(gcols.size() < 5){
                if(!readNext_gff(gline)){ gffEOF = true; break; }
                continue;
            }
            string gchr = gcols[0];
            string type = gcols[2];
            long gstart=0, gend=0;
            try{ gstart = stol(gcols[3]); gend = stol(gcols[4]); } catch(...){
                if(!readNext_gff(gline)){ gffEOF = true; break; }
                continue;
            }

            // This part about work with chromosome 
            // should be done later

            //if(gchr < vchr){ if(!readNext_gff(gline)){ gffEOF = true; break; } continue; }
            //if(gchr > vchr) break;

            if (vchr != "CM000665.2") break;  //for tests. needs to be replaced later

            if(gstart > vpos) break; // vpos + vLength

            if(gff_is_interest(wantedTypes, type)){
                Feature f;
                f.seqid = gchr; f.type = type; f.start = gstart;
                f.end = gend; f.raw = gline;
                window.push_back(std::move(f));
            }
            if(!readNext_gff(gline)){ gffEOF = true; break; }
        }

        while(!window.empty() && window.front().end < vpos) window.pop_front();

        for(const auto &f : window){
            if(f.seqid != "NC_000003.12" || vchr != "CM000665.2") continue; // (regionChr) now works only for this names of chromosom.
            if(overlaps(f.start, f.end, vpos, vpos)) cout << vline <<  "\n"; // vpos + vLength. ()
        }
    }
    return 0;
}
