#include "api.hpp"
#include <vector>
#include <string>
#include <iostream>

#define INF ((unsigned) ~0)

using namespace std;

catAPI::catAPI(){
    this->target_name = "";
}

catAPI::catAPI(ifstream &vcf, ifstream &gff3){
    this->target_name = "";

    this->vcf_stream = &vcf;
    this->gff_stream = &gff3;
}

int catAPI::get_gff_chr(const string &str){
  return stoi(str.substr(7,2));
}

int catAPI::get_vcf_chr(const string &str){
  return  stoi(str.substr(6,2)) - 62 ;
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

    this->target_chr = stoi(reg.substr(0,p));
    if(p  == reg.size()-1) return true;

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
        return true;
    }
    return false;
}

void catAPI::analyse()
{
    deque<Candidate> candidates;
    bool gffEOF = false;

    string gline;
    string vline;

    int vchr;
    long vpos;

    int gchr;

    string type;

    long gstart;
    long gend;


    if(!readNext_gff(gline)) gffEOF = true;
    this->skipLines(*this->vcf_stream, vline);
    this->skipLines(*this->gff_stream, gline);


    if (this->useRegion && !this->Sregion && !this->Eregion){
      this->Sregion = 1; this->Eregion = INF;
    }

    while(getline(*this->vcf_stream, vline)){
      vector<std::string> vcols;
      auto parts_view = std::views::split(vline, '\t');
      for (auto &&rng : parts_view) vcols.emplace_back(rng.begin(), rng.end());

      vchr = get_vcf_chr(vcols[0]);
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

        gchr = get_gff_chr(gcols[0]);
        type = gcols[2];
        gstart = 0;
        gend = 0;

        try{ gstart = stol(gcols[3]); gend = stol(gcols[4]); } catch(...){
          if(!readNext_gff(gline)){ gffEOF = true; break; }
          continue;
        }

        if (gstart > vpos) break;
        if(gchr < vchr){ if(!readNext_gff(gline)){ gffEOF = true; break; } continue; }
        if(gchr > vchr) break;

        if(fits_the_type(type)){
          Candidate f;
          f.seqid = gcols[0]; f.type = type; f.start = gstart;
          f.end = gend; f.raw = gline;
          candidates.push_back(std::move(f));
        }
        if(!readNext_gff(gline)){ gffEOF = true; break; }
      }

      while(!candidates.empty() && candidates.front().end < vpos) candidates.pop_front();


      for(const auto &f : candidates){
        if (get_gff_chr(f.seqid) != vchr) continue;
        if(this->overlaps(f.start, f.end, vpos, vpos)) cout << "[" << f.type << "] (" << "chr " << vchr << ") "<< vline <<  "\n"; break; // vpos + vLength.
      }
    }
}
