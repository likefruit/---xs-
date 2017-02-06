
/* this handle handle black download */

#include "main.h"

FILE* g_black_file = NULL;

void handle_black_data(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    xs_logd("recv black end and handle");
    char* key = req->u.black.cardid;
    fprintf(g_black_file, "%s\n", key);
    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
}

void handle_black_end(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    req = req;
    if(g_black_file)
        fclose(g_black_file);
    g_black_file = NULL;
    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
}

void handle_black_start(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    xs_logd("recv black start and handle");
    req = req;
    if(g_black_file) fclose(g_black_file);
    g_black_file = fopen(MD_BLACK_FILE, "w+");
    if(g_black_file)
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
    else
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_err, "error open black file");
}

/* recv command pos->black */
void handle_black(void* model, int fd, struct xs_ctrl_t* ctrl)
{
    md_req_t* req = (md_req_t*)model;

    if(strcmp(req->u.black.option, __xs_start) == 0)
    {
        xs_logd("black start now..............");
        md_show_info();
        handle_black_start(req, fd, ctrl);
    }
    else if(strcmp(req->u.black.option, __xs_data) == 0)
    {
        xs_logd("black data now..............");
        handle_black_data(req, fd, ctrl);
    }
    else if(strcmp(req->u.black.option, __xs_end) == 0)
    {
        xs_logd("black end now..............");
        handle_black_end(req, fd, ctrl);
        md_restart_gui();
    }
    else
    {
        xs_loge("what is the option=%s", req->u.black.option);
        xs_model_rsp_callback(fd, md_default_callback, ctrl,
                3, __xs_err, "do not know option", req->u.black.option);
    }
}

void md_black_init(xs_ctrl_t* ctrl)
{
    xs_ctrl_reg_handler(ctrl, __xs_pos, __xs_black, handle_black);
}

