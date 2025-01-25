#ifndef HTTP_HEADERMAP_H_
#define HTTP_HEADERMAP_H_

#include "rbtree.h"
#include "utils.h"

typedef struct {
    StringView key;
    StringView value;
} HttpHeader;

// TODO: multimap
typedef struct {
    RBTree tree;
} HttpHeaderMap;

void http_headermap_init(HttpHeaderMap *map, void *user_data, Arena *arena);
void http_headermap_insert(HttpHeaderMap *map, HttpHeader header);
void http_headermap_insert_cstrs(HttpHeaderMap *map, const char *key, const char *value);

#endif // HTTP_HEADERMAP_H_
