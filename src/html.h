#ifndef HTML_H_
#define HTML_H_

#include "utils.h"

typedef struct {
    Arena arena;
    ArenaStringBuilder sb;
    size_t indentation;
} Html;

void html_begin(Html *html);
void html_end(Html *html);
void html_tag_begin(Html *html, const char *tag);
void html_tag_end(Html *html, const char *tag);
void html_text(Html *html, StringView sv);
void html_head_begin(Html *html);
void html_head_end(Html *html);
void html_body_begin(Html *html);
void html_body_end(Html *html);
void html_title(Html *html, StringView title);
void html_h1_begin(Html *html);
void html_h1_end(Html *html);
void html_render_to_sb_and_free(Html *html, ArenaStringBuilder *sb);

void html_append_current_indentation(Html *html);

#endif // HTML_H_
