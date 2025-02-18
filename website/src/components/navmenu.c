#include "navmenu.h"

#include "components/linkitem.h"

void comp_navigation_menu(Html *html) {
    html_nav_begin(html);
    html_ul_begin(html);
    comp_link_item(html, "About", "#about");
    comp_link_item(html, "Projects", "#projects");
    comp_link_item(html, "Games", "#games");
    comp_link_item(html, "Contact", "#contact");

    html_ul_end(html);
    html_nav_end(html);
}
