#ifndef MEW_INCLUDE_MEW_HTML_H_
#define MEW_INCLUDE_MEW_HTML_H_

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
    Allocator alloc;
    StringBuilder sb;
    Attributes attributes;
    Classes classes;
    size_t indentation;
} Html;

void html_tag_begin(Html *html, const char *tag);
void html_tag_end(Html *html, const char *tag);
void html_tag_short(Html *html, const char *tag);
void html_push_attribute(Html *html, Attribute attribute);
void html_push_attribute_cstrs(Html *html, const char *name, const char *value);
void html_push_class(Html *html, StringView cls);
void html_push_class_cstr(Html *html, const char *cls);
void html_text_cstr(Html *html, const char *text);
void html_text(Html *html, StringView sv);
void html_render_to_sb_and_free(Html *html, StringBuilder *sb);

Html html_begin(void);
void html_end(Html *html);

void html_image(Html *html, StringView src);
void html_hyperlink(Html *html, StringView text, StringView href);
void html_hyperlink_cstr(Html *html, const char *text, const char *href);
void html_title(Html *html, StringView title);
void html_title_cstr(Html *html, const char *title);

#define HTML_TAG_LIST \
    X(a) \
    X(abbr) \
    X(acronym) \
    X(address) \
    X(applet) \
    X(area) \
    X(article) \
    X(aside) \
    X(audio) \
    X(b) \
    X(base) \
    X(basefont) \
    X(bdi) \
    X(bdo) \
    X(big) \
    X(blockquote) \
    X(body) \
    X(br) \
    X(button) \
    X(canvas) \
    X(caption) \
    X(center) \
    X(cite) \
    X(code) \
    X(col) \
    X(colgroup) \
    X(data) \
    X(datalist) \
    X(dd) \
    X(del) \
    X(details) \
    X(dfn) \
    X(dialog) \
    X(dir) \
    X(div) \
    X(dl) \
    X(dt) \
    X(em) \
    X(embed) \
    X(fieldset) \
    X(figcaption) \
    X(figure) \
    X(font) \
    X(footer) \
    X(form) \
    X(frame) \
    X(frameset) \
    X(h1) \
    X(h2) \
    X(h3) \
    X(h4) \
    X(h5) \
    X(h6) \
    X(head) \
    X(header) \
    X(hgroup) \
    X(hr) \
    X(i) \
    X(iframe) \
    X(img) \
    X(input) \
    X(ins) \
    X(kbd) \
    X(label) \
    X(legend) \
    X(li) \
    X(link) \
    X(main) \
    X(map) \
    X(mark) \
    X(menu) \
    X(meta) \
    X(meter) \
    X(nav) \
    X(noframes) \
    X(noscript) \
    X(object) \
    X(ol) \
    X(optgroup) \
    X(option) \
    X(output) \
    X(p) \
    X(param) \
    X(picture) \
    X(pre) \
    X(progress) \
    X(q) \
    X(rp) \
    X(rt) \
    X(ruby) \
    X(s) \
    X(samp) \
    X(script) \
    X(search) \
    X(section) \
    X(select) \
    X(small) \
    X(source) \
    X(span) \
    X(strike) \
    X(strong) \
    X(style) \
    X(sub) \
    X(summary) \
    X(sup) \
    X(svg) \
    X(table) \
    X(tbody) \
    X(td) \
    X(template) \
    X(textarea) \
    X(tfoot) \
    X(th) \
    X(thead) \
    X(time) \
    X(title) \
    X(tr) \
    X(track) \
    X(tt) \
    X(u) \
    X(ul) \
    X(var) \
    X(video) \
    X(wbr)

#define X(tag) \
    void html_ ## tag ## _begin(Html *html); \
    void html_ ## tag ## _end(Html *html); \
    void html_ ## tag ## _short(Html *html);
HTML_TAG_LIST
#undef X

void html_append_current_indentation(Html *html);
void html_render_class(Html *html);
void html_render_attributes(Html *html);

#endif // MEW_INCLUDE_MEW_HTML_H_
