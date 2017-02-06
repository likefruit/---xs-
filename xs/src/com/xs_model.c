#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

void* xs_model_alloc(int argc)
{
    xs_model_t* m = xs_zalloc(sizeof(xs_model_t) + argc * sizeof(char*));
    m->argc = argc;
    return m;
}

xs_model_t* xs_model_create(int argc, ...)
{
    va_list ap;
    int i;
    va_start(ap, argc);
    xs_model_t* m = xs_zalloc(sizeof(xs_model_t) + argc * sizeof(char*));
    m->argc = argc;
    for(i=0; i<argc; i++)
    {
        m->argv[i] = xs_strdup(va_arg(ap, char*));
    }

    va_end(ap);
    return m;
}

int xs_model_call(int fd, xs_model_t** r, int argc, ...)
{
    int ret;
    va_list ap;
    int i;

    va_start(ap, argc);
    xs_model_t* m = xs_zalloc(sizeof(xs_model_t) + argc * sizeof(char*));
    m->argc = argc;
    for(i=0; i<argc; i++)
    {
        m->argv[i] = xs_strdup(va_arg(ap, char*));
    }
     va_end(ap);

     ret = xs_model_send_block(m, fd, 60000);
     if(ret != 0)
         return ret;

     xs_logi("send success");

     ret = xs_model_recv_block((void**)&m, fd, 60000);
     if(ret < 0)
         return ret;

     xs_logi("recv success");
     *r = m;

     return ret;
}

void xs_model_rsp_callback(int fd, void(*callback)(xs_aio_t*), xs_ctrl_t* ctrl, int argc, ...)
{
    va_list ap;
    xs_model_t* m;
    va_start(ap, argc);
    m = xs_model_createv(argc, ap);
    va_end(ap);

    char* buf; int size;
    buf = xs_model_to_buf_and_free_model(m, &size);
    xs_aio_send(fd, callback, buf, size, ctrl);
}

void xs_model_delete(xs_model_t* model)
{
    int i;
    for(i=0; i<model->argc; i++)
    {
        if(model->argv(i))
            xs_free(model->argv[i]);
    }
    xs_free(model);
}

xs_model_t* xs_model_from_buf(char* buf, int size)
{
    xs_dict_t* dict;
    xs_model_t* model;
    char* argc;
    int i;
    size = size;
    
    dict = xs_dict_from_buf(buf, NULL); 
    argc = xs_dict_find_value(dict, __xs_argc); /* now argc == 2 */
    model = xs_model_alloc(atoi(argc));

    for(i=0; i<model->argc; i++)
    {
        model->argv[i] = xs_dict_str(dict, xs_itoa_r(i, argc)); /* argv[0] = clis, argv[1] = "init" */
    }
    xs_dict_destroy(dict);
    return model;
}

int xs_model_recv_block(void** model, int fd, int timeout)
{
    int size;
    char* buf;
    int ret;
    
    *model = NULL;

    ret = xs_sock_recv_block(fd, &size, 4, timeout);
    if(ret != 4)
    {
        xs_loge("error %d", ret);
        return -1;
    }

    size = ntohl(size);
    xs_logd("size=%d", size);
    if(size >= 16380)
        return -1;

    buf = (char*)xs_malloc(size+1);
    buf[size] = 0;

    ret = xs_sock_recv_block(fd, buf, size, timeout);
    if(ret == size)
    {
        xs_logd("%s", buf);
        *model = xs_model_from_buf(buf, size);
        xs_logd("%s", ((xs_model_t*)(*model))->argv(0));

        xs_free(buf);
        return 0;
    }
    xs_loge("error");
    xs_free(buf);
    return -1;
}

int xs_model_send_block(void* model, int fd, int timeout)
{
    char* buf;
    int size;
    int ret;
    buf = xs_model_to_buf_and_free_model((xs_model_t*)model, &size);
    xs_logd("buf=%s", buf+4);
    ret = xs_sock_send_block(fd, buf, size, timeout);
    xs_free(buf);
    if(ret != size)
    {
        xs_loge("error");
        return -1;
    }
    xs_logd("sucess");
    return 0;
}

char* xs_model_to_buf(xs_model_t* model, int* size)
{
    char buf[32];
    xs_dict_t* dict = NULL;
    int i;
    
    dict = xs_dict_add_int(dict, __xs_argc, model->argc);
    for(i=0; i<model->argc; i++)
    {
        if(model->argv[i])
            dict = xs_dict_add_str(dict, xs_itoa_r(i, buf), model->argv[i]);
    }

    return xs_dict_to_buf_with_head(dict, size);
}
#ifdef __cplusplus
}
#endif
