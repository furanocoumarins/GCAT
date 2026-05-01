#include "utils.h"

using namespace std;

string utils::get_gff3_file(string CHROM){
    cpr::Response r = cpr::Get(cpr::Url{"https://www.ncbi.nlm.nih.gov/nuccore/"+CHROM});
    string re = utils::get_Assembly_link(r.text);

    download_genome_gff(QString().fromStdString(re));
    return re;
}

string utils::get_Assembly_link(const std::string& content)
{

    lxb_html_parser_t *parser = lxb_html_parser_create();
    lxb_status_t status = lxb_html_parser_init(parser);
    if (status != LXB_STATUS_OK) return "";

    lxb_html_document_t *document = lxb_html_document_create();

    status = lxb_html_document_parse(
         document,
         (const lxb_char_t *)content.c_str(),
         content.length()
         );

    lxb_dom_collection_t *collection = lxb_dom_collection_make(lxb_dom_interface_document(document), 128);
    lxb_dom_elements_by_tag_name(
         lxb_dom_interface_element(document->body),
         collection,
         (const lxb_char_t *)"a",
         1
    );
    string res = "";
    for (size_t i = 0; i < lxb_dom_collection_length(collection); i++)
    {
         lxb_dom_element_t *element = lxb_dom_collection_element(collection, i);

         size_t len;
         const lxb_char_t *href = lxb_dom_element_get_attribute(element, (const lxb_char_t *)"href", 4, &len);

         if (href && std::string((const char*)href).find("datasets") != std::string::npos) {
             res = (const char*)href;
             int len = res.length();
             return res.substr(17, len - 18);
         }

    }
    lxb_dom_collection_destroy(collection, true);
    lxb_html_document_destroy(document);
    lxb_html_parser_destroy(parser);

    return "no find";
}

void utils::download_genome_gff(const QString &url)
{
    QString path = "ncbi_files";
    QDir dir = QDir::currentPath();
    if (!dir.exists(path)) {
        dir.mkdir(path);
    }
    QString downloadPath = dir.filePath(path);

    QProcess *DL = new QProcess();
    DL->setWorkingDirectory(downloadPath);

    QObject::connect(DL, &QProcess::finished, [=](int exitCode) {
        QString URL = url;
         if (exitCode != 0) {
             DL->deleteLater();
             return;
         }
         DL->deleteLater();

         QProcess *unzip = new QProcess();
         unzip->setWorkingDirectory(downloadPath);

         QObject::connect(unzip, &QProcess::finished, [=](int code) {
             clear_dir(downloadPath, URL);
             unzip->deleteLater();
         });
         unzip->start("unzip", {"-o", "ncbi_dataset.zip"});
         qDebug() << "unzip";
    });
    QStringList args;
    args << "download" << "genome" << "accession" << url << "--include" << "gff3";
    DL->start("ncbi-datasets", args);
}

void utils::clear_dir(const QDir &release_dir, QString fileName) {
    QString relativePath = "ncbi_dataset/data/GCF_000001405.40/genomic.gff";

    if (QFile::rename(release_dir.filePath(relativePath), release_dir.filePath(fileName+".gff"))) {
        QStringList dest { "README.md","md5sum.txt","ncbi_dataset.zip"};
        for(int i=0;i<3; i++){
            if (QFile::exists(release_dir.filePath(dest[i]))){
                QFile::remove(release_dir.filePath(dest[i]));
            }else{
                qDebug() << release_dir.dirName();
            }
        }
        QDir dir = release_dir.filePath("ncbi_dataset");
        dir.removeRecursively();
    }
}