#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string_view>
#include <ranges>

#include "cpr/include/cpr/cpr.h"
#include <lexbor/html/parser.h>
#include <lexbor/dom/interfaces/element.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>


namespace utils {
    std::string get_gff3_file(std::string CHROM); // CM000665.2

    std::string get_Assembly_link(const std::string& html_content);

    void download_genome_gff(const QString &url);

    void clear_dir(const QDir &dir, QString fileName);
}

#endif // UTILS_H
