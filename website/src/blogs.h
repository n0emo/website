#ifndef WEBSITE_SRC_BLOGS_H_
#define WEBSITE_SRC_BLOGS_H_

#include "mew/str.h"

typedef struct Blog {
    StringView dir;
    StringView name;
    StringView desc;
    StringView author;
} Blog;

typedef struct {
    Blog *items;
    size_t count;
    size_t capacity;
} BlogList;

bool get_blogs(Allocator alloc, BlogList *list);
void blog(StringBuilder *sb, Blog desc, StringView text);
bool get_blog(Allocator alloc, StringView dir, Blog *desc);
bool get_blog_text(Allocator alloc, StringView dir, StringBuilder *blog_text);

#endif // WEBSITE_SRC_BLOGS_H_
