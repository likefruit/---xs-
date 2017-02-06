
#ifndef __MD_GB2312_H__
#define __MD_GB2312_H__

#define MD_KEY_DOT 52
#define MD_KEY_CANCEL 14
#define MD_KEY_F1 44
#define MD_KEY_F2 60
#define MD_KEY_F3 61
#define MD_KEY_F4 62

#define MD_MENU_HEIGHT 20
#define MD_TITLE_HEIGHT 20
#define MD_HEADER_HEIGHT 20
#define MD_MARGIN 10
#define MD_SCREEN_WIDTH 320
#define MD_SCREEN_HEIGHT 240

#define PER_PAGE1 8

#define MD_BOOK_DRAW_FOTMAT "%-6s%-16s%-7s%s"
#define MD_HEADER_FORMAT_FOOD "%-6s%-16s%-7s%s"
#define MD_PRINTER_PATH		"/dev/printer"


/* which business on */
typedef enum md_win_status_t
{
    MD_WELCOME,
    MD_OPER_SEL,
    MD_READCARD,     /* show tips: please read card */
    MD_READCARD_INFO, /* display card into, press enter into COMMANDLIST */
    MD_COMMAND_LIST, /* the command according the card info */
    MD_EDIT,     /* ���ʹ��� */
    MD_WRITECARD,
    MD_ERROR,        /* ������Ϣ���� */
    MD_CRITICAL,        /* ������Ϣ���� */
    MD_TEST,        /* the window is for test */
    MD_SHOW_INFO,
    MD_BOOK,
    MD_CANCEL_BOOK,
    MD_BUY,
    MD_CANCEL_BUY,
    MD_BOOKMENU,
    MD_WEEKFOOD,
    MD_MODIFY_ZONE,
    MD_MODIFY_DEPT,
    MD_MODIFY_BED,
    MD_HELP
} md_win_status_t;


typedef struct md_t
{
    int buy_time;

    xs_vec_t* vzone;
    xs_vec_t* vdept;

    char zone_sel[48];
    char dept_sel[48];

    double total_money;             /* calc in booking, for show */
    double total_money_old;     /* init when enter MD_BOOK, enter MD_CANCEL_BOOK, enter MD_BUY(is zero), for use in MD_WRITECARDs */
    double total_money_now;
    HWND mainWnd;
#define md_update_wnd(_rc) InvalidateRect(g_md.mainWnd, _rc, TRUE)
#define md_update_rc(_index) md_update_wnd(&g_md.rc[_index])
    char input[48];                     /* clean when enter OPER_SEL, MD_BOOK, MD_BUY */
#define MAX_INPUT_SIZE (sizeof(g_md.input)-1)
#define md_input_append(_ch) do{ int len=strlen(g_md.input); if(len<(int)MAX_INPUT_SIZE){g_md.input[len]=_ch;g_md.input[len+1]=0;} }while(0)
#define md_input_append_and_update(_ch, _rc) do{md_input_append(_ch); md_update_wnd(_rc);}while(0)
#define md_input_is_full() (strlen(g_md.input) == MAX_INPUT_SIZE)
#define md_input_is_empty() (g_md.input[0] == 0)
#define md_input_cancel() do{ int len = md_input_size(); if(len > 0) { g_md.input[len-1] = 0; } }while(0)
#define md_input_cancel_and_update(_rc) do{ md_input_cancel(); md_update_wnd(_rc);  }while(0)
#define md_input_size() (strlen(g_md.input))
#define md_input() g_md.input
#define md_input_clear() (*g_md.input = 0)

    RECT rc[4];                 /* init in drawButtom, use for redrawWindow */
    RECT rc_item[PER_PAGE1*2];      /* init in drawTable, use for redrawWindow */
    int cur_perpage;    /* init in drawtable, record perpage count in table   */
    int item_total;         /* init in drawtable, for calc page */
    int cur_item_sel;       /* init zero for EVERY_STATUS, when UP/DOWN key strick, change and redraw */

    xs_vec_t* dinner_all;       /* init when program start, use for show dinner_all */
    xs_vec_t* order_list;       /* init when readcard */
    int cur_order;                  /* init when enter MD_BOOK MD_BUY MD_CANCEL MD_CANCEL_BUY */
#define md_order_file() xs_vec_get(g_md.order_list, g_md.cur_order)
#define md_order_list_clear() do{if(g_md.order_list){xs_vec_destroy(g_md.order_list, xs_free); g_md.order_list = NULL; g_md.cur_order = 0;}}while(0)

    xs_vec_t* oper;                 /* init when program start */
    xs_vec_t* book;             /* load when enter MD_BOOK, MD_BUY, MD_CANCEL_BOOK, MD_CANCEL_BUY */
#define md_book_clear() do{if(g_md.book) {xs_vec_destroy(g_md.book, (void(*)(void*))xs_model_delete); g_md.book=NULL;}}while(0)
#define md_book_create(step) do{md_book_clear(); g_md.book = xs_vec_create(step);}while(0)
#define md_book_append(ptr) xs_vec_add(g_md.book, ptr)

    int booking_delay;
    char* company;
    char* book_date;
    char* buy_date;
    int cur_oper_id;
    int business; /*  */
#define MD_BUSINESS_NONE 0
#define MD_BUSINESS_BOOK 1
#define MD_BUSINESS_CANCEL_BOOK 2
#define MD_BUSINESS_BUY 3
#define MD_BUSINESS_CANCEL_BUY 4

    PLOGFONT fontL;
    PLOGFONT fontB;

    char balance[48], name[48], zone[48], dept[48], num[48], id[48], orderid[48];
    char cancel_order[48];
    char daily[48];
    char order_gen[48];        /* init when enter MD_BOOK, MD_BUY, MD_CANCEL_BOOK, MD_CANCEL_BUY, gen or split from order_list[cur_order] */
#define md_is_normal_card() (strtod(g_md.daily, NULL) < 0.001)
    char card_buff[1024];
    unsigned long cardno;
    char* username;
    //    int old_cur;
    int printer_fd ;

    /* for win status */
    md_win_status_t status;
    md_win_status_t old_status;
#define md_save_status() g_md.old_status = g_md.status
#define md_load_status() g_md.status = g_md.old_status
#define md_set_status(_status) g_md.status = _status
#define md_set_status_and_redraw(_status) do{md_set_status(_status);md_update_wnd(NULL);}while(0)
#define md_load_status_and_redraw() do{md_load_status();md_update_wnd(NULL);}while(0)

    char* info;
#define md_set_info(__info) do{if(g_md.info) xs_free(g_md.info); g_md.info = xs_strdup(__info);}while(0)

    /*  */
    int key_allow; /* is allow key input */
#define md_is_key_allow() (g_md.key_allow == 0)
#define md_disable_key() (g_md.key_allow = 1)
#define md_enable_key() (g_md.key_allow = 0)

    int timerid;
} md_t;

