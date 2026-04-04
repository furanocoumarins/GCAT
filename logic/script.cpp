#include <iostream>
#include "api.hpp"


using namespace std;


int main(int argc, char** argv){
    catAPI *api = new catAPI("CM000665.2");

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
        if(!fits_the_region(argv[3], regionChr, regionStart, regionEnd)){  //убрать в api->analyse(), либо самому проверять
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
    api->skipLines(vcf, vline);
    api->skipLines(gff, gline);

// тут нужно  все перелопатить

    return 0;
}
