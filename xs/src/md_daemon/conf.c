
/* this handle handle conf download */

#include "main.h"

xs_vec_t* g_conf = NULL;
void handle_conf_data(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    xs_logd("recv conf end and handle");
    char* key = req->u.conf.conf_name;
    char* value = req->u.conf.conf_value;
    char error[256];
    if(req->argc != 5)
    {
        sprintf(error, "argc error = %d", req->argc);
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_err, error);
        return;
    }

    xs_vec_add(g_conf, xs_model_create(2, key, value));
    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
}

void handle_conf_end(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    xs_logd("recv conf end and handle");
    req = req;
    if(mw_save_file_from_vec(MD_CONF_FILE, g_conf) == 0)
    {
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
    }
    else
    {
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_err, "write to file failure");
    }
    xs_vec_destroy(g_conf, (void(*)(void*))xs_model_delete);
    g_conf = NULL;
}

void handle_conf_start(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    xs_logd("recv conf start and handle");
    req = req;
    if(g_conf)
        xs_vec_destroy(g_conf, (void(*)(void*))xs_model_delete);
    g_conf = xs_vec_create(8);
    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
}

/* recv command pos->conf */
void handle_conf(void* model, int fd, struct xs_ctrl_t* ctrl)
{
    md_req_t* req = (md_req_t*)model;
    
    if(strcmp(req->u.conf.option, __xs_start) == 0)
    {
        md_show_info();
        handle_conf_start(req, fd, ctrl);
    }
    else if(strcmp(req->u.conf.option, __xs_data) == 0)
    {
        handle_conf_data(req, fd, ctrl);
    }
    else if(strcmp(req->u.conf.option, __xs_end) == 0)
    {
        handle_conf_end(req, fd, ctrl);
        md_restart_gui();
    }
    else
    {
        xs_loge("what is the option=%s", req->u.conf.option);
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 
                3, __xs_err, "do not know option", req->u.conf.option);
    }
}

void md_conf_init(xs_ctrl_t* ctrl)
{
    xs_ctrl_reg_handler(ctrl, __xs_pos, __xs_conf, handle_conf);
}
