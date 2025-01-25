#include "rbtree.h"

#include <assert.h>
#include <string.h>
#include <strings.h>

RBTreeNode *rbtree_new_node(RBTree *tree, const void *element) {
    size_t size = sizeof(RBTreeNode) + tree->element_size;
    RBTreeNode *ptr = tree->alloc.ftable->alloc(tree->alloc.data, size);

    if (ptr == NULL) {
        tree->oom = true;
    }

    bzero(ptr, sizeof(RBTreeNode));
    memcpy(ptr->data, element, tree->element_size);

    return ptr;
}

RBTree rbtree_new(size_t element_size, btree_cmp_t *cmp, void *user_data) {
    return (RBTree) {
        .alloc = new_malloc_allocator(),
        .element_size = element_size,
        .cmp = cmp,
        .user_data = user_data,
        .oom = false,
        .root = NULL,
    };
}

void *rbtree_get(RBTree *tree, const void *element) {
    RBTreeNode *iter = tree->root;

    while (true) {
        if (iter == NULL) {
            return NULL;
        }

        int cmp = tree->cmp(iter->data, element, tree->user_data);
        if (cmp == 0) {
            return iter->data;
        } else if (cmp < 0) {
            iter = iter->left;
        } else if (cmp > 0) {
            iter = iter->right;
        }
    }
}

RBTreeNode *rbtree_bst_insert(RBTree *tree, RBTreeNode *node) {
    RBTreeNode *iter = tree->root;

    while (true) {
        int cmp = tree->cmp(iter->data, node->data, tree->user_data);
        if (cmp == 0) {
            RBTreeNode *ret;

            if (iter == tree->root) {
                ret = tree->root;
                tree->root = node;
            } else {
                ret = iter;
                node->parent = iter->parent;
                if (node->parent->left == iter) {
                    node->parent->left = node;
                } else {
                    node->parent->right = node;
                }
            }

            return ret;

        } else if (cmp < 0) {
            if (iter->left == NULL) {
                iter->left = node;
                node->parent = iter;
                return NULL;
            }

            iter = iter->left;
            continue;

        } else if (cmp > 0) {
            if (iter->right == NULL) {
                iter->right = node;
                node->parent = iter;
                return NULL;
            }

            iter = iter->right;
            continue;
        }
    }
}

RBTreeNode *rbtree_insert(RBTree *tree, const void *element) {
    RBTreeNode *new_node = rbtree_new_node(tree, element);

    if (tree->root == NULL) {
        tree->root = new_node;
        return NULL;
    }

    return rbtree_bst_insert(tree, new_node);

    // TODO: fixup
}

void *rbtree_remove(RBTree *tree, const void *element) {
    (void) tree;
    (void) element;
    assert(0 && "btree_remove is not implemented");
}

void rbtree_iterate_ascending_impl(RBTree *tree, RBTreeNode *node, btree_iter_f_t *iter_f) {
    if (node == NULL) {
        return;
    }

    rbtree_iterate_ascending_impl(tree, node->left, iter_f);
    iter_f(node->data, tree->user_data);
    rbtree_iterate_ascending_impl(tree, node->right, iter_f);
}

void rbtree_iterate_ascending(RBTree *tree, btree_iter_f_t *iter_f) {
    rbtree_iterate_ascending_impl(tree, tree->root, iter_f);
}
