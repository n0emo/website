#include "hashmap.h"

#include <strings.h>

#define HASHMAP_INITIAL_BUCKETS 16

size_t round_to(size_t value, size_t roundTo);
HashMapBucket *get_bucket_for_index(HashMap *map, size_t index);
HashMapBucket *get_bucket_for_key(HashMap *map, const void *key);
void alloc_buckets(HashMap *map, size_t count);
void hashmap_expand(HashMap *map);
size_t bucket_size(HashMap *map);
bool map_equals(HashMap *map, const void *a, const void *b);

void hashmap_init(HashMap *map, void *user_data, hashfunc_t *hashfunc, hashmap_equals_t *equals, size_t key_size, size_t value_size) {
    map->alloc = new_malloc_allocator();
    map->user_data = user_data;
    map->hashfunc = hashfunc;
    map->equals = equals;
    map->key_size = round_to(key_size, 8);
    map->value_size = round_to(value_size, 8);
    map->element_count = 0;
    alloc_buckets(map, HASHMAP_INITIAL_BUCKETS);
}

void hashmap_insert(HashMap *map, const void *key, const void *value) {
    map->element_count++;
    if ((float) map->element_count / map->bucket_count  >= 0.75) {
        hashmap_expand(map);
    }

    uint64_t hash = map->hashfunc((void *) key, map->user_data);
    size_t index = hash % map->bucket_count;
    size_t map_index = index;
    while (true) {
        HashMapBucket *bucket = get_bucket_for_index(map, index);
        if (!bucket->initialized) {
            // TODO: Robin-hood hashing
            bucket->map_index = map_index;
            memcpy(&bucket->data, key, map->key_size);
            memcpy((char *)&bucket->data + map->key_size, value, map->value_size);
            bucket->initialized = true;
            break;
        } else if (map_equals(map, &bucket->data, key)) {
            memcpy(bucket->data + map->key_size, value, map->value_size);
            break;
        }
        index++;
        if (index >= map->bucket_count) {
            index = 0;
        }
    }
}

void *hashmap_get(HashMap *map, const void *key) {
    HashMapBucket *bucket = get_bucket_for_key(map, key);
    if (bucket == NULL) return NULL;
    else return bucket->data + map->key_size;
}

bool hashmap_pop(HashMap *map, const void *key, void **found_key, void **value) {
    HashMapBucket *bucket = get_bucket_for_key(map, key);
    if (bucket == NULL) false;
    bucket->initialized = false;
    if (found_key != NULL) *found_key = bucket->data;
    if (value != NULL) *value = bucket->data + map->key_size;
    return true;
}

bool hashmap_iterate(HashMap *map, hashmap_iter_t iter) {
    for (size_t i = 0; i < map->bucket_count; i++) {
        HashMapBucket *bucket = &map->buckets[i];
        if (!bucket->initialized) continue;
        if (!iter(bucket->data, bucket->data + map->key_size, map->user_data)) return false;
    }
    return true;
}

size_t round_to(size_t value, size_t roundTo) {
    return (value + (roundTo - 1)) & ~(roundTo - 1);
}

HashMapBucket *get_bucket_for_index(HashMap *map, size_t index) {
    size_t size = bucket_size(map);
    return (HashMapBucket *) ((char *) map->buckets + size * index);
}

HashMapBucket *get_bucket_for_key(HashMap *map, const void *key) {
    uint64_t hash = map->hashfunc((void *) key, map->user_data);
    size_t index = hash % map->bucket_count;
    size_t map_index = index;
    while (true) {
        HashMapBucket *bucket = get_bucket_for_index(map, index);
        if (!bucket->initialized) {
            return NULL;
        }
        if (bucket->map_index == map_index && map_equals(map, bucket->data, key)) {
            return bucket;
        }
        index++;
        if (index >= map->bucket_count) {
            index = 0;
        }
    }
}

void alloc_buckets(HashMap *map, size_t count) {
    map->bucket_count = count;
    size_t size = bucket_size(map);
    map->buckets =  mem_alloc(&map->alloc, size * count);
    bzero(map->buckets, size * count);
}

void hashmap_expand(HashMap *map) {
    HashMapBucket *old_buckets = map->buckets;
    size_t old_count = map->bucket_count;
    size_t size = bucket_size(map);
    alloc_buckets(map, old_count * 2);
    map->element_count = 0;
    for (size_t i = 0; i < old_count; i++) {
        HashMapBucket *bucket = (HashMapBucket *) ((char *) old_buckets + size * i);
        hashmap_insert(map, bucket->data, bucket->data + map->key_size);
    }
    mem_free(&map->alloc, old_buckets);
}

size_t bucket_size(HashMap *map) {
    return round_to(sizeof(HashMapBucket) + map->key_size + map->value_size, 8);

}

bool map_equals(HashMap *map, const void *a, const void *b) {
    return (map->equals != NULL && map->equals(a, b, map->user_data))
        || memcmp(a, b, map->key_size) == 0;
}
