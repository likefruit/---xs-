

#include "main.h"
FILE* g_foodfile[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void handle_food_start(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    int i;
    char file[256];
    req = req;

    mkdir(MD_FOOD_DIR, 0777);

    for(i=0; i<7; i++)
    {
        if(g_foodfile[i]) fclose(g_foodfile[i]);
        sprintf(file, "%s/%d", MD_FOOD_DIR, i);
        g_foodfile[i] = fopen(file, "w+");
    }

    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
}


void handle_food_data(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    char error[64];
    int index;

    if(req->argc != 10)
    {
        sprintf(error, "argc error = %d", req->argc);
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_err, error);
        return;
    }

    index = atoi(req->u.food._6day) - 1;
    xs_logd("handle food index=%d, req->u.food._6day=%s", index, req->u.food._6day);
    if(index > 6)
    {
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_err, "week is > 7");
        return;
    }

    xs_model_t* m = xs_model_create(6, req->u.food._1id,
            req->u.food._2name,
            req->u.food._3type,
            req->u.food._4cls,
            req->u.food._5price,
            req->u.food._7time);
 //   xs_vec_add(g_food[index], m);

    char* buf = xs_model_to_buf_and_free_model(m, NULL);
    fprintf(g_foodfile[index], "%s\n", buf+4);
    xs_free(buf);
    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
}

void handle_food_end(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    int i;
//    char file[256];
    req = req;

    /* all data have record in memory */
    for(i=0; i<7; i++)
    {
#if 0
        sprintf(file, "%s/%d", MD_FOOD_DIR, i);
        if(mw_save_file_from_vec(file, g_food[i]) != 0)
        {
            xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_err, "write to file failure");
            return;
        }
#endif
        fclose(g_foodfile[i]);
    }
        
    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
}

/* recv with pos->book command */
void handle_food(void* model, int fd, struct xs_ctrl_t* ctrl)
{
    md_req_t* req = (md_req_t*)model;
    
    if(strcmp(req->u.food.option, __xs_start) == 0)
    {
        md_show_info();
        handle_food_start(req, fd, ctrl);
    }
    else if(strcmp(req->u.food.option, __xs_data) == 0)
    {
        handle_food_data(req, fd, ctrl);
    }
    else if(strcmp(req->u.food.option, __xs_end) == 0)
    {
        handle_food_end(req, fd, ctrl);
        md_restart_gui();
    }
    else
    {
        xs_loge("what is the option=%s", req->u.food.option);
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 
                3, __xs_err, "do not know option", req->u.food.option);
    }
}

void md_food_init(xs_ctrl_t* ctrl)
{
    xs_ctrl_reg_handler(ctrl, __xs_pos, __xs_food, handle_food);
}
