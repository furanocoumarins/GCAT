#ifndef apihpp
#define apihpp

#include <fstream>
#include <sstream>
#include <string>
#include <ranges>
#include <string_view>

#include <unordered_set>

using namespace std;

class catAPI{
public:
    void skipLines(ifstream &file, string &line);

private:
    inline bool overlaps(long start1, long end1, long start2, long end2);

    bool fits_the_type(unordered_set<string> &types, const string &type);

    bool fits_the_region(const string &reg, string &chr, long &rstart, long &rend);

    unordered_set<string> parseTypes(const string &s);
};

class Candidate{
    long start = 0;
    long end   = 0;
    std::string seqid;
    std::string type;
    std::string raw;
};
#endif // apihpp