extern md_t g_md;

static inline char* md_get_title()
{
    if(g_md.status == MD_MODIFY_ZONE)
        return "�޸Ĳ���";
    if(g_md.status == MD_MODIFY_DEPT)
        return "�޸Ŀ���";
    if(g_md.status == MD_MODIFY_BED)
        return "�޸Ĵ�λ";
    if(g_md.status == MD_WEEKFOOD)
    {
        return "��ʳ��";
    }
    if(g_md.status == MD_WELCOME)
    {
        return "��ӭ";
    }
    if(g_md.status == MD_OPER_SEL)
    {
        return "ѡ�����Ա";
    }
    if(g_md.status == MD_READCARD)
    {
        return "����";
    }
    if(g_md.status == MD_READCARD_INFO)
    {
        return "����Ϣȷ��";
    }
    if(g_md.status == MD_ERROR)
    {
        return "����";
    }
    if(g_md.status == MD_BOOKMENU)
    {
        return "����ѡ��";
    }
    if(g_md.status == MD_BOOK)
    {
        return "����/�޸Ķ���";
    }
    if(g_md.status == MD_CANCEL_BOOK)
    {
        return "ȡ������";
    }
    if(g_md.status == MD_BUY)
    {
        return "��ʱ����";
    }
    if(g_md.status == MD_CANCEL_BUY)
    {
        return "ȡ����ʱ����";
    }
    if(g_md.status == MD_WRITECARD)
    {
        return "д��";
    }
    if(g_md.status == MD_SHOW_INFO)
    {
        return "֪ͨ";
    }
    // add by xueguoliang for #7
    if(g_md.status == MD_HELP)
    {
        return "����";
    }
    // add end
    assert(0);
    return NULL;
}

static inline void md_format_battery(int b, char* buf)
{
    if(b == -1 || b > 255)
    {
        strcpy(buf, "��ȡ����ʧ��");
        return;
    }

    sprintf(buf, "ʣ�������%d%%", (int)(b*1.0/255*100));
}

#define md_set_read_card_error(_ret) do{\
    if(ret == -2) show_error2(MD_ERROR_NO_CARD, MD_ERROR, 0);\
    else if(ret == -1) show_error2(MD_ERROR_READCARD, MD_ERROR, 0);\
}while(0)

