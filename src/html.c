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

void html_push_attribute(Html *html, Attribute attribute) {
    ARRAY_APPEND_ARENA(&html->attributes, attribute, &html->arena);
}

void html_push_attribute_cstrs(Html *html, const char *name, const char *value) {
    Attribute attribute = {
        .name = cstr_to_sv(name),
        .value = cstr_to_sv(value),
    };
    html_push_attribute(html, attribute);
}

void html_pop_attributes(Html *html, size_t count) {
    if (html->attributes.count > count) {
        html->attributes.count -= count;
    } else {
        html->attributes.count = 0;
    }
}

void html_tag_begin(Html *html, const char *tag) {
    html_append_current_indentation(html);
    sb_append_cstr(&html->sb, "<");
    sb_append_cstr(&html->sb, tag);
    for (size_t i = 0; i < html->attributes.count; i++) {
        sb_append_cstr(&html->sb, " ");
        sb_append_sv(&html->sb, html->attributes.items[i].name);
        sb_append_cstr(&html->sb, "=\"");
        sb_append_sv(&html->sb, html->attributes.items[i].value);
        sb_append_cstr(&html->sb, "\"");
    }
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

void html_text(Html *html, const char *text) {
    html_append_current_indentation(html);
    sb_append_cstr(&html->sb, text);
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

void html_title(Html *html, const char *title) {
    html_append_current_indentation(html);
    sb_append_cstr(&html->sb, "<title>");
    sb_append_cstr(&html->sb, title);
    sb_append_cstr(&html->sb, "</title>\n");
}

void html_nav_begin(Html *html) {
    html_tag_begin(html, "nav");
}

void html_nav_end(Html *html) {
    html_tag_end(html, "nav");
}

void html_div_begin(Html *html) {
    html_tag_begin(html, "div");
}

void html_div_end(Html *html) {
    html_tag_end(html, "div");
}

void html_a_begin(Html *html) {
    html_tag_begin(html, "a");
}

void html_a_end(Html *html) {
    html_tag_end(html, "a");
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

void html_ul_begin(Html *html) {
    html_tag_begin(html, "ul");
}

void html_ul_end(Html *html) {
    html_tag_end(html, "ul");
}

void html_li_begin(Html *html) {
    html_tag_begin(html, "li");
}

void html_li_end(Html *html) {
    html_tag_end(html, "li");
}

void html_render_to_sb_and_free(Html *html, ArenaStringBuilder *sb) {
    sb_append_sb(sb, html->sb);
    arena_free(&html->arena);
}
