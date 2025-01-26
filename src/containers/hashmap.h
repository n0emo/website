#ifndef CONTAINERS_HASHMAP_H_
#define CONTAINERS_HASHMAP_H_

#include <stdbool.h>
#include <stddef.h>

#include "alloc.h"

typedef uint64_t hashfunc_t(const void *value, void *user_data);
typedef bool hashmap_equals_t(const void *a, const void *b, void *user_data);
typedef bool hashmap_iter_t(const void *key, const void *value, void *user_data);

typedef struct HashMapBucket {
    size_t map_index;
    bool initialized;
    char data[];
} HashMapBucket;

typedef struct HashMap {
    Allocator alloc;
    hashfunc_t *hashfunc;
    hashmap_equals_t *equals;
    void *user_data;
    size_t key_size;
    size_t value_size;
    size_t bucket_count;
    size_t element_count;
    HashMapBucket *buckets;
} HashMap;

void hashmap_init(HashMap *map, void *user_data, hashfunc_t *hashfunc, hashmap_equals_t *equals, size_t key_size, size_t value_size);
void hashmap_insert(HashMap *map, const void *key, const void *value);
bool hashmap_pop(HashMap *map, const void *key, void **found_key, void **value);
void *hashmap_get(HashMap *map, const void *key);
bool hashmap_iterate(HashMap *map, hashmap_iter_t iter);

#endif // CONTAINERS_HASHMAP_H_
