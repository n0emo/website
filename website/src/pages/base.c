#include "base.h"

#include "mew/html.h"

#include "components/navmenu.h"
#include "components/footer.h"

void page_base_begin(Html *html, StringView title) {
    html_head_begin(html);
    html_title(html, title);
    html_push_attribute_cstrs(html, "name", "viewport");
    html_push_attribute_cstrs(html, "content", "width=device-width");
    html_meta_short(html);
    html_push_attribute_cstrs(html, "rel", "stylesheet");
    html_push_attribute_cstrs(html, "href", "/styles/base.css");
    html_link_short(html);
    html_head_end(html);

    html_body_begin(html);
    comp_navigation_menu(html);
    html_main_begin(html);
}

void page_base_end(Html *html) {
    html_main_end(html);
    comp_footer(html);
    html_body_end(html);
    html_end(html);
}
