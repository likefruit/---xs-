#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

void xs_ctrl_accept(xs_ev_sock_t* ev)
{
    int fd;
    xs_ctrl_t* ctrl = ev->arg;

    while((fd = accept(ev->fd, NULL, NULL)) > 0)
    {
        xs_logi("accept");
        ctrl->r(fd, ctrl);
    }

    if(errno == EAGAIN)
    {
        xs_ev_add2(ev->ev);
    }
    else
    {
        xs_loge("server error");
        xs_ctrl_destroy(ev->arg);
    }
}

xs_ctrl_t* xs_ctrl_create(uint16_t port, int timeout, 
        xs_ctrl_recv_t r, xs_ctrl_handle_t h)
{
    xs_ctrl_t* ctrl = xs_malloc(sizeof(*ctrl));

    ctrl->objs = xs_hash_create();
    ctrl->port = port;
    ctrl->timeout = timeout;
    ctrl->server = NULL;
    if(r == NULL)
        r = xs_ctrl_recv_df;
    if(h == NULL)
        h = xs_ctrl_handle_df;
    ctrl->r = r;
    ctrl->h = h;
    
    if(ctrl->port > 0)
        ctrl->server = xs_server_start_tcp(port, 10, xs_ctrl_accept, ctrl);

    return ctrl;
}

void xs_ctrl_recv_df_cbk(xs_aio_t* aio)
{
    xs_ctrl_t* ctrl = aio->ptr;
    if(aio->result == XS_AIO_DONE)
    {
        xs_logi("buf=%s", aio->buf);
        ctrl->h(aio->fd, aio->buf, aio->buflen, ctrl);
    }
    else
    {
        xs_logd("a ,close socket ******************************* no");
        xs_close_socket(aio->fd);
    }
    xs_aio_free(aio);
}

void xs_ctrl_recv_df(int fd, xs_ctrl_t* ctrl)
{
    xs_logi("recv now");
    xs_aio_recv(fd, xs_ctrl_recv_df_cbk, ctrl);
}

void xs_ctrl_handle_df(int fd, char* buf, int size, xs_ctrl_t* ctrl)
{
    xs_model_t* model;
    xs_logd("recv buf=%s", buf);
    model = xs_model_from_buf(buf, size); /* 0`clis`1`init`argc`2 */ 
    xs_object_t* obj = xs_hash_find(ctrl->objs, model->argv[0]);
    
    if(obj)
    {
        xs_object_handler_t handle = xs_hash_find(obj, model->argv[1]);
        if(handle)
        {
            handle(model, fd, ctrl);
        }
        else
        {
            xs_loge("can not find operation %s", model->argv[1]);
            xs_close_socket(fd);
        }
    }
    else
    {
        xs_loge("can not find object %s", model->argv[0]);
        xs_close_socket(fd);
    }
    xs_model_delete(model);
}

void xs_ctrl_object_clear(xs_hash_node_t* node)
{
    xs_hash_destroy(node->value, NULL);
}

void xs_ctrl_destroy(xs_ctrl_t* ctrl)
{
    if(ctrl->objs)
        xs_hash_destroy(ctrl->objs, xs_ctrl_object_clear);
    if(ctrl->server)
        xs_sock_close(ctrl->server);

    xs_free(ctrl);
}

int xs_ctrl_send_block_ip(xs_ctrl_t* ctrl, char* ip, char* buf, int size)
{
    int fd;
    int ret;

    fd = xs_sock_connect(ctrl->port, ip);
    if(fd < 0)
    {
        return fd; 
    }

    ret = xs_ctrl_send_block(ctrl, fd, buf, size);
    if(ret != fd)
        xs_close_socket(fd);
    return ret;
}

#ifdef __cplusplus
}
#endif
