#ifndef apihpp
#define apihpp

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>

#include <unordered_set>
#include <deque>

using namespace std;

class catAPI{
public:
    bool useRegion = false;

    catAPI();
    catAPI(string target_name);
    catAPI(ifstream &vcf, ifstream &gff);


    unordered_set<string> types;
    void skipLines(ifstream &file, string &line);
    void parseTypes(const string &s);
    bool parse_region(const string &reg);
    void analyse();
    void prepare(string name, string chr, long start, long end);
    ifstream* gff_stream;
    ifstream* vcf_stream;

private:
    inline bool overlaps(long start1, long end1, long start2, long end2);
    bool fits_the_type(const string &type);
    bool readNext_gff(string &line);

    string target_name;
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
