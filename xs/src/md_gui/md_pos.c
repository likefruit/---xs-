#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include <glob.h>

#include <sys/ioctl.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>

#include "xs.h"
#include "mddef.h"
#include "opcard.h"
#include "md_gb2312.h"

int g_sock;

void drawTitle(HDC dc, RECT* rc)
{
    char* title = md_get_title();

    RECT rcTitle;
    memcpy(&rcTitle, rc, sizeof(*rc));
    rcTitle.bottom = MD_TITLE_HEIGHT + rcTitle.top;
    PLOGFONT font = SelectFont(dc, g_md.fontB);
    DrawText (dc, title, -1, &rcTitle,  DT_VCENTER|DT_CENTER | DT_SINGLELINE|DT_CENTER);
    SelectFont(dc, font);
    rc->top = rcTitle.bottom;
}

void drawSingleText(HDC dc, RECT* rc, const char* buf)
{
    rc->left += MD_MARGIN;
    rc->right -= MD_MARGIN;
    SetTextColor (dc, PIXEL_black);
    DrawText (dc, buf, -1, rc, DT_WORDBREAK);
    rc->left -= MD_MARGIN;
    rc->right += MD_MARGIN;
}

BOOL md_handle_input( WPARAM wParam)
{
    int len = strlen(g_md.input);
    switch(wParam)
    {
        case SCANCODE_1:
        case SCANCODE_2:
        case SCANCODE_3:
        case SCANCODE_4:
        case SCANCODE_5:
        case SCANCODE_6:
        case SCANCODE_7:
        case SCANCODE_8:
        case SCANCODE_9:
        case SCANCODE_0:
            if(len == MAX_INPUT_SIZE)
                return TRUE;
            md_input_append_and_update(( wParam-SCANCODE_1+1)%10 + '0', &g_md.rc[1]);
            return TRUE;

        case MD_KEY_DOT:
            if(len == MAX_INPUT_SIZE)
                return TRUE;
            md_input_append_and_update('.', &g_md.rc[1]);
            return TRUE;

        case MD_KEY_CANCEL:
            if(len == 0)
                return TRUE;
            md_input_cancel_and_update(&g_md.rc[1]);
            return TRUE;

    }
    return FALSE;
}

md_t g_md;

void drawTable(HDC dc, RECT* rc1, char** menu, int size, int perpage, BOOL selBorder)
{
    int curSel = g_md.cur_item_sel;
    int begin = curSel / perpage *perpage;
    int h = MD_MENU_HEIGHT, m = MD_MARGIN, i;

    g_md.cur_perpage = perpage;
    g_md.item_total = size;

    RECT* rc = xs_malloc(sizeof(RECT));
    memcpy(rc, rc1, sizeof(RECT));

    rc->left += m;

    for(i=begin; i<begin+perpage ; ++i)
    {
        rc->bottom = rc->top + h;
        memcpy(&g_md.rc_item[i-begin], rc, sizeof(*rc));

        if(i < size && menu)
        {
            DrawText(dc, menu[i], -1, rc, DT_SINGLELINE|DT_VCENTER);

            if(i == curSel && selBorder)
            {
                Rectangle(dc, rc->left, rc->top, rc->right, rc->bottom);
            }
        }

        rc->top = rc->bottom;
    }
    rc->left -= m;

    rc1->top = rc->top;
    xs_free(rc);
}

/*
   0 is  xingqiyi
   1 is xingqier
   2 is xingqisan
   3 is xingqisi
   dengdeng ...
   */
int get_week_day()
{
    time_t timep;
    struct tm *p;
    time(&timep);
    p=localtime(&timep);

    /* 0 to 6, but 0 is Sunday */
    if(p->tm_wday == 0)
        return 6;

    return p->tm_wday-1;
}

int dinner_cmp(const char* v1, const char* v2)
{
    const md_week_food_t* f1 =* (const md_week_food_t** )v1;
    const md_week_food_t* f2 =* (const md_week_food_t** )v2;

    int cb1 = atoi(f1->cb);
    int cb2 = atoi(f2->cb);
    if(cb1 != cb2)
        return cb1 - cb2;

    double p1 = strtod(f1->price, NULL);
    double p2 = strtod(f2->price, NULL);
    return (int)(p1*100 - p2*100);
}

char* init_dinner_list()
{
    /*
       first get day time, arrcording day
       */
    int wday = get_week_day();
    //wday += g_md.book_date
    wday += g_md.booking_delay;
    wday %= 7;
    char filename[256];
    sprintf(filename, "%s/%d", MD_FOOD_DIR, wday);
    xs_vec_t* vec = mw_load_file_to_vec(filename);

    if(vec == NULL)
    {
        return "error load dinner list";
    }

    xs_vec_sort(vec, dinner_cmp);

    g_md.dinner_all = vec;
    return NULL;
}

char* init_oper_list( void )
{
    g_md.oper = mw_load_file_to_vec(MD_OPER_FILE);
    if(g_md.oper == NULL)
    {
        return "error load oper list";
    }
    return NULL;
}

char* md_load_config_file( void )
{
    int i;
    xs_vec_t* vec;
    md_conf_model_t* conf;
    vec = mw_load_file_to_vec(MD_CONF_FILE);
    if(vec == NULL)
    {
        return "load config error";
    }
    for(i=0; i<vec->count; i++)
    {
        conf = (md_conf_model_t*)xs_vec_get(vec, i);
        if(strcmp(conf->key, "delay") == 0)
        {
            g_md.booking_delay = atoi(conf->value);
            time_t timep;
            struct tm tm;

            time(&timep);
            timep += 86400*g_md.booking_delay;
            gmtime_r(&timep, &tm);
            char buf[256];
            sprintf(buf, "%d-%02d-%02d", tm.tm_year + 1900,
                    tm.tm_mon+1, tm.tm_mday);
            g_md.book_date = xs_strdup(buf);

            /* init buy date */
            time(&timep);
            gmtime_r(&timep, &tm);
            sprintf(buf, "%d-%02d-%02d", tm.tm_year + 1900,
                    tm.tm_mon+1, tm.tm_mday);
            g_md.buy_date = xs_strdup(buf);
            

        }
        else if(strcmp(conf->key, "company") == 0)
        {
            g_md.company = xs_strdup(conf->value);
        }
        else if(strcmp(conf->key, "dept")  == 0 || strcmp(conf->key, "zone") == 0)
        {
            xs_vec_t* vzone = xs_vec_create(8);
            char* p = strtok(conf->value, "|");
            while(p)
            {
                xs_vec_add(vzone, xs_strdup(p));
                p = strtok(NULL, "|");
            }

            if(strcmp(conf->key, "dept") == 0)
                g_md.vdept = vzone;
            else
                g_md.vzone = vzone;
        }
    }
    xs_vec_destroy(vec, (void(*)(void*))xs_model_delete);
    return NULL;
}

int xs_get_local_time(int *xs_year,
        int *xs_month,
        int *xs_day,
        int *xs_hour,
        int *xs_minute,
        int *xs_second)
{
    time_t timep;
    struct tm tm;

    time(&timep);

    gmtime_r(&timep, &tm);

    *xs_year = tm.tm_year + 1900;
    *xs_month = tm.tm_mon+1;
    *xs_day = tm.tm_mday;

    *xs_hour = tm.tm_hour;
    *xs_minute = tm.tm_min;
    *xs_second = tm.tm_sec;

    return 0;
}

char* md_date()
{
    static char buf[48];
    int year, month, date, hour, minute, second;
    xs_get_local_time(&year, &month, &date, &hour, &minute, &second);
    sprintf(buf, "%04d%02d%02d", year, month, date);
    return buf;
}

char* md_date_order()
{
    char* p = md_date();
    char* q = xs_strdup(p);
    sprintf(p, "o%s", q);
    xs_free(q);
    return p;
}

void md_gen_order(char type)
{
    char* buf = g_md.order_gen;
    int year, month, date, hour, minute, second;
    xs_get_local_time(&year, &month, &date, &hour, &minute, &second);
    sprintf(buf, "%c%04d%02d%02d%02d%02d%02d.%s", type, year, month, date, hour, minute, second, g_md.id);

    char order_file[256];
    sprintf(order_file, "%s/%s", MD_ORDER_DIR, buf);
    g_md.cur_order = xs_vec_add(g_md.order_list, xs_strdup(order_file));
}

char* md_get_tmp_file()
{
    static char p[256];
    strcpy(p, "/var/run/tempXXXXXX");
    int ret = mkstemp(p);
    //  assert(ret > 0);
    close(ret);
    return p;
}

double calc_total_money(BOOL calc)
{
    /* do not calc AND daily card, return the daily if g_md.book has count */
    if(!calc && !md_is_normal_card())
    {
        if(g_md.book->count > 0)
            return strtod(g_md.daily, NULL);

        return 0;
    }

    /* calc total item and sum them */
    md_book_node_t* booknode;
    double total = 0;
    int i = 0;

    if(g_md.book == NULL)
        return 0;

    for(i = 0; i < g_md.book->count; i++)
    {
        booknode = xs_vec_get(g_md.book, i);
        if(booknode != NULL)
        {
            total += strtod(booknode->price, NULL) * atoi(booknode->count);
        }
    }

    return total;
}

