#include "mew/containers/vector.h"

#include <string.h>
#include <strings.h>
#include <stddef.h>

void mew_vec_init(MewVector *vec, Allocator alloc, size_t element_size) {
    bzero(vec, sizeof(*vec));
    vec->alloc = alloc;
    vec->element_size = element_size;
}

void mew_vec_reserve(MewVector *vec, size_t new_capacity) {
    if (new_capacity < vec->capacity) return;

    vec->capacity = new_capacity;

    if (vec->data == NULL) {
        vec->data = mem_alloc(vec->alloc, new_capacity * vec->element_size);
    } else {
        // TODO: realloc
        void *new_data = mem_alloc(vec->alloc, new_capacity * vec->element_size);
        memcpy(new_data, vec->data, vec->capacity);
        mem_free(vec->alloc, vec->data);
        vec->data = new_data;
    }
}

void mew_vec_destroy(MewVector *vec) {
    if (vec->data == NULL) return;

    mem_free(vec->alloc, vec->data);
}

void *mew_vec_get(MewVector *vec, size_t index) {
    if (index >= vec->count) return NULL;

    return vec + index;
}

void mew_vec_push(MewVector *vec, const void *element) {
    if (vec->count == vec->capacity) {
        mew_vec_reserve(vec, vec->capacity * 2);
    }

    char *ptr = vec->data;
    ptr += vec->count * vec->element_size;
    memcpy(ptr, element, vec->element_size);
    vec->count++;
}

void mew_vec_insert_at(MewVector *vec, const void *element, size_t index) {
    if (vec->count == vec->capacity) {
        mew_vec_reserve(vec, vec->capacity * 2);
    }

    if (index == vec->count) {
        mew_vec_push(vec, element);
        return;
    }

    for (ptrdiff_t i = vec->count; i >= (ptrdiff_t) index; i--) {
        char *ptr = vec->data;
        ptr += vec->element_size * i;
        memcpy(ptr, ptr - vec->element_size, vec->element_size);
    }

    char *ptr = vec->data;
    ptr += vec->element_size * index;
    memcpy(ptr, element, vec->element_size);
    vec->count++;
}

void mew_vec_delete_at(MewVector *vec, size_t index) {
    if (index >= vec->count) return;

    vec->count--;

    for (size_t i = index; i < vec->count; i++) {
        char *ptr = vec->data;
        ptr += vec->element_size * i;
        memcpy(ptr, ptr + vec->element_size, vec->element_size);
    }
}

