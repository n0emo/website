#include "utils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

#define REGION_DEFAULT_CAPACITY (8 * 1024)

void *arena_alloc(Arena *arena, size_t bytes) {
    size_t size = (bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

    if (arena->begin == NULL) {
        assert(arena->end == NULL);
        size_t capacity = REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        arena->begin = new_region(capacity);
        arena->end = arena->begin;
    }

    while (arena->end->count + size > arena->end->capacity && arena->end->next != NULL) {
        arena->end = arena->end->next;
    }

     if (arena->end->count + size > arena->end->capacity) {
        assert(arena->end->next == NULL);
        size_t capacity = REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        arena->end->next = new_region(capacity);
        arena->end = arena->end->next;
    }

    void *result = &arena->end->data[arena->end->count];
    arena->end->count += size;
    return result;
}

void arena_free(Arena *arena) {
    Region *region = arena->begin;
    while (region != NULL) {
        Region *next = region->next;
        free_region(region);
        region = next;
    }

    arena->begin = NULL;
    arena->end = NULL;
}

Region *new_region(size_t capacity) {
    size_t size = sizeof(Region) + sizeof(uintptr_t) * capacity;
    Region *region = malloc(size);

    region->next = NULL;
    region->count = 0;
    region->capacity = capacity;

    return region;
}

void free_region(Region *region) {
    free(region);
}

char *arena_sprintf(Arena *a, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int size = vsnprintf(NULL, 0, format, args);
    va_end(args);

    assert(size >= 0);
    char *result = (char*)arena_alloc(a, size + 1);
    va_start(args, format);
    vsnprintf(result, size + 1, format, args);
    va_end(args);

    return result;
}

void sb_append_char(ArenaStringBuilder *sb, char c)  {
    ARRAY_APPEND_ARENA(sb, c, sb->arena);
}

void sb_append_cstr(ArenaStringBuilder *sb, const char *s) {
    while(*s != '\0') {
        sb_append_char(sb, *s);
        s++;
    }
}

void sb_append_sv(ArenaStringBuilder *sb, StringView sv) {
    for (size_t i = 0; i < sv.count; i++) {
        sb_append_char(sb, sv.items[i]);
    }
}

void sb_append_sb(ArenaStringBuilder *sb, ArenaStringBuilder other) {
    for (size_t i = 0; i < other.count; i++) {
        sb_append_char(sb, other.items[i]);
    }

}

StringView cstr_to_sv(const char *cstr) {
    return (StringView) {
        .items = cstr,
        .count = strlen(cstr),
    };
}

StringView sb_to_sv(ArenaStringBuilder sb) {
    return (StringView) {
        .items = sb.items,
        .count = sb.count,
    };
}

bool sv_eq_cstr(StringView sv, const char *cstr) {
    return strlen(cstr) == sv.count && strncmp(sv.items, cstr, sv.count) == 0;
}

bool sv_starts_with(StringView sv, StringView prefix) {
    if (sv.count < prefix.count) return false;
    return strncmp(sv.items, prefix.items, prefix.count) == 0;
}

bool sv_starts_with_cstr(StringView sv, const char *prefix) {
    return sv_starts_with(sv, cstr_to_sv(prefix));
}

StringView sv_slice_from(StringView sv, size_t index) {
    if (sv.count < index) {
        return (StringView) {0};
    }

    return (StringView) {
        .items = sv.items + index,
        .count = sv.count - index,
    };
}

StringView sv_trim_right_to_cstr(StringView sv, char *chars) {
    StringView result = sv;

    while (result.count > 0 && strchr(chars, result.items[0]) == NULL) {
        result = sv_slice_from(sv, 1);
    }

    return result;
}

StringView sv_chop_by(StringView *sv, char c) {
    StringView result = {
        .items = sv->items,
        .count = 0,
    };

    while (sv->count > 0 && sv->items[0] != c) {
        result.count++;
        *sv = sv_slice_from(*sv, 1);
    }

    if (sv->count > 0) {
        *sv = sv_slice_from(*sv, 1);
    }

    return result;
}

bool file_exists_in_dir(StringView file, StringView dir) {
    if (file.items[0] == '/') file = sv_slice_from(file, 1);

    size_t size = snprintf(
        NULL, 0,
        "%.*s/%.*s",
        (int) dir.count, dir.items,
        (int) file.count, file.items
    );

    char *path = alloca(size + 1);
    snprintf(
        path, size + 1,
        "%.*s/%.*s",
        (int) dir.count, dir.items,
        (int) file.count, file.items
    );

    if (strncmp(path, "../", 3) == 0 ||
        strncmp(path + size - 3, "/..", 3) == 0 ||
        strstr(path, "/../") != NULL) {
        return false;
    }

    struct stat buf = {0};
    return stat(path, &buf) == 0;
}