char* md_init()
{
    mkdir(MD_ORDER_DIR, 0777);
    memset(&g_md, 0, sizeof(md_t));

    /* need communi cation with daemon */
#if 0
    g_md.sockfd = xs_sock_connect(MD_DAEMON_GUI_PORT, "127.0.0.1");
    if(g_md.sockfd < 0)
    {
        xs_loge("connect to daemon error");
        exit(1);
    }
#endif
    g_md.timerid = 100;

    char* ret;
    ret = md_load_config_file();
    if(ret ) return ret;
    ret = init_dinner_list();
    if(ret) return ret;
    ret = init_oper_list();
    if(ret)  return ret;
    return NULL;
}

void show_error(char* error, md_win_status_t status, int forbit_input)
{
    md_set_info(error);
    md_save_status();
    md_set_status(status);


    if(forbit_input)
        md_disable_key();
    else
        md_enable_key();

    md_update_wnd(NULL);
}

void show_help()
{
    int ret;
    int fd;
    char buf[256];
#if 0
    fd = open("/dev/tlc549",O_RDWR);
    read(fd, &ret, 4);
    //    sleep(1);
    close(fd);
#endif

    fd = open("/dev/tlc549",O_RDWR);
    if(fd == -1)
    {
        md_format_battery(-1, buf);
    }
    else
    {
        read(fd, &ret, 4);
        //   sprintf(buf,"battry = %d%\n",(int)(((double)(ret)/255)*100));
        md_format_battery(ret, buf);
        //     sleep(1);
        close(fd);
    }

    show_error(buf, MD_HELP, 0);
}

void show_error2(md_error_t error, md_win_status_t status, int forbit_input)
{
    show_error(md_get_error(error), status, forbit_input);
}

#define show_error_by_id show_error2

void md_free_table(char** table, int count)
{
    int i;
    for(i=0; i<count; i++)
    {
        if(table[i])
            xs_free(table[i]);
    }
    xs_free(table);
}

void drawBottom(int area, char* buf, HDC dc, RECT* rc, BOOL border, int style)
{
    int i;
    /* default split 4 rect */
    for(i=0; i<4; i++)
    {
        memcpy(&g_md.rc[i], rc, sizeof(RECT));
        g_md.rc[i].left = i*78+MD_MARGIN;
        g_md.rc[i].right = g_md.rc[i].left + 68;

        g_md.rc[i].top += 10;
        g_md.rc[i].bottom = g_md.rc[i].top + 30;
    }

    if(g_md.status == MD_OPER_SEL)
    {
        //    g_md.rc[0].right = g_md.rc[0].left + 100;
        g_md.rc[1].left = g_md.rc[0].right + MD_MARGIN;
        g_md.rc[1].right = MD_SCREEN_WIDTH - MD_MARGIN;
    }

#if 0
    if(g_md.status == MD_BUY)
    {
        g_md.rc[0].right += 30;
        g_md.rc[1].left = g_md.rc[0].right + MD_MARGIN;
        g_md.rc[1].right = MD_SCREEN_WIDTH - MD_MARGIN;
    }
#endif

    if(border)
        Rectangle(dc, g_md.rc[area].left, g_md.rc[area].top, g_md.rc[area].right, g_md.rc[area].bottom);

    DrawText(dc, buf, -1, &g_md.rc[area], style/*DT_SINGLELINE|DT_VCENTER|DT_CENTER*/);
}

