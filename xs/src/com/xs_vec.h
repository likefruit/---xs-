#ifdef __cplusplus
extern "C"{
#endif
#ifndef XS_VECTOR_H
#define XS_VECTOR_H

typedef struct xs_vec_t
{
    void** vs;
    int size;       /* total size */
    int count;      /* count now */
    int step;       /* when size not enough, make more */
} xs_vec_t;

#ifndef xs_malloc
#define xs_malloc malloc
#endif

#ifndef xs_free
#define xs_free free
#endif

#ifndef xs_realloc
#define xs_realloc realloc
#endif

static inline xs_vec_t* xs_vec_create(int step)
{
    xs_vec_t* vec = (xs_vec_t*)xs_malloc(sizeof(*vec));
    vec->count = 0;
    vec->size = step;
    vec->step = step;
    vec->vs = (void**)xs_malloc(sizeof(void*)*step);
    return vec;
}

static inline void xs_vec_destroy(xs_vec_t* vec, void(*free_node)(void*))
{
    int i;

    if(vec == NULL)
        return;
    if(free_node)
    {
        for(i=0; i<vec->count; ++i)
        {
            free_node(vec->vs[i]);
        }
    }
    xs_free(vec->vs);
    xs_free(vec);
}

static inline int xs_vec_add(xs_vec_t* vec, void* data)
{
    if(vec == NULL)
    {
        xs_logd("error vec is null");
        return -1;
    }
    if(vec->size == vec->count)
    {
        int newsize = vec->size + vec->step;
        void** p = (void**)xs_malloc(newsize * sizeof(void*));
        int i;
        for(i=0; i<vec->size; i++)
        {
            p[i] = vec->vs[i];
        }
        xs_free(vec->vs);

        vec->size = newsize;
        vec->vs = p;
    }
    vec->vs[vec->count++] = data;
    return vec->count - 1;
#if 0
    if(vec->size == vec->count)
    {
        vec->vs = (void**)xs_realloc(vec->vs, (vec->size+vec->step)+sizeof(void*));
        vec->size += vec->step;
    }
    vec->vs[vec->count++] = data;

    xs_mempool_check_magic(vec->vs);
    
    return vec->count-1;
#endif
}

static inline void* xs_vec_rm(xs_vec_t* vec, int i)
{
    /* 0, 1, 2, 3, 4*/
    void* data = vec->vs[i];
    if(i >= vec->count)
        return NULL;

    for(;i<vec->count-1; ++i)
    {
        vec->vs[i] = vec->vs[i+1];
    }

    vec->count --;
    return data;
}

#if 0
static inline void xs_vec_ins(xs_vec_t* vec, int i, void* data)
{
    if(vec->size == vec->count)
    {
        vec->vs = (void**)xs_realloc(vec->vs, (vec->size+vec->step)+sizeof(void*));
        vec->size += vec->step;
    }

    int j;
    for(j=vec->count-1; j > i; j++)
    {
        vec->vs[j] = vec->vs[j-1];
    }
    vec->vs[i] = data;
    ++vec->count;
}
#endif

static inline void  xs_vec_sort(xs_vec_t* vec, int(*cmp_func)(const char*, const char*))
{
#if 0
    void xs_sort(void *base, int num, int size,
                 int (*cmp_func)(const void *, const void *),
                 void (*swap_func)(void *, void *, int size));
#endif
    xs_sort((char*)vec->vs, vec->count, sizeof(void*), cmp_func, NULL);
}

static inline void* xs_vec_get(xs_vec_t* vec, int idx)
{
    if(idx >= vec->count)
        return NULL;
    return vec->vs[idx];
}

#endif // XS_VECTOR_H
#ifdef __cplusplus
}
#endif
