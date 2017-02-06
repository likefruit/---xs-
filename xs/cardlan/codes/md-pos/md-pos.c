#include <stdio.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>

#include <string.h>

#include "xs_vec.h"
/*
 * MD_DINNER_DIR������һ�ܲ��ף������������߸��ļ�
 * /var/run/dinner/1 ��һ����
 * /var/run/dinner/2 �ܶ����� 
 * ...�Դ�����
 * �豸��ʼ��ʱ��Ӧ��Ҫ��ȡ���ڲ���ȡ���ܼ��������������Ǹ��ļ�
 *
 * �ļ���ʽΪ��
 * ��Ʒid`��Ʒ����`��Ʒ�۸�`�ͱ�`���
 * ÿһ��һ����Ʒ������� * 1.3.4`���ѳ���`4.3`[1]���`[1]���T *
 * ����ļ����Ե����·������Ǳ�����û�У������Լ������ļ�������
 * */
#define MD_DINNER_DIR  "/sdcard/food"

/*
 * MD_OPER_FILE������Ա�б��ļ����ӣ� * Ѧ����`112233
 * һ�������֣�һ�������룬o�� * */
#define MD_OPER_FILE    "/sdcard/oper"

/*
 * ����Ŀ¼����Ϊ���ö��˶��ͣ����Զ�����һ��Ŀ�� * ÿ��������һ���ļ���ɣ��ļ�����ʽΪ �˻�ID_������ * �ļ����ݸ�ʽΪ��
 * ��Ʒid`����`ʱ��
 *
 * �����ž͵���ʱ��ɣ�����2013��c4������4��6��0��Ķ��ͣ���ô�����ž��� 20130214140630
 *
 * ʱ����Ƕ���һ��ķ���һ���ǹ̶��������������������������Ľӿڰ�����һ�б��� *
 * */
#define MD_BOOK_DIR     "/sdcard/book"
/* 
 * �Ͷ���һ�����ɣ�ʱ��Ҳ��һ���ģ�һ����ǵ�ǰʱ�䣬����˵��׼�б仯�� * */
#define MD_BUY_DIR      "/sdcard/buy"
/*
 * д�����ļ���ok�ˣ��ļ�����Ԥ���ļ���һ������ * */
#define MD_CANCEL_DIR   "/sdcard/cancel"


#define STROK_FLAG		" "
/* ��Ʒ�б� */
typedef struct md_dinner_node_t
{
    int seq;
    char* id;        /* id�ţ���Ʒid���Ȋ�010100 */
    char* name;      /* ���� */
    char* price;     /* �۸� */
    char* type;      /* ���ͣ���ʳ|����|��|�ȵ� */
    char* class;     /* �����ͨ|����|�ȵ� */
    char* dbID;
    char* wday;
    char* time;
} md_dinner_node_t;

/* to �뺽�������̬�������Ѿ�����һ����ֱ���ö�̬���ݾ�ok��*/
xs_vec_t* g_dinner_all = NULL;
xs_vec_t* g_dinner_book = NULL;

/* ����Ա�б� */
typedef struct md_oper_node_t
{
    char* name;
    char* pass;
} md_oper_node_t;
xs_vec_t* g_oper = NULL;

/* �˻���Ϣ�������Ϣ�ӿ��϶�ȡ */
typedef struct md_acc_t
{
    char* name;     /* ���� */
    char* money;    /* ��� */
    char* id;       /* ���� */
    char* addr;     /* ��ַ��Ϣ*/
} md_acc_t;
/* ��ǰ�˻���Ϣ */
md_acc_t g_acc;


/* which business on */
typedef enum md_win_status_t
{
    MD_WELCOME,     /* ��ӭҳ�� */
    MD_OPER_SEL,    /* ѡ�������*/
    MD_OPER_PASS, 
    MD_COMMAND,     /* �����, 1)dingcan, 2)tuican, 3)jishixiaofei, 4)dingcanchaxun */
    MD_BOOKING,     /* ���ʹ��� */
    MD_CANCELBOOK,  /* �˶�����*/
    MD_BUY,         /* ��ʱ���Ѵ��� */
    MD_QUERY,       /* ���Ͳ�ѯ */
    MD_ERROR        /* ������Ϣ���� */
} md_win_status_t;