void md_oper_sel_draw(HDC dc, RECT* rc)
{
    int total = g_md.item_total = g_md.oper->count;

    char** table = xs_malloc(sizeof(char*) * total);
    int i;
    for(i=0; i<total; i++)
    {
        md_oper_t* model = xs_vec_get(g_md.oper, i);
        table[i] = xs_malloc(240);
        sprintf(table[i], "[%02d] %s", i+1, model->opername);
    }

    drawTable(dc, rc, table, total, PER_PAGE1, TRUE);

    md_free_table(table, total);

    char* display = xs_strdup("************************");
    display[strlen(g_md.input)] = 0;

    drawBottom(0, md_get_res(MD_RES_ENTER_PASSWORD), dc, rc, FALSE, DT_SINGLELINE|DT_VCENTER);
    drawBottom(1, display, dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
    xs_free(display);
}

void md_redraw_item(int index)
{
    RECT* rc = &g_md.rc_item[index%g_md.cur_perpage];
    rc->left --;
    rc->right ++;
    rc->top --;
    rc->bottom ++;
    md_update_wnd(rc);
}

BOOL md_handle_cursor(WPARAM wParam)
{
    int old_page;
    int new_page;
    int i;

    /* if cur_perpage is zero, mean I do not handle this msg */
    if(g_md.cur_perpage == 0)
        return FALSE;

    switch(wParam)
    {
        case SCANCODE_CURSORBLOCKUP:
            if(g_md.cur_item_sel > 0)
            {
                old_page = g_md.cur_item_sel/g_md.cur_perpage;
                g_md.cur_item_sel --;
                md_redraw_item(g_md.cur_item_sel+1);
                break;
            }
            return TRUE;

        case SCANCODE_CURSORBLOCKDOWN:
            if(g_md.cur_item_sel < g_md.item_total-1)
            {
                old_page = g_md.cur_item_sel/g_md.cur_perpage;
                g_md.cur_item_sel++;
                md_redraw_item(g_md.cur_item_sel-1);
                break;
            }
            return TRUE;
        default:
            return FALSE;
    }

    new_page = g_md.cur_item_sel/g_md.cur_perpage;
    if(new_page == old_page)
    {
        md_redraw_item(g_md.cur_item_sel);
    }
    else
    {
        for(i=0; i<g_md.cur_perpage; i++)
        {
            md_redraw_item( i);
        }
    }
    return TRUE;
}

BOOL md_handle_page(WPARAM wParam)
{
    int page;
    int cur_sel;
    int i;

    if(g_md.cur_perpage == 0)
        return FALSE;

    page = g_md.cur_item_sel/g_md.cur_perpage;

    if(wParam != SCANCODE_CURSORBLOCKRIGHT
            && wParam != SCANCODE_CURSORBLOCKLEFT)
        return FALSE;

    if(wParam == SCANCODE_CURSORBLOCKLEFT)
    {
        /* prev page */ 
        if(page == 0)
            return FALSE;

        page--;
        g_md.cur_item_sel = page*g_md.cur_perpage;
    }
    if(wParam == SCANCODE_CURSORBLOCKRIGHT)
    {
        page++;
        cur_sel = page*g_md.cur_perpage;
        if(cur_sel >= g_md.item_total)
            return FALSE;
        g_md.cur_item_sel = cur_sel;
    }

    for(i=0; i<g_md.cur_perpage; i++)
    {
        md_redraw_item( i);
    }

    return TRUE;
}

void md_set_cur_oper()
{
    xs_vec_t* vec = g_md.oper;
    int index = g_md.cur_item_sel;
    md_oper_t* oper = xs_vec_get(vec, index);
    if(g_md.username)
        xs_free(g_md.username);
    g_md.username = xs_strdup(oper->opername);
    g_md.cur_oper_id = index;
}

void md_oper_sel_key(WPARAM wParam)
{
    /* handle input */
    if(md_handle_input(wParam))
        return;

    /* handle UP/DOWN cursor */
    if(md_handle_cursor(wParam))
        return;

    if(wParam == SCANCODE_ENTER)
    {
        md_set_cur_oper();
        md_oper_t* oper = xs_vec_get(g_md.oper, g_md.cur_oper_id);

        if(strcmp(oper->password, md_input()) == 0)
        {
            md_set_status_and_redraw(MD_READCARD);
            return;
        }
        else
        {
            show_error_by_id(MD_ERROR_PASSWORD, MD_ERROR, 0);
        }
    }
}

void md_welcome_draw(HDC dc, RECT* rc)
{
    /* draw welcome.jpg */
    static BITMAP wel;

    if(LoadBitmap(dc, &wel, "wel.jpg") != 0)
    {
        /* if not picture, show text only */
        drawTitle(dc, rc);
        drawSingleText(dc, rc, md_get_res(MD_RES_WELCOME));
        return;
    }

    FillBoxWithBitmap(dc, rc->left, rc->top, rc->right, rc->bottom, &wel);
    UnloadBitmap(&wel);
}

void md_welcome_key(WPARAM wParam)
{
    wParam = wParam;
    /* any key enter MD_OPER_SEL */
    md_set_status_and_redraw(MD_OPER_SEL);
}

void md_readcard_draw(HDC dc, RECT* rc)
{
#define MD_READ_CARD_MENU_ITEM 1
    char* p[10];
    int i;
    for(i=0; i<MD_READ_CARD_MENU_ITEM; i++)
    {
        p[i] = md_get_res(MD_RES_READ_CARD_TIP1+i);
    }
    drawTable(dc, rc, p, MD_READ_CARD_MENU_ITEM, PER_PAGE1, FALSE);
    //   drawSingleText(dc, rc, md_get_res(MD_RES_READCARD_TIP));
}

/* find all the order of this custom, and save the file path in g_md.order_list */
void md_init_order_list()
{
    // order format: o20130211105931.12     this is book order
    // order fotmat: u20130211105931.12     this is buy order
    char temp[128];
    glob_t result;
    int ret;
    sprintf(temp, MD_ORDER_DIR"/o%s*.%s", md_date(), g_md.id);

    ret = glob(temp, 0, NULL, &result);
    if(ret < 0)
    {
        assert(0);
    }

    md_order_list_clear();
    g_md.order_list = xs_vec_create(8);
    xs_logd("glob count=%d", result.gl_pathc);
    for(ret = 0; ret<(int)result.gl_pathc; ret++)
    {
        xs_vec_add(g_md.order_list, xs_strdup(result.gl_pathv[ret]));
        xs_logd("%s", result.gl_pathv[ret]);
    }

    globfree(&result);
}

void md_readcard_key(WPARAM wParam)
{
    int ret;
    if(wParam == SCANCODE_ENTER)
    {
        ret = read_card(&g_md.cardno, g_md.card_buff, NULL, NULL);
        if(ret != 0)
        {
            md_set_read_card_error(ret);
            return;
        }

        if(is_valid_card(g_md.card_buff) != 0)
        {
            md_set_info(md_get_error(MD_ERROR_INVALID_CARD));
            return;
        }

        read_card_data(g_md.card_buff, g_md.balance,
                g_md.name, g_md.zone,
                g_md.dept, g_md.num,
                g_md.id, g_md.orderid, g_md.daily);

        md_init_order_list();
        md_input_clear();
        md_set_status_and_redraw(MD_READCARD_INFO);
        return;
    }

    if(wParam == MD_KEY_CANCEL)
    {
        md_input_clear();
        g_md.cur_item_sel = g_md.cur_oper_id;
        
        md_set_status_and_redraw(MD_OPER_SEL);
        return ;
    }
}

BOOL md_is_book_order(const char* order_file, char* order)
{
    char* p = xs_strdup(order_file);
    char* z;
    char* result;

    result = strtok(p, "/");
    while(result)
    {
        z = result;
        result = strtok(NULL, "/");
    }

    strcpy(order, z);
    xs_logd("order is %s", order);

    xs_free(p);
    strcpy(g_md.cancel_order, order);
    return *order == 'o'; /* o is book, u is buy */
}

int md_find_book_order(char* order)
{
    int i;
    char* order_file;
    for(i=0; i<g_md.order_list->count; i++)
    {
        order_file = xs_vec_get(g_md.order_list, i);
        if(md_is_book_order(order_file, order))
        {
            return i;
        }
    }
    return -1;
}

void md_readcard_info_draw(HDC dc, RECT* rc)
{
#define MD_READCARD_INFO_FORMAT "%-10s: %s\n"
#define MD_READCARD_TIP_COUNT 2
    char* buf = xs_malloc(2030);
    char* pos = xs_malloc(2030);
    char* book_file = xs_malloc(240);
    char** table = xs_zalloc(sizeof(char*)*10);
    int i;
    g_md.item_total = 0;

    sprintf(buf, MD_READCARD_INFO_FORMAT, md_get_res(MD_RES_ACCOUNT_NAME), g_md.name);
    table[g_md.item_total++] = xs_strdup(buf);

    sprintf(pos, "%s/%s/%s", g_md.zone, g_md.dept, g_md.num);
    sprintf(buf, MD_READCARD_INFO_FORMAT, md_get_res(MD_RES_ACCOUNT_POS), pos);
    xs_free(pos);
    table[g_md.item_total++] = xs_strdup(buf);

    /* find book order, not buy order in orders */
    g_md.cur_order = md_find_book_order(g_md.order_gen);

    if(g_md.cur_order >= 0) /* have load from file success */
    {
        sprintf(buf, MD_READCARD_INFO_FORMAT, md_get_res(MD_RES_ACCOUNT_BOOK_STATUS), md_get_res(MD_RES_ACCOUNT_HAVE_BOOK));
        table[g_md.item_total++] = xs_strdup(buf);
    }
    else
    {
        sprintf(buf, MD_READCARD_INFO_FORMAT, md_get_res(MD_RES_ACCOUNT_BOOK_STATUS), md_get_res(MD_RES_ACCOUNT_HAVE_NOT_BOOK));
        table[g_md.item_total++] = xs_strdup(buf);
    }

    /* normal card */
    if(strtod(g_md.daily, NULL) < 0.001)
    {
        sprintf(buf, MD_READCARD_INFO_FORMAT, md_get_res(MD_RES_ACCOUNT_TYPE), md_get_res(MD_RES_ACCOUNT_TYPE_COMMON));
        table[g_md.item_total++] = xs_strdup(buf);
    }
    else
    {
        sprintf(buf, MD_READCARD_INFO_FORMAT, md_get_res(MD_RES_ACCOUNT_TYPE), md_get_res(MD_RES_ACCOUNT_TYPE_DAILY));
        table[g_md.item_total++] = xs_strdup(buf);
        sprintf(buf, MD_READCARD_INFO_FORMAT, md_get_res(MD_RES_ACCOUNT_TYPE_DAILY_MONEY), g_md.daily);
        table[g_md.item_total++] = xs_strdup(buf);
    }

    sprintf(buf, MD_READCARD_INFO_FORMAT, md_get_res(MD_RES_ACCOUNT_BALANCE), g_md.balance);
    table[g_md.item_total++] = xs_strdup(buf);

    for(i=0; i<MD_READCARD_TIP_COUNT; i++)
    {
        table[g_md.item_total++] = xs_strdup(md_get_res(MD_RES_READCARD_INFO_TIP1+i));
    }

    drawTable(dc, rc, table, g_md.item_total, PER_PAGE1, FALSE);

    xs_free(buf);
    xs_free(book_file);
    md_free_table(table, g_md.item_total);
}

BOOL md_is_valid_account()
{
    //g_md.cardno;
    FILE* fblack = fopen(MD_BLACK_FILE, "r");
    if(fblack == NULL)
        return TRUE;

    char buf[32];

    while(fgets(buf, 32, fblack))
    {
        unsigned long int one = strtoul(buf, NULL, 10);
        if(one == g_md.cardno)
        {
            fclose(fblack);
            return TRUE;
        }
    }

    fclose(fblack);
    return FALSE;
}

void md_readcard_info_key(WPARAM wParam)
{
    if(wParam == SCANCODE_ENTER)
    {
        /* first check if it is in write list */
        if(!md_is_valid_account())
        {
            show_error_by_id(MD_ERROR_ON_BLACK_LIST, MD_ERROR, 0);
            return;
        }

        g_md.cur_item_sel = 0;
        md_input_clear();
        md_set_status_and_redraw(MD_BOOKMENU);
    }
    if(wParam == MD_KEY_CANCEL)
    {
        md_set_status_and_redraw(MD_READCARD);
        return;
    }
    if(wParam == MD_KEY_F2)
    {
        md_set_status_and_redraw(MD_MODIFY_ZONE);
        return;
    }
}

void md_error_draw(HDC dc, RECT* rc)
{
    drawSingleText(dc, rc, g_md.info);
}

void md_error_key(WPARAM wParam)
{
    wParam = wParam;
    md_load_status_and_redraw();
}

void md_bookmenu_draw(HDC dc, RECT* rc)
{
#define BOOKMENU_ITEM 3
    char* menu[10];
    int i;
    for(i=0; i<BOOKMENU_ITEM; i++)
    {
        menu[i] = md_get_res(MD_RES_BOOK_MENU1+i);
    }

    drawTable(dc, rc, menu, i, PER_PAGE1, TRUE);
}

void md_bookmenu_key(WPARAM wParam)
{
    if(md_handle_cursor(wParam))
        return;

    if(wParam == MD_KEY_CANCEL)
    {
        md_set_status_and_redraw(MD_READCARD);
        return;
    }

    if(wParam == SCANCODE_ENTER)
    {
        if(g_md.cur_item_sel == 0)
        {
            goto SELECT1;
        }
        else if(g_md.cur_item_sel == 1)
        {
            goto SELECT2;
        }
        else if(g_md.cur_item_sel == 2)
        {
            goto SELECT3;
        }
        else if(g_md.cur_item_sel == 3)
        {
            goto SELECT4;
        }
        return;
    }

    if(wParam == SCANCODE_1)
    {
SELECT1:
        g_md.business = MD_BUSINESS_BOOK;

        /* if g_md.book not null, clean it */
        md_book_clear();

        /* load order if exist */
        if(g_md.cur_order >= 0)
        {
            g_md.book = mw_load_file_to_vec(xs_vec_get(g_md.order_list, g_md.cur_order));
            /* calc money,  do not calc, mean, if daily card, total is daily money*/
            g_md.total_money_old = calc_total_money(FALSE);
        }
        else
        {
            /* if not exist, create vec for g_md.book */
            md_gen_order('o');
            g_md.book = xs_vec_create(8);
            g_md.total_money_old = 0;
        }

        g_md.cur_item_sel = 0;

        md_set_status_and_redraw(MD_BOOK);
        return;
    }
    if(wParam == SCANCODE_2)
    {
SELECT2:
        g_md.business = MD_BUSINESS_CANCEL_BOOK;
        md_book_clear();
        if(g_md.cur_order >= 0)
        {

            g_md.book = mw_load_file_to_vec(xs_vec_get(g_md.order_list, g_md.cur_order));
        }
        else
        {
            show_error_by_id(MD_ERROR_NO_ORDER, MD_ERROR, 0);
            return;
        }

        g_md.total_money_old = calc_total_money(FALSE);
        g_md.cur_item_sel = 0;
        md_set_status_and_redraw(MD_CANCEL_BOOK);
        return;
    }
    if(wParam == SCANCODE_3)
    {
SELECT3:
        md_book_clear();
        md_gen_order('u');
        g_md.book = xs_vec_create(8);

        g_md.total_money_old = 0;

        g_md.business = MD_BUSINESS_BUY;
        g_md.cur_item_sel = 0;
        md_set_status_and_redraw(MD_BUY);
        return;
    }
    if(wParam == SCANCODE_4)
    {
SELECT4:
#if 0
        g_md.business = MD_BUSINESS_CANCEL_BUY;
        md_set_status_and_redraw(MD_CANCEL_BUY);
#endif
        return;
    }
}

void book_food_buffer(char* p, md_book_node_t* wf)
{
    sprintf(p, MD_BOOK_DRAW_FOTMAT, md_get_res
            (atoi(wf->cb)+MD_RES_CB1-1), wf->dinner, wf->price, wf->count);
}

void drawBookingVec(HDC dc, RECT* rc)
{
    char** table = xs_malloc(sizeof(char*) * g_md.book->count);

    int i;
    xs_logd("draw item g_md.book->count=%d" , g_md.book->count);
    for(i=0; i<g_md.book->count; i++)
    {
        md_book_node_t* model = xs_vec_get(g_md.book, i);
        table[i] = xs_malloc(500);
        book_food_buffer(table[i], model);
        xs_logd("draw item g_md.book->count=%d" , g_md.book->count);
    }

    drawTable(dc, rc, table, g_md.book->count, PER_PAGE1-1, TRUE);

    md_free_table(table, g_md.book->count);
}

void drawHeader(HDC dc, RECT* rc1, char* fmt, ...)
{
    RECT rc;
    memcpy(&rc, rc1, sizeof(rc));
    rc.left += MD_MARGIN;
    rc.bottom = rc.top + MD_HEADER_HEIGHT;
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    DrawText(dc, buf, -1, &rc, DT_SINGLELINE|DT_VCENTER);
    rc1->top = rc.bottom;
}

void md_book_draw_detail(HDC dc, RECT* rc)
{

    drawHeader(dc, rc, MD_BOOK_DRAW_FOTMAT, md_get_res(MD_RES_BOOK_HEADER1)
            , md_get_res(MD_RES_BOOK_HEADER2), md_get_res(MD_RES_BOOK_HEADER3)
            , md_get_res(MD_RES_BOOK_HEADER4));

    drawBookingVec(dc, rc);

    return;
}

void md_book_draw(HDC dc, RECT* rc)
{
    md_book_draw_detail(dc, rc);

    char buf[32];
    g_md.total_money = calc_total_money(TRUE);
    sprintf(buf,"%g", g_md.total_money);

    drawBottom(0, md_get_res(MD_RES_TIP_INPUT_DINNER), dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
    drawBottom(1, g_md.input, dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
    drawBottom(2, md_get_res(MD_RES_TIP_TOTAL_MONEY), dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
    drawBottom(3, buf , dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER);

}

int md_find_food_in_booking_list(char* id)
{
    md_book_node_t* booknode;
    int i;
    char buf[64];
    for(i=0; i<g_md.book->count; i++)
    {
        booknode = xs_vec_get(g_md.book, i);
        sprintf(buf, "%s.%s", booknode->cb, booknode->dinner_id);
        if(strcmp(buf, id) == 0)
        {
            return i;
        }
    }
    return -1;
}
int xs_do_print(char *pzText);
int open_printer(void);
int close_printer();
#define __EMPTY_LINE__  " "
#define _xs_buff_len_   1024
//#define MD_BOUNDRY "----------------------------------------------------------------"
  #define MD_BOUNDRY "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
#define MD_PRINT_LINE(fmt, ...) do{\
    static char buff[_xs_buff_len_] = {0}; \
    sprintf(buff, fmt, ##__VA_ARGS__);\
    xs_do_print(buff); \
    xs_do_print(__EMPTY_LINE__); /*print more empty line*/ \
}while(0)

#define MD_PRINT_BOUNDRY() xs_do_print(MD_BOUNDRY);

#define MD_PRINT_TAIL() do{int i;for(i=0;i<5; i++)xs_do_print(__EMPTY_LINE__); }while(0)
void md_print_weekfood()
{
    char buf[64];
    if(open_printer() != 0)
        return;


    int i;
    MD_PRINT_LINE("%-6s%-16s%-5s%s", md_get_res(MD_RES_104),
            md_get_res(MD_RES_105),
            md_get_res(MD_RES_106),
            md_get_res(MD_RES_107));
    for(i=0; i<g_md.dinner_all->count; i++)
    {
        md_week_food_t* wf = xs_vec_get(g_md.dinner_all, i);
        sprintf(buf, "%s.%s", wf->cb, wf->id);
        MD_PRINT_LINE("%-6s%-16s%-5s%s", buf, wf->name, wf->price, wf->type);
    }


    MD_PRINT_TAIL();

    close_printer();
}

void md_book_add_dinner(char* key)
{
    char buf[32];
    int bookid;
    int bookcount;
    md_week_food_t* weekfood;
    md_book_node_t* booknode;
    weekfood = md_find_week_food(g_md.dinner_all, key);
    if(weekfood == NULL)
    {
        show_error(md_get_error(MD_ERROR_NOT_FOUND_DINNER), MD_ERROR, 0);
        return;
    }

    bookid = md_find_food_in_booking_list(key);
    if(bookid != -1)
    {
        //  g_md.cur_item_sel = bookid;

        booknode = xs_vec_get(g_md.book, bookid);
        if(booknode == NULL)
            return;
        bookcount = atoi(booknode->count);
        bookcount ++;
        xs_free(booknode->count);
        booknode->count = xs_strdup(xs_itoa_r(bookcount, buf));

        return;
    }

    booknode = xs_model(md_book_node_t);
    booknode->account = xs_strdup(g_md.id); /* need read card */
    booknode->card = xs_strdup(xs_itoa(g_md.cardno));  /* need read card */
    booknode->count = xs_strdup("1");
    booknode->date = xs_strdup(g_md.book_date);
    booknode->dinner = xs_strdup(weekfood->name);
    booknode->dinner_id = xs_strdup(weekfood->id);
    booknode->oper = xs_strdup(g_md.username);
    booknode->orderid = xs_strdup(g_md.order_gen); /* determine when card on */
    booknode->price = xs_strdup(weekfood->price);
    booknode->type = xs_strdup(weekfood->type);
    booknode->cb = xs_strdup(weekfood->cb);
    booknode->book_type1 = xs_strdup(MD_ORDER_BOOK);
    booknode->zone = xs_strdup(g_md.zone);
    booknode->dept = xs_strdup(g_md.dept);
    booknode->bed = xs_strdup(g_md.num); 
    xs_vec_add(g_md.book, booknode);

    xs_logd("find food, add vec id=%s, name=%s", booknode->dinner_id, booknode->dinner);
    //    g_md.cur_item_sel = xs_vec_add(g_md.book, booknode);
}

void md_book_key(WPARAM wParam)
{
    md_book_node_t* booknode;
    md_week_food_t* weekfood;
    int bookcount;
    int bookid;
    int oldsel = g_md.cur_item_sel;
    char buf[32];
#if 0
    if(wParam == MD_KEY_CANCEL)
    {
        md_set_status_and_redraw(MD_READCARD);
        return;
    }
#endif

    if(md_handle_input(wParam))
        return;

    if(md_handle_cursor(wParam))
        return;

    switch(wParam)
    {
        case SCANCODE_ENTER:
            if(md_input_is_empty() && g_md.book->count)
            {
                // TODO need some prepare, such as path, orderid
                md_set_status_and_redraw(MD_WRITECARD);
                return;
            }

            if(md_input_is_empty())
            {
                show_error_by_id(MD_ERROR_DINNER_KEY_EMPTY, MD_ERROR, 0);
                return;
            }

            memset(buf, 0, sizeof(buf));
            strcpy(buf, md_input());
            weekfood = md_find_week_food(g_md.dinner_all, buf);
            if(weekfood == NULL)
            {
                show_error(md_get_error(MD_ERROR_NOT_FOUND_DINNER), MD_ERROR, 0);
                break;
            }

            bookid = md_find_food_in_booking_list(buf);
            if(bookid != -1)
            {
                md_redraw_item(oldsel);
                g_md.cur_item_sel = bookid;
                md_input_clear();
                md_update_wnd(&g_md.rc[1]);
                goto PLUS_DINNER;
            }

            booknode = xs_model(md_book_node_t);
            booknode->account = xs_strdup(g_md.id); /* need read card */
            booknode->card = xs_strdup(xs_itoa(g_md.cardno));  /* need read card */
            booknode->count = xs_strdup("1");
            booknode->date = xs_strdup(g_md.book_date);
            booknode->dinner = xs_strdup(weekfood->name);
            booknode->dinner_id = xs_strdup(weekfood->id);
            booknode->oper = xs_strdup(g_md.username);
            booknode->orderid = xs_strdup(g_md.order_gen); /* determine when card on */
            booknode->price = xs_strdup(weekfood->price);
            booknode->type = xs_strdup(weekfood->type);
            booknode->cb = xs_strdup(weekfood->cb);
            booknode->book_type1 = xs_strdup(MD_ORDER_BOOK);
            booknode->zone = xs_strdup(g_md.zone);
            booknode->dept = xs_strdup(g_md.dept);
            booknode->bed = xs_strdup(g_md.num); 

            xs_logd("find food, add vec id=%s, name=%s", booknode->dinner_id, booknode->dinner);
            g_md.cur_item_sel = xs_vec_add(g_md.book, booknode);

            md_input_clear();
            md_update_wnd(&g_md.rc[1]);
            md_update_wnd(&g_md.rc[3]);
            md_redraw_item(g_md.cur_item_sel);
            md_redraw_item(oldsel);

            break;

        case SCANCODE_CURSORBLOCKRIGHT:
PLUS_DINNER:
            booknode = xs_vec_get(g_md.book, g_md.cur_item_sel);
            if(booknode == NULL)
                break;
            bookcount = atoi(booknode->count);
            bookcount ++;
            xs_free(booknode->count);
            booknode->count = xs_strdup(xs_itoa_r(bookcount, buf));
            md_redraw_item(g_md.cur_item_sel);
            /* calc money */
            md_update_wnd(&g_md.rc[3]);

            break;

        case SCANCODE_CURSORBLOCKLEFT:

            booknode = xs_vec_get(g_md.book, g_md.cur_item_sel);
            if(booknode == NULL)
                break;
            bookcount = atoi(booknode->count);
            bookcount --;
            if(bookcount > 0)
            {
                md_redraw_item(g_md.cur_item_sel);
                xs_free(booknode->count);
                booknode->count = xs_strdup(xs_itoa_r(bookcount, buf));
            }
            else
            {
                xs_vec_rm(g_md.book, g_md.cur_item_sel);
                if(g_md.cur_item_sel < g_md.book->count)
                {

                }
                else
                {
                    if(g_md.cur_item_sel > 0)
                        g_md.cur_item_sel --;
                }
                int i;
                for(i=g_md.cur_item_sel%g_md.cur_perpage; i<g_md.cur_perpage; i++)
                {
                    md_redraw_item(i);
                }
            }
            md_update_wnd(&g_md.rc[3]);
            break;
        case MD_KEY_F2:
            g_md.cur_item_sel = 0;
            md_set_status_and_redraw(MD_WEEKFOOD);
            break;
        case MD_KEY_F3:
            md_print_weekfood();
            break;
        default:
            break;
    }
}

void md_cancelbook_draw(HDC dc, RECT* rc)
{
    md_book_draw_detail(dc, rc);
}

void md_cancelbook_key(WPARAM wParam)
{
    if(md_handle_cursor(wParam))
        return;
    if(wParam == MD_KEY_CANCEL)
    {
        md_set_status_and_redraw(MD_READCARD);
        return;
    }
    if(SCANCODE_ENTER == wParam)
    {
        // TODO need what prepare?
        md_set_status_and_redraw(MD_WRITECARD);
        return;
    }
}

char* md_get_buy_time(int buy_time)
{
    static char buf[32];
    int hour = buy_time / 2;
    int minute = buy_time % 2 * 30;
    sprintf(buf, "%02d:%02d", hour, minute);
    return buf;
}

int md_get_now_buy_time()
{
    int year, month, mday, hour, minute, second;
    xs_get_local_time(&year, &month, &mday, &hour, &minute, &second);
    return hour * 2 + minute / 30;
}

void md_buy_draw(HDC dc, RECT* rc)
{
    drawBottom(0, md_get_res(MD_RES_INPUT_MONEY_TIP), dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_CENTER);
    drawBottom(1, md_input(), dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
    drawBottom(2, md_get_res(MD_RES_TIME), dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_CENTER);

    /*g_md.buy_time*/
    if(g_md.buy_time == 0)
    {
        g_md.buy_time = md_get_now_buy_time() + 1;
    }
    
    drawBottom(3, md_get_buy_time(g_md.buy_time), dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_CENTER);
}

void md_buy_key(WPARAM wParam)
{
#if 0
    if(wParam == MD_KEY_CANCEL)
    {
        md_set_status_and_redraw(MD_READCARD);
        return;
    }
#endif
    int now_buy_time = md_get_now_buy_time();

    if(md_handle_input(wParam))
        return;

    switch(wParam)
    {
        case SCANCODE_CURSORBLOCKUP:
            if(g_md.buy_time < 47)
            {
                g_md.buy_time++;
                md_update_rc(3);
            }

            break;
        case SCANCODE_CURSORBLOCKDOWN:
            if(g_md.buy_time > now_buy_time + 1)
            {
                g_md.buy_time--;
                md_update_rc(3);
            }
            break;
    }
    
    if(wParam == SCANCODE_ENTER)
    {
        md_book_node_t* booknode = xs_model(md_book_node_t);
        booknode = xs_model(md_book_node_t);
        booknode->account = xs_strdup(g_md.id); /* need read card */
        booknode->card = xs_strdup(xs_itoa(g_md.cardno));  /* need read card */
        booknode->count = xs_strdup("1");
        booknode->date = xs_strdup(g_md.buy_date);
        //      booknode->dinner = xs_strdup(weekfood->name);
        booknode->dinner_id = xs_strdup("0"); /*  */
        booknode->oper = xs_strdup(g_md.username);
        booknode->orderid = xs_strdup(g_md.order_gen); /* determine when card on */
        booknode->price = xs_strdup(md_input());
        //      booknode->type = xs_strdup(weekfood->type);
        //      booknode->time = xs_strdup(weekfood->time);
        booknode->book_type1 = xs_strdup(MD_ORDER_BUY);
        booknode->zone = xs_strdup(g_md.zone);
        booknode->dept = xs_strdup(g_md.dept);
        booknode->bed = xs_strdup(g_md.num); 
        booknode->time = xs_strdup(md_get_buy_time(g_md.buy_time));

        md_book_create(8);
        md_book_append(booknode);

        // TODO need what prepare?
        md_set_status_and_redraw(MD_WRITECARD);
        return;
    }
}

#if 0
void md_cancel_draw(HDC dc, RECT* rc)
{

}

void md_cancel_key(WPARAM wParam)
{

}
#endif

void md_writecard_draw(HDC dc, RECT* rc)
{
#define MD_WRITE_CARD_MENU_ITEM 3
    char* p[10];
    int i;
    for(i=0; i<MD_WRITE_CARD_MENU_ITEM; i++)
    {
        p[i] = xs_strdup(md_get_res(MD_RES_WRITE_CARD_TIP1+i));
    }
    drawTable(dc, rc, p, MD_WRITE_CARD_MENU_ITEM, PER_PAGE1, FALSE);
}

/* be careful of Reentrant */
char* md_writecard_book_calc_balance(double money)
{
    /* set new balance */
    static char balance[48];
    double cost = money - g_md.total_money_old;
    sprintf(balance, "%g", strtod(g_md.balance, NULL) - cost);
    return balance;
}

void md_set_last_order()
{
    int i;
    for(i=0; i<g_md.order_list->count; i++)
    {
        if(i != g_md.cur_order)
        {
            char* p = xs_vec_get(g_md.order_list, i);
            char* result;
            char* order;
            result = strtok(p, "/");
            while(result)
            {
                order = result;
                result = strtok(NULL, "/");
            }
            strcpy(g_md.orderid, order);

            return;
        }

    }
    *g_md.orderid = 'f';
    return;
}

int md_writecard_cancel_book()
{
    int ret;
    /* write card */
    char* book_file = md_order_file();
    char* book_file_tmp = md_get_tmp_file();
    char* balance = md_writecard_book_calc_balance(0);
    char cmd[512];

    sprintf(cmd, "mv %s %s", book_file, book_file_tmp);
    system(cmd);

    md_set_last_order();

    ret = write_card_data(g_md.cardno, g_md.card_buff, balance, NULL, g_md.zone, g_md.dept, g_md.num, NULL, g_md.orderid, NULL);
    g_md.total_money_now = strtod(balance, NULL);
    if(ret != 0)
    {
        show_error_by_id(MD_ERROR_WRITE_CARD_ERROR, MD_ERROR, 0);
        sprintf(cmd, "mv %s %s", book_file_tmp, book_file);
        system(cmd);
        return -1;
    }

    /* delete book file */
    unlink(book_file_tmp);


    // TODO need clear?
    md_set_status_and_redraw(MD_READCARD);
    return 0;
}

int md_writecard_book()
{
    int ret;
    /* write card */
    char* book_file = md_order_file();
    char* book_file_tmp = md_get_tmp_file();
    char* balance;

    /* buy and book is same function */
    if(g_md.business == MD_BUSINESS_BOOK)
        balance = md_writecard_book_calc_balance(calc_total_money(FALSE));
    else
        balance = md_writecard_book_calc_balance(strtod(md_input(), NULL));

    strcpy(g_md.orderid, g_md.order_gen);
    g_md.total_money_now = strtod(balance, NULL);
    if(g_md.total_money_now < 0.0)
    {
        show_error_by_id(MD_ERROR_NO_MONEY, MD_ERROR, 0);
        return -1;
    }

    ret = mw_save_file_from_vec(book_file_tmp, g_md.book);
    if(ret != 0)
    {
        show_error_by_id(MD_ERROR_WRITE_BOOK_FILE, MD_ERROR, 0);
        return -1;
    }


    ret = write_card_data(g_md.cardno, g_md.card_buff, balance, NULL, g_md.zone, g_md.dept, g_md.num, NULL, g_md.orderid, NULL);
    if(ret != 0)
    {
        show_error_by_id(MD_ERROR_WRITE_CARD_ERROR, MD_ERROR, 0);
        unlink(book_file_tmp);
        return -1;
    }

    xs_system( "mv %s %s", book_file_tmp, book_file);

    //  md_book_clear();
    // TODO need clean?
    md_set_status_and_redraw(MD_READCARD);
    return 0;
}

int md_writecard_buy()
{
    return md_writecard_book();
}

#define  PRINTER_NUM      32
#define PRINTER_MOMTOR    0xA0
#define PRINTER_TEM	  0xA1

static FILE *fp_zk;         //fp_zk字库文件指针
static int fp_fd;              //fp_fd打印机设备标识

int open_printer(void)
{
    fp_fd = open("/dev/printer",O_RDWR);//打开打印机设备
    if(fp_fd == -1)
    {
        printf("Open printer wrong!\r\n");
        return 1;
    }
    fp_zk = fopen("/opt/GT20L32S4W1-T.bin", "r");//打开字库文件(含GB2312和ASCII)
    if(fp_zk == NULL)
    {
        printf("Cannot open GT20L32S4W1 file!\n");
        return 2;
    }
    return 0;
}

int close_printer(void)
{
    fclose(fp_zk);      //关闭字库文件
    close(fp_fd);       //关闭打印机设备
    return 0;
}

int xs_do_print(char *pzText)
{

    unsigned char string[960][48];//(384/8=48)*(24*40=960)
    unsigned char n,QM,WM,i;
    int cur_h=0,cur_l=0;//cur_h当前列,cur_l当前行
    unsigned char temp1,temp2,temp3;//临时变量
    cur_h = 0;cur_l = 0;
    memset(string,0x00,46080);//清空打印缓冲区
    n = 0;
    while ((n < PRINTER_NUM ) && (*(pzText+n) > 0 ))
    {
        QM = *(pzText+n);
        if ( QM <= 0x80 )
        {
            if(cur_h > 93)
            {
                cur_h = 0;
                cur_l++;
            }
            else
            {
                fseek(fp_zk,0x01dff00+(QM-0x20)*48,0);
                for(i=0;i<24;i++)
                {
                    temp1 = getc(fp_zk);
                    temp2 = getc(fp_zk);
                    if(cur_h%2==0)
                    {
                        string[i+24*cur_l][(cur_h+0)/2] = (string[i+24*cur_l][(cur_h+0)/2] | temp1);
                        string[i+24*cur_l][(cur_h+1)/2] = (string[i+24*cur_l][(cur_h+1)/2] | temp1);
                        string[i+24*cur_l][(cur_h+2)/2] = (string[i+24*cur_l][(cur_h+2)/2] | temp2);
                    }
                    else
                    {
                        string[i+24*cur_l][(cur_h+0)/2] = (string[i+24*cur_l][(cur_h+0)/2] | (temp1/16));
                        string[i+24*cur_l][(cur_h+1)/2] = (string[i+24*cur_l][(cur_h+1)/2] | (temp1*16));
                        string[i+24*cur_l][(cur_h+2)/2] = (string[i+24*cur_l][(cur_h+2)/2] | (temp2/16));
                    }
                }
                cur_h = cur_h + 3;
            }
            n++;
        }
        else
        {
            if ( n+1 < PRINTER_NUM )
            {
                WM = *(pzText+n+1);
                if (WM > 0 )
                {
                    fseek(fp_zk,0x076f80+((QM-0xb0)*94+(WM-0xa1))*72,0);
                    for(i=0;i<24;i++)
                    {
                        temp1 = getc(fp_zk);
                        temp2 = getc(fp_zk);
                        temp3 = getc(fp_zk);
                        if(cur_h%2==0)
                        {
                            string[i+24*cur_l][(cur_h+0)/2] = (string[i+24*cur_l][(cur_h+0)/2] | temp1);
                            string[i+24*cur_l][(cur_h+1)/2] = (string[i+24*cur_l][(cur_h+1)/2] | temp1);
                            string[i+24*cur_l][(cur_h+2)/2] = (string[i+24*cur_l][(cur_h+2)/2] | temp2);
                            string[i+24*cur_l][(cur_h+3)/2] = (string[i+24*cur_l][(cur_h+3)/2] | temp2);
                            string[i+24*cur_l][(cur_h+4)/2] = (string[i+24*cur_l][(cur_h+4)/2] | temp3);
                            string[i+24*cur_l][(cur_h+5)/2] = (string[i+24*cur_l][(cur_h+5)/2] | temp3);
                        }
                        else
                        {
                            string[i+24*cur_l][(cur_h+0)/2] = (string[i+24*cur_l][(cur_h+0)/2] | (temp1/16));
                            string[i+24*cur_l][(cur_h+1)/2] = (string[i+24*cur_l][(cur_h+1)/2] | (temp1*16));
                            string[i+24*cur_l][(cur_h+2)/2] = (string[i+24*cur_l][(cur_h+2)/2] | temp2/16);
                            string[i+24*cur_l][(cur_h+3)/2] = (string[i+24*cur_l][(cur_h+3)/2] | temp2*16);
                            string[i+24*cur_l][(cur_h+4)/2] = (string[i+24*cur_l][(cur_h+4)/2] | temp3/16);
                            string[i+24*cur_l][(cur_h+5)/2] = (string[i+24*cur_l][(cur_h+5)/2] | temp3*16);
                        }
                    }
                    cur_h = cur_h + 6;
                    n++;
                }
                n++;
            }
            else n++;
        }
    }
#ifdef __XS_GREATER_PRINT__
    ioctl(fp_fd,PRINTER_MOMTOR,7000);//加快打印速度
    ioctl(fp_fd,PRINTER_TEM,10000);   //加热时间增加
#endif

    write(fp_fd, string, (cur_l+1)*1150);//填写打印缓冲区

#ifdef __XS_GREATER_PRINT__
    ioctl(fp_fd,PRINTER_MOMTOR,7000);//加快打印速度
    ioctl(fp_fd,PRINTER_TEM,7000);
#endif
    //write(fp_fd,string,(cur_l+1)*1150);//填写打印缓冲区
    return 0;

}

void print_ticket()
{

    md_book_node_t *booknode;
    int year, month, mday,hour,minute,second;
#if 0
    static char path[256] = {0};

    if(g_md.book)
        md_book_clear();

    sprintf(path, "%s/%s", MD_ORDER_DIR, g_md.orderid);
    g_md.book = mw_load_file_to_vec(path);
    if(g_md.book == NULL)
    {
        xs_logd("load order error!");
        return;
    }
#endif

    open_printer();

    /* print company */
    MD_PRINT_LINE("%s %s", g_md.company, md_get_res(MD_RES_ORDER_TICKET_TIP));

    xs_get_local_time(&year, &month, &mday, &hour, &minute, &second);
    MD_PRINT_LINE("%-8s%04d-%02d-%02d %02d:%02d:%02d", g_md.username, year, month, mday, hour, minute, second);


    //   MD_PRINT_LINE("%-6s%-6s%-6s%-6s", g_md.name, g_md.zone, g_md.dept, g_md.num);
    MD_PRINT_LINE("%s[%s/%s/%s]", g_md.name, g_md.zone, g_md.dept, g_md.num);
    if(*g_md.orderid == 'o')
        MD_PRINT_LINE("%s:%s", md_get_res(MD_RES_108), md_get_res(MD_RES_109));
    else if(*g_md.orderid == 'u')
        MD_PRINT_LINE("%s:%s", md_get_res(MD_RES_108), md_get_res(MD_RES_110));
    else
        MD_PRINT_LINE("%s:%s", md_get_res(MD_RES_108), md_get_res(MD_RES_111));
    MD_PRINT_LINE("%s:%s", md_get_res(MD_RES_103), g_md.orderid);

#define MD_ORDER_FORMAT "%-16s%-6s%-6s%s"
#define MD_BUY_FORMAT "%-10s%-10s%s"
//    MD_PRINT_LINE("%s", MD_BOUNDRY);
    MD_PRINT_BOUNDRY();
    if(*g_md.orderid == 'o')
        MD_PRINT_LINE(MD_ORDER_FORMAT, md_get_res(MD_RES_100), md_get_res(MD_RES_101), md_get_res(MD_RES_BOOK_HEADER1), md_get_res(MD_RES_102));
    else if(*g_md.orderid == 'u')
        MD_PRINT_LINE(MD_BUY_FORMAT, md_get_res(MD_RES_100), md_get_res(MD_RES_TOTAL_TIP), md_get_res(MD_RES_TIME));
        
    int i = 0;
    for(i = 0; i < g_md.book->count; i++)
    {
        booknode = xs_vec_get(g_md.book, i);
        if(booknode->dinner)
        {
            MD_PRINT_LINE(MD_ORDER_FORMAT, booknode->dinner,
                    booknode->price,
                    md_get_res(atoi(booknode->cb)+MD_RES_CB1-1),
                    booknode->count);
        }
        else
        {
            MD_PRINT_LINE(MD_BUY_FORMAT, md_get_res(MD_RES_BUY_TIP), booknode->price, md_get_buy_time(g_md.buy_time));
        }
    }

    MD_PRINT_BOUNDRY();
    MD_PRINT_LINE("%s: %g", md_get_res(MD_RES_TOTAL_TIP),  calc_total_money(*g_md.orderid == 'u'));
    MD_PRINT_LINE("%s: %g", md_get_res(MD_RES_LAST_TIP),  g_md.total_money_now);
    MD_PRINT_LINE( "%s", md_get_res(MD_RES_TICKET_TIP));

    MD_PRINT_TAIL();

    close_printer();

    md_book_clear();
}

void md_print_ticket_cancel_book()
{
    md_book_node_t *booknode;
    int year, month, mday,hour,minute,second;
    open_printer();

    /* print company */
    MD_PRINT_LINE("%s %s", g_md.company, md_get_res(MD_RES_ORDER_TICKET_TIP));

    xs_get_local_time(&year, &month, &mday, &hour, &minute, &second);
    MD_PRINT_LINE("%-8s%04d-%02d-%02d %02d:%02d:%02d", g_md.username, year, month, mday, hour, minute, second);


    //   MD_PRINT_LINE("%-6s%-6s%-6s%-6s", g_md.name, g_md.zone, g_md.dept, g_md.num);
    MD_PRINT_LINE("%s[%s/%s/%s]", g_md.name, g_md.zone, g_md.dept, g_md.num);
    MD_PRINT_LINE("%s:%s", md_get_res(MD_RES_108), md_get_res(MD_RES_111));
    MD_PRINT_LINE("%s:%s", md_get_res(MD_RES_103), g_md.cancel_order);

    MD_PRINT_BOUNDRY();
    MD_PRINT_LINE( "%-16s%-6s%-6s%s", md_get_res(MD_RES_100), md_get_res(MD_RES_101), md_get_res(MD_RES_BOOK_HEADER1),
            md_get_res(MD_RES_102));

    int i = 0;
    for(i = 0; i < g_md.book->count; i++)
    {
        booknode = xs_vec_get(g_md.book, i);
        if(booknode->dinner)
        {
            MD_PRINT_LINE("%-16s%-6s%-6s%s", booknode->dinner,
                    booknode->price,
                    md_get_res(atoi(booknode->cb)+MD_RES_CB1-1),
                    booknode->count);
        }
        else
        {
            MD_PRINT_LINE("%-16s%-6s%-6s%s", md_get_res(MD_RES_BUY_TIP), booknode->price, "", booknode->count);
        }
    }
    //MD_PRINT_LINE("%s", MD_BOUNDRY);
    MD_PRINT_BOUNDRY();
    MD_PRINT_LINE("%s: -%g", md_get_res(MD_RES_TOTAL_TIP),  calc_total_money(FALSE));
    MD_PRINT_LINE("%s: %g", md_get_res(MD_RES_LAST_TIP),  g_md.total_money_now);
    MD_PRINT_LINE( "%s", md_get_res(MD_RES_TICKET_TIP));

    MD_PRINT_TAIL();


    close_printer();

    md_book_clear();
}


void md_writecard_key(WPARAM wParam)
{
    if(wParam == SCANCODE_ENTER)
    {
        if(g_md.business == MD_BUSINESS_BOOK)
        {
            if(0 == md_writecard_book())
                print_ticket();

            return;
        }
        if(g_md.business == MD_BUSINESS_CANCEL_BOOK)
        {
            if(md_writecard_cancel_book() == 0)
                md_print_ticket_cancel_book();

            return;
        }
        if(g_md.business == MD_BUSINESS_BUY)
        {
            if(0 == md_writecard_buy())
                print_ticket();

            return;
        }
        if(g_md.business == MD_BUSINESS_CANCEL_BUY)
        {
            return;
        }
        assert(0); /* program to here is big error */
        return;
    }
    if(wParam == SCANCODE_CURSORBLOCKLEFT)
    {
        if(g_md.business == MD_BUSINESS_BOOK)
        {
            // TODO maybe need recovery
            md_set_status_and_redraw(MD_BOOK);
            return;
        }
        if(g_md.business == MD_BUSINESS_CANCEL_BOOK)
        {
            md_set_status_and_redraw(MD_CANCEL_BOOK);
            return;
        }
        if(g_md.business == MD_BUSINESS_BUY)
        {
            md_set_status_and_redraw(MD_BUY);
            return;
        }
        if(g_md.business == MD_BUSINESS_CANCEL_BUY)
        {
            md_set_status_and_redraw(MD_CANCEL_BUY);
            return;
        }
        assert(0); /* program to here is big error */
        return;
    }
    if(wParam == MD_KEY_CANCEL)
    {
        md_set_status_and_redraw(MD_READCARD);
        return;
    }
}

void md_weekfood_draw(HDC dc, RECT* rc)
{
    drawHeader(dc, rc, MD_HEADER_FORMAT_FOOD,
            md_get_res(MD_RES_BOOK_HEADER1),
            md_get_res(MD_RES_BOOK_HEADER2),
            md_get_res(MD_RES_BOOK_HEADER3),
            md_get_res(MD_RES_BOOK_HEADER5));

    char** table = xs_malloc(sizeof(char*) * g_md.dinner_all->count);
    int i;
    for(i=0; i<g_md.dinner_all->count; i++)
    {
        md_week_food_t* wf = xs_vec_get(g_md.dinner_all, i);
        table[i] = xs_malloc(240);
        sprintf(table[i], MD_HEADER_FORMAT_FOOD, md_get_res(atoi(wf->cb)+MD_RES_CB1-1), wf->name, wf->price, wf->type);
    }

    drawTable(dc, rc, table, g_md.dinner_all->count, PER_PAGE1-1, TRUE);

    md_free_table(table, g_md.dinner_all->count);
}

void md_weekfood_key(WPARAM wParam)
{
    char buf[32];

    if(g_md.dinner_all == NULL || g_md.dinner_all->count == 0)
        return;
    if(md_handle_cursor(wParam))
        return;

    if(wParam == SCANCODE_ENTER)
    {
        md_week_food_t* wf;
        wf = xs_vec_get(g_md.dinner_all, g_md.cur_item_sel);
        sprintf(buf, "%s.%s", wf->cb, wf->id);
        md_book_add_dinner(buf);
        return;
    }

    if(wParam == MD_KEY_CANCEL)
    {
        g_md.cur_item_sel = 0;
        md_set_status_and_redraw(MD_BOOK);
        return;
    }

    if(wParam >= SCANCODE_1 && wParam <= SCANCODE_9)
    {
        md_week_food_t* wf;
        wf = xs_vec_get(g_md.dinner_all, g_md.cur_item_sel);
        sprintf(buf, "%s.%s", wf->cb, wf->id);
        int i;
        for(i=0; i<(int)(wParam-SCANCODE_1+1); i++)
        {
            md_book_add_dinner(buf);
        }
        return;
    }

    md_handle_page(wParam);
}

void md_showinfo_draw(HDC hdc, RECT* rc)
{
    md_error_draw(hdc, rc);
}

void md_showinfo_key(WPARAM wParam)
{
    md_error_key(wParam);
}

void md_modify_zone_draw(HDC hdc, RECT* rc)
{
    int i;
    xs_vec_t* vec = g_md.vzone;

    char** menu = xs_malloc(sizeof(char*)*vec->count);
    for(i=0; i<g_md.vzone->count; i++)
    {
        menu[i] = xs_strdup((char*)xs_vec_get(vec, i));
    }

    drawTable(hdc, rc, menu, vec->count, PER_PAGE1, TRUE);

    md_free_table(menu, vec->count);
}

void md_modify_zone_key(WPARAM wParam)
{
    if(md_handle_cursor(wParam))
        return;

    if(wParam == SCANCODE_ENTER)
    {
        strcpy(g_md.zone_sel, xs_vec_get(g_md.vzone, g_md.cur_item_sel));
        g_md.cur_item_sel = 0;
        md_set_status_and_redraw(MD_MODIFY_DEPT);
        return;
    }

    if(wParam == MD_KEY_CANCEL)
    {
        md_set_status_and_redraw(MD_READCARD_INFO);
        return;
    }

    md_handle_page(wParam);
}

void md_modify_dept_draw(HDC hdc, RECT* rc)
{
    int i;
    xs_vec_t* vec = g_md.vdept;

    char** menu = xs_malloc(sizeof(char*)*vec->count);
    for(i=0; i<g_md.vzone->count; i++)
    {
        menu[i] = xs_strdup((char*)xs_vec_get(vec, i));
    }

    drawTable(hdc, rc, menu, vec->count, PER_PAGE1, TRUE);

    md_free_table(menu, vec->count);
}

void md_modify_dept_key(WPARAM wParam)
{
    if(md_handle_cursor(wParam))
        return;

    if(wParam == SCANCODE_ENTER)
    {
        strcpy(g_md.dept_sel, xs_vec_get(g_md.vdept, g_md.cur_item_sel));
        md_set_status_and_redraw(MD_MODIFY_BED);
        return;
    }

    if(wParam == MD_KEY_CANCEL)
    {
        md_set_status_and_redraw(MD_READCARD_INFO);
        return;
    }

    md_handle_page(wParam);
}

void md_modify_bed_draw(HDC dc, RECT* rc)
{
    drawBottom(0, md_get_res(MD_RES_BED_TIP), dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_CENTER);
    drawBottom(1, md_input(), dc, rc, TRUE, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
}

void md_modify_bed_key(WPARAM wParam)
{
    /* handle input */
    if(md_handle_input(wParam))
        return;

    if(wParam == SCANCODE_ENTER)
    {
        strcpy(g_md.zone, g_md.zone_sel);
        strcpy(g_md.dept, g_md.dept_sel);
        strncpy(g_md.num, g_md.input, 45);

        md_set_status_and_redraw(MD_READCARD_INFO);
    }
}

#define _md_handle_msg(func1, func2) do { \
    if(msg == MSG_PAINT) { if(g_md.status != MD_WELCOME) drawTitle(dc, rc);func1(dc, rc);} \
    else  func2(wParam); \
    return; \
}while(0)

void md_handle_msg(int msg, WPARAM wParam, HDC dc, RECT* rc)
{
    if(g_md.status == MD_MODIFY_BED)
    {
        _md_handle_msg(md_modify_bed_draw, md_modify_bed_key);
    }
    if(g_md.status == MD_MODIFY_ZONE)
    {
        _md_handle_msg(md_modify_zone_draw, md_modify_zone_key);
    }
    if(g_md.status == MD_MODIFY_DEPT)
    {
        _md_handle_msg(md_modify_dept_draw, md_modify_dept_key);
    }
    if(g_md.status == MD_WELCOME)
    {
        _md_handle_msg(md_welcome_draw, md_welcome_key);
    }
    if(g_md.status == MD_OPER_SEL)
    {
        _md_handle_msg(md_oper_sel_draw, md_oper_sel_key);
    }
    if(g_md.status == MD_READCARD)
    {
        _md_handle_msg(md_readcard_draw, md_readcard_key);
    }
    if(g_md.status == MD_READCARD_INFO)
    {
        _md_handle_msg(md_readcard_info_draw, md_readcard_info_key);
    }
    if(g_md.status == MD_ERROR || g_md.status == MD_HELP)
    {
        _md_handle_msg(md_error_draw, md_error_key);
    }
    if(g_md.status == MD_BOOKMENU)
    {
        _md_handle_msg(md_bookmenu_draw, md_bookmenu_key);
    }
    if(g_md.status == MD_BOOK)
    {
        _md_handle_msg(md_book_draw, md_book_key);
    }
    if(g_md.status == MD_CANCEL_BOOK)
    {
        _md_handle_msg(md_cancelbook_draw, md_cancelbook_key);
    }
    if(g_md.status == MD_BUY)
    {
        _md_handle_msg(md_buy_draw, md_buy_key);
    }
#if 0
    if(g_md.status == MD_CANCEL_BUY)
    {
        _md_handle_msg(md_cancel_draw, md_cancel_key);
    }
#endif
    if(g_md.status == MD_WRITECARD)
    {
        _md_handle_msg(md_writecard_draw, md_writecard_key);
    }
    if(g_md.status == MD_WEEKFOOD)
    {
        _md_handle_msg(md_weekfood_draw, md_weekfood_key);
    }
    if(g_md.status == MD_SHOW_INFO)
    {
        _md_handle_msg(md_showinfo_draw, md_showinfo_key);
    }
}

int mainProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    if(message == MSG_PAINT)
    {
        HDC dc;
        RECT rc;
        GetClientRect(hWnd, &rc);
        int oldBkMode;
        dc = BeginPaint(hWnd);
        oldBkMode = SetBkMode(dc, BM_TRANSPARENT);
        md_handle_msg(message, wParam, dc, &rc);
        SetBkMode(dc, oldBkMode);
        EndPaint(hWnd, dc);
    }
    else if(message == MSG_KEYDOWN)
    {
        if(!xs_file_exist(MD_RUNNING_FLAG))
        {
            FILE* fp = fopen(MD_RUNNING_FLAG, "w+");
            fclose(fp);
        }

        // add for #7 bug F1 show battery and help
        if(wParam == MD_KEY_F1 && g_md.status != MD_WELCOME && g_md.status != MD_HELP
                && g_md.status != MD_ERROR && g_md.status != MD_CRITICAL)
        {
            show_help();
        }
        else
        {
            xs_logd("message key = %d", (int)wParam);
            if(md_is_key_allow())
                md_handle_msg(message, wParam, HWND_NULL, NULL);
        }
    }
    else if(message == MSG_CREATE)
    {
        char* ret;

        g_md.fontL = CreateLogFont ("type1", "song", "GB2312",
                FONT_WEIGHT_LIGHT, FONT_SLANT_ROMAN,
                FONT_SETWIDTH_NORMAL, FONT_OTHER_TTFNOCACHE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 12, 0);
        if(g_md.fontL == NULL)
        {
            xs_logd("error");
            exit(1);
        }
        g_md.fontB = CreateLogFont( NULL, "song", "GB2312",
                FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                16, 0);
        if(g_md.fontB == NULL)
        {
            xs_logd("error");
            exit(1);
        }

        ret = md_init();
        if(ret != NULL)
        {
            show_error(ret, MD_ERROR, 0);
            md_disable_key();
        }

        BOOL bRet = SetTimer(hWnd, g_md.timerid, 200); /* two second, every second send keep alive */
        if(bRet == FALSE)
        {
            xs_loge("can not start timer");
            exit(1);
        }
    }
    else if(message == MSG_TIMER)
    {
        xs_logd("enter timer");
        if(g_sock > 0)
        {
            md_daemon_gui_t* rsp;
            if(xs_model_call(g_sock, (xs_model_t**)&rsp, 2, __xs_pos, __xs_gui_keepalive) != 0)
            {
                xs_loge("call to keep alive error");
                exit(1);
            }

            int status = atoi(rsp->status);

            if(status == MD_DAEMON_STATUS_TRANS_START)
            {
                xs_logd("recv trans start");
                if(!xs_file_exist(MD_RUNNING_FLAG))
                {
                    FILE* fp = fopen(MD_RUNNING_FLAG, "w+");
                    fclose(fp);
                }
                show_error_by_id(MD_ERROR_ON_COMMUNICATION,MD_SHOW_INFO, 1 );
            }
            else if(status == MD_DAEMON_STATUS_TRANS_END)
            {
                // trans end, quit directly, daemon while start another
                close(g_sock);
                //    xs_model_call(g_md.sockfd, (xs_model_t**)&rsp, 2, __xs_pos, __xs_gui_exiting);
                exit(1);
            }

            xs_model_delete((xs_model_t*)rsp);
        }
        else
        {
            xs_logd("gsock is %d", g_sock);
        }
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

void sig_handler(int sig)
{
    if(sig == SIGUSR1)
    {
        xs_logd("signal SIGUSR1 .....");
        show_error_by_id(MD_ERROR_ON_COMMUNICATION,MD_SHOW_INFO, 1 );
    }
}

int MiniGUIMain(int argc, const char* argv[])
{
    argc = argc;
    argv = argv;
    MSG msg;

    md_close_debug();

    g_sock = atoi(argv[1]);
    xs_logd("g_sock is %d", g_sock);
    //    signal(SIGUSR1, sig_handler);

    MAINWINCREATE CreateInfo;

    xs_init();

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER /*| WS_CAPTION */;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "hello";
    CreateInfo.hMenu = 0;
    CreateInfo.hIcon = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.MainWindowProc = mainProc;      /* 这个mainProc是处理各种消息的函数，以后主要代码在这里 */
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 320;
    CreateInfo.by = 240;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    /* 创建这个窗口 */
    g_md.mainWnd = CreateMainWindow(&CreateInfo);

    if(g_md.mainWnd == HWND_INVALID)
        return -1;

    /* 显示窗口 */
    ShowWindow(g_md.mainWnd, SW_SHOWNORMAL);

    /* 进入死循玿*/
    while(GetMessage(&msg, g_md.mainWnd))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    MainWindowThreadCleanup(g_md.mainWnd);

    return 0;
}
