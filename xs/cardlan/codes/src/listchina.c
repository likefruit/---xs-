/*
 ** $Id: listview.c,v 1.25 2007-08-30 01:20:10 xwyan Exp $
 **
 ** listview.c: Sample program for MiniGUI Programming Guide
 **      Usage of LISTVIEW control.
 **
 ** Copyright (C) 2004 ~ 2007 Feynman Software.
 **
 ** License: GPL
 */

#include "maintest.h"
#include <sys/time.h>
#include <time.h>

int settimeofday(const struct timeval *tv , const struct timezone *tz);

#define 	RTC_STATIC   	900
#define 	RTC_EDITYEAR 	910
#define 	RTC_EDITMON 	911
#define 	RTC_EDITDAY 	912
#define 	RTC_EDITHOUR 	913
#define 	RTC_EDITMIN 	914
#define 	RTC_EDITSEC 	915

char Textyear[4],Textmon[2],Textday[2],Texthour[2],Textmin[2],Textsec[2];
static int tyear,tmon,tday,thour,tmin,tsec,pyear,pmon,pday,phour,pmin,psec;
char rtcbuffer[60] = {0,};
static char key=1;
char count=0;
int rtc;
int time_fb;
time_t t,ts;   
struct tm tm,*tms;  
struct timeval tv= { 0, 0 };

static HWND hwndedit;

static CTRLDATA CtrlRtc[] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        10,
        15,
        380,
        18,
        RTC_STATIC,
        "请依次设置,单项完成后回车确认:",
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        51,
        45,
        42,
        23,
        RTC_EDITYEAR,
        NULL,
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        119,
        45,
        25,
        23,
        RTC_EDITMON,
        NULL,
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        170,
        45,
        25,
        23,
        RTC_EDITDAY,
        NULL,
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        69,
        80,
        25,
        24,
        RTC_EDITHOUR,
        NULL,
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        119,
        80,
        25,
        24,
        RTC_EDITMIN,
        NULL,
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        170,
        80,
        25,
        24,
        RTC_EDITSEC,
        NULL,
        0
    }
};

static DLGTEMPLATE DlgRtc =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 320, 240,
    "系统时间修改",
    0, 0,
    TABLESIZE(CtrlRtc), 
    CtrlRtc,
    0
};

