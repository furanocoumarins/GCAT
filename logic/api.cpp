#include "api.hpp"

using namespace std;

catAPI::catAPI(){
    this->target_name = "";
    this->types.insert("all");
}

catAPI::catAPI(string target){
    this->target_name = target;
    this->types.insert("all");
}

inline bool catAPI::overlaps(long start1, long end1, long start2, long end2){
    return end1 >= start2 || end2 >= start1;
}

bool catAPI::fits_the_type(unordered_set<string> &types, const string &type){
    return types.count(type) > 0 || types.count("all") > 0 ? true : false;
}

bool catAPI::fits_the_region(const string &reg, string &chr, long &rstart, long &rend){

    auto p = reg.find(':');
    if(p==string::npos) return false;

    chr = reg.substr(0,p);
    auto q = reg.find('-', p+1);
    if(q==string::npos) return false;


    try {
        rstart = stol(reg.substr(p+1, q-(p+1)));
        rend   = stol(reg.substr(q+1));
    }catch(...) { return false; }

    if(rstart > rend) swap(rstart, rend);

    return true;
}

void catAPI::skipLines(ifstream &file, string &line){
    while(true){
        streampos pos = file.tellg();
        if(!getline(file, line)) break;
        if(!line.empty() && line[0]=='#') continue;
        file.seekg(pos); break;
    }
}

unordered_set<string> catAPI::parseTypes(const string &s){
    unordered_set<string> types;
    string cur;
    stringstream ss(s);

    if(s.find(',') != string::npos)
        while(getline(ss, cur, ','))
            if(!cur.empty()) types.insert(cur);
            else;
    else types.insert(s);

    return types;
}

bool catAPI::readNext_gff(string &line){
    while(getline(this->gff_stream, line)){
        if(line.empty()) continue;
        if(line[0]=='#') continue;
        return true;
    }
    return false;
}

void catAPI::prepare(string target_name, string chr, long S, long E){
    this->target_name = target_name;
    if (chr != "CM000665.2") throw "for tests. needs to be replaced later";
    this->target_chr = chr;
    this->Sregion = S; //todo: провекри добавить
    this->Eregion = E;
}

void catAPI::analyse()
{
    deque<Candidate> candidates;
    bool gffEOF = false;

    string gline;
    string vline;

    string vchr;
    long vpos;

    string gchr;

    string type;

    long gstart;
    long gend;



    if(!readNext_gff(gline)) gffEOF = true;

    while(getline(this->vcf_stream, vline)){
        auto vcols = std::ranges::views::split(vline, '\t');

        vchr = vcols[0];
        vpos = 0;

        try{
            vpos = stol(vcols[1]);
        }catch(...) { continue; }

        if(vchr != target_chr) continue;
        if(vpos < Sregion || vpos > Eregion) continue;


        while(!gffEOF){
            auto gcols = std::views::split(gline, '\t');
            if(gcols.size() < 5){
                if(!readNext_gff(gline)){ gffEOF = true; break; }
                continue;
            }
            gchr = gcols[0];
            type = gcols[2];
            gstart = 0;
            gend = 0;
            try{ gstart = stol(gcols[3]); gend = stol(gcols[4]); } catch(...){
                if(!readNext_gff(gline)){ gffEOF = true; break; }
                continue;
            }

            // This part about work with chromosome
            // should be done later

            //if(gchr < vchr){ if(!readNext_gff(gline)){ gffEOF = true; break; } continue; }
            //if(gchr > vchr) break;

            if(gstart > vpos) break; // vpos + vLength

            if(fits_the_type(types, type)){
                Candidate f;
                f.seqid = gchr; f.type = type; f.start = gstart;
                f.end = gend; f.raw = gline;
                candidates.push_back(std::move(f));
            }
            if(!readNext_gff(gline)){ gffEOF = true; break; }
        }

        while(!candidates.empty() && candidates.front().end < vpos) candidates.pop_front();
    }
}








