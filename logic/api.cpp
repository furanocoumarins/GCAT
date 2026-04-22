#include "api.hpp"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

catAPI::catAPI(){
    this->target_name = "";
}

catAPI::catAPI(ifstream &vcf, ifstream &gff3){
    this->target_name = "";

    this->vcf_stream = &vcf;
    this->gff_stream = &gff3;
}

inline bool catAPI::overlaps(long start1, long end1, long start2, long end2){
    return (end1 >= start2 && end2 >= start1);
}

bool catAPI::fits_the_type(const string &type){
    return this->types.count(type) > 0 || this->types.count("all") > 0 ? true : false;
}

bool catAPI::parse_region(const string &reg){
    auto p = reg.find(':');
    if(p==string::npos) return false;

    this->target_chr = reg.substr(0,p);
    auto q = reg.find('-', p+1);
    if(q==string::npos) return false;

    try {
        this->Sregion = stol(reg.substr(p+1, q-(p+1)));
        this->Eregion = stol(reg.substr(q+1));
    }catch(...) { return false; }

    if(this->Sregion > this->Eregion) swap(this->Sregion, this->Eregion);
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

void catAPI::parseTypes(const string &s){
    string cur;
    stringstream ss(s);
    if(s.find(',') != string::npos)
        while(getline(ss, cur, ','))
            if(!cur.empty()) this->types.insert(cur);
            else;
    else this->types.insert(s);
}

bool catAPI::readNext_gff(string &line){
    while(getline(*this->gff_stream, line)){
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
    this->skipLines(*this->vcf_stream, vline);
    this->skipLines(*this->gff_stream, gline);



    while(getline(*this->vcf_stream, vline)){
      vector<std::string> vcols;
      auto parts_view = std::views::split(vline, '\t');
      for (auto &&rng : parts_view) vcols.emplace_back(rng.begin(), rng.end());

      vchr = vcols[0];
      vpos = 0;

      try{
          vpos = stol(vcols[1]);
      }catch(...) { continue; }


      if (this->useRegion){
        if (vchr != this->target_chr) continue;
        if (vpos < this->Sregion || vpos > this->Eregion) continue;
      }

      while(!gffEOF){
        vector<std::string> gcols;
        auto parts_view = std::views::split(gline, '\t');
        for (auto &&rng : parts_view) gcols.emplace_back(rng.begin(), rng.end());

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


        if (gstart > vpos) break;

          // This part about work with chromosome
          // should be done later

          //if(gchr < vchr){ if(!readNext_gff(gline)){ gffEOF = true; break; } continue; }
          //if(gchr > vchr) break;

        if(fits_the_type(type)){
          Candidate f;
          f.seqid = gchr; f.type = type; f.start = gstart;
          f.end = gend; f.raw = gline;
          candidates.push_back(std::move(f));
        }
        if(!readNext_gff(gline)){ gffEOF = true; break; }
      }

      while(!candidates.empty() && candidates.front().end < vpos) candidates.pop_front();

      for(const auto &f : candidates){
        if(f.seqid != "NC_000003.12" || vchr != "CM000665.2") continue; // (regionChr) now works only for this names of chromosom.
        if(this->overlaps(f.start, f.end, vpos, vpos)) cout << vline <<  "\n"; break; // vpos + vLength. ()
      }
    }
}
