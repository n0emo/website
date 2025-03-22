#ifndef MEW_INCLUDE_MEW_CONTAINERS_RBTREE_H_
#define MEW_INCLUDE_MEW_CONTAINERS_RBTREE_H_

#include <stddef.h>
#include <stdbool.h>

#include "mew/alloc.h"

typedef struct RBTree RBTree;
typedef struct RBTreeNode RBTreeNode;

typedef enum RBTreeColor {
    RBTREE_RED   = 0,
    RBTREE_BLACK = 1,
} RBTreeColor;

typedef int btree_cmp_t(const void *a, const void *b, void *user_data);
typedef void btree_iter_f_t(const void *element, void *user_data);

struct RBTree {
    Allocator alloc;

    size_t element_size;
    btree_cmp_t *cmp;
    void *user_data;

    bool oom;

    RBTreeNode *root;
};

struct RBTreeNode {
    RBTreeNode *parent;
    RBTreeNode *left;
    RBTreeNode *right;
    RBTreeColor color;
    char data[];
};

RBTree rbtree_new(size_t element_size, btree_cmp_t *cmp, void *user_data);
void *rbtree_get(RBTree *tree, const void *element);
RBTreeNode *rbtree_insert(RBTree *tree, const void *element);
void *rbtree_remove(RBTree *tree, const void *element);
void rbtree_iterate_ascending(RBTree *tree, btree_iter_f_t *iter_f);

#endif // MEW_INCLUDE_MEW_CONTAINERS_RBTREE_H_
