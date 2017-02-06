#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_AIO_H__
#define __XS_AIO_H__

#define XS_AIO_MAX_BUFFER 8192

typedef struct xs_aio_t
{
    int             fd;
    char*           buf;
    int             buflen;
    int             iolen;
    void            (*func)(struct xs_aio_t*);
#define XS_AIO_DONE 1
#define XS_AIO_FAIL 0
    int             result;
    void*           ptr;
    xs_ev_sock_t*   sev;
    int             ret;
} xs_aio_t;

void xs_aio_recv(int fd, void(*func)(xs_aio_t*), void* ptr);
void xs_aio_send(int fd, void(*func)(xs_aio_t*), char* buf, int len, void* ptr);
void _xs_aio_wait_close(xs_aio_t* aio);
static inline void xs_aio_send_and_close(int fd, char* buf, int len)
{
    xs_aio_send(fd, _xs_aio_wait_close, buf, len, NULL);
}

#define xs_aio_free(__aio) do{ if(!__aio)break;\
    if(__aio->buf)xs_free(__aio->buf);\
    if(__aio->sev)xs_free(__aio->sev->ev);\
    xs_free(__aio);\
}while(0)

#endif
#ifdef __cplusplus
}
#endif