typedef struct md_win_status_node_t
{
    md_win_status_t status; /* ��ǰ����״��*/
    unsigned char cursel;   /* �ڵ�ǰҳ���µ�ѡ��*/
    unsigned char count;    /* ���ҳ�ж��ٿ�ѡ�� */
    unsigned char page;     /* ��ǰ�ڵڼ�ҳ */
    unsigned char page_count; /* һ���ж��ٶ�*/
    unsigned char per_page; /* ÿҳ����ѡ�����Ƕ��� */
    char* error;            /* ������Ϣ */
} md_win_status_node_t;

xs_vec_t *g_win_all = NULL ;
int g_win_cur = -1;
int g_win_book = -1;
static RECT rcCircle = {0, 10, 300, 200};

#define MD_MENU_HEIGHT 20   /* �˵�����ռ����*/
#define MD_MARGIN 10        /* ��ʾ�߽�ո� */

#if 0
/* ��ʾ������Ϣ���� */
static void show_error(char* str, HWND wnd)
{
    md_win_status_node_t* node = malloc(sizeof(*node));
    node->error = str;
    node->status = MD_ERROR;

    g_win_cur = xs_vec_add(g_win_all, node);

    if(wnd != HWND_NULL)
        InvalidateRect(wnd, NULL, TRUE);
}
#endif

/* Added by tsihang 2013-04-04 */
static void init_md_win( void )
{
	g_win_all = malloc(sizeof(xs_vec_t));
	if(g_win_all == NULL)
	{
		printf("malloc g_win_all error!\n");
		return ;
	}
#if 0
	g_win_all->count = 0;
	g_win_all->size = 0;
	g_win_all->step = 0;
	g_win_all->vs = NULL;
#endif
	g_win_all = xs_vec_create(8);	
}

/* ��ʼ���˵��б�
 * һ���Դ��ļ��������˵����ڴ棬
 * ������ʱҪʹ���ⲿ������ 
 * ��������Ķ��壬���������Ҫ����дһ�µ�
 * */
static void init_dinner_list()
{
	char buf[1024];
	int idx = 0;
	md_dinner_node_t* dinner;
	FILE* f;
	char* tmp;
	g_dinner_all = xs_vec_create(8);
	/* load dinner list */
	f = fopen(MD_DINNER_DIR, "r");
	if(f == NULL)
	{
		show_error("can not open file /sdcard/food", HWND_NULL);
		return;
	}

	while(fgets(buf, 1024, f))
	{
		if(strlen(buf) <= 3)
			break;
		/* buffer format is 0101002`name`price` */
		dinner = malloc(sizeof(*dinner));
		dinner->seq = idx++;	/* How many foods here */

		/* Food id */
		tmp	 = strtok(buf, STROK_FLAG);
		if(tmp == NULL)
		{
			show_error("file /sdcard/food format error", HWND_NULL);
			break;
		}
		dinner->id = strdup(tmp);	
#ifdef DEBUG
		printf("id = %s\n", dinner->id);
#endif
		/* Food name */
		tmp = strtok(NULL, STROK_FLAG);
		if(tmp == NULL)
		{
			show_error("file /sdcard/food format error", HWND_NULL);
			break;
		}
		dinner->name = strdup(tmp);
#ifdef DEBUG
		printf("name = %s\n", dinner->name);
#endif		
		/* Food class */
		tmp = strtok(NULL, STROK_FLAG);
		if(tmp == NULL)
		{
			show_error("file /sdcard/food format error", HWND_NULL);
			break;
		}
		dinner->class= strdup(tmp);
#ifdef DEBUG
		printf("class = %s\n", dinner->class);
#endif
		/* Food type */
		tmp = strtok(NULL, STROK_FLAG);
		if(tmp == NULL)
		{
			show_error("file /sdcard/food format error", HWND_NULL);
			break;
		}
		dinner->type= strdup(tmp);
#ifdef DEBUG
		printf("type = %s\n", dinner->type);
#endif		
		/* Food price */
		tmp = strtok(NULL, STROK_FLAG);
		if(tmp == NULL)
		{
			show_error("file /sdcard/food format error", HWND_NULL);
			break;
		}
		dinner->price = strdup(tmp);
#ifdef DEBUG
		printf("price = %s\n", dinner->price);
#endif
		/* Food ID in database */
		tmp = strtok(NULL, STROK_FLAG);
		if(tmp == NULL)
		{
			show_error("file /sdcard/food format error", HWND_NULL);
			break;
		}
		dinner->dbID= strdup(tmp);
#ifdef DEBUG
		printf("ID in DB = %s\n", dinner->dbID);
#endif
		/* W day */
		tmp = strtok(NULL, STROK_FLAG);
		if(tmp == NULL)
		{
			show_error("file /sdcard/food format error", HWND_NULL);
			break;
		}
		dinner->wday= strdup(tmp);
#ifdef DEBUG
		printf("wday = %s\n", dinner->wday);
#endif
		/* Food when to sharing */
		tmp = strtok(NULL, STROK_FLAG);
		if(tmp == NULL)
		{
			show_error("file /sdcard/food format error", HWND_NULL);
			break;
		}
		dinner->time= strdup(tmp);
#ifdef DEBUG
		printf("time = %s\n", dinner->time);
#endif		
		xs_vec_add(g_dinner_all, dinner);
	}

	
	fclose(f);
}

