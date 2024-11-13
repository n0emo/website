#ifndef HTML_H_
#define HTML_H_

#include "utils.h"

typedef struct {
    StringView name;
    StringView value;
} Attribute;

typedef struct {
    Attribute *items;
    size_t count;
    size_t capacity;
} Attributes;

typedef struct {
    StringView *items;
    size_t count;
    size_t capacity;
} Classes;

typedef struct {
    Arena arena;
    ArenaStringBuilder sb;
    Attributes attributes;
    Classes classes;
    size_t indentation;
} Html;

void html_begin(Html *html);
void html_end(Html *html);
void html_push_attribute(Html *html, Attribute attribute);
void html_push_attribute_cstrs(Html *html, const char *name, const char *value);
void html_pop_attributes(Html *html, size_t count);
void html_push_class(Html *html, StringView cls);
void html_push_class_cstr(Html *html, const char *cls);
void html_pop_classes(Html *html, size_t count);
void html_tag_short(Html *html, const char *tag);
void html_tag_begin(Html *html, const char *tag);
void html_tag_end(Html *html, const char *tag);
void html_text(Html *html, const char *text);
void html_head_begin(Html *html);
void html_head_end(Html *html);
void html_body_begin(Html *html);
void html_body_end(Html *html);
void html_title(Html *html, const char *title);
void html_link(Html *html);
void html_nav_begin(Html *html);
void html_nav_end(Html *html);
void html_div_begin(Html *html);
void html_div_end(Html *html);
void html_a_begin(Html *html);
void html_a_end(Html *html);
void html_h1_begin(Html *html);
void html_h1_end(Html *html);
void html_h2_begin(Html *html);
void html_h2_end(Html *html);
void html_ul_begin(Html *html);
void html_ul_end(Html *html);
void html_li_begin(Html *html);
void html_li_end(Html *html);
void html_render_to_sb_and_free(Html *html, ArenaStringBuilder *sb);

void html_append_current_indentation(Html *html);
void html_render_class(Html *html);
void html_render_attributes(Html *html);

#endif // HTML_H_
