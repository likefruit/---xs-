/*
 * Copyright 2012-2015 Xueguoliang(xueguoliang@gmail.com)
 * */
#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_MEMPOOL_H__
#define __XS_MEMPOOL_H__

#define XS_MEM_HEAD 8
#define XS_MEM_TAIL 4
#define XS_MEMPOOL_MIN_BLOCK_SIZE 32
#define XS_MEM_EXTRA (XS_MEM_HEAD+XS_MEM_TAIL)

#define XS_USE_MEMPOOL

extern pthread_key_t g_thread_key;

typedef struct xs_mempool_block_t
{
#define XS_MEMPOOL_HOT  1
#define XS_MEMPOOL_COOL 0
    uint16_t        magic;
    uint16_t        file;    /* file index */
    uint16_t        line;  /* who malloc this memory, index of the thread */
    uint16_t        thid:12;    /* line */
    uint16_t        index:4;    /* 0000--1111, if 1111, just free */
    
    struct xs_mempool_block_t* next;
} xs_mempool_block_t;

typedef struct xs_mempool_trunk_t
{
    struct xs_mempool_trunk_t* next;
    int             size; /* size is 64k 128k 256k 512k or 1M */
    char            buf[];
} xs_mempool_trunk_t;

typedef struct xs_mempool_t
{
#ifdef XS_USE_MEMPOOL
    xs_mempool_trunk_t* trunks;
    xs_mempool_block_t** blocks;
    int             min_block_index;
    int             max_block_index;
    int             min_block_size;
    int             max_block_size;
    xs_list_t       extra; /* some malloc > max_block_size */
    uint32_t        use_buff;
#endif
    /* the thread of the mempool */
    int             rtid;
    void*           rtdata;
} xs_mempool_t;
/* general by Makefile */
xs_mempool_t* xs_mempool_create(void* rtdata, int rtid, int max_alloc_size);
extern xs_mempool_t* g_mp;

#ifdef XS_USE_MEMPOOL
extern char* g_mem_files[];
extern int g_mem_file_count;
void xs_mempool_check_magic(void* ptr);

void xs_mempool_destroy(xs_mempool_t* mem_pool);

void* xs_mempool_alloc(xs_mempool_t* pool, int size, const char* file, int line, uint16_t thread_id);
void xs_mempool_free(xs_mempool_t* pool, void* ptr);
void xs_mempool_check_mem_leak(xs_mempool_t* mem_pool);

static inline uint32_t xs_mempool_get_block_size(xs_mempool_block_t* block)
{
    return XS_MEMPOOL_MIN_BLOCK_SIZE << block->index;
}

static inline xs_mempool_block_t* xs_mempool_get_block(void* ptr)
{
    return (xs_mempool_block_t*)((char*)ptr - XS_MEM_HEAD);
}

#define xs_mem_dump(ptr) do\
{\
    xs_mempool_block_t* block = (xs_mempool_block_t*)((char*)ptr - XS_MEM_HEAD);\
    xs_loge("file=%s, line=%d, block->index=%d", g_mem_files[block->file], block->line, block->index);\
}while(0)
#endif

#endif
#ifdef __cplusplus
}
#endif
