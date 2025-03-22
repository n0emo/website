#include "project.h"

void comp_project(Html *html, CompProject project) {
    html_push_class_cstr(html, "project");
    html_div_begin(html);

    html_hyperlink_cstr(html, project.name, project.source_code);
    html_p_begin(html);
    html_text_cstr(html, project.desctiption);
    html_p_end(html);

    html_div_end(html);
}
