#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_SERVER_H__
#define __XS_SERVER_H__

#define XS_CLIS_TIMEOUT 60000

typedef struct xs_cli_model_t
{
    int     count;
    char*   object;   /* object */
    char*   op;       /* operation */
    char*   arg[];
} xs_cli_model_t;

typedef struct xs_rsp_model_t
{
    int     count;
    char*   result;
    char*   reason;
    char*   arg[];
} xs_rsp_model_t;

typedef struct xs_clis_t
{
    xs_ctrl_t*      ctrl;
    xs_dict_t*      cmds;
    int             cmds_count;
} xs_clis_t;

#define __xs_clis "clis"
#define __xs_init "init"
#define __xs_show "show"

#define __xs_memory "memory"
#define __xs_check "check"

extern xs_clis_t g_clis;
void xs_clis_init(int port);
void xs_clis_exit();
void xs_clis_reg_command(char* command);
#define xs_clis_reg_handler(_objname, _opname, _op) xs_ctrl_reg_handler(g_clis.ctrl, _objname, _opname, _op)

void xs_server_init(int thread_count, void(*quit)(), int argc, char* argv[]);
#define xs_server_run xs_ev_run
#define xs_server_fini xs_fini

xs_ev_sock_t* xs_server_start_tcp(uint16_t port, int buf, void(*accept_func)(xs_ev_sock_t*), void* ptr);

#endif
#ifdef __cplusplus
}
#endif
