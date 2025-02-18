#include "mew/alloc.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "mew/log.h"

/********************************** Memory ************************************/

void *mem_alloc(Allocator allocator, size_t bytes) {
    return allocator.ftable->alloc(allocator.data, bytes);
}

void mem_free(Allocator allocator, void *ptr) {
    allocator.ftable->free(allocator.data, ptr);
}

void *mem_calloc(Allocator allocator, size_t count, size_t size) {
    return allocator.ftable->calloc(allocator.data, count, size);
}

void *mem_realloc(Allocator allocator, void *ptr, size_t bytes) {
    return allocator.ftable->realloc(allocator.data, ptr, bytes);
}

char *mem_sprintf(Allocator allocator, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int size = vsnprintf(NULL, 0, format, args);
    va_end(args);

    assert(size >= 0);
    char *result = (char*)mem_alloc(allocator, size + 1);
    va_start(args, format);
    vsnprintf(result, size + 1, format, args);
    va_end(args);

    return result;
}

void *mem_memdup(Allocator allocator, const void *mem, size_t size) {
    void *new = mem_alloc(allocator, size);
    memcpy(new, mem, size);
    return new;
}

char *mem_strdup(Allocator allocator, const char *s) {
    size_t len = strlen(s);
    char *new = mem_alloc(allocator, len + 1);
    strncpy(new, s, len);
    new[len] = '\0';
    return new;
}

/*********************************** Malloc ***********************************/

void *malloc_alloc(void *data, size_t bytes) {
    (void) data;
    return malloc(bytes);
}

void malloc_free(void *data, void *ptr) {
    (void) data;
    free(ptr);
}

void *malloc_calloc(void *data, size_t count, size_t size) {
    (void) data;
    return calloc(count, size);
}

void *malloc_realloc(void *data, void *ptr, size_t bytes) {
    (void) data;
    return realloc(ptr, bytes);
}

Allocator new_malloc_allocator() {
    return (Allocator) {
        .data = NULL,
        .ftable = &malloc_table,
    };
}

/*********************************** Arena ************************************/

void *arena_alloc(void *data, size_t bytes) {
    Arena *arena = (Arena *) data;
    size_t size = (bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

    if (arena->begin == NULL) {
        assert(arena->end == NULL);
        size_t capacity = REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        arena->begin = new_region(capacity);
        arena->end = arena->begin;
    }

    while (arena->end->count + size > arena->end->capacity && arena->end->next != NULL) {
        arena->end = arena->end->next;
    }

     if (arena->end->count + size > arena->end->capacity) {
        assert(arena->end->next == NULL);
        size_t capacity = REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        arena->end->next = new_region(capacity);
        arena->end = arena->end->next;
    }

    void *result = &arena->end->data[arena->end->count];
    arena->end->count += size;
    return result;
}

void arena_free(void *data, void *ptr) {
    (void) data;
    (void) ptr;
}

void *arena_calloc(void* data, size_t count, size_t size) {
    void *new = arena_alloc(data, count * size);
    if (new) bzero(new, count * size);
    return new;
}

void *arena_realloc(void *data, void *ptr, size_t bytes) {
    (void) data;
    (void) ptr;
    (void) bytes;
    assert(0 && "Not implemented");
}

Allocator new_arena_allocator(Arena *arena) {
    return (Allocator) {
        .data = arena,
        .ftable = &arena_table,
    };
}

void arena_free_arena(Arena *arena) {
    Region *region = arena->begin;
    while (region != NULL) {
        Region *next = region->next;
        free_region(region);
        region = next;
    }

    arena->begin = NULL;
    arena->end = NULL;
}

Region *new_region(size_t capacity) {
    size_t size = sizeof(Region) + sizeof(uintptr_t) * capacity;
    Region *region = malloc(size);

    region->next = NULL;
    region->count = 0;
    region->capacity = capacity;

    return region;
}

void free_region(Region *region) {
    free(region);
}
