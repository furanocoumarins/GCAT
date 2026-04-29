#include "utils.h"

using namespace std;

void utils::get_gff3_file(string CHROM){
    lxb_status_t status;
    lxb_html_parser_t *parser = lxb_html_parser_create();
    status = lxb_html_parser_init(parser);
    lxb_html_parser_destroy(parser);
}