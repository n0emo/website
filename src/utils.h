#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "alloc.h"

/********* Error handling **********/

#define return_defer(ret) do { \
    result = ret; \
    goto defer; \
} while(0) \

#define try(...) if (!(__VA_ARGS__)) return_defer(false)

/********* Dynamic array **********/

#define INITIAL_CAP 128

#define ARRAY_APPEND(array, item, alloc) do { \
    if ((array)->items == NULL) { \
        (array)->items = (typeof((array)->items)) mem_alloc(alloc, INITIAL_CAP * sizeof((array)->items[0])); \
        (array)->count = 0; \
        (array)->capacity = INITIAL_CAP; \
    } else if ((array)->count >= (array)->capacity) { \
        size_t new_capacity = (array)->capacity * 2; \
        typeof((array)->items[0]) *items = mem_alloc(alloc, new_capacity * sizeof((array)->items[0])); \
        memcpy(items, (array)->items, (array)->capacity); \
        (array)->items = items; \
        (array)->capacity = new_capacity; \
    } \
    (array)->items[(array)->count] = (item); \
    (array)->count++; \
} while(0) \

/********* String **********/

bool str_contains(const char *s, char c);

/********* String builder **********/

typedef struct StringView StringView;

typedef struct {
    Allocator *alloc;
    char *items;
    size_t count;
    size_t capacity;
} StringBuilder;

void sb_append_char(StringBuilder *sb, char c);
void sb_append_buf(StringBuilder *sb, const char *buf, size_t size);
void sb_append_cstr(StringBuilder *sb, const char *s);
void sb_append_sv(StringBuilder *sb, StringView sv);
void sb_append_sb(StringBuilder *sb, StringBuilder other);

/********* String view **********/

#define SV_FMT "%.*s"
#define SV_ARG(sv) (int) (sv).count, (sv).items

struct StringView {
    const char *items;
    size_t count;
};

int sv_cmp(StringView a, StringView b);
bool sv_eq_cstr(StringView sv, const char *cstr);
bool sv_eq_sv(StringView a, StringView b);
StringView cstr_to_sv(const char *cstr);
StringView sb_to_sv(StringBuilder sb);
bool sv_contains(StringView sv, char c);
bool sv_starts_with(StringView sv, StringView prefix);
bool sv_starts_with_cstr(StringView sv, const char *prefix);
StringView sv_slice(StringView sv, size_t start, size_t count);
StringView sv_slice_from(StringView sv, size_t index);
StringView sv_trim_right_by_cstr(StringView sv,const char *chars);
StringView sv_trim_left_by_cstr(StringView sv, const char *chars);
StringView sv_trim_left_to_cstr(StringView sv, const char *chars);
StringView sv_trim_right_to_cstr(StringView sv, const char *chars);
StringView sv_trim_space(StringView sv);
StringView sv_chop_by(StringView *sv, char c);
StringView sv_dup(Allocator *alloc, StringView sv);

/********** Filesystem ***********/

bool read_file_to_sb(const char *path, StringBuilder *sb);

#endif // UTILS_H_

