#ifndef SAMTOOLS_H
#define SAMTOOLS_H

#include <string>
#include <filesystem>
#include <vector>
#include "../windows/MAIN/mainwindow.h"
#include "../utils/utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

extern "C" {
#include <htslib/hts.h>
#include <htslib/vcf.h>
}



namespace smt{
    struct MaskInterval {
        int start;
        int end;
    };

    bool SamToBam(const std::string& Sam, const std::string& Bam, int threads = 4);

    bool checkType(const std::string& gff_type, const std::vector<Ui::placeTYPE>& flags);

    int VcfChrom2i(const std::string& vcf_chrom);

    bool FilterVcf(int targetChr,int startPOS,int endPOS,const std::vector<Ui::placeTYPE>& ptypeFLAGS,const std::string& vcfPath,const std::string& OutputPath);
}
#endif // SAMTOOLS_H
