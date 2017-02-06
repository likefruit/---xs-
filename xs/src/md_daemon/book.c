
#include "main.h"
#include <glob.h>

/*
    trans book order --> computer;

    1) computer -> start, PM need

*/

typedef struct md_book_trans_status_t
{
    xs_vec_t** vec;
    glob_t gl;
} md_book_trans_status_t;

md_book_trans_status_t g_book_trans_status;

void clean_trans_status()
{
#if 1
    if(g_book_trans_status.vec)
    {
        int j;
        for(j=0; j<(int)g_book_trans_status.gl.gl_pathc; j++)
        {
            if(g_book_trans_status.vec[j])
                xs_vec_destroy(g_book_trans_status.vec[j], (void(*)(void*))xs_model_delete);
        }

        xs_free(g_book_trans_status.vec);
        g_book_trans_status.vec = NULL;

        globfree(&g_book_trans_status.gl);
    }
#endif
}

void handle_book_start(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
   int ret;
   req = req;

   clean_trans_status();

    ret = glob(MD_ORDER_DIR"/*", 0, NULL, &g_book_trans_status.gl);
    if(ret < 0)
    {
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_err, "glob call error");
        return;
    }

    g_book_trans_status.vec = xs_zalloc(sizeof(xs_vec_t*) * g_book_trans_status.gl.gl_pathc);

    /* do not load at start, memory maybe not enough */

    xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_ok, xs_itoa(g_book_trans_status.gl.gl_pathc));
}

void send_book_node(int fd, xs_ctrl_t* ctrl, md_book_node_t* node, int free_model)
{
    char* buf;
    int size;
    if(node->book_type1 == NULL) node->book_type1 = xs_strdup(__xs_err);

    if(free_model)
        buf = xs_model_to_buf_and_free_model((xs_model_t*)node, &size);
    else
        buf = xs_model_to_buf((xs_model_t*)node, &size);

    xs_aio_send(fd, md_default_callback, buf, size, ctrl);
    return;
}

/* send curfile curidx,  if(curidx==vec.count) {curfile++, try load file} */
void handle_book_data(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    req = req;
    md_book_node_t* node = NULL;
    int i = atoi(req->u.book.i);
    int j = atoi(req->u.book.j);

    /* should be end */
    if(i >= (int)g_book_trans_status.gl.gl_pathc)
    {
        return send_book_node(fd, ctrl, xs_model(md_book_node_t), 1);
    }

    /* get book order data from global vec */
    xs_vec_t* vec = g_book_trans_status.vec[i];
    if(vec == NULL)
    {
        /* if null, maybe not load, load now */
        vec = mw_load_file_to_vec(g_book_trans_status.gl.gl_pathv[i]);
        if(vec)
            g_book_trans_status.vec[i] = vec;
        else
            return send_book_node(fd, ctrl, xs_model(md_book_node_t), 1);
    }

    node = xs_vec_get(vec, j);
    if(node == NULL)
    {
        return send_book_node(fd, ctrl, xs_model(md_book_node_t), 1);
    }
    /* send the book node */
    send_book_node(fd, ctrl, node, 0);
}

void handle_book_end(md_req_t* req, int fd, xs_ctrl_t* ctrl)
{
    clean_trans_status();
    req = req;
    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
}

/* recv with pos->book command */
void handle_book(void* model, int fd, struct xs_ctrl_t* ctrl)
{
    md_req_t* req = (md_req_t*)model;

    if(strcmp(req->u.book.option, __xs_data) == 0)
    {
        handle_book_data(req, fd, ctrl);
    }
    else if(strcmp(req->u.book.option, __xs_rm) == 0)
    {
        int i = atoi(req->u.book.i);
        if(i >= (int)g_book_trans_status.gl.gl_pathc)
            xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);

        /* clear the vec if need */
        if(g_book_trans_status.vec[i])
        {
            xs_vec_destroy(g_book_trans_status.vec[i], (void(*)(void*))xs_model_delete);
            g_book_trans_status.vec[i] = NULL;
        }

        /* delete the file */
        unlink(g_book_trans_status.gl.gl_pathv[i]);
        xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, __xs_ok);
    }
    else if(strcmp(req->u.book.option, __xs_start) == 0)
    {
        md_show_info();
        //md_restart_gui();
        handle_book_start(req, fd, ctrl);
    }
    else if(strcmp(req->u.book.option, __xs_end) == 0)
    {
        handle_book_end(req, fd, ctrl);
        md_restart_gui();
    }
}

void md_book_init(xs_ctrl_t* ctrl)
{
    g_book_trans_status.vec = NULL;
    xs_ctrl_reg_handler(ctrl, __xs_pos, __xs_book, handle_book);
}
