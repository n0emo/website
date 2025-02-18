#include "blogs.h"

#include <dirent.h>
#include <unistd.h>

#include "mew/ini.h"
#include "mew/log.h"
#include "mew/utils.h"

bool get_blogs(Allocator alloc, BlogList *list) {
    bool result = true;
    DIR *dir;
    struct dirent *dirent;
    StringBuilder sb = { .alloc = alloc, 0 };

    dir = opendir("blogs");
    if (dir == NULL) return_defer(false);

    while ((dirent = readdir(dir))) {
        const char *d = dirent->d_name;
        if (strncmp(d, ".", 1) == 0 || strncmp(d, "..", 2) == 0) continue;
        const char *path = mem_sprintf(alloc, "blogs/%s/metadata.ini", d);

        if (!read_file_to_sb(path, &sb)) {
            sb.count = 0;
            continue;
        }

        Ini ini = { .alloc = alloc, .sections = {0} };
        StringView sv = {
            .items = mem_memdup(alloc, sb.items, sb.count),
            .count = sb.count,
        };
        if (parse_ini(sv, &ini)) {
            IniSection *metadata = ini_get_section(ini, cstr_to_sv("Metadata"));
            if (metadata == NULL) continue;

            IniItem *name = ini_get_item(*metadata, cstr_to_sv("name"));
            if (name == NULL) continue;

            IniItem *desc = ini_get_item(*metadata, cstr_to_sv("desc"));
            if (desc == NULL) continue;

            IniItem *author = ini_get_item(*metadata, cstr_to_sv("author"));
            if (author == NULL) continue;

            Blog blog = {
                .dir = cstr_to_sv(mem_strdup(alloc, d)),
                .name = sv_dup(alloc, name->value),
                .desc = sv_dup(alloc, desc->value),
                .author = sv_dup(alloc, author->value),
            };

            ARRAY_APPEND(list, blog, alloc);
        } else {
            log_error("Error parsing %s", path);
        }

        sb.count = 0;
    }

defer:
    closedir(dir);
    return result;
}

bool get_blog(Allocator alloc, StringView dir, Blog *blog_desc) {
    StringBuilder sb = { .alloc = alloc, 0 };
    const char *path = mem_sprintf(alloc, "blogs/" SV_FMT "/metadata.ini", SV_ARG(dir));

    if (!read_file_to_sb(path, &sb)) return false;
    Ini ini = { .alloc = alloc, {0} };
    if (!parse_ini(sb_to_sv(sb), &ini)) return false;

    // TODO: move to separate function
    IniSection *metadata = ini_get_section(ini, cstr_to_sv("Metadata"));
    if (metadata == NULL) return false;

    IniItem *name = ini_get_item(*metadata, cstr_to_sv("name"));
    if (name == NULL) return false;

    IniItem *desc = ini_get_item(*metadata, cstr_to_sv("desc"));
    if (desc == NULL) return false;

    IniItem *author = ini_get_item(*metadata, cstr_to_sv("author"));
    if (author == NULL) return false;

    *blog_desc = (Blog) {
        .dir = dir,
        .name = sv_dup(alloc, name->value),
        .desc = sv_dup(alloc, desc->value),
        .author = sv_dup(alloc, author->value),
    };

    return true;
}

bool get_blog_text(Allocator alloc, StringView dir, StringBuilder *blog_text) {
    const char *path = mem_sprintf(alloc, "blogs/" SV_FMT "/en.md", SV_ARG(dir));
    return read_file_to_sb(path, blog_text);
}