typedef enum
{
    MD_RES_WELCOME,
    MD_RES_ENTER_PASSWORD,
    MD_RES_READCARD_TIP,
    MD_RES_ACCOUNT_NAME,
    MD_RES_ACCOUNT_BALANCE,
    MD_RES_ACCOUNT_TYPE,
    MD_RES_ACCOUNT_TYPE_COMMON,
    MD_RES_ACCOUNT_TYPE_DAILY,
    MD_RES_ACCOUNT_TYPE_DAILY_MONEY,
    MD_RES_ACCOUNT_HAVE_BOOK,
    MD_RES_ACCOUNT_HAVE_NOT_BOOK,
    MD_RES_ACCOUNT_BOOK_STATUS,
    MD_RES_ACCOUNT_BALANCE,
    MD_RES_READCARD_INFO_TIP1,
    MD_RES_READCARD_INFO_TIP2,

    MD_RES_BOOK_MENU1,
    MD_RES_BOOK_MENU2,
    MD_RES_BOOK_MENU3,
    MD_RES_BOOK_MENU4,
    MD_RES_BOOK_MENU5,
    MD_RES_BOOK_MENU6,
    MD_RES_BOOK_MENU7,
    MD_RES_BOOK_MENU8,
    MD_RES_BOOK_MENU9,

    MD_RES_BOOK_HEADER1,
    MD_RES_BOOK_HEADER2,
    MD_RES_BOOK_HEADER3,
    MD_RES_BOOK_HEADER4,
    MD_RES_BOOK_HEADER5,
    MD_RES_BOOK_HEADER6,
    MD_RES_BOOK_HEADER7,
    MD_RES_BOOK_HEADER8,

    MD_RES_CB1,
    MD_RES_CB2,
    MD_RES_CB3,
    MD_RES_CB4,
    MD_RES_CB5,
    MD_RES_CB6,
    MD_RES_CB7,
    MD_RES_CB8,

    MD_RES_TIP_INPUT_DINNER,
    MD_RES_TIP_TOTAL_MONEY,

    MD_RES_WRITE_CARD_TIP1,
    MD_RES_WRITE_CARD_TIP2,
    MD_RES_WRITE_CARD_TIP3,

    MD_RES_READ_CARD_TIP1,
    MD_RES_READ_CARD_TIP2,
    MD_RES_READ_CARD_TIP3,

    MD_RES_INPUT_MONEY_TIP,
    MD_RES_BUY_TIP,
    MD_RES_TOTAL_TIP,
    MD_RES_LAST_TIP,
    MD_RES_TICKET_TIP,
    MD_RES_ORDER_TICKET_TIP,
    MD_RES_CANCEL_TICKET_TIP,
    MD_RES_100,
    MD_RES_101,
    MD_RES_102,
    MD_RES_103,
    MD_RES_104,
    MD_RES_105,
    MD_RES_106,
    MD_RES_107,
    MD_RES_108,
    MD_RES_109,
    MD_RES_110,
    MD_RES_111,
    MD_RES_ACCOUNT_POS,
    MD_RES_BED_TIP,
    MD_RES_TIME,
    MD_RES_ALL
} md_res_t;