/* init win status */
static void init_pos_machine()
{
	/* ��ʼ����һ������*/
	md_win_status_node_t* win = malloc(sizeof(md_win_status_node_t));
	if(win == NULL)
	{
		printf("md_win_status_node_t malloc error !\n");
		return;
	}
	win->status = MD_WELCOME;
	win->error = "Welcome to use POS MACHINE \n\n"
		 "-- a mature booking windowing system "
	        "and GUI support system for all kinds of order devices.\n\n"
	        "Copyright (C) 2013 ~ 2020 POS-SOFT Co., Ltd.\n"
	        "< xueguoliang@gmail.com >";

	g_win_cur = xs_vec_add(g_win_all, win);

}
/* ��rc�ľ����ڻ��˵�menu */
static void drawMenu(HDC dc, RECT* rc, const char* menu[], int size)
{
    int h = MD_MENU_HEIGHT, m = MD_MARGIN, i;
    rc->bottom = rc->top + h;
    rc->left += m;

    md_win_status_node_t* win = g_win_all->vs[g_win_cur];
    if(win == NULL)
    {
		printf("win is null:\n");
		return;
    }
	
    win->count = size; /* real size of menu */
    for(i=0; i<size; ++i)
    {
        if(win->cursel == i)
        {
            gal_pixel oldColor = SetBrushColor(dc, PIXEL_blue);
            Rectangle(dc, rc->left, rc->top, rc->right, rc->bottom);
            SetBrushColor(dc, oldColor);

            /* maybe need do more */
            TextOut(dc, rc->left, rc->top, menu[i]);
        }
        else
        {
            TextOut(dc, rc->left, rc->top, menu[i]);
        }
        rc->top = rc->bottom + m;
        rc->bottom = rc->top + h;
    }
    rc->left -= m;
}

/* �����˵� */
static void drawMainWnd(HDC dc, RECT* rc)
{
    static const char* menu[] = 
    {
        /* main win menu list */
        "[1] book dinner",
        "[2] buy dinner",
        "[3] cancel book",
        "[4] set operator name",
        "[5] show book list",
        "[6] all book list"
    };
    drawMenu(dc, rc, menu, TABLESIZE(menu));
}

static void drawBooking(HDC dc, RECT* rc)
{
    TextOut(dc, 0, 0, "booking"); 
}

static void drawCancelBook(HDC dc, RECT* rc)
{
    TextOut(dc, 0, 0, "cancel"); 
}

static void drawBuy(HDC dc, RECT* rc)
{
    TextOut(dc, 0, 0, "buy"); 
}

static void drawError(HDC dc, RECT* rc)
{
    md_win_status_node_t* win = xs_vec_get(g_win_all, g_win_cur);
    TextOut(dc, 0, 0, win->error);
}
static void drawWelcome(HDC dc, RECT* rc)
{
	/* show welcome text, future maybe draw one picture */
	md_win_status_node_t* win = xs_vec_get(g_win_all, g_win_cur);
	//TextOut(dc, 0, 0, win->error);
	rc->top = 30;
	rc->bottom -= 50;
	rc->left = 10;
	rc->right -= 10;
	SetTextColor (dc, PIXEL_black);
	SetBkMode (dc, BM_TRANSPARENT);
	DrawText (dc, win->error, -1, rc, DT_WORDBREAK | DT_CENTER);
}

static void mainDraw(HWND hWnd)
{
	int i = 0;
	HDC dc;
	RECT rc;
	GetClientRect(hWnd, &rc);
	int oldBkMode;
	md_win_status_node_t* win = xs_vec_get(g_win_all, g_win_cur);

	dc = BeginPaint(hWnd);
	oldBkMode = SetBkMode(dc, BM_TRANSPARENT); 

	switch (win->status)
	{
		case MD_WELCOME:
			drawWelcome(dc, &rc);
			break;

		case MD_OPER_SEL:
			drawMainWnd(dc, &rc);
			break;
			
		case MD_BOOKING:
			drawBooking(dc, &rc);
			break;
		case MD_CANCELBOOK:
			drawCancelBook(dc, &rc);
			break;
		case MD_BUY:
			drawBuy(dc, &rc);
			break;
		case MD_ERROR:
			drawError(dc, &rc);
			break;
	}

	SetBkMode(dc, oldBkMode);

	EndPaint(hWnd, dc);
}

