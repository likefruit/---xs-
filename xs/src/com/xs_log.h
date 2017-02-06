#ifdef __cplusplus
extern "C"{
#endif
#ifndef __LOG_H__
#define __LOG_H__

#ifdef WIN32
#define USE_ZLOG 0
#else
#define USE_ZLOG 0
#endif

#if USE_ZLOG
#include "zlog.h"
int xs_loginit();
extern zlog_category_t* __log__global__;

#define xs_logfini zlog_fini
#define log_debug(...) zlog_debug(__log__global__, ##__VA_ARGS__)
#define log_info(...) zlog_info(__log__global__, ##__VA_ARGS__)
#define log_notice(...) zlog_notice(__log__global__, ##__VA_ARGS__)
#define log_warn(...) zlog_warn(__log__global__, ##__VA_ARGS__)
#define log_error(...) zlog_error(__log__global__, ##__VA_ARGS__)
#define log_fatal(...) zlog_fatal(__log__global__, ##__VA_ARGS__)

#define xs_log log_error

#else
#include <pthread.h>
static inline int xs_loginit(){return 0;}
static inline void xs_logfini(){}

#ifndef XS_LOG_TO_FILE
#ifdef XS_USE_MEMPOOL
#define xs_printf(fmt, ...) do{xs_mempool_t* mp = (xs_mempool_t*)pthread_getspecific(g_thread_key);printf("thread=%d, file=%s, line=%d  "fmt"\n", (int)mp->rtid, __FILE__, __LINE__,  ##__VA_ARGS__);}while(0)
#else
#define xs_printf(fmt, ...) do{\
    printf("file=%s, line=%d  "fmt"\n", __FILE__, __LINE__,  ##__VA_ARGS__);}while(0)
#endif
#else
#define xs_printf(fmt, ...) do {\
    FILE* _logfile = fopen("xs.log", "a+");\
    if(_logfile == NULL) break;\
    char* _logbuffer = xs_malloc(8000);\
    fprintf(_logfile, "file=%s, line=%d  "fmt"\n", __FILE__, __LINE__,  ##__VA_ARGS__);\
    fclose(_logfile);\
    xs_free(_logbuffer);\
} while(0)
#endif

#define log_debug xs_printf
#define log_info xs_printf
#define log_error xs_printf
#define log_warn xs_printf
#define log_notice xs_printf
#define log_fatal xs_printf

#endif

#define xs_logd log_debug
#define xs_logi log_info
#define xs_logn log_notice
#define xs_logw log_warn
#define xs_loge log_error
#define xs_logf log_fatal

#endif
#ifdef __cplusplus
}
#endif
