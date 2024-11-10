#include "html.h"

void html_begin(Html *html) {
    html->sb.arena = &html->arena;
    sb_append_cstr(&html->sb, "<!DOCTYPE html>\n");
    sb_append_cstr(&html->sb, "<html>\n");
    html->indentation = 1;
}

void html_end(Html *html) {
    sb_append_cstr(&html->sb, "</html>\n");
    html->indentation = 0;
}

void html_tag_begin(Html *html, const char *tag) {
    html_append_current_indentation(html);
    sb_append_cstr(&html->sb, "<");
    sb_append_cstr(&html->sb, tag);
    sb_append_cstr(&html->sb, ">\n");
    html->indentation++;
}

void html_tag_end(Html *html, const char *tag) {
    html->indentation--;
    html_append_current_indentation(html);
    sb_append_cstr(&html->sb, "</");
    sb_append_cstr(&html->sb, tag);
    sb_append_cstr(&html->sb, ">\n");
}

void html_text(Html *html, StringView sv) {
    html_append_current_indentation(html);
    sb_append_sv(&html->sb, sv);
    sb_append_cstr(&html->sb, "\n");
}

void html_head_begin(Html *html) {
    html_tag_begin(html, "head");
}

void html_head_end(Html *html) {
    html_tag_end(html, "head");
}

void html_body_begin(Html *html) {
    html_tag_begin(html, "body");
}

void html_body_end(Html *html) {
    html_tag_end(html, "body");
}

void html_title(Html *html, StringView title) {
    html_append_current_indentation(html);
    sb_append_cstr(&html->sb, "<title>");
    sb_append_sv(&html->sb, title);
    sb_append_cstr(&html->sb, "</title>\n");
}

void html_h1_begin(Html *html) {
    html_tag_begin(html, "h1");
}

void html_h1_end(Html *html) {
    html_tag_end(html, "h1");
}

void html_append_current_indentation(Html *html) {
    for (size_t i = 0; i < html->indentation; i++) {
        sb_append_cstr(&html->sb, "  ");
    }
}

void html_render_to_sb_and_free(Html *html, ArenaStringBuilder *sb) {
    sb_append_sb(sb, html->sb);
    arena_free(&html->arena);
}