static int RtcProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case MSG_INITDIALOG:
            { 
                time (&ts);   
                tms = localtime (&ts);   
                hwndedit=GetDlgItem (hDlg, RTC_EDITYEAR);
                SendMessage (hwndedit, EM_LIMITTEXT, 4, 0L);
                hwndedit=GetDlgItem (hDlg, RTC_EDITMON);
                SendMessage (hwndedit, EM_LIMITTEXT, 2, 0L);
                hwndedit=GetDlgItem (hDlg, RTC_EDITDAY);
                SendMessage (hwndedit, EM_LIMITTEXT, 2, 0L);
                hwndedit=GetDlgItem (hDlg, RTC_EDITHOUR);
                SendMessage (hwndedit, EM_LIMITTEXT, 2, 0L);
                hwndedit=GetDlgItem (hDlg, RTC_EDITMIN);
                SendMessage (hwndedit, EM_LIMITTEXT, 2, 0L);
                hwndedit=GetDlgItem (hDlg, RTC_EDITSEC);
                SendMessage (hwndedit, EM_LIMITTEXT, 2, 0L);
                return 1;
            }
        case MSG_PAINT:
            {
                HDC hdc = BeginPaint (hDlg);
                bzero(rtcbuffer,sizeof(rtcbuffer));
                sprintf(rtcbuffer,"当前时间:%04d年%02d月%02d日%02d时%02d分%02d秒",tms->tm_year+1900,tms->tm_mon+1,tms->tm_mday, tms->tm_hour,tms->tm_min,tms->tm_sec); 
                SetBkColor( hdc,1000);
                SetTextColor( hdc,6000);
                TextOut (hdc, 17, 145,rtcbuffer);
                TextOut (hdc, 98, 49,"年"); 
                TextOut (hdc, 148, 49,"月");
                TextOut (hdc, 199, 49,"日");
                TextOut (hdc, 98, 84,"时");
                TextOut (hdc, 148, 84,"分");
                TextOut (hdc, 199, 84,"秒");
                EndPaint (hDlg, hdc); 
                return 0;
            }
        case MSG_KEYUP:
            {
                if((count==4)||(count==6)||(count==8)||(count==10)||(count==12)||(count==14))
                {
                    SendMessage (hDlg, MSG_KEYDOWN, SCANCODE_ENTER, 0L);
                }  
                else
                {
                    count++;
                }
            }
            break;
        case MSG_KEYDOWN:
            {
                switch(wParam)
                {
                    case SCANCODE_BACKSPACE:
                        {
                            GetDlgItemText(hDlg,RTC_EDITYEAR,Textyear,4);
                            if(strlen(Textyear) == 0)
                            {
                                EndDialog (hDlg, wParam);
                                close (rtc);
                            }
                            count--;
                            count--;
                        }
                        break;
                    case SCANCODE_ENTER:
                        {
                            switch(key)
                            {
                                case 1:
                                    GetDlgItemText(hDlg,RTC_EDITYEAR,Textyear,4);
                                    pyear = atoi(Textyear);
                                    if((pyear >=1900)&&(pyear < 2013))
                                    {
                                        tm.tm_year=(pyear-1900);
                                        SendMessage (hDlg, MSG_KEYDOWN, SCANCODE_TAB, 0L);
                                        key++;
                                        count--;
                                    }
                                    else
                                    {
                                        MessageBox (hDlg, "年份取值范围是：1900 － 2012 \n请重新输入年份信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                                        count--;
                                        count--;
                                    }
                                    break;
                                case 2:
                                    GetDlgItemText(hDlg,RTC_EDITMON,Textmon,2);
                                    pmon = atoi(Textmon);
                                    if((pmon >=1)&&(pmon < 13))
                                    {
                                        tm.tm_mon=(pmon-1);
                                        SendMessage (hDlg, MSG_KEYDOWN, SCANCODE_TAB, 0L);
                                        key++;
                                        count--;
                                    }
                                    else 
                                    {
                                        MessageBox (hDlg, "月份取值范围是：01 － 12 \n请重新输入月份信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                                        count--;
                                        count--;
                                    }
                                    break;
                                case 3:
                                    GetDlgItemText(hDlg,RTC_EDITDAY,Textday,2);
                                    pday = atoi(Textday);
                                    if((pday >=1)&&(pday < 32))
                                    {
                                        tm.tm_mday=pday;
                                        SendMessage (hDlg, MSG_KEYDOWN, SCANCODE_TAB, 0L);
                                        key++;
                                        count--;
                                    }
                                    else
                                    {
                                        MessageBox (hDlg, "日期取值范围是：01 － 31 \n请重新输入日期信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                                        count--;
                                        count--;
                                    }
                                    break;
                                case 4:
                                    GetDlgItemText(hDlg,RTC_EDITHOUR,Texthour,2);
                                    phour = atoi(Texthour);
                                    if((phour >=1)&&(phour < 24))
                                    {
                                        tm.tm_hour=phour;
                                        SendMessage (hDlg, MSG_KEYDOWN, SCANCODE_TAB, 0L);
                                        key++;
                                        count--;
                                    }
                                    else
                                    {
                                        MessageBox (hDlg, "小时取值范围是：01 － 24 \n请重新输入小时信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                                        count--;
                                        count--;
                                    }
                                    break;
                                case 5:
                                    GetDlgItemText(hDlg,RTC_EDITMIN,Textmin,2);
                                    pmin = atoi(Textmin);
                                    if((pmin >=1)&&(pmin < 60))
                                    {
                                        tm.tm_min=pmin;
                                        SendMessage (hDlg, MSG_KEYDOWN, SCANCODE_TAB, 0L);
                                        key++;
                                        count--;
                                    }
                                    else
                                    {
                                        MessageBox (hDlg, "分钟取值范围是：01 － 59 \n请重新输入分钟信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                                        count--;
                                        count--;
                                    }
                                    break;
                                case 6:
                                    GetDlgItemText(hDlg,RTC_EDITSEC,Textsec,2);
                                    psec = atoi(Textsec);
                                    if((psec >=1)&&(psec < 60))
                                    {
                                        MessageBox (hDlg, "时间已修改OK", "温馨提示！", MB_OK | MB_ICONINFORMATION);				
                                        tm.tm_sec=psec;
                                        t=mktime(&tm);
                                        tv.tv_sec=t;
                                        time_fb=settimeofday(&tv,NULL);
                                        if(time_fb!=0)
                                        {
                                            perror("time error");
                                        }
                                        key=1;
                                        count=0;
                                        EndDialog (hDlg, wParam);
                                    }
                                    else
                                    {
                                        MessageBox (hDlg, "秒取值范围是：1 － 59 \n请重新输入秒信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                                        count--;
                                        count--;
                                    }
                                    break;
                            }
                        }
                        break;
                }
            }
            break;
        case MSG_CLOSE:
            {
                EndDialog (hDlg, wParam);
            }
            break;
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int RTC_Server(HWND hWnd)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "RTC" , 0 , 0);
#endif

    if (!InitMiniGUIExt()) {
        return 2;
    }

    DlgRtc.controls = CtrlRtc;
    DlgRtc.controlnr = TABLESIZE(CtrlRtc);

    DialogBoxIndirectParam (&DlgRtc, HWND_DESKTOP, RtcProc, 0L);

    MiniGUIExtCleanUp ();

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif



