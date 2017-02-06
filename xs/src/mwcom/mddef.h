
#include "xs.h"

#ifdef __cplusplus
extern "C"{
#endif
#ifndef MW_CONST_H
#define MW_CONST_H

/* use with mw and daemon */
#define __xs_pos "pos" /* object */

#define __xs_run "run"
#define __xs_getid "getid"
#define __xs_setid "setid"
#define __xs_trans "trans"

#define __xs_gui_keepalive "ka"
#define __xs_gui_exiting "exit"
#define MD_DAEMON_STATUS_NORMAL 0
#define MD_DAEMON_STATUS_TRANS_START 1
#define MD_DAEMON_STATUS_TRANS_END 2

#define __xs_conf "conf"  /* 配置信息 */
#define __xs_black "black" /* 黑名单 */
#define __xs_food "food"    /* 周食谱 */
#define __xs_book "book"
#define __xs_cancel "cancel"
#define __xs_oper "oper"

#define __xs_start "start"
#define __xs_end "end"
#define __xs_data "data"
#define __xs_rm "rm"

#define MD_OPER_FILE "oper"
#define MD_BLACK_FILE "black"
#define MD_CONF_FILE "conf"
#define MD_ID_FILE "id"
#define MD_ORDER_DIR "/var/run/order"
#define MD_FOOD_DIR "/var/run/food"
#define MD_RUNNING_FLAG "running.flg"

#define __xs_gui "mdgui.bin"
#define __xs_daemon "mddaemon.bin"

#define __xs_token "`"
/* must align with xs_model_t */
typedef struct md_oper_t
{
    int count;
    char* id;
    char* opername;
    char* password;
} md_oper_t;

static inline xs_vec_t* mw_load_file_to_vec(const char* file)
{
    xs_model_t* model;
    xs_vec_t* vec;
    //   char* value;
    //   char* saveptr = NULL;
    char buf[2048];
    //   int i;

    /* open file */
    FILE* f = fopen(file, "r");
    if(f == NULL)
    {
        return NULL;
    }

    /* create dynamic vec */
    vec = xs_vec_create(8);

    /* read line, and strtok by space */
    while(fgets(buf, sizeof(buf), f))
    {
        /* just \n in line, ignore blank */
        if(strlen(buf) <= 2)
            continue;

        model = xs_model_from_buf(buf, strlen(buf));
        xs_vec_add(vec, model);
#if 0
        model = xs_model_alloc(count);

        value = strtok_r(buf, __xs_token, &saveptr);
        for(i=0; i<count; i++)
        {
            model->argv(i) = xs_strdup(value);
            value = strtok_r(NULL, __xs_token, &saveptr);
            if(value == NULL)
                break;
        }


        if(i == count)
            xs_vec_add(vec, model);
        else /* ignore the error line */
            xs_model_delete(model);
#endif
    }


    fclose(f);
    return vec;
}

static inline int mw_save_file_from_vec(const char* file, xs_vec_t* vec)
{
    /* w+ mean if file exist, clean it, or file no exist, create it */
    unlink(file);
    FILE* f = fopen(file, "w+");
    int i;
    xs_model_t* model;
    int size;
    char* buf;

    if(f == NULL)
    {
        xs_logd("error open file");
        return -1;
    }
    xs_logd("file opened");

    /* just write one empty file */
    if(vec == NULL)
    {
        fclose(f);
        return -1;
    }

    for(i=0; i<vec->count; i++)
    {
        model = (xs_model_t*)xs_vec_get(vec, i);
        xs_logd("model is %p", model);
        buf = xs_model_to_buf(model, &size);
        xs_logd("buf is %s", buf+4);

        fprintf(f, "%s\n", buf+4);
        xs_logd("ready to free buf");
        xs_free(buf);
    }

    xs_logd("all ok");

    fclose(f);
    return 0;
}
typedef struct md_conf_node_t
{
    int argc;
    char* key;
    char* value;
} md_conf_node_t;

#define md_conf_model_t md_conf_node_t

typedef struct md_week_food_t
{
    int argc;
    char* id;
    char* name;
    char* type;
    char* cls;
    char* price;
    char* cb;
} md_week_food_t;

static inline md_week_food_t* md_find_week_food(xs_vec_t* vec, char* id)
{
    int i;
    md_week_food_t* food;
    char buf[64];
    for(i=0; i<vec->count; i++)
    {
        food = (md_week_food_t*)xs_vec_get(vec, i);
        if(food)
        {
            sprintf(buf, "%s.%s", food->cb, food->id);
            if(strcmp(buf, id) == 0)
            {
                return food;
            }
        }
    }
    return NULL;
}


typedef struct md_book_node_t
{
    int argc;
    char* account;
    char* card;
    char* oper;
    char* date;
    char* dinner_id;
    char* dinner;
    char* count;
    char* orderid;
    char* price;
    char* type;
    char* cb;
    char* book_type1;
    char* zone;
    char* dept;
    char* bed;
    char* time;
#define MD_ORDER_BOOK "book"
#define MD_ORDER_BUY "buy"
#define MD_ORDER_ERROR "error"  /* in trans, if book_type is error, mean this is not a book struct, but infomations */
} md_book_node_t;

typedef struct md_req_t
{
    int argc;
    char* pos;
    char* act;

    union
    {
        struct 
        {
            char* filename;
            char* filelen;
        } trans;

        struct
        {
            char* filename;
        } run;

        struct 
        {
            char* option; /* start | end | data */
            char* id;
            char* oper;
            char* password;
        } oper;

        struct
        {} getid;

        struct
        {
            char* option;       /* start or end or more */
            char* conf_name;
            char* conf_value;
        } conf;

        struct
        {
            char* option; /* start or end or data */
            char* _1id;
            char* _2name;
            char* _3type;
            char* _4cls;   
            char* _5price; /* price */
            char* _6day;  /* 1 2 3 4 5 6 7 */
            char* _7time; /* 1 2 3 */
        } food;

        struct
        {

            char* option;       /* start, data, end */
            char* i;
            char* j;

        } book;
        /*
         * when start, send the file to computer
         * when end, delete file
         * */

        struct 
        {
            char* option; /* same with book */
        } cancel;

        struct 
        {
            char* option;  /* same with book */
        } buy;

        struct 
        {
            char* id;
        } setid;

        struct
        {
            char* option;
            char* cardid;
        } black;

    } u;
} md_req_t;

typedef struct md_gui_daemon_t
{
    int argc;
    char* obj;
    char* action;
    char* extra;
} md_gui_daemon_t;

typedef struct md_daemon_gui_t
{
    int argc;
    char* status; /* status of daemon */
    char* infomation; /* info show in gui */
} md_daemon_gui_t;

#ifndef WIN32
int uart_set_proper(int fd, int baud, int databits, int stopbits, int parity);


#define MD_NO_LOG 0

static inline void md_close_debug()
{
#if MD_NO_LOG
    int fd = open("/dev/null", O_RDWR);

    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    if(fd > 2)
        close(fd);
#endif
}
#endif

#endif // MW_CONST_H
#ifdef __cplusplus
}
#endif
