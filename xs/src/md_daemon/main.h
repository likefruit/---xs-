#ifndef __MAIN_H__
#define __MAIN_H__

#include "xs.h"
#include "../mwcom/mddef.h"

extern pid_t g_sub;
void md_start_gui();

extern int g_status;

#define md_show_info() g_status = MD_DAEMON_STATUS_TRANS_START
#define md_restart_gui() g_status = MD_DAEMON_STATUS_TRANS_END

void response(int fd, xs_ctrl_t* ctrl, char* result, char* reason, void(*callback)(xs_aio_t* aio));
void trans_ok_response_send_complete(xs_aio_t* aio);
void trans_err_response_send_complete(xs_aio_t* aio);

/* new function */
void md_default_callback(xs_aio_t* aio);
void md_conf_init(xs_ctrl_t* ctrl);
void md_oper_init(xs_ctrl_t* ctrl);
void md_book_init(xs_ctrl_t* ctrl);
void md_food_init(xs_ctrl_t* ctrl);
void md_black_init(xs_ctrl_t* ctrl);

#endif
