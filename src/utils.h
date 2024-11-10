#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <stdlib.h>
#define UTILS_MALLOC malloc
#define UTILS_REALLOC realloc

/********* Error handling **********/

#define return_defer(ret) do { \
    result = ret; \
    goto defer; \
} while(0) \

#define try(...) if (!(__VA_ARGS__)) return_defer(false)

/********* Dynamic array **********/

#define INITIAL_CAP 4

#define ARRAY_APPEND(array, item) do { \
    if ((array)->items == NULL) { \
        (array)->items = (typeof((array)->items)) UTILS_MALLOC(INITIAL_CAP * sizeof((array)->items[0])); \
        (array)->count = 0; \
        (array)->capacity = INITIAL_CAP; \
    } else if ((array)->count >= (array)->capacity) { \
        size_t new_capacity = (array)->capacity * 2; \
        (array)->items = (typeof((array)->items)) UTILS_REALLOC((array)->items, new_capacity * sizeof((array)->items[0])); \
        (array)->capacity = new_capacity; \
    } \
    (array)->items[(array)->count] = (item); \
    (array)->count++; \
} while(0) \

/********* String builder **********/

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} StringBuilder;

void sb_append_cstr(StringBuilder *sb, const char *s);

/********* String view **********/

typedef struct {
    const char *items;
    size_t count;
} StringView;

bool sv_eq_cstr(StringView sv, const char *cstr);
StringView cstr_to_sv(const char *cstr);
bool sv_starts_with(StringView sv, StringView prefix);
bool sv_starts_with_cstr(StringView sv, const char *prefix);
StringView sv_slice_from(StringView sv, size_t index);
StringView sv_trim_right_to_cstr(StringView sv, char *chars);
StringView sv_chop_by(StringView *sv, char c);

#endif // UTILS_H_

