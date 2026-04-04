#include "api.hpp"

using namespace std;

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










