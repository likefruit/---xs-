#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_MALLOC_H__
#define __XS_MALLOC_H__


extern void* __xs_malloc(int size, const char* file, int line);
extern void* __xs_zalloc(int size, const char* file, int line);
extern char* __xs_strdup(const char* ss, const char* file, int line);
extern void __xs_free(void* ptr);
#if 0
extern void* __xs_realloc(void* ptr, int size, const char* file, int line);
#endif

#define xs_malloc(size) __xs_malloc(size, __FILE__, __LINE__)
#define xs_zalloc(size) __xs_zalloc(size, __FILE__, __LINE__)
#define xs_strdup(ss) __xs_strdup(ss, __FILE__, __LINE__)
//#define xs_realloc(ptr, _size) __xs_realloc(ptr, _size, __FILE__, __LINE__)
#define xs_free __xs_free

#endif
#ifdef __cplusplus
}
#endif
