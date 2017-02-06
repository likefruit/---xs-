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

#define 	GPRS_STATIC   800
#define 	GPRS_EDITIP1 	810
#define 	GPRS_EDITIP2 	811
#define 	GPRS_EDITIP3 	812
#define 	GPRS_EDITIP4 	813
#define 	GPRS_EDITPORT 	814

unsigned char TcpIpBufd[35];  //服务器IP地址
unsigned char TcpIpPort[6];  //服务器PORT地址

#define USECARDIP 1
#define GPRSPR 1

int sockfd,ret,i,statuss,pip,pport;
unsigned char  socketflag=0;

char Textip1[3],Textip2[3],Textip3[3],Textip4[3],Textport[4];
char gprsflag,sendflag,receiveflag1,receiveflag2,receiveflag3,receiveflag4,receiveflag5;
char buf[50],buff[50],sendbuf[50],receivebuf1[50],receivebuf2[50],receivebuf3[50],receivebuf4[50],receivebuf5[50];

static HWND hwndedit;

static CTRLDATA CtrlGprs[] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        10,
        15,
        380,
        18,
        GPRS_STATIC,
        "请依次设置,单项完成后回车确认:",
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        55,
        45,
        33,
        23,
        GPRS_EDITIP1,
        NULL,
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        100,
        45,
        33,
        23,
        GPRS_EDITIP2,
        NULL,
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        145,
        45,
        33,
        23,
        GPRS_EDITIP3,
        NULL,
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        190,
        45,
        33,
        24,
        GPRS_EDITIP4,
        NULL,
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        55,
        80,
        40,
        24,
        GPRS_EDITPORT,
        NULL,
        0
    },
};

static DLGTEMPLATE DlgGprs =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 320, 240,
    "GPRS连接测试",
    0, 0,
    TABLESIZE(CtrlGprs), 
    CtrlGprs,
    0
};
/*
 *************************************************************************************************************
 - 函数名称 : int createProxy(int *sock)
 - 函数说明 : 读GPRS数据
 - 输入参数 : 无
 - 输出参数 : 无
 *************************************************************************************************************
 */

int createProxy(int *sock)
{
    unsigned char TcpIpBuf[16];
    char *IPBuf1,*IPBuf2,*IPBuf3,*IPBuf4;
    char *hostname;
    static struct hostent *hptr = NULL;
    struct sockaddr_in tcpaddr;
    int trueSocket;
    int setValue=1;
    int flags;
    fd_set connectionAble;
    int selectRet,iResult=0;
    struct timeval vl;

    setValue=1;
    iResult=0;       
    vl.tv_sec  = 5;
    vl.tv_usec = 0;	
    trueSocket=socket(AF_INET,SOCK_STREAM,0);
    tcpaddr.sin_family = AF_INET;

#if USECARDIP
    if((TcpIpBufd[0]>='0')&&(TcpIpBufd[0]<='9'))
    {
        memcpy(TcpIpBuf,TcpIpBufd,16);
        sscanf(TcpIpBuf, "%d.%d.%d.%d",(int *)&IPBuf1,(int *)&IPBuf2,(int *)&IPBuf3,(int *)&IPBuf4);
        sprintf(TcpIpBuf,"%d.%d.%d.%d",(int )IPBuf1,(int )IPBuf2,(int )IPBuf3,(int )IPBuf4);

#if GPRSPR
        printf("client IP %s,Port %s\n",TcpIpBuf,TcpIpPort);
#endif 

        tcpaddr.sin_port = htons(atoi(TcpIpPort));
        tcpaddr.sin_addr.s_addr = inet_addr(TcpIpBuf); 
    }else
    {
        hostname = TcpIpBufd;
#if GPRSPR  
        printf("hostname client...%s\n",hostname);
#endif   
        if((hptr == NULL)||(socketflag >3))
        {

#if GPRSPR  
            printf("hostname is  create\n");
#endif

            if((hptr = gethostbyname(hostname))==NULL)
            {
#if GPRSPR  
                printf("hostname is bad\n");
#endif
                iResult=-1;
                return iResult;
            }

        }
        else 
        {

#if GPRSPR   
            printf("hostname is  hold\n");
#endif
        }




#if GPRSPR   
        printf("client2 IP %s,Port %s\n",inet_ntoa(*((struct in_addr*)(hptr->h_addr))),TcpIpPort);
#endif
        tcpaddr.sin_port=htons(atoi(TcpIpPort));
        tcpaddr.sin_addr.s_addr=inet_addr(inet_ntoa(*((struct in_addr*)(hptr->h_addr))));	
    }

#else
    tcpaddr.sin_port = htons(atoi("4000"));
    tcpaddr.sin_addr.s_addr = inet_addr("192.168.18.119");
#endif

    bzero(&(tcpaddr.sin_zero),8);
    setsockopt(trueSocket,SOL_SOCKET,SO_REUSEADDR,(const char*)&setValue,sizeof(int));

    if((flags = fcntl(trueSocket, F_GETFL)) < 0 )
    {
#if GPRSPR
        perror("fcntl F_SETFL");
#endif
        iResult=-1;
        return iResult;
    }
    flags |= O_NONBLOCK;  
    if(fcntl(trueSocket, F_SETFL,flags) < 0)
    {
#if GPRSPR
        perror("fcntl");
#endif
        iResult=-1;
        return iResult;
    }
    connect(trueSocket,(struct sockaddr *)&tcpaddr,sizeof(struct sockaddr)); 
    FD_ZERO(&connectionAble);
    FD_SET(trueSocket,&connectionAble);
    selectRet=select(trueSocket+1,0,&connectionAble,0,&vl);
    if(selectRet == 0)
    {
#if GPRSPR
        printf("connection timeout!\n");
#endif
        iResult=-1;
        socketflag ++;
        return iResult;
    }
    else if(selectRet > 0)
    {
        flags = fcntl(trueSocket, F_GETFL,0);
        flags &= ~ O_NONBLOCK;
        fcntl(trueSocket, F_SETFL,flags);
        FD_CLR(trueSocket,&connectionAble);
        *sock=trueSocket;
        iResult = 0;
        socketflag = 0;
#if GPRSPR
        printf("selectRet = %d\n",selectRet);
        printf("connection sucess!\n");
#endif     
    }

    return iResult;
}
/*
 *************************************************************************************************************
 - 函数名称 : int write_datas_gprs(int sockfd, unsigned char *buffer, int buf_len)
 - 函数说明 : 读GPRS数据
 - 输入参数 : 无
 - 输出参数 : 无
 *************************************************************************************************************
 */
