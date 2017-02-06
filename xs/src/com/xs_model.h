#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_MODEL_H__
#define __XS_MODEL_H__

typedef struct xs_model_t
{
    int             argc;
    char*           argv[];
} xs_model_t;

#define xs_model_argc(__type1) ((sizeof(__type1)-sizeof(int))/sizeof(char*))
#define xs_model(__type) (__type*)xs_model_alloc(xs_model_argc(__type))

#define argv(n) argv[n]

#define __xs_argc "argc"
#define __xs_ok "ok"
#define __xs_err "err"
#define xs_success(buf) ((buf) && strcmp((buf), __xs_ok) == 0)

void* xs_model_alloc(int argc);
void xs_model_delete(xs_model_t* model);
xs_model_t* xs_model_create(int argc, ...);

xs_model_t* xs_model_from_buf(char* buf, int size);
char* xs_model_to_buf(xs_model_t* model, int* size);
static inline char* xs_model_to_buf_and_free_model(xs_model_t* model, int*size)
{
    char* buf = xs_model_to_buf(model, size);
    xs_model_delete(model);
    return buf;
}
#define xs_model_set(_m, _i, _v) ((xs_model_t*)_m)->argv[_i] = xs_strdup(_v)

/* this function, so danger */
static inline void xs_model_set_all(void* model, ...)
{
    int i;
    va_list ap;
    char* p;
    xs_model_t* m = (xs_model_t*)model;

    va_start(ap, model);

    for(i=0; i<m->argc; i++)
    {
        p = va_arg(ap, char*);
        if(p)
            m->argv[i] = xs_strdup(p);
    }
    va_end(ap);
}


/* for client */
int xs_model_call(int fd, xs_model_t** r, int argc, ...);
int xs_model_send_block(void* model, int fd, int timeout);
 int xs_model_recv_block(void** model, int fd, int timeout);

/* for server functions */
static inline void xs_model_send_and_close(xs_model_t* model, int fd)
{
    char* buf;
    int size;

    buf = xs_model_to_buf(model, &size);
    xs_aio_send_and_close(fd, buf, size);
}
static inline void xs_model_rsp(int fd, char* result, char* reason, char* other)
{
    xs_model_t* req = xs_model_create(3, result, reason, other);
    xs_model_send_and_close(req, fd);
    xs_model_delete(req); 
}

static inline xs_model_t* xs_model_createv(int argc, va_list ap)
{
    int i;
    xs_model_t* m = (xs_model_t*)xs_model_alloc(argc);
    for(i=0; i<argc; i++)
    {
        m->argv[i] = xs_strdup(va_arg(ap, char*));
    }
    return m;
}

void xs_model_rsp_callback(int fd, void(*callback)(xs_aio_t*), xs_ctrl_t* ctrl, int argc, ...);

#endif
#ifdef __cplusplus
}
#endif
