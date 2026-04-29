#ifndef UTILS_H
#define UTILS_H
#include <string>

#include "cpr/include/cpr/cpr.h"
#include <lexbor/html/parser.h>
#include <lexbor/dom/interfaces/element.h>

namespace utils {
    std::string get_gff3_file(std::string CHROM); // CM000665.2

    std::string get_Assembly_link(const std::string& html_content);
}

#endif // UTILS_H
