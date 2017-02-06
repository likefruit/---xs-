
#include "main.h"

xs_vec_t* g_oper = NULL;

char* md_oper_add_vec(md_req_t* req)
{
    char* id = req->u.oper.id;
    char* oper = req->u.oper.oper;
    char* password = req->u.oper.password;

    if(req->argc != 6)
    {
        return "arg is error";
    }

    xs_model_t* model = xs_model_create(3, id, oper, password);
    xs_vec_add(g_oper, model);
    return NULL;
}

/* recv command pos->oper */
void handle_oper_start(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    req = req;
    if(g_oper)
        xs_vec_destroy(g_oper, (void(*)(void*))xs_model_delete);
        
    g_oper = xs_vec_create(8);    
        
    /* send response and wait next cmd */
    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
}

void handle_oper_data(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    char* error;
    error = md_oper_add_vec(req);
    if(error == NULL)
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
    else
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_err, error);
}

void handle_oper_end(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    req = req;
    xs_logd("enter oper end");

    if(mw_save_file_from_vec(MD_OPER_FILE, g_oper) == 0)
    {
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
    }
    else
    {
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_err, "write to file failure");
    }
    xs_vec_destroy(g_oper, (void(*)(void*))xs_model_delete);
    g_oper = NULL;
}


void handle_oper(void* model, int fd, struct xs_ctrl_t* ctrl)
{
    md_req_t* req = (md_req_t*)model;
    
    if(strcmp(req->u.oper.option, __xs_start) == 0)
    {
        md_show_info();
        handle_oper_start(req, fd, ctrl);
    }
    else if(strcmp(req->u.oper.option, __xs_data) == 0)
    {
        handle_oper_data(req, fd, ctrl);
    }
    else if(strcmp(req->u.oper.option, __xs_end) == 0)
    {
        handle_oper_end(req, fd, ctrl);
     //   xs_ev_add_msg_ev(XS_EV_MSG_MEM_CHECK , 0, NULL, 0);
        md_restart_gui();
    }
    else
    {
        xs_loge("what is the option=%s", req->u.oper.option);
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 3, __xs_err, "do not know option", req->u.oper.option);
    }
}

void md_oper_init(xs_ctrl_t* ctrl)
{
    xs_ctrl_reg_handler(ctrl, __xs_pos, __xs_oper, handle_oper);
}
