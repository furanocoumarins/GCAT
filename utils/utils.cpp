#include "utils.h"

//std::string utils::SBORKA = "GCF_000001405.40";

inline bool utils::exists_file (const std::string& name) { //Fastest way to check if a file exists using standard C++/C++11,14,17/C?
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

bool utils::download_ncbi_genome(const std::string& output) {
    utils::create_main_folder();
    std::filesystem::current_path(utils::getHomeDirectory()/".gcat");

    CURL* curl;
    FILE* fp;
    CURLcode res;

    if (utils::exists_file(output)) return true;

    std::string url = std::string("https://ftp.ncbi.nlm.nih.gov/genomes/refseq/vertebrate_mammalian/Homo_sapiens/all_assembly_versions/GCF_000001405.40_GRCh38.p14/GCF_000001405.40_GRCh38.p14_genomic.gff.gz");
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

size_t utils::write_data(void* ptr, size_t size, size_t nmemb, FILE* stream){
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool utils::create_main_folder(){
    std::filesystem::path homedir = utils::getHomeDirectory();
    if (homedir.empty()) {
        return false;
    }

    std::filesystem::path folder = homedir / ".gcat";

    utils::create_folder(folder);

    return 0;
}

bool utils::create_folder(const std::string& name){
    try {
        if (std::filesystem::create_directory(name)) {
            return true;
        } else {
            return false;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        return false;
    }
}

std::filesystem::path utils::getHomeDirectory() {
    #if defined(_WIN32)
        const char* home = std::getenv("USERPROFILE");
    #else
        const char* home = std::getenv("HOME");
    #endif
        if (!home) {
        return "";
    }
    return std::filesystem::path(home);
}

bool utils::unzip() {
    std::filesystem::current_path(utils::getHomeDirectory()/".gcat");
    utils::create_folder("chrs");
    std::filesystem::current_path(utils::getHomeDirectory()/".gcat"/"chrs");

    std::vector<std::string> chrs;

    for (int i = 1; i <= 24; ++i) {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << i;
        chrs.push_back(ss.str());
    } // 23=X 24=Y
    // NC_012920.1 митохондрия (не добавляется)

    for (const std::string& chr : chrs) {
        if (!exists_file ("chromosome_" + chr + ".gff"))
        {
            std::string command = "gzip -dc ../gcf.gff.gz | grep -w \"NC_0000" + chr + ".*\" > chromosome_" + chr + ".gff";
            int result = std::system(command.c_str());

            if (result != 0)
                return false;
        }

    }
    return true;
}