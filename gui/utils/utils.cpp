#include "utils.h"
#include <QDebug>
using namespace std;

string utils::get_gff3_file(string CHROM){
    cpr::Response r = cpr::Get(cpr::Url{"https://www.ncbi.nlm.nih.gov/nuccore/"+CHROM});
    string re = utils::get_Assembly_link(r.text);
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
             return "https://www.ncbi.nlm.nih.gov" + res;
         }

    }
    lxb_dom_collection_destroy(collection, true);
    lxb_html_document_destroy(document);
    lxb_html_parser_destroy(parser);

    return "no find";
}