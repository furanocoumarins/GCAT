#include <iostream>
#include "api.hpp"


using namespace std;


int main(int argc, char** argv){
    if(argc < 3){
        cerr << "Usage: " << argv[0] << " file1.vcf file2.sff3 [-r REGION] [-t TYPES]\n";
        return 1;
    }

    string vcfPath = argv[1];
    string gffPath = argv[2];

    string region_arg;
    string types_arg = "all";

    int idx = 3;

    ifstream vcf(vcfPath);
    if(!vcf){ cerr << "\033[31mCannot open \033[0m" << vcfPath << "\n"; return 1; }
    ifstream gff(gffPath);
    if(!gff){ cerr << "\033[31mCannot open \033[0m" << gffPath << "\n"; return 1; }

    while(idx < argc){
        string a = argv[idx];
        if(a == "-t" || a == "--types"){
            if(idx+1 >= argc){ cerr<<"-t requires an argument\n"; return 1; }
            types_arg = argv[idx+1];
            idx += 2;
        } else if (a == "-r" || a == "--region"){
            if(idx+1 >= argc){ cerr<<"-r requires an argument\n"; return 1; }
            region_arg = argv[idx+1];
            idx += 2;
        } else if(a.size() > 0 && a[0] == '-'){
            cerr << "\033[31mUnknown flag: \033[0m" << a << "\n"; return 1;
        }
    }

    catAPI *api = new catAPI(vcf, gff);

    if(region_arg.size() > 0){
        if(!api->parse_region(region_arg)){
            cerr << "\033[31mBad region format.\033[0m\n"
            "\tfor all chrromoseme use -r CHR:\n"
            "\tfor certain region use -r CHR:START-END\n"; return 1;
        }
        api->useRegion = true;
    }

    api->parseTypes(types_arg);

    if(api->types.empty()){
        cerr << "\033[31mNo type was specified\033[0m\n"; return 1;
    }

    api->analyse();
    return 0;
}