int write_datas_gprs(int fd, unsigned char *buffer, int buf_len)
{
    struct timeval tv;
    fd_set w_set;
    int bytes_to_write_total = buf_len;
    int bytes_have_written_total = 0;
    int bytes_write = 0;
    int result = -1;
    unsigned char *ptemp = buffer;

    if (-1 == fd || NULL == buffer || buf_len <=0)
    {
#if GPRSPR
        printf("Send Buffer is Nc\n");
#endif
        return -1;
    }

    while (bytes_to_write_total > 0)
    {	
        FD_ZERO(&w_set);
        FD_SET(fd,&w_set);
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        result = select(fd+1, NULL, &w_set, NULL, &tv);
        if (result < 0)
        {
            if (EINTR == errno)
            {
                continue;
            }

#if GPRSPR
            printf("open Send Macine is error1\n");
#endif
            return -1;
        }
        else if (0 == result)    //this means timeout, it is not an error, so we return 0.
        {

#if GPRSPR
            printf("Send Data Timeout --->3\n");	
#endif

            return 0;
        }
        else
        {
            if (FD_ISSET(fd, &w_set))
            {

#if GPRSPR
                printf("bytes_to_write_total = %d, STr =%s\n",bytes_to_write_total,ptemp);
                printf("fd -----------> %d\n",fd);
#endif

                bytes_write = send(fd, ptemp, bytes_to_write_total, 0);

#if GPRSPR
                printf("bytes_write = %02X\n",bytes_write);
#endif

                if (bytes_write < 0)
                {
                    if (EAGAIN == errno || EINTR == errno)
                    {
                        continue;
                    }
#if GPRSPR
                    printf("open Send Macine is error2\n");
#endif
                    return -1; 
                }
                else if (0 == bytes_write)
                {
                    //this means that the server has close the connection gracefully.
#if GPRSPR
                    printf("Send Data Timeout --->2\n");
#endif
                    return -1;

                }
                else
                {
                    bytes_to_write_total -= bytes_write;
                    bytes_have_written_total += bytes_write;
                    ptemp += bytes_have_written_total;
#if GPRSPR
                    printf("Write GPRS data\n");
#endif
                }
            }
        }
    }
#if GPRSPR
    printf("----------------->Send Data OK\n");
#endif
    return 0;
}
/*
 *************************************************************************************************************
 - 函数名称 : unsigned char read_datas_tty(int fd)
 - 函数说明 : 读GPRS数据
 - 输入参数 : 无
 - 输出参数 : 无
 *************************************************************************************************************
 */
