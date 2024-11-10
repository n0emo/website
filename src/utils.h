#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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

/********* Arena **********/

typedef struct Region Region;

struct Region {
    Region *next;
    size_t count;
    size_t capacity;
    uintptr_t data[];
};

typedef struct {
    Region *begin;
    Region *end;
} Arena;

void *arena_alloc(Arena *arena, size_t bytes);
void arena_free(Arena *arena);
Region *new_region(size_t capacity);
void free_region(Region *region);

/********* Dynamic array **********/

#define INITIAL_CAP 128

#define ARRAY_APPEND(array, item) do { \
    if ((array)->items == NULL) { \
        (array)->items = (typeof((array)->items)) malloc(INITIAL_CAP * sizeof((array)->items[0])); \
        (array)->count = 0; \
        (array)->capacity = INITIAL_CAP; \
    } else if ((array)->count >= (array)->capacity) { \
        size_t new_capacity = (array)->capacity * 2; \
        (array)->items = (typeof((array)->items)) realloc((array)->items, new_capacity * sizeof((array)->items[0])); \
        (array)->capacity = new_capacity; \
    } \
    (array)->items[(array)->count] = (item); \
    (array)->count++; \
} while(0) \

#define ARRAY_APPEND_ARENA(array, item, arena) do { \
    if ((array)->items == NULL) { \
        (array)->items = (typeof((array)->items)) arena_alloc(arena, INITIAL_CAP * sizeof((array)->items[0])); \
        (array)->count = 0; \
        (array)->capacity = INITIAL_CAP; \
    } else if ((array)->count >= (array)->capacity) { \
        size_t new_capacity = (array)->capacity * 2; \
        typeof((array)->items[0]) *items = arena_alloc(arena, new_capacity * sizeof((array)->items[0])); \
        memcpy(items, (array)->items, (array)->capacity); \
        (array)->items = items; \
        (array)->capacity = new_capacity; \
    } \
    (array)->items[(array)->count] = (item); \
    (array)->count++; \
} while(0) \

/********* String builder **********/

typedef struct {
    Arena *arena;
    char *items;
    size_t count;
    size_t capacity;
} ArenaStringBuilder;

void sb_append_cstr(ArenaStringBuilder *sb, const char *s);

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

