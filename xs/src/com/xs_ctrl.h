#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_CTRL_H__
#define __XS_CTRL_H__

typedef void (*xs_ctrl_recv_t)(int, struct xs_ctrl_t*);
typedef void (*xs_ctrl_handle_t)(int, char*, int, struct xs_ctrl_t*);

typedef struct xs_ctrl_t
{
    xs_hash_t*          objs;
    xs_ev_sock_t*       server;
    xs_ctrl_recv_t      r;
    xs_ctrl_handle_t    h;
    uint16_t            port;
    int                 timeout;
} xs_ctrl_t;

xs_ctrl_t* xs_ctrl_create(uint16_t port, int timeout, xs_ctrl_recv_t r, 
        xs_ctrl_handle_t h);
void xs_ctrl_destroy(xs_ctrl_t* ctrl);

int xs_ctrl_send_block_ip(xs_ctrl_t* ctrl, char* ip, char* buf, int size);
static inline int xs_ctrl_send_block(xs_ctrl_t* ctrl, int fd, char* buf, int size)
{
    int ret;
    ret = xs_sock_send_block(fd, buf, size, ctrl->timeout);
    if(ret == size)
        return fd;
    return -1;
}

static inline char* xs_ctrl_recv_block(xs_ctrl_t* ctrl, int fd, int* size)
{
    int head;
    int ret;
    char* buf;
    ret = xs_sock_recv_block(fd, (char*)&head, sizeof(head), ctrl->timeout);    
    if(ret != sizeof(head))
        return NULL;
    head = ntohl(head);

    buf = (char*)xs_malloc(head+1);
    buf[head] = 0;
    ret = xs_sock_recv_block(fd, buf, head, ctrl->timeout);
    if(ret != head)
    {
        xs_free(buf);
        return NULL;
    }

    *size = head;

    return buf;
}

static inline char* xs_ctrl_recv_block_ip(xs_ctrl_t* ctrl, char* ip, int* size)
{
    int fd = xs_sock_connect(ctrl->port, ip);
    if(fd < 0)
    {
        return NULL;
    }
    char* buf = xs_ctrl_recv_block(ctrl, fd, size);
    if(!buf)
    {
        xs_close_socket(fd);
    }
    return buf;
}

void xs_ctrl_recv_df(int fd, xs_ctrl_t* ctrl);
void xs_ctrl_handle_df(int fd, char* buf, int size, xs_ctrl_t* ctrl);

#define xs_ctrl_reg_obj(ctrl, objname, obj) \
    xs_hash_add(ctrl->objs, objname, obj)
static inline void xs_ctrl_reg_handler(xs_ctrl_t* ctrl, char* objname, char* opname, xs_object_handler_t op)
{
    xs_object_t* obj = (xs_object_t*)xs_hash_find(ctrl->objs, objname);
    if(obj == NULL)
    {
        obj = xs_object_create();
        xs_ctrl_reg_obj(ctrl, objname, obj);
    }

    if(xs_object_add_op(obj, opname, op) < 0)
    {
        /* add fail */
        xs_hash_del(obj, (char*)opname, NULL);
        xs_object_add_op(obj, opname, op);
    }
}

#endif

#ifdef __cplusplus
}
#endif
