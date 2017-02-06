#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

void __xs_aio_recv_data(xs_ev_sock_t* sev)
{
    xs_aio_t* aio = sev->arg;
    int ret;

    xs_logd("recv data sev->fd=%d, aio->iolen=%d", sev->fd, aio->iolen);

    /* recv head */
    if(aio->iolen < 4)
    {
        char* p = (char*)&aio->buflen;
        ret = read(sev->fd, p+aio->iolen, 4-aio->iolen);

        if(ret > 0)
        {
            aio->iolen += ret;
            if(aio->iolen < 4)
            {
                xs_ev_add2(sev->ev);
                return;
            }
            else if(aio->iolen == 4)
            {
                aio->buflen = ntohl(aio->buflen);
                if(aio->buflen > XS_AIO_MAX_BUFFER)
                {

                    aio->buf = NULL;
                    aio->result = XS_AIO_FAIL;
                    aio->func(aio);
                    return;
                }
                xs_logd("recv head buffer length = %d", aio->buflen);
                aio->buf = xs_malloc(aio->buflen + 1);
                aio->buf[aio->buflen] = 0;
            }
        }
        else if (ret < 0 && errno == EAGAIN)
        {
            xs_logd("add to ev poll");
            xs_ev_add2(sev->ev);
            return ;
        }
        else
        {
            xs_logd("here ret=%d", ret);
            aio->ret = ret;
            aio->func(aio);
            return;
        }
    }

    ret = read(sev->fd, aio->buf + (aio->iolen - 4),
            aio->buflen - (aio->iolen - 4));
    if(ret > 0)
    {
        aio->iolen += ret;
        if(aio->iolen == aio->buflen + 4)
        {
            aio->result = XS_AIO_DONE;
            aio->ret = ret;
            xs_logd("************************callback ok fd=%d", aio->fd);
            aio->func(aio);
        }
        else
        {
            xs_logd("here");
            xs_ev_add2(sev->ev);
        }
    }
    else if(ret < 0 && errno == EAGAIN)
    {
        xs_logd("here");
        xs_ev_add2(sev->ev);
    }
    else
    {
        xs_logd("here ret=%d", ret);
        aio->ret = ret;
        aio->func(aio);
    }
}

void xs_aio_recv(int fd, void(*func)(xs_aio_t*), void* ptr)
{
    xs_aio_t* aio = xs_zalloc(sizeof(*aio));
    aio->fd = fd;
    aio->buf = NULL;
    aio->buflen = 0;
    aio->iolen = 0;
    aio->func = func;
    aio->result = XS_AIO_FAIL;
    aio->ptr = ptr;
    
    aio->sev = xs_ev_add_sock_ev(fd, EPOLLIN, __xs_aio_recv_data, aio);
}

void __xs_aio_send_data(xs_ev_sock_t* sev)
{
    xs_aio_t* aio = sev->arg;

    int ret;

    ret = write(aio->fd, 
            aio->buf + aio->iolen,
            aio->buflen - aio->iolen);
    if (ret > 0)
    {
        aio->iolen += ret;
        if(aio->iolen == aio->buflen)
        {
            aio->result = XS_AIO_DONE;
            aio->ret = ret;
            aio->func(aio);
        }
    }
    else if(ret < 0 && errno == EAGAIN)
    {
        xs_ev_add2(sev->ev);
    }
    else /* <0 and ==0 */
    {
        xs_loge("socket send data error, ret=%d, errno=%d, fd=%d", ret, errno, aio->fd);
        aio->ret = ret;
        aio->func(aio);
    }
}

void xs_aio_send(int fd, void(*func)(xs_aio_t*), char* buf, int len, void* ptr)
{
    xs_aio_t* aio = xs_zalloc(sizeof(*aio));

    aio->fd = fd;
    aio->buf = buf;
    aio->buflen = len;
    aio->iolen = 0;
    aio->func = func;
    aio->result = XS_AIO_FAIL;
    aio->ptr = ptr;

    aio->sev = xs_ev_add_sock_ev(fd, EPOLLOUT, __xs_aio_send_data, aio);
}

void _xs_aio_wait_close(xs_aio_t* aio)
{
    if(aio->result == XS_AIO_DONE)
    {
        xs_sock_wait_peer_close(aio->fd);
    }
    else
    {
        xs_close_socket(aio->fd);
    }
    xs_aio_free(aio);
}

#ifdef __cplusplus
}
#endif
