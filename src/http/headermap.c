#include "http/headermap.h"

int header_cmp(const void *a, const void *b, void *user_data);

void http_headermap_init(HttpHeaderMap *map, void *user_data, Arena *arena) {
    map->tree = rbtree_new(sizeof(HttpHeader), header_cmp, user_data);
    map->tree.alloc = new_arena_allocator(arena);
}

void http_headermap_insert(HttpHeaderMap *map, HttpHeader header) {
    rbtree_insert(&map->tree, &header);
}

void http_headermap_insert_cstrs(HttpHeaderMap *map, const char *key, const char *value) {
    HttpHeader header = {
        .key = cstr_to_sv(key),
        .value = cstr_to_sv(value),
    };

    http_headermap_insert(map, header);
}

int header_cmp(const void *a, const void *b, void *user_data) {
    (void) user_data;
    const HttpHeader *first = (const HttpHeader *) a;
    const HttpHeader *second = (const HttpHeader *) b;
    return sv_cmp(first->key, second->key);
}

