#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

pthread_key_t g_thread_key;
xs_mempool_t* g_mp;

void xs_init()
{
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup( MAKEWORD( 2, 1 ), &wsaData );
#endif
    pthread_key_create(&g_thread_key, NULL);
    xs_loginit();
    g_process.mp = g_mp = xs_mempool_create(NULL, 0, 16000); 
    pthread_setspecific(g_thread_key, g_mp);
}

void xs_fini()
{
#ifdef XS_USE_MEMPOOL
    xs_mempool_destroy(g_mp);
#endif
    xs_logfini();
    pthread_key_delete(g_thread_key);
#ifdef WIN32
    WSACleanup();
#endif
}
#ifdef __cplusplus
}
#endif
