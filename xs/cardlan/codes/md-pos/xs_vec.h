#ifdef __cplusplus
extern "C"{
#endif
#ifndef XS_VECTOR_H
#define XS_VECTOR_H


/* Added by tsihang*/
#define DEBUG
/* 动态数组 */
typedef struct xs_vec_t
{
    void** vs;      /* 保存的数据 */
    int size;       /* 空间大小 */
    int count;      /* 有用数据大小 */
    int step;       /* 步长，当空间不够的时候，按照这个参数来增大空间 */
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

#define XS_VEC_FOREACH(data, idx, vec) for(idx=0; idx<vec->count; data=vec->vs[idx++])

/* 创建动态数组 */
static inline xs_vec_t* xs_vec_create(int step)
{
    xs_vec_t* vec = (xs_vec_t*)xs_malloc(sizeof(*vec));
    vec->count = 0;
    vec->size = step;
    vec->step = step;
    vec->vs = (void**)xs_malloc(sizeof(void*)*step);
    return vec;
}

static inline void xs_vec_clear(xs_vec_t* vec, void(*free_node)(void*))
{
    int i;
    if(free_node)
    {
        for(i=0; i<vec->count; ++i)
        {
            free_node(vec->vs[i]);
        }
    }
    vec->count = 0;
}

/* 删除动态数据，free_node是用来删除元素的回调, 
 * free_node如果是空，则对元素不做任何操作
 * 这个可以用于一个元素属于多个数组的情况 */
static inline void xs_vec_destroy(xs_vec_t* vec, void(*free_node)(void*))
{
    xs_vec_clear(vec, free_node);
    xs_free(vec->vs);
    xs_free(vec);
}

/* 往数组里增加一个元素，放在最后,
 * 并返回它的index */
static inline int xs_vec_add(xs_vec_t* vec, void* data)
{
	int ret = vec->count;

	if(vec->size == vec->count)
	{
	    vec->vs = (void**)xs_realloc(vec->vs, (vec->size+vec->step)+sizeof(void*));
	    vec->size += vec->step;
	}
	vec->vs[vec->count++] = data;
	return ret;
}

/* 删除位置为i的元素 */
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

    return data;
}

/* 往数组i的位置插入一个，这个最好别用，效率狂底 */
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

/* 对数组进行排序, cmp_func是元素比较的回调 */
static inline void  xs_vec_sort(xs_vec_t* vec, int(*cmp_func)(const void*, const void*))
{
    xs_sort(vec->vs, vec->count, sizeof(void*), cmp_func, NULL);
}

static inline void* xs_vec_get(xs_vec_t* vec, int idx)
{
    if(idx >= vec->count)
        return NULL;
    return vec->vs[idx];
}

static inline int xs_vec_set(xs_vec_t* vec, int idx, void* data, void** old)
{
    if(idx >= vec->count)
        return -1;

    if(old)
        *old = vec->vs[idx];
    vec->vs[idx] = data;

    return 0;
}


#endif // XS_VECTOR_H
#ifdef __cplusplus
}
#endif
