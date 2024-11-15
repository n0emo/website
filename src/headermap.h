#ifndef HEADERMA_H_
#define HEADERMA_H_

#include <stdio.h>

#include "alloc.h"
#include "utils.h"

typedef struct {
    ssize_t head;
    ssize_t tail;
} HeaderMapLinks;

typedef struct {
    StringBuilder key;
    StringBuilder value;
} Bucket;

typedef struct {
    Bucket *items;
    size_t count;
    size_t capacity;
    HeaderMapLinks extra_links;
} Buckets;

typedef struct {
    size_t *items;
    size_t count;
    size_t capacity;
} Indices;

typedef struct {
    size_t entry;
    size_t extra;
} ExtraLink;

typedef struct {
    StringBuilder value;
    ExtraLink prev;
    ExtraLink next;
} ExtraValue;

typedef struct {
    ExtraValue *items;
    size_t cout;
    size_t capacity;
} ExtraValues;

typedef struct {
    Allocator alloc;
    Indices indices;
    Buckets entries;
    ExtraValues extra_values;
} HeaderMap;

#endif // HEADERMA_H_

