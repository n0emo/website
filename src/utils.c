#include "utils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

bool str_contains(const char *s, char c) {
    while (*s != '\0') {
        if (*s == c) return true;
        s++;
    }
    return false;
}

void sb_append_char(StringBuilder *sb, char c)  {
    ARRAY_APPEND(sb, c, sb->alloc);
}

void sb_append_buf(StringBuilder *sb, const char *buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        sb_append_char(sb, buf[i]);
    }
}

void sb_append_cstr(StringBuilder *sb, const char *s) {
    while(*s != '\0') {
        sb_append_char(sb, *s);
        s++;
    }
}

void sb_append_sv(StringBuilder *sb, StringView sv) {
    sb_append_buf(sb, sv.items, sv.count);
}

void sb_append_sb(StringBuilder *sb, StringBuilder other) {
    sb_append_buf(sb, other.items, other.count);
}

StringView cstr_to_sv(const char *cstr) {
    return (StringView) {
        .items = cstr,
        .count = strlen(cstr),
    };
}

StringView sb_to_sv(StringBuilder sb) {
    return (StringView) {
        .items = sb.items,
        .count = sb.count,
    };
}

bool sv_eq_cstr(StringView sv, const char *cstr) {
    return strlen(cstr) == sv.count && strncmp(sv.items, cstr, sv.count) == 0;
}

bool sv_eq_sv(StringView a, StringView b) {
    if (a.count != b.count) return false;
    return memcmp(a.items, b.items, a.count) == 0;
}

bool sv_starts_with(StringView sv, StringView prefix) {
    if (sv.count < prefix.count) return false;
    return strncmp(sv.items, prefix.items, prefix.count) == 0;
}

bool sv_starts_with_cstr(StringView sv, const char *prefix) {
    return sv_starts_with(sv, cstr_to_sv(prefix));
}

bool sv_contains(StringView sv, char c) {
    for (size_t i = 0; i < sv.count; i++) {
        if (sv.items[i] == c) return true;
    }

    return false;
}

StringView sv_slice(StringView sv, size_t start, size_t count) {
    if (sv.count < start + count) return (StringView) {0};

    return (StringView) {
        .items = sv.items + start,
        .count = count,
    };
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

StringView sv_trim_left_by_cstr(StringView sv, const char *chars) {
    StringView result = sv;

    while (result.count > 0 && strchr(chars, result.items[result.count - 1]) != NULL) {
        result.count--;
    }

    return result;
}

StringView sv_trim_right_by_cstr(StringView sv, const char *chars) {
    StringView result = sv;

    while (result.count > 0 && strchr(chars, result.items[0]) != NULL) {
        result = sv_slice_from(sv, 1);
    }

    return result;
}

StringView sv_trim_left_to_cstr(StringView sv, const char *chars) {
    StringView result = sv;

    while (result.count > 0 && strchr(chars, result.items[result.count - 1]) == NULL) {
        result.count--;
    }

    return result;
}

StringView sv_trim_right_to_cstr(StringView sv, const char *chars) {
    StringView result = sv;

    while (result.count > 0 && strchr(chars, result.items[0]) == NULL) {
        result = sv_slice_from(sv, 1);
    }

    return result;
}

StringView sv_trim_space(StringView sv) {
    const char *chars = " \r\n\t";
    return sv_trim_right_by_cstr(sv_trim_left_by_cstr( sv, chars), chars);
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

StringView sv_dup(Allocator *alloc, StringView sv) {
    return (StringView) {
        .items = mem_memdup(alloc, sv.items, sv.count),
        .count = sv.count,
    };
}

bool read_file_to_sb(const char *path, StringBuilder *asb) {
    bool result = true;
    int fd = open(path, O_RDONLY);
    if (fd < 0) return_defer(false);
    struct stat stat_buf;
    if (stat(path, &stat_buf) < 0) return_defer(false);

    char buf[1024] = {0};
    while ((long) asb->count < stat_buf.st_size) {
        ssize_t size = read(fd, buf, sizeof(buf));
        if (size < 0) return_defer(false);
        sb_append_buf(asb, buf, (size_t) size);
    }

defer:
    if (fd > 0) close(fd);
    return result;
}
