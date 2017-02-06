#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_DEF_H__
#define __XS_DEF_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

#include <regex.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
//#include <dirent.h>

#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif

#ifndef container_of
#ifdef WIN32
#define container_of(ptr, type, member)                \
            (type *)((char *)ptr - offsetof(type, member))
#else
#define container_of(ptr, type, member) ({            \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#endif
#endif
#define xs_entry(ptr, type, member) \
    container_of(ptr, type, member)





#endif
#ifdef __cplusplus
}
#endif
