
#include "main.h"
#include <sys/ioctl.h>

void quit()
{}

int g_remain = 0;
int g_transfd = -1;
int g_status = 0;
int g_sockpair[2];
xs_ctrl_t* g_ctrl;

void trans_rsp_send_ok(xs_aio_t* aio);

char* g_trans_filename = NULL;


void trans_recv_data(xs_aio_t* aio)
{
    char tmp[32];
    if(aio->result == XS_AIO_DONE)
    {
        /* write content to file */
        write(g_transfd, aio->buf, aio->buflen);
        g_remain -= aio->buflen;

        response(aio->fd, aio->ptr, "ok", xs_itoa_r(g_remain, tmp), trans_ok_response_send_complete);
    }
    else /* tran error */
    {
        if(aio->ret == 0)
        {
            extern void __xs_aio_recv_data(xs_ev_sock_t* sev);
            __xs_aio_recv_data(aio->sev);
            return;
        }
        else
        {
            response(aio->fd, aio->ptr, "err", "receive data return < 0", trans_err_response_send_complete);
        }
    }

    xs_aio_free(aio);
}

void trans_err_response_send_complete(xs_aio_t* aio)
{
    xs_ctrl_t* ctrl = aio->ptr;
    if(g_transfd > 0)
    {
        close(g_transfd);
        g_transfd = -1;
        unlink(g_trans_filename);
        md_restart_gui();
    }
    ctrl->r(aio->fd, ctrl);
    xs_aio_free(aio);
}
void trans_ok_response_send_complete(xs_aio_t* aio)
{
    xs_ctrl_t* ctrl = aio->ptr;
    if(aio->result == XS_AIO_DONE)
    {
        if(g_remain > 0)
        {
            xs_aio_recv(aio->fd, trans_recv_data, ctrl);
        }
        else
        {
            close(g_transfd);
            g_transfd = -1;
            ctrl->r(aio->fd, ctrl);
            md_restart_gui();
        }
    }
    else /* response send failure, 很少出现这种情况的，咋办，目前的处理是关闭文件，并删除掉不完整的文件 */
    {
        close(g_transfd);
        g_transfd = -1;
        ctrl->r(aio->fd, ctrl);
        unlink(g_trans_filename);
        md_restart_gui();
    }
    xs_aio_free(aio);
}

void response(int fd, xs_ctrl_t* ctrl, char* result, char* reason, void(*callback)(xs_aio_t* aio))
{
    int size;
    char* buf;
    xs_model_t* model = xs_model_alloc(2);
    model->argv[0] = xs_strdup(result);
    model->argv[1] = xs_strdup(reason);

    buf = xs_model_to_buf(model, &size);
    xs_model_delete(model);
    xs_aio_send(fd, callback, buf, size, ctrl);
}

xs_vec_t* g_conf_vec = NULL;


void handle_keepalive(void* model, int fd, struct xs_ctrl_t* ctrl)
{
    model = model;
    xs_logd("recv keep alive");
    xs_model_rsp_callback(fd, md_default_callback, ctrl, 1, xs_itoa(g_status));
}

void md_default_callback(xs_aio_t* aio)
{
    xs_ctrl_t* ctrl = aio->ptr;
    ctrl->r(aio->fd, ctrl);
    xs_aio_free(aio);
}

void get_pos_id_from_drv(char* buf)
{
    int f = open("/dev/typea", O_RDWR);
    ioctl(f, 0xB5, buf);
    close(f);
    sprintf(buf, "%02x%02x%02x%02x", buf[8], buf[9], buf[10], buf[11]);

    FILE* id = fopen(MD_ID_FILE, "w+");
    if(id)
    {
        fprintf(id, "%s", buf);
        fclose(id);
    }
}

void get_pos_id_from_file(char* buf)
{
    int fd = open(MD_ID_FILE, O_RDWR);
    read(fd, buf, 8);
    buf[8] = 0;
    close(fd);
}

void handle_getid(void* model, int fd, struct xs_ctrl_t* ctrl)
{
    model = model;
    char buf[32];

    if(xs_file_exist(MD_ID_FILE))
    {
        get_pos_id_from_file(buf);
    }
    else
    {
        get_pos_id_from_drv(buf);
    }

    xs_model_rsp_callback(fd, md_default_callback, ctrl, 2, __xs_ok, buf);
}


#define md_set_trans_filename(_filename) do{\
    if(g_trans_filename) xs_free(g_trans_filename);\
    g_trans_filename = xs_strdup(_filename);\
    }while(0)

void handle_trans(void* model, int fd, struct xs_ctrl_t* ctrl)
{
    char tmp[32];
    ctrl = ctrl;
    md_req_t* m = model;
    /* save file info, and open file */
    g_remain = atoi(m->u.trans.filelen);
    unlink(m->u.trans.filename);
    md_set_trans_filename(m->u.trans.filename);
    g_transfd = creat(m->u.trans.filename, 0777);
    if(g_transfd < 0)
    {
        response(fd, ctrl, "err", "can not open file", trans_err_response_send_complete);
        return;
    }

    response(fd, ctrl, "ok", xs_itoa_r(g_remain, tmp), trans_ok_response_send_complete);
    md_show_info();
}

void md_ctrl_recv_df_cbk(xs_aio_t* aio)
{
    xs_ctrl_t* ctrl = aio->ptr;
    if(aio->result == XS_AIO_DONE)
    {
        xs_logi("buf=%s", aio->buf);
        ctrl->h(aio->fd, aio->buf, aio->buflen, ctrl);
    }
    else
    {
        /* if gui socket error, may be child proccess has quit */
        if(aio->fd == g_sockpair[0])
        {
            md_start_gui();
        }
        xs_loge("error recv ret=%d, errno=%d", aio->ret, errno);
        ctrl->r(aio->fd, ctrl);
    }
    xs_aio_free(aio);
}

