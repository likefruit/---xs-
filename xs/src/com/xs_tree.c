#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

void xs_tree_insert(xs_tree_t* tree, xs_tree_node_t* parent, xs_tree_node_t* node, const void* key)
{
    node->parent = parent;
    if(parent == NULL)
    {
        tree->root = node;
    }
    else  
    {
        rb_insert(&node->node, &parent->child, key);
    }
}

void xs_tree_remove(xs_tree_t* tree, xs_tree_node_t* node)
{
    if(node->parent == NULL)
        tree->root = NULL;
    else
        rb_erase(&node->node, &node->parent->child);
}

xs_tree_node_t* xs_tree_find(xs_tree_node_t* parent,
        void* key)
{
    struct rb_node* node;
    node = rb_find(&parent->child, key);
    if(node)
        return xs_entry(node, xs_tree_node_t, node);
    return NULL;
}
#ifdef __cplusplus
}
#endif
