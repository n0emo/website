#ifndef MEW_INCLUDE_MEW_CONTAINERS_VECTOR_H_
#define MEW_INCLUDE_MEW_CONTAINERS_VECTOR_H_

// TODO: testing
// TODO: define_for_type

#include <stdbool.h>
#include <stddef.h>

#include "mew/alloc.h"

typedef struct MewVector {
    Allocator alloc;
    size_t capacity;
    size_t count;
    size_t element_size;
    void *data;
} MewVector;

void mew_vec_init(MewVector *vec, Allocator alloc, size_t element_size);

void mew_vec_destroy(MewVector *vec);

void mew_vec_reserve(MewVector *vec, size_t new_capacity);

void *mew_vec_get(MewVector *vec, size_t index);

void mew_vec_push(MewVector *vec, const void *element);

void mew_vec_insert_at(MewVector *vec, const void *element, size_t index);

void mew_vec_delete_at(MewVector *vec, size_t index);

#endif // MEW_INCLUDE_MEW_CONTAINERS_VECTOR_H_
