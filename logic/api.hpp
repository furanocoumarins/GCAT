#ifndef apihpp
#define apihpp

#include <fstream>
#include <sstream>
#include <string>
#include <ranges>
#include <string_view>

#include <unordered_set>
#include <deque>

using namespace std;

class catAPI{
public:
    catAPI();
    catAPI(string target_name);

    void skipLines(ifstream &file, string &line);

private:
    inline bool overlaps(long start1, long end1, long start2, long end2);

    bool fits_the_type(unordered_set<string> &types, const string &type);

    bool fits_the_region(const string &reg, string &chr, long &rstart, long &rend);

    unordered_set<string> parseTypes(const string &s);

    bool readNext_gff(string &line);

    void prepare(string name, string chr, long start, long end);
    void analyse();

    unordered_set<string> types;

    string target_name;
    ifstream gff_stream;
    ifstream vcf_stream;

    string target_chr;
    long Sregion;
    long Eregion;
};

class Candidate{
public:
    long start = 0;
    long end   = 0;
    std::string seqid;
    std::string type;
    std::string raw;
};
#endif // apihpp
