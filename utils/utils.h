#ifndef UTILS_H
#define UTILS_H

#include <curl/curl.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>


namespace utils {
    size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

    bool download_ncbi_gff(const std::string& target, const std::string& output) {
        CURL* curl;
        FILE* fp;
        CURLcode res;


        std::string url = "https://api.ncbi.nlm.nih.gov/datasets/v2/genome/accession/GCF_000001405.40/download?include_annotation_type=GENOME_GFF&chromosomes=" + target;


        curl = curl_easy_init();
        if (!curl) {
            return false;
        }

        fp = fopen(output.c_str(), "wb");
        if (!fp) {
            curl_easy_cleanup(curl);
            return false;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);

        res = curl_easy_perform(curl);

        fclose(fp);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::remove(output.c_str());
            return false;
        }
        return true;
    }


    bool uzip(const std::string& path, const std::string& dir) {
        std::string command = "unzip -o " + path + " -d " + dir;

        int result = std::system(command.c_str());

        return (result == 0);
    }

}

#endif // UTILS_H
