#ifndef UTILS_H
#define UTILS_H

#include <curl/curl.h>
#include <sys/stat.h>
#include <filesystem>
#include <vector>
#include <sstream>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>


namespace utils {
    size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream);

    bool download_ncbi_genome(const std::string& output);

    bool unzip();

    inline bool exists_file(const std::string& name);

    std::filesystem::path getHomeDirectory();
    bool create_main_folder();
    bool create_folder(const std::string& name);
    //extern std::string SBORKA;
}

#endif // UTILS_H
