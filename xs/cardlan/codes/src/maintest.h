#include <stdio.h>									//定义输入／输出函数 
#include <stdlib.h>								 	//定义杂项函数及内存分配函数
#include <errno.h>								 	//定义错误码 
#include <string.h>									//字符串处理 
#include <netdb.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <popt.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>							 	

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>
#include <sqlite3.h>

static HWND     hIconView;

static BITMAP myicons [12];

static const char* iconfiles[12] =
{
    "./res/acroread.png",
    "./res/icons.png",
    "./res/looknfeel.png",
    "./res/package_games.png",
    "./res/tux.png",
    "./res/xemacs.png",
    "./res/gimp.png",
    "./res/kpilot.png",
    "./res/multimedia.png",
    "./res/realplayer.png",
    "./res/usb.png",
    "./res/xmms.png"
};

static const char *iconlabels[12] =
{
    "blue灯",
    "电量",
    "蜂鸣器",
    "GPRS",
    "时钟",
    "打印",
    "SD",
    "按键测试",
    "mifare",
    "综合",
    "待开发",
    "待开发"
};

#define BLUE_ID        0x00
#define BAT_ID         0x01
#define BEEP_ID        0x02
#define GPRS_ID        0x03
#define RTC_ID         0x04
#define PRINT_ID       0x05
#define SD_ID          0x06
#define KEY_ID         0x07
#define MIFIRE_ID      0x08
#define DELAY_ID1      0x10
#define DELAY_ID2      0x11
#define ALL_ID         0x09

#define IDC_ICONVIEW    100
#define IDC_MAIN_EDIT   101
#define DATE_TIMER      102

static CTRLDATA CtrlBook[] =
{
    {
        CTRL_ICONVIEW,
        WS_BORDER | WS_CHILD | WS_VISIBLE,
        0, 0, 320, 210,
        IDC_ICONVIEW,
        "",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | WS_BORDER  | ES_BASELINE | ES_AUTOWRAP,
        0, 	210,  320, 	20,
        IDC_MAIN_EDIT,
        "",
        0
    }
};

static DLGTEMPLATE DlgIcon =
{
    WS_BORDER ,
    WS_EX_NONE,
    0, 0, 320, 240,
    "",
    0, 0,
    TABLESIZE(CtrlBook), CtrlBook,
    0
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TPSPARAM(x,y,z)		((x << 24) | (y << 16) | z)

#define LED1  1
#define LED2  2
#define OFF   0
#define ON    1
#define BLINK 2
#define GPIO1 1
#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define LOW   0
#define HIGH  1
#define NONE	0

enum
{
    SETLED,
    SETGPIO,
    SETLP,
    SETVMAIN,
    SETVCORE,
    SETLDO
};
#define BLUELOOK        GPIO1   //蓝色灯照明RMB
//################################-----M1卡测试程序-----########################################
//KEYA KEYB
#define		KEYA			0x0A
#define		KEYB			0x0B

#define 	DO_TYPEA_M1             0xB0
#define		DO_TYPEA_CPU            0xB1
#define		DO_TYPEB_CPU		0xB4
#define         FIFO_RCV_LEN            0xB3
#define		WRITE_TYPE		0xB2
//WRITE type
#define	        W_CARD                  0x01
#define 	W_CHAR			0x02
#define 	W_CPU                   0x03


#define 	RC531_M1_CSN            0xAB
//TYPEA--------------M1 Command
#define		RC531_MFOUTSELECT       0xA0
#define 	RC531_REQIDL            0xA1
#define 	RC531_REQALL            0xA2
#define 	RC531_ANTICOLL          0xA3
#define 	RC531_SELECT            0xA4
#define 	RC531_AUTHENT           0xA5
#define 	RC531_READ              0xA6
#define 	RC531_WRITE             0xA7
#define 	RC531_INC               0xA8
#define 	RC531_DEC               0xA9
#define 	RC531_HALT              0xAA

//TYPEA--------------FM1208------------------
#define		TYPEA_CPU_REST		0xAC
#define		TYPEA_CPU_PPS		0xAD
//TYPEB--------------SFEN------------------
#define		GET_SFEN_CSN	        0xAE

#define 	MI_OK		        0x00

#define		MIFARE_X		10
#define		MIFARE_Y		70

struct card_buf
{
    unsigned char key[6];
    unsigned char mode;
    unsigned char rwbuf[16];
    unsigned char money[4];
};
#define         OVERTIME        10
#define 	IDC_EDIT 	100
#define         TIME_BASE         1500
#define 	ID_TIMER_MIFARE	TIME_BASE

static CTRLDATA CtrlMifare[] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        10,
        15,
        380,
        18,
        IDC_STATIC,
        "Input sector number",
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        10,
        45,
        260,
        24,
        IDC_EDIT,
        NULL,
        0
    }

};

static DLGTEMPLATE DlgMifare =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 320, 240,
    "M1 card test program",
    0, 0,
    TABLESIZE(CtrlMifare), 
    CtrlMifare,
    0
};

