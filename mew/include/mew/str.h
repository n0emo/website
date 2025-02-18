#ifndef STR_H_
#define STR_H_

#include <stdbool.h>
#include <sys/types.h>

#include "alloc.h"

bool str_contains(const char *s, char c);

typedef struct StringView StringView;

typedef struct {
    Allocator alloc;
    char *items;
    size_t count;
    size_t capacity;
} StringBuilder;

void sb_append_char(StringBuilder *sb, char c);
void sb_append_buf(StringBuilder *sb, const char *buf, size_t size);
void sb_append_cstr(StringBuilder *sb, const char *s);
void sb_append_sv(StringBuilder *sb, StringView sv);
void sb_append_sb(StringBuilder *sb, StringBuilder other);

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
StringView sv_dup(Allocator alloc, StringView sv);
size_t sv_count_char(StringView sv, char c);
ssize_t sv_index_char(StringView sv, char c);
ssize_t sv_last_index_char(StringView sv, char c);

#endif // STRINGVIEW_H_
