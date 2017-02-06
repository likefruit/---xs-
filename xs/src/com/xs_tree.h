#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_TREE_H__
#define __XS_TREE_H__


typedef struct xs_tree_node_t
{
    struct rb_root child;
    struct rb_node node;

    struct xs_tree_node_t* parent;
} xs_tree_node_t;

typedef struct xs_tree_t
{
    xs_tree_node_t*     root;
} xs_tree_t;

void xs_tree_insert(xs_tree_t* tree, xs_tree_node_t* parent, xs_tree_node_t* node, const void* key);
void xs_tree_remove(xs_tree_t* r, xs_tree_node_t* n);
xs_tree_node_t* xs_tree_find(xs_tree_node_t* parent, void* key);

#endif
#ifdef __cplusplus
}
#endif