static char* g_res[] =
{
    "��ӭʹ���ֳֻ���",                                                                                         /* MD_RES_WELCOME */
    "��������",                                                                                                    /* MD_RES_ENTER_PASSWORD */
    "��ˢ��������������\n�밴OK����ʼ����\nF2��ʾ�ܲ���\nF3��ӡ�ܲ���",   /* MD_RES_READCARD_TIP */
    "�˻�����",             /* MD_RES_ACCOUNT_NAME */
    "�˻����",             /* MD_RES_ACCOUNT_BALANCE */
    "�˻�����",             /* MD_RES_ACCOUNT_TYPE */
    "��ͨ��",                  /* MD_RES_ACCOUNT_TYPE_COMMON */
    "�սῨ",                  /* MD_RES_ACCOUNT_TYPE_DAILY */
    "�ս���",                 /* MD_RES_ACCOUNT_TYPE_DAILY_MONEY */
    "�˿��������ж���",     /* MD_RES_ACCOUNT_HAVE_BOOK */
    "�˿�����û�ж���",     /* MD_RES_ACCOUNT_HAVE_NOT_BOOK */
    "�������",                     /* MD_RES_ACCOUNT_BOOK_STATUS */
    "���",           /*MD_RES_ACCOUNT_BALANCE,*/
    "[OK��] ���붩������",             /* MD_RES_READCARD_INFO_TIP1 */
    "[C��] ����ˢ������",                     /* MD_RES_READCARD_INFO_TIP2 */

    "[1��] ����/�޸Ķ���",  /* MD_RES_BOOK_MENU1 */
    "[2��] ȡ������",  /* MD_RES_BOOK_MENU2 */
    "[3��] ��ʱ����",  /* MD_RES_BOOK_MENU3 */
    "[4��] ȡ����ʱ����",  /* MD_RES_BOOK_MENU4 */
    "MD_RES_BOOK_MENU5",  /* MD_RES_BOOK_MENU5 */
    "MD_RES_BOOK_MENU6",  /* MD_RES_BOOK_MENU6 */
    "MD_RES_BOOK_MENU7",  /* MD_RES_BOOK_MENU7 */
    "MD_RES_BOOK_MENU8",  /* MD_RES_BOOK_MENU8 */
    "MD_RES_BOOK_MENU9",  /* MD_RES_BOOK_MENU9 */

    "�ͱ�", /*MD_RES_BOOK_HEADER1*/
    "��Ʒ����", /*MD_RES_BOOK_HEADER2*/
    "�۸�", /*MD_RES_BOOK_HEADER3*/
    "����", /*MD_RES_BOOK_HEADER4*/
    "����", /*MD_RES_BOOK_HEADER5*/
    "", /*MD_RES_BOOK_HEADER6*/
    "", /*MD_RES_BOOK_HEADER7*/
    "", /*MD_RES_BOOK_HEADER8*/

    "���",/* MD_RES_CB1 */
    "���",/* MD_RES_CB2 */
    "���",/* MD_RES_CB3 */
    "",/* MD_RES_CB4 */
    "",/* MD_RES_CB5 */
    "",/* MD_RES_CB6 */
    "",/* MD_RES_CB7 */
    "",/* MD_RES_CB8 */

    "��Ʒ��", /*MD_RES_TIP_INPUT_DINNER,*/
    "�ܶ�", /*MD_RES_TIP_TOTAL_MONEY,*/

    "[OK��] ��ʼд��", /* MD_RES_WRITE_CARD_TIP1, */
    "[P��] ���±༭����", /*MD_RES_WRITE_CARD_TIP2*/
    "[C��] ���ص���������", /* MD_RES_WRITE_CARD_TIP3, */

    "[OK��] ��ʼ����", /*MD_RES_READ_CARD_TIP1,*/
    "[F2��] ��ʾ��ʳ��", /*MD_RES_READ_CARD_TIP2,*/
    "[F3��] ��ӡ��ʳ��", /*MD_RES_READ_CARD_TIP3,*/

    "���ѽ��",   //MD_RES_INPUT_MONEY_TIP,
    "��ʱ����",
    "�����ܶ�",
    "�������",
    "��СƱΪ��������ƾ֤, �����Ʊ��棡",
    "ҵ��СƱ",
    "�˲�СƱ",
    "��Ŀ",
    "����",
    "����",
    "������",
    "���", /*MD_RES_BOOK_HEADER1*/
    "����", /*MD_RES_BOOK_HEADER2*/
    "�۸�", /*MD_RES_BOOK_HEADER3*/
    "����", /*MD_RES_107*/
    "ҵ������",/* MD_RES_108 */
    "ԤԼ���",/* MD_RES_109 */
    "��ʱ����",/* MD_RES_110 */
    "ȡ������",/* MD_RES_111 */
    "סԺ��ַ",
    "��λ",
    "����ʱ��",
};

static inline char* md_get_res(md_res_t id)
{
    if(id >= MD_RES_ALL)
        assert(0);

    return g_res[id];
}

static char* g_error[] =
{
    "��ȷ",
    "�������",
    "��������",
    "�������󣬸�Ӧ��û�з��ֿ�",
    "�Ƿ���",
    "���Ҳ�Ʒʧ��",
    "д�����ļ�ʧ��",
    "д��ʧ��",
    "�˿�δ����",
    "��Ʒ�Ų���Ϊ��",
    "�ֳֻ�����ͨ��",
    "�ÿ��ں�������",
    "����",
};

typedef enum
{
    MD_OK,
    MD_ERROR_PASSWORD,
    MD_ERROR_READCARD,
    MD_ERROR_NO_CARD,
    MD_ERROR_INVALID_CARD,
    MD_ERROR_NOT_FOUND_DINNER,
    MD_ERROR_WRITE_BOOK_FILE,
    MD_ERROR_WRITE_CARD_ERROR,
    MD_ERROR_NO_ORDER,
    MD_ERROR_DINNER_KEY_EMPTY,
    MD_ERROR_ON_COMMUNICATION,
    MD_ERROR_ON_BLACK_LIST,
    MD_ERROR_NO_MONEY,

    MD_ERROR_ALL
} md_error_t;

static inline char* md_get_error(md_error_t error)
{
    if(error >= MD_ERROR_ALL)
        assert(0);
    return g_error[error];

}


#endif
