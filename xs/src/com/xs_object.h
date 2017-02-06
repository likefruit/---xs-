#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_OBJECT_H__
#define __XS_OBJECT_H__

struct xs_ctrl_t;
typedef xs_hash_t xs_object_t;

typedef void (*xs_object_handler_t)(void* model, int fd, struct xs_ctrl_t* ctrl);

static inline xs_object_t* xs_object_create()
{
    xs_hash_t* hash = xs_hash_create();
    return hash;
}

static inline void xs_object_delete(xs_object_t* obj)
{
    xs_hash_destroy(obj, NULL);
}

static inline int xs_object_add_op(xs_object_t* obj, char* name, xs_object_handler_t op)
{
    return xs_hash_add(obj, name, (void*)op);
}

#endif
#ifdef __cplusplus
}
#endif