unsigned char read_datas_tty(int fd,int wait,unsigned char *buffer)
{
    int retval;
    fd_set rfds;
    struct timeval tv;
    int ret = 0;
    int rilen = 0;
    unsigned char status;

    tv.tv_sec = wait;                 // the rcv wait time
    tv.tv_usec = 0;                   // 50ms

    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(fd,&rfds);
        retval = select(fd+1,&rfds,NULL,NULL,&tv);
        if(retval ==-1)
        {
            perror("select()");
            status = 2;
            break;
        }
        else if(retval)
        {
            ret= read(fd,buffer + rilen,512);
#if  GPRSPR
            {
                int i;
                printf("readsocket :");
                for(i=0;i<ret;i++)
                {
                    printf("%02X",buffer[i]);
                }
                printf("\n");
            }
#endif

            rilen += ret;                                          
            if(ret > 0)
            {                                           
                status = 0;
            }
            else
            {                     
                status = 3;
            } 
            break;
        }
        else
        {
            status = 1;
            break;
        }
    }

    return(status);
}

static int GprsProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case MSG_INITDIALOG:
            { 
                sendflag = 0;
                receiveflag1 = 0;receiveflag2 = 0;receiveflag3 = 0;receiveflag4 = 0;receiveflag5 = 0;
                gprsflag = 0;
                hwndedit=GetDlgItem (hDlg, GPRS_EDITIP1);
                SendMessage (hwndedit, EM_LIMITTEXT, 3, 0L);
                hwndedit=GetDlgItem (hDlg, GPRS_EDITIP2);
                SendMessage (hwndedit, EM_LIMITTEXT, 3, 0L);
                hwndedit=GetDlgItem (hDlg, GPRS_EDITIP3);
                SendMessage (hwndedit, EM_LIMITTEXT, 3, 0L);
                hwndedit=GetDlgItem (hDlg, GPRS_EDITIP4);
                SendMessage (hwndedit, EM_LIMITTEXT, 3, 0L);
                hwndedit=GetDlgItem (hDlg, GPRS_EDITPORT);
                SendMessage (hwndedit, EM_LIMITTEXT, 4, 0L);
                return 1;
            }
        case MSG_PAINT:
            {
                HDC hdc = BeginPaint (hDlg);
                SetBkColor( hdc,1000);
                SetTextColor( hdc,6000);
                TextOut (hdc, 10, 49,"IP:"); 
                TextOut (hdc, 10, 84,"PORT:");
                TextOut (hdc, 90, 49,".");
                TextOut (hdc, 135, 49,".");
                TextOut (hdc, 180, 49,".");
                if(gprsflag == 1) TextOut (hdc, 10, 80,"GPRS 连接失败");
                if(sendflag == 1) TextOut (hdc, 110, 84,sendbuf);
                if(receiveflag1 == 1) TextOut (hdc, 18, 104,receivebuf1);
                if(receiveflag2 == 1) TextOut (hdc, 18, 124,receivebuf2);
                if(receiveflag3 == 1) TextOut (hdc, 18, 144,receivebuf3);
                if(receiveflag4 == 1) TextOut (hdc, 18, 164,receivebuf4);
                if(receiveflag5 == 1) TextOut (hdc, 18, 184,receivebuf5);
                EndPaint (hDlg, hdc); 
                return 0;
            }
        case MSG_KEYDOWN:
            {
                switch(wParam)
                {//1
                    case SCANCODE_BACKSPACE:
                        {
                            GetDlgItemText(hDlg,GPRS_EDITIP1,Textip1,3);
                            if(strlen(Textip1) == 0)
                            {
                                close(sockfd);
                                sockfd = -1;
                                EndDialog (hDlg, wParam);
                            }
                        }
                        break;
                    case SCANCODE_ENTER:
                        {//2
                            GetDlgItemText(hDlg,GPRS_EDITIP1,Textip1,3);
                            pip = atoi(Textip1);
                            if((pip < 1)||(pip > 255))
                            {
                                MessageBox (hDlg, "ip取值范围是：1 － 254 \n请重新输入ip信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                            }
                            GetDlgItemText(hDlg,GPRS_EDITIP2,Textip2,3);
                            pip = atoi(Textip2);
                            if((pip < 1)||(pip > 255))
                            {
                                MessageBox (hDlg,"ip取值范围是：1 － 254 \n请重新输入ip信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                            }
                            GetDlgItemText(hDlg,GPRS_EDITIP3,Textip3,3);
                            pip = atoi(Textip3);
                            if((pip < 1)||(pip > 255))
                            {
                                MessageBox (hDlg, "ip取值范围是：1 － 254 \n请重新输入ip信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                            }
                            GetDlgItemText(hDlg,GPRS_EDITIP4,Textip4,3);
                            pip = atoi(Textip4);
                            if((pip < 1)||(pip > 255))
                            {
                                MessageBox (hDlg, "ip取值范围是：1 － 254 \n请重新输入ip信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                            }
                            GetDlgItemText(hDlg,GPRS_EDITPORT,Textport,4);
                            pport = atoi(Textport);
                            if((pip < 0)||(pip > 9999))
                            {
                                MessageBox (hDlg, "port取值范围是：0000 － 9999 \n请重新输入port信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                            }
                            else
                            {
                                memset (&TcpIpBufd, 0, sizeof (TcpIpBufd));
                                memset (&TcpIpPort, 0, sizeof (TcpIpPort));
                                strcat(TcpIpBufd,Textip1);
                                strcat(TcpIpBufd,"."); 
                                strcat(TcpIpBufd,Textip2);   
                                strcat(TcpIpBufd,"."); 
                                strcat(TcpIpBufd,Textip3);   
                                strcat(TcpIpBufd,"."); 
                                strcat(TcpIpBufd,Textip4);   
                                strcat(TcpIpPort,Textport);   
                                strcat(TcpIpBufd,"\0");
                                strcat(TcpIpPort,"\0");
                                printf("\n");
                                printf("TcpIpBufd -----------> %s\n",TcpIpBufd);
                                printf("TcpIpPort -----------> %s\n",TcpIpPort);
                                ret = createProxy(&sockfd);
                                if(ret == 0)
                                {
                                    memset (&buff, 0, sizeof (buff));
                                    memset (&buf, 0, sizeof (buf));
                                    sprintf(buff,"ASDF");     
                                    ret = write_datas_gprs(sockfd,buff,4);
                                    if(ret != 0)
                                    {
                                        printf("GPRS 发送失败\n");
                                        sprintf(sendbuf, "GPRS send <ASDF> NG");
                                        sendflag = 1;
                                        SendMessage(hDlg, MSG_PAINT,0, 0);
                                    }
                                    else
                                    {
                                        sprintf(sendbuf, "GPRS send <ASDF> OK");
                                        printf("GPRS 发送成功\n");
                                        sendflag = 1;
                                        SendMessage(hDlg, MSG_PAINT,0, 0);
                                    } 
                                    for(i = 0;i < 5;i++)
                                    {
                                        memset (&buff, 0, sizeof (buff));
                                        statuss = read_datas_tty(sockfd,20,buff);
                                        if(statuss == 0)
                                        {
                                            printf("GPRS 读返回数据\n");
                                            printf("read data:%s\n",buff);
                                            if(i ==0)
                                            {
                                                receiveflag1 = 1; 
                                                sprintf(receivebuf1, "GPRS receive %d:",i+1);
                                                strcat(receivebuf1,buff);
                                                SendMessage(hDlg, MSG_PAINT,0, 0);
                                            }
                                            if(i ==1)
                                            {
                                                receiveflag2 = 1;
                                                sprintf(receivebuf2, "GPRS receive %d:",i+1);
                                                strcat(receivebuf2,buff);
                                                SendMessage(hDlg, MSG_PAINT,0, 0);
                                            }
                                            if(i ==2)
                                            {
                                                receiveflag3 = 1;
                                                sprintf(receivebuf3, "GPRS receive %d:",i+1);
                                                strcat(receivebuf3,buff);
                                                SendMessage(hDlg, MSG_PAINT,0, 0);
                                            }
                                            if(i == 3 )
                                            {
                                                receiveflag4 = 1;
                                                sprintf(receivebuf4, "GPRS receive %d:",i+1);
                                                strcat(receivebuf4,buff);
                                                SendMessage(hDlg, MSG_PAINT,0, 0);
                                            }
                                            if(i == 4)
                                            {
                                                receiveflag5 = 1;
                                                sprintf(receivebuf5, "GPRS receive %d:",i+1);
                                                strcat(receivebuf5,buff);
                                                SendMessage(hDlg, MSG_PAINT,0, 0);
                                                usleep(9999999);
                                                SendMessage(hDlg, MSG_CLOSE,0, 0);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    gprsflag = 1;
                                    printf("GPRS连接失败\n");
                                }
                            }
                        }//2
                        break;
                }//1
            }
            break;
        case MSG_CLOSE:
            {
                close(sockfd);
                sockfd = -1;
                EndDialog (hDlg, wParam);
            }
            break;
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int gprs_test(HWND hWnd)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "GPRS" , 0 , 0);
#endif

    if (!InitMiniGUIExt()) {
        return 2;
    }

    DlgGprs.controls = CtrlGprs;
    DlgGprs.controlnr = TABLESIZE(CtrlGprs);

    DialogBoxIndirectParam (&DlgGprs, HWND_DESKTOP, GprsProc, 0L);

    MiniGUIExtCleanUp ();

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif



