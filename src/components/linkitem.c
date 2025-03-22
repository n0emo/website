#include "linkitem.h"

void comp_link_item(Html *html, const char *title, const char *destination) {
    html_li_begin(html);
    html_hyperlink_cstr(html, title, destination);
    html_li_end(html);
}
