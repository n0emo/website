#include "technology.h"

#include "mew/http/common.h"

void comp_technology(Html *html, const char *name) {
    html_push_class_cstr(html, "technology-wrap");
    html_div_begin(html);
    html_push_class_cstr(html, "technology");
    html_div_begin(html);

    StringBuilder src = { .alloc = html->alloc, 0 };
    sb_append_cstr(&src, "/logos/");
    http_urlencode(cstr_to_sv(name), &src);
    sb_append_cstr(&src, ".svg\0");
    html_push_attribute_cstrs(html, "src", src.items);
    html_tag_short(html, "img");

    html_p_begin(html);
    html_text_cstr(html, name);
    html_p_end(html);

    html_div_end(html);
    html_div_end(html);
}
