#ifndef MEW_INCLUDE_MEW_HTTP_HEADERMAP_H_
#define MEW_INCLUDE_MEW_HTTP_HEADERMAP_H_

#include "mew/containers/hashmap.h"
#include "mew/str.h"

typedef struct HttpHeader {
    StringView key;
    StringView value;
} HttpHeader;

typedef struct HttpHeaderMapEntry {
    HttpHeader header;
    struct HttpHeaderMapEntry *next;
} HttpHeaderMapEntry;

typedef struct HttpHeaderMapEntries {
    HttpHeaderMapEntry *items;
    size_t count;
    size_t capacity;
} HttpHeaderMapEntries;

// TODO: multimap
typedef struct {
    Allocator alloc;
    HttpHeaderMapEntries entries;
    HashMap indices;
} HttpHeaderMap;

void http_headermap_init(HttpHeaderMap *map, Allocator alloc);
void http_headermap_insert(HttpHeaderMap *map, HttpHeader header);
void http_headermap_insert_cstrs(HttpHeaderMap *map, const char *key, const char *value);

#endif // MEW_INCLUDE_MEW_HTTP_HEADERMAP_H_
