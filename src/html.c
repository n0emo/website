#include "html.h"

#include <assert.h>

void html_begin(Html *html) {
    html->sb.arena = &html->arena;
    sb_append_cstr(&html->sb, "<!DOCTYPE html>\n");
    html_tag_begin(html, "html");
}

void html_end(Html *html) {
    html_tag_end(html, "html");
    assert(html->indentation == 0);
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
    assert(html->attributes.count >= count);
    html->attributes.count -= count;
}

void html_push_class(Html *html, StringView cls) {
    ARRAY_APPEND_ARENA(&html->classes, cls, &html->arena);
}

void html_push_class_cstr(Html *html, const char *cls) {
    StringView sv = cstr_to_sv(cls);
    html_push_class(html, sv);
}

void html_pop_classes(Html *html, size_t count) {
    assert(html->classes.count >= count);
    html->classes.count -= count;
}

void html_tag_short(Html *html, const char *tag) {
    html_append_current_indentation(html);
    sb_append_cstr(&html->sb, "<");
    sb_append_cstr(&html->sb, tag);
    html_render_class(html);
    html_render_attributes(html);
    sb_append_cstr(&html->sb, "/>\n");
}

void html_tag_begin(Html *html, const char *tag) {
    html_append_current_indentation(html);
    sb_append_cstr(&html->sb, "<");
    sb_append_cstr(&html->sb, tag);
    html_render_class(html);
    html_render_attributes(html);
    sb_append_cstr(&html->sb, ">\n");
    html->indentation++;
}

void html_tag_end(Html *html, const char *tag) {
    assert(html->indentation > 0);
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

void html_link(Html *html) {
    html_tag_short(html, "link");
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

void html_h2_begin(Html *html) {
    html_tag_begin(html, "h2");
}

void html_h2_end(Html *html) {
    html_tag_end(html, "h2");
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

void html_append_current_indentation(Html *html) {
    for (size_t i = 0; i < html->indentation; i++) {
        sb_append_cstr(&html->sb, "  ");
    }
}


void html_render_class(Html *html) {
    if (html->classes.count > 0) {
        sb_append_cstr(&html->sb, " class=\"");
        sb_append_sv(&html->sb, html->classes.items[0]);
        for (size_t i = 1; i < html->classes.count; i++) {
            sb_append_char(&html->sb, ' ');
            sb_append_sv(&html->sb, html->classes.items[i]);
        }
        sb_append_cstr(&html->sb, "\"");
    }
}

void html_render_attributes(Html *html) {
    for (size_t i = 0; i < html->attributes.count; i++) {
        sb_append_cstr(&html->sb, " ");
        sb_append_sv(&html->sb, html->attributes.items[i].name);
        sb_append_cstr(&html->sb, "=\"");
        sb_append_sv(&html->sb, html->attributes.items[i].value);
        sb_append_cstr(&html->sb, "\"");
    }
}
