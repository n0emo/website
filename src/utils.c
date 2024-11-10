#include "utils.h"

#include <stdio.h>

void sb_append_cstr(StringBuilder *sb, const char *s) {
    while(*s != '\0') {
        ARRAY_APPEND(sb, *s);
        s++;
    }
}

StringView cstr_to_sv(const char *cstr) {
    return (StringView) {
        .items = cstr,
        .count = strlen(cstr),
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
