#include "mew/http/headermap.h"

#include "mew/utils.h"

hashfunc_t header_hash;
hashmap_equals_t header_equals;

void http_headermap_init(HttpHeaderMap *map, Allocator alloc) {
    hashmap_init(
        &map->indices,
        (void *) map,
        hashmap_sv_hash,
        hashmap_sv_equals,
        sizeof(StringView),
        sizeof(size_t)
    );
    map->indices.alloc = alloc;
    map->entries = (HttpHeaderMapEntries) {0};
    map->alloc = alloc;
}

void http_headermap_insert(HttpHeaderMap *map, HttpHeader header) {
    void *value = hashmap_get(&map->indices, &header.key);
    if (value == NULL) {
        HttpHeaderMapEntry entry = {
            .header = header,
            .next = NULL,
        };
        ARRAY_APPEND(&map->entries, entry, map->alloc);
        size_t index = map->entries.count - 1;
        hashmap_insert(&map->indices, &header.key, &index);
    } else {
        size_t index = * (size_t *) value;
        HttpHeaderMapEntry *entry = &map->entries.items[index];
        while (entry->next != NULL) {
            entry = entry->next;
        }
        entry->next = mem_alloc(map->alloc, sizeof(HttpHeaderMapEntry));
        *entry->next = (HttpHeaderMapEntry) {
            .header = header,
            .next = NULL
        };
    }
}

void http_headermap_insert_cstrs(HttpHeaderMap *map, const char *key, const char *value) {
    HttpHeader header = {
        .key = cstr_to_sv(key),
        .value = cstr_to_sv(value),
    };

    http_headermap_insert(map, header);
}

