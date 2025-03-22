#ifndef MEW_INCLUDE_MEW_UTILS_H_
#define MEW_INCLUDE_MEW_UTILS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "alloc.h" // IWYU pragma: export

/********* Error handling **********/

#define return_defer(ret) do { \
    result = ret; \
    goto defer; \
} while(0) \

#define try(...) if (!(__VA_ARGS__)) return_defer(false)

/********* Dynamic array **********/

#define INITIAL_CAP 128

#define ARRAY_APPEND(array, item, alloc) do { \
    if ((array)->items == NULL) { \
        (array)->items = (typeof((array)->items)) mem_alloc(alloc, INITIAL_CAP * sizeof((array)->items[0])); \
        (array)->count = 0; \
        (array)->capacity = INITIAL_CAP; \
    } else if ((array)->count >= (array)->capacity) { \
        size_t new_capacity = (array)->capacity * 2; \
        typeof((array)->items[0]) *items = mem_alloc(alloc, new_capacity * sizeof((array)->items[0])); \
        memcpy(items, (array)->items, (array)->capacity); \
        (array)->items = items; \
        (array)->capacity = new_capacity; \
    } \
    (array)->items[(array)->count] = (item); \
    (array)->count++; \
} while(0) \

/************ Other **************/

void wait_for_ctrl_c(void);

#endif // MEW_INCLUDE_MEW_UTILS_H_
