#include <iostream>
#include "api.hpp"


using namespace std;


int main(int argc, char** argv){
    if(argc < 3){
        cerr << "Usage: " << argv[0] << " file1.vcf file2.sff3 [REGION] [-t TYPES]\n";
        return 1;
    }

    string vcfPath = argv[1];
    string gffPath = argv[2];

    string region_arg;
    string types_arg = "all";

    int idx = 3;

    ifstream vcf(vcfPath);
    if(!vcf){ cerr << "Cannot open " << vcfPath << "\n"; return 1; }
    ifstream gff(gffPath);
    if(!gff){ cerr << "Cannot open " << gffPath << "\n"; return 1; }

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

    catAPI *api = new catAPI(vcf, gff);

    if(region_arg.size() > 0){
        if(!api->parse_region(region_arg)){
            cerr << "Bad region format. Use CHR:START-END\n"; return 1;
        }
        api->useRegion = true;
    }

    api->parseTypes(types_arg);

    if(api->types.empty()){
        cerr << "No type was specified\n"; return 1;
    }

    // тут нужно  все перелопатить

    api->analyse();

    return 0;
}
