#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

#if USE_ZLOG
zlog_category_t* __log__global__ = NULL;

int xs_loginit()
{
    int rc;
    zlog_category_t* c;

    rc = zlog_init("zlog.conf");
    if(rc)
    {
        printf("zlog init failed\n");
        return -1;
    }

    c = zlog_get_category("my_cat");
    if(!c)
    {
        printf("get cat failed\n");
        zlog_fini();
        return -2;
    }

    __log__global__ = c;

    return 0;
}
#endif
#ifdef __cplusplus
}
#endif
