#ifndef ALLOC_H_
#define ALLOC_H_

#include <stddef.h>
#include <stdint.h>

/********************************** Memory ************************************/

typedef struct Allocator Allocator;

void *mem_alloc(Allocator allocator, size_t bytes);
void mem_free(Allocator allocator, void *ptr);
void *mem_calloc(Allocator allocator, size_t count, size_t size);
void *mem_realloc(Allocator allocator, void *ptr, size_t bytes);
char *mem_sprintf(Allocator allocator, const char *format, ...)
    __attribute__((format(printf, 2, 3)));
void *mem_memdup(Allocator allocator, const void *mem, size_t size);
char *mem_strdup(Allocator allocator, const char *s);

/********************************* Allocator **********************************/

typedef void *allocator_alloc_t(void *data, size_t bytes);
typedef void allocator_free_t(void *data, void *ptr);
typedef void *allocator_calloc_t(void *data, size_t count, size_t size);
typedef void *allocator_realloc_t(void *data, void *ptr, size_t bytes);

typedef struct {
    allocator_alloc_t *const alloc;
    allocator_free_t *const free;
    allocator_calloc_t *const calloc;
    allocator_realloc_t *const realloc;
} AllocatorFunctionTable;

struct Allocator {
    void *data;
    const AllocatorFunctionTable *ftable;
};

/*********************************** Malloc ***********************************/

allocator_alloc_t malloc_alloc;
allocator_free_t malloc_free;
allocator_calloc_t malloc_calloc;
allocator_realloc_t malloc_realloc;
Allocator new_malloc_allocator();

static const AllocatorFunctionTable malloc_table = {
    malloc_alloc,
    malloc_free,
    malloc_calloc,
    malloc_realloc,
};

/*********************************** Arena ************************************/

#define REGION_DEFAULT_CAPACITY (8 * 1024)

typedef struct Region Region;

struct Region {
    Region *next;
    size_t count;
    size_t capacity;
    uintptr_t data[];
};

typedef struct {
    Region *begin;
    Region *end;
} Arena;

allocator_alloc_t arena_alloc;
allocator_free_t arena_free;
allocator_calloc_t arena_calloc;
allocator_realloc_t arena_realloc;
Allocator new_arena_allocator(Arena *arena);

static const AllocatorFunctionTable arena_table = {
    arena_alloc,
    arena_free,
    arena_calloc,
    arena_realloc,
};

void arena_free_arena(Arena *arena);
Region *new_region(size_t capacity);
void free_region(Region *region);

#endif // ALLOC_H_
