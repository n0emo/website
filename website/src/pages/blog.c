#include "blog.h"

#include "base.h"

void blog(StringBuilder *sb, Blog desc, StringView text) {
    Html html = html_begin();

    page_base_begin(&html, desc.name);

    html_hyperlink_cstr(&html, "Back", "/blogs");

    html_h1_begin(&html);
    html_text(&html, desc.name);
    html_h1_end(&html);

    html_p_begin(&html);
    html_text(&html, text);
    html_p_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}