void main_loop(HWND hWnd, int id)
{
	switch(id)
	{
		case 1: 
			
			break;
		case 2: break;
		case 3: break;
		
	}
}
static void mainKeyDown(HWND wnd, WPARAM wParam, LPARAM lParam)
{    
	RECT refresh;
	
	switch(wParam)
	{
		case SCANCODE_BACKSPACE:
			break;
		case SCANCODE_ENTER:
			printf("g_win_cur %d\n", g_win_cur);
			if(g_win_cur == MD_WELCOME)
			{
				printf("_____ \n");
				g_win_cur = MD_OPER_SEL;
				//InvalidateRect(wnd, NULL, TRUE);
			}
			break;
		case SCANCODE_CURSORBLOCKLEFT:
			/* ��߰�ť�������һ������*/
			if(g_win_cur != 0)
			{
				g_win_cur --;
				InvalidateRect(wnd, NULL, TRUE);
			}
			break;
		case SCANCODE_CURSORBLOCKRIGHT:
			/* �ұ߰�ť�������һ������*/
			if(g_win_cur != g_win_all->count -1)
			{
				g_win_cur++;
				InvalidateRect(wnd, NULL, TRUE);
			}
			break;
			
		case SCANCODE_CURSORBLOCKUP:;
			break;
			
		case SCANCODE_CURSORBLOCKDOWN:
			break;
			
		/* return booking window */
		case SCANCODE_F1:
			g_win_cur = g_win_book;
			InvalidateRect(wnd, NULL, TRUE);
			break;

		case SCANCODE_0:
			break;
		case SCANCODE_1:
		case SCANCODE_2:
		case SCANCODE_3:
		case SCANCODE_4:
		case SCANCODE_5:
		case SCANCODE_6:
		case SCANCODE_7:
		case SCANCODE_8:
		case SCANCODE_9:
		case SCANCODE_F2:
		case SCANCODE_F3:
		case SCANCODE_F4:
		case SCANCODE_TAB:
		break;
	}
}

static int mainProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		  /* */
		/* draw GUI */
		case MSG_PAINT:
			mainDraw(hWnd);
			break;

		/* receive user command, 
		 * according user command, 
		 * change win, 
		 * and than redraw window */
		case MSG_KEYDOWN:
			mainKeyDown(hWnd, wParam, lParam);
			break;
		/* close windows */
		case MSG_CLOSE:
			DestroyMainWindow(hWnd);
			PostQuitMessage(hWnd);
			break;
	}

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

/* ���������൱��c���Ե�main���� */
int MiniGUIMain(int argc, const char* argv[])
{
	printf("Welcome!\n");
	MSG msg;
	HWND hMainWnd;
	MAINWINCREATE CreateInfo;

	init_md_win();	/* added by tsihang for init g_win_all structure */
	init_pos_machine();
	init_dinner_list();

	CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER /*| WS_CAPTION */;
	CreateInfo.dwExStyle = WS_EX_NONE;
	CreateInfo.spCaption = "Hello";
	CreateInfo.hMenu = 0;
	CreateInfo.hIcon = 0;
	CreateInfo.hCursor = GetSystemCursor(0);
	CreateInfo.MainWindowProc = mainProc;      /* ���mainProc�Ǵ��������Ϣ�ĺ������Ժ���Ҫ���������� */
	CreateInfo.lx = 0;
	CreateInfo.ty = 0;
	CreateInfo.rx = 320;
	CreateInfo.by = 240;
	CreateInfo.iBkColor = COLOR_lightwhite;
	CreateInfo.dwAddData = 0;
	CreateInfo.hHosting = HWND_DESKTOP;

	/* ����������� */
	hMainWnd = CreateMainWindow(&CreateInfo);

	if(hMainWnd == HWND_INVALID)
	    return -1;

	/* ��ʾ���� */
	ShowWindow(hMainWnd, SW_SHOWNORMAL);

	/* ������ѭ�x*/
	while(GetMessage(&msg, hMainWnd))
	{
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}

	MainWindowThreadCleanup(hMainWnd);

    return 0;
}
