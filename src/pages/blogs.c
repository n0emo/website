#include "blogs.h"

#include "base.h"

// TODO: render markdown to HTML
void blogs(StringBuilder *sb, BlogList list) {
    Html html = html_begin();
    page_base_begin(&html, cstr_to_sv("Blogs"));

    html_h1_begin(&html);
    html_text_cstr(&html, "Recent blog posts:");
    html_h1_end(&html);

    for (size_t i = 0; i < list.count; i++) {
        Blog d = list.items[i];
        html_div_begin(&html);

        html_h2_begin(&html);
        const char *blog_destination = mem_sprintf(
            html.alloc,
            "/blogs/" SV_FMT,
            SV_ARG(d.dir)
        );
        html_hyperlink(&html, d.name, cstr_to_sv(blog_destination));
        html_h2_end(&html);

        const char *blog_desc = mem_sprintf(
            html.alloc,
            SV_FMT " - by " SV_FMT,
            SV_ARG(d.desc), SV_ARG(d.author)
        );

        html_p_begin(&html);
        html_text_cstr(&html, blog_desc);
        html_p_end(&html);

        html_div_end(&html);
    }

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}