void md_recv_handle(int fd, xs_ctrl_t* ctrl)
{
    xs_aio_recv(fd, md_ctrl_recv_df_cbk, ctrl);
}

#define MAINWND_FILE "./"__xs_gui

#define  POWEROFF		3
#define  KILLPDA                 0
static int asyncio_fd = -1;
int PowerOff(void)
{
    int fd;
    fd = open("/dev/power",O_RDWR);
    if(fd < 0)
    {
        return -1;
    }
    ioctl(fd,KILLPDA);
    close(fd);
    return fd;
}
void signal_handler(int num)//ŽŠÀíº¯Êý
{
    int status = -1;
    int asyiono = -1;
    num = num;

    read(asyncio_fd, &status, 4);//ÅÐ¶ÏÊÇÄÇžöÇý¶¯·¢³öµÄsigno ÐÅºÅ

    asyiono = (char)status;	//µÍ8Î»±£ŽæµÄÎªÄÇžöÒì²œIO

    switch(asyiono)
    {
    case POWEROFF:
    {
        system("ps");
        system("ppp-off");
        system("kill -9 $(ps | grep \"pppd call gprs\" |grep -v 'grep' | awk '{print $1}')");
        system("kill -9 $(ps | grep \"/etc/ppp/gprs\" |grep -v 'grep' | awk '{print $1}')");
        system("ps");
        PowerOff();
    }
        break;
    default:
        break;
    }
}
int InitAsyncio(void)//³õÊŒ»¯Òì²œioÇý¶¯
{
    int oflags;
    if ((asyncio_fd = open("/dev/asyncio",O_RDWR)) < 0)
    {
        exit(1);
    }
    signal(SIGIO, signal_handler);			//°ó¶šactionµœ	SIGIO signal
    fcntl(asyncio_fd, F_SETOWN, getpid());		//ÉèÖÃSTDIN_FILENODµÄownerÎªµ±Ç°œø³Ì
    oflags = fcntl(asyncio_fd, F_GETFL);		//»ñÈ¡ÎÄŒþŽò¿ªµÄ·œÊœ
    fcntl(asyncio_fd, F_SETFL, oflags | FASYNC);	//ÉèÖÃSTDIN_FILENOµÄÎÄŒþ×ŽÌ¬±êÖŸ£šÔöŒÓÁËFASYNC±êÖŸ£©.
    return 0;
}

void time_out(xs_ev_time_t* tev)
{
    tev = tev;
    if(xs_file_exist(MD_RUNNING_FLAG))
    {
        unlink(MD_RUNNING_FLAG);
    }
    else
    {
        PowerOff();
    }
}

int main(int argc, char* argv[])
{
    int ret, fd;
    xs_ctrl_t* ctrl;

    md_close_debug();
    InitAsyncio();
    xs_server_init(2, quit, argc, argv);

    mkdir(MD_FOOD_DIR, 0777);
    mkdir(MD_ORDER_DIR, 0777);

    fd = open("/dev/ttyS1", O_RDWR);
    if(fd< 0)
    {
        xs_loge("error open serial");
        return -1;
    }
    xs_logi("open ttyS1 success");
    ret = uart_set_proper(fd, 115200, 8, 1, 'N');  //setproper();

    if(ret != 0)
    {
        xs_loge("error");
        return -1;
    }
    g_sockpair[0] = 0;

    ctrl = xs_ctrl_create(0, 60000, md_recv_handle, NULL);

    xs_ctrl_reg_handler(ctrl, __xs_pos, __xs_trans, handle_trans);
    xs_ctrl_reg_handler(ctrl, __xs_pos, __xs_getid, handle_getid);

    /* communication with gui */
    xs_ctrl_reg_handler(ctrl, __xs_pos, __xs_gui_keepalive, handle_keepalive);

    md_conf_init(ctrl);
    md_oper_init(ctrl);
    md_book_init(ctrl);
    md_food_init(ctrl);
    md_black_init(ctrl);
    
    /*start read something */
    ctrl->r(fd, ctrl);
    g_ctrl = ctrl;

    md_start_gui();

    /*5 minute */
    xs_ev_add_time_ev(5*1000*60, time_out, NULL);

    xs_server_run();
    return 0;
}

void md_start_gui()
{
    pid_t sub;
    int ret;

    g_status = MD_DAEMON_STATUS_NORMAL;
    if(g_sockpair[0] > 0)
    {
        close(g_sockpair[0]);
        g_sockpair[0] = 0;
    }

    /* fork the mainwindow */
    if(xs_file_exist(MAINWND_FILE))
    {
        if(socketpair(AF_UNIX, SOCK_STREAM, 0, g_sockpair) < 0)
        {
            xs_loge("socket pair error, errno=%d", errno);
            exit(1);
        }

        xs_logd("sock1=%d, sock2=%d", g_sockpair[0], g_sockpair[1]);

        sub = fork();
        if(sub == 0)
        {
            close(g_sockpair[0]);
            /* child */
            ret = execl(MAINWND_FILE, MAINWND_FILE, xs_itoa(g_sockpair[1]), NULL);
            if(ret < 0)
                exit(1);
        }
        else
        {
            g_ctrl->r(g_sockpair[0], g_ctrl);
            /* parent */
            close(g_sockpair[1]);
        }
    }
}
