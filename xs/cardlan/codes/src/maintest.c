#include "maintest.h"

static PLOGFONT logfontttf1,logfontttf2;
BITMAP wchange;
static char updown;
char keybuf[50];
static int asyncio_fd = -1;
#define  POWEROFF		3
#define  KILLPDA                 0
#define 	GPIO_L                 0
#define 	GPIO_H                 1

#define		POWER_GP	       0x01
#define		RESET		       0x02
#define		DTR		       0x03

/**
 *@brief   打开控制GPRS RC531电源的设备文件
 *@param  void
 *@return int  初始化完成后的句柄
 */
static int InitializePower(void)
{
    int fd;
    fd = open("/dev/pwrgprs",O_RDWR);// 打开控制GPRS RC531电源的设备文件
    //fd = open("/dev/power",O_RDWR);// 打开控制GPRS RC531电源的设备文件
    if(fd==-1)
    {
        printf("GPRS Power wrong\r\n");
        return -1;
    }
    return fd;
}

/**
 *@brief   关闭GPIO
 *@param  fd		
 类型  	int  打开控制GPRS RC531电源的设备文件
 *@return void 	关闭 控制GPRS RC531电源的设备文件
 */
static void	UninitializePower(int fd)
{
    close(fd);
}


int wavecom_power_no(void)
{
    int gfd = -1;
    gfd = InitializePower();
    if(gfd < 0)
    {
        return -1;
    }
    ioctl(gfd,POWER_GP,GPIO_H);
    ioctl(gfd,RESET,GPIO_H);
    usleep(60000);
    ioctl(gfd,RESET,GPIO_L);
    usleep(685000);
    ioctl(gfd,POWER_GP,GPIO_L);
    //ioctl(gfd,DTR,GPIO_L);
    UninitializePower(gfd);
    return 0;
}

int wavecom_power_off(void)
{
    int gfd = -1;
    gfd = InitializePower();
    if(gfd < 0)
    {
        return -1;
    }
    ioctl(gfd,POWER_GP,GPIO_H);
    usleep(60000);
    ioctl(gfd,POWER_GP,GPIO_L);
    usleep(685000);
    ioctl(gfd,RESET,GPIO_H);
    //ioctl(gfd,DTR,GPIO_H);
    UninitializePower(gfd);
    return 0;
}

#define 	KILLPDA                 0

int PowerOff(void)
{
    int fd;
    fd = open("/dev/power",O_RDWR);
    if(fd < 0)
    {
        printf("Find Error Power OFF\r\n");
        return -1;
    }
    ioctl(fd,KILLPDA);
    close(fd);
    return fd;
}
//##############################################------------blue_test----------###################
static int blue_test(HWND hWnd)//blue测试程序
{
    char count = 2;
    char buf[1024];
    int fd;
    int tpsparam;

    printf("--------测试blue灯---------\n");
    while(count--)
    {
        fd = open("/dev/tps",O_RDWR);							
        if(fd==-1)
        {
            printf("open /dev/tps wrong\r\n");
            return -1;

        }
        printf("blue灯:开\n");
        tpsparam = TPSPARAM(SETGPIO,BLUELOOK, HIGH);
        write(fd, &tpsparam, 4);  
        sleep(1);
        close(fd);
        fd = open("/dev/tps",O_RDWR);							
        if(fd==-1)
        {
            printf("open /dev/tps wrong\r\n");
            return -1;

        }
        printf("blue灯:关\n");
        tpsparam = TPSPARAM(SETGPIO,BLUELOOK, LOW);
        write(fd, &tpsparam, 4);
        sleep(1);
        close(fd);
    }
    printf("blue灯测试结束！\n");
    MessageBox (hWnd, "blue灯测试结束！", "提示", MB_OK | MB_ICONINFORMATION);
    return 0;   
}
//##############################################------------bat_test----------###################
static int bat_test(HWND hWnd)//bat测试程序
{
    int ret;
    char buf[1024];
    int fd;

    fd = open("/dev/tlc549",O_RDWR);	
    read(fd, &ret, 4);
    sleep(1);
    close(fd);

    fd = open("/dev/tlc549",O_RDWR); 
    if(fd == -1)      				
    {
        printf("open /dev/tlc549 wrong\r\n");
        return -1;
    }

    printf("--------测试电量检测装置---------\n");
    read(fd, &ret, 4);
    sprintf(buf,"battry = %d%\n",(int)(((double)(ret)/255)*100));
    printf("%s\n",buf);
    MessageBox (hWnd,buf, "电量检测", MB_OK | MB_ICONINFORMATION);
    sleep(1);
    close(fd);
    printf("电量检测装置测试结束！\n");
    return 0;
}
//##############################################------------key_test----------###################
static DLGTEMPLATE DlgKEY =		
{	WS_NONE,
    WS_EX_NONE,
    0,0,320,240,
    "键盘测试",
    0,
    0,
    0,
    NULL,
    0
};
static int DialogBoxkeyProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    HDC hdcMem; 
    switch (message)
    {
        case MSG_INITDIALOG:
            {
                updown = 0;
                return(1);
            }
        case MSG_PAINT:
            {
                hdc   =   BeginPaint   (hDlg);
                hdcMem   =   CreateCompatibleDC   (hdc);
                FillBoxWithBitmap(hdcMem,0,0,320,240,&wchange);
                SetBkMode(hdcMem,BM_TRANSPARENT);

                SelectFont(hdcMem,logfontttf2);
                SetTextColor(hdcMem, 43690);
                TextOut(hdcMem, 70,50, "键盘按键测试:");
                TextOut(hdcMem, 70,150, "返回键退出");
                if(updown == 1)
                {
                    updown = 0;
                    TextOut(hdcMem, 70,100, "您已按下按键：");
                    TextOut(hdcMem, 190,100,keybuf);
                }
                if(updown == 2)
                {
                    updown = 0;
                    TextOut(hdcMem, 70,100, "您已松开按键：");
                    TextOut(hdcMem, 190,100,keybuf);
                }
                BitBlt   (hdcMem,   0,   0,   320,   240,   hdc,   0,   0,   0);
                DeleteCompatibleDC   (hdcMem);
                EndPaint   (hDlg,   hdc);
            }
            break;
        case MSG_KEYUP:
            {
                updown = 2;
                SendMessage(hDlg, MSG_PAINT,0, 0);
            }
            break;
        case MSG_KEYDOWN:
            {
                updown = 1;
                switch(wParam)
                {
                    case SCANCODE_BACKSPACE:
                        {
                            EndDialog (hDlg, wParam);
                        }
                        break;
                    case SCANCODE_CURSORBLOCKDOWN:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"DOWN",4);  keybuf[4] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_CURSORBLOCKRIGHT:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"RIGHT",4);keybuf[5] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;
                    case SCANCODE_CURSORBLOCKUP:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"UP",2);keybuf[2] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_CURSORBLOCKLEFT:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"LEFT",4);keybuf[4] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_0:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"0",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_1:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"1",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_2:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"2",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_3:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"3",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_4:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"4",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_5:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"5",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_6:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"6",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_7:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"7",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_8:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"8",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_9:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"9",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_PERIOD:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,".",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_KEYPADPLUS:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"+",1);keybuf[1] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_Z:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"F1",2);keybuf[2] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_F2:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"F2",2);keybuf[2] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_F3:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"F3",2);keybuf[2] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_F4:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"F4",4);keybuf[2] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_TAB:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"TAB",3);keybuf[3] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;	
                    case SCANCODE_ENTER:
                        {
                            memset (&keybuf, 0, sizeof (keybuf));
                            memcpy(keybuf,"OK",2);keybuf[2] = 0;
                            SendMessage(hDlg, MSG_PAINT,0, 0);
                        }
                        break;
                }
            }
            break;
        case MSG_CLOSE:
            {
                EndDialog (hDlg, 0);
                return 0;
            }
            break;
    }	
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}		
static int key_test(HWND hWnd)//键盘测试程序
{
    DlgKEY.controls = NULL;	
    DialogBoxIndirectParam (&DlgKEY, hWnd, DialogBoxkeyProc, 0L);
	

    printf("键盘测试结束！\n");
    MessageBox (hWnd, "键盘测试结束！", "提示", MB_OK | MB_ICONINFORMATION);
    return 0;
}
//################################-----M1卡测试程序-----########################################
static RECT rcCircle = {0, 10, 300, 200};
static int overtime;
static int fd;
static int block;
static int flagread;

static int DialogBoxMifareProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    char strText[4];
    struct card_buf test;
printf("DialogBoxMifareProc \n");
    switch (message)
    {
        case MSG_INITDIALOG:
            {

                fd = open("/dev/typea",O_RDWR);
                if(fd == -1)
                {
                    printf("open typea error wrong\r\n");
                }
                ioctl(fd, DO_TYPEA_M1);
                return 1;
            }

        case MSG_KEYDOWN:
            {
                switch(wParam)
                {
                    case SCANCODE_BACKSPACE:
                        {
                            GetDlgItemText(hDlg,IDC_EDIT,strText,3);
                            if(strlen(strText) == 0)
                            {
                                KillTimer (hDlg, ID_TIMER_MIFARE);
                                EndDialog (hDlg, wParam);
                                close(fd);
                            }	
                        }
                        break;
                    case SCANCODE_ENTER:
                        {
                            GetDlgItemText(hDlg,IDC_EDIT,strText,3);
                            block = atoi(strText);
				  printf("block %d\n", block);
                            if((block >=0)&&(block < 16))
                            {
                                flagread = 1;
                                SetTimer (hDlg, ID_TIMER_MIFARE, 10);
                                block = block * 4;
                            }
                            else
                            {
                                MessageBox (hDlg, "扇区取值范围是：0 － 15，请\n重新输入扇区信息！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                                flagread = 0;					
                            }
                        }
                        break;
                }		
            }
            break;
        case MSG_PAINT:
            {

                static 	RECT rc1;
                HDC hdc = BeginPaint (hDlg);
                char buffer[60] = {0,};
                static char disp;
                if(flagread)
                {

                    int flag = 1;
                    int t = 0;
                    int i;
                    int ret;

                    static int  receive_len[1] ={0};
                    static char receive_buf[50]={0};
                    static char strchr[6];
                    struct card_buf test;

                    disp = 1;

                    test.mode = KEYA;
                    memset(test.key,0xFF,6);
                    memset(test.rwbuf,0xff,16);
                    memset(test.money,0x00,4);

                    flag = 1;
                    t = 5;
                    while(flag)
                    {
                        switch(t)
                        {
                            case 5:
                                ret = ioctl(fd, 0xAB);
                                if(ret == MI_OK)t++;
                                else 
                                {
                                    ret = ioctl(fd, 0xAB);
                                    if(ret == MI_OK)t++;
                                    else flag = 0;
                                }
                                break;
                            case 6:
                                ioctl(fd, WRITE_TYPE, W_CHAR);
                                ret = write(fd, &test, sizeof(struct card_buf));
                                if(ret == MI_OK)t++;
                                else flag = 0;
                                break;
                            case 7:
                                ioctl(fd, FIFO_RCV_LEN, receive_len);
                                read(fd, receive_buf, receive_len[0]);

                                ret = ioctl(fd, RC531_AUTHENT,(block + 3));
                                if(ret == MI_OK)t++;
                                else flag = 0;
                                break;
                            case 8:
                                ret = ioctl(fd, RC531_READ,(block + 0));
                                if(ret == MI_OK)t++;
                                else flag = 0;
                                break;
                            case 9:
                                ret = ioctl(fd, RC531_READ,(block + 1));
                                if(ret == MI_OK)t++;
                                else flag = 0;
                                break;
                            case 10:
                                ret = ioctl(fd, RC531_READ,(block + 2));
                                if(ret == MI_OK)t++;
                                else flag = 0;
                                break;
                            case 11:
                                ret = ioctl(fd, RC531_READ,(block + 3));
                                if(ret == MI_OK)t++;
                                else flag = 0;
                                break;
                            default:
                                flag = 0;
                                t = 0;
                                break;
                        }
                        if(t > 8)
                        {
                            ioctl(fd, FIFO_RCV_LEN, receive_len);
                            read(fd, receive_buf, receive_len[0]);
                            bzero(buffer,sizeof(buffer));
                            for(i = 0; i < receive_len[0]; i++)
                            {
                                bzero(strchr,sizeof(strchr));
                                sprintf(strchr,"%02X ",receive_buf[i]);
                                strcat(buffer,strchr);
                            }
                            disp++;
                            rc1.left = MIFARE_X; rc1.top  = MIFARE_Y + 15*disp; rc1.right = 300; rc1.bottom = 161;
                            SetBkColor (hdc, COLOR_lightwhite);
                            Rectangle (hdc, rc1.left, rc1.top, rc1.right, rc1.bottom);
                            InflateRect (&rc1, -1, -1);
                            SelectFont (hdc, logfontttf1);
                            DrawText (hdc, buffer, -1, &rc1, DT_NOCLIP | DT_LEFT | DT_WORDBREAK);
                            if(t == 12)KillTimer (hDlg, ID_TIMER_MIFARE);
                        }
                    }
                    switch(t)
                    {
                        case 5:
                            KillTimer (hDlg, ID_TIMER_MIFARE);
                            MessageBox (hDlg, "请把卡片放在读卡区！", "温馨提示！", MB_OK | MB_ICONINFORMATION);
                            break;
                        case 7:
                            KillTimer (hDlg, ID_TIMER_MIFARE);
                            bzero(buffer,sizeof(buffer));
                            sprintf(buffer,"验证卡片失败,只能得到\n  CSN：%02X %02X %02X %02X",receive_buf[0],receive_buf[1],receive_buf[2],receive_buf[3]);
                            MessageBox (hDlg, buffer, "温馨提示！", MB_OK | MB_ICONINFORMATION);
                            break;
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                            KillTimer (hDlg, ID_TIMER_MIFARE);
                            MessageBox (hDlg, "注意：可能所密钥控制字KEYA的权限不够！", "读操作失败！", MB_OK | MB_ICONINFORMATION);
                            break;

                    }
                }
                disp = 1;
                bzero(buffer,sizeof(buffer));
                sprintf(buffer,"读%02d扇区内容如下:",block/4);
                rc1.left = MIFARE_X; rc1.top  = MIFARE_Y + 15*disp; rc1.right = 300; rc1.bottom = 161;
                SetBkColor (hdc, COLOR_lightwhite);
                Rectangle (hdc, rc1.left, rc1.top, rc1.right, rc1.bottom);
                InflateRect (&rc1, -1, -1);

                SelectFont (hdc, logfontttf2);
                DrawText (hdc, buffer, -1, &rc1, DT_NOCLIP | DT_LEFT | DT_WORDBREAK);

                EndPaint (hDlg, hdc);
                break;
            }
        case MSG_TIMER:
            {	
                if (wParam == ID_TIMER_MIFARE) 
                {
                    overtime--;
                    if(overtime == 0)
                    {
                        KillTimer (hDlg, ID_TIMER_MIFARE);
                        EndDialog (hDlg, wParam);
                        close(fd);
                    }
                    else
                    {
                        InvalidateRect (hDlg,&rcCircle, TRUE);
                    }
                }
            }
            break;
            return 0;
        case MSG_CLOSE:
            {
                KillTimer (hDlg, ID_TIMER_MIFARE);
                EndDialog (hDlg, wParam);
                close(fd);
            }
            break;

    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static int mifare_test(HWND hWnd)//M1卡测试程序
{	
    overtime = OVERTIME*10;
    block    = 0;
    DlgMifare.controls = CtrlMifare;	
    DialogBoxIndirectParam (&DlgMifare, hWnd, DialogBoxMifareProc, 0L);
    return 0;
}
//##############################################------------print_test----------###################
#define  PRINTER_NUM      32
#define PRINTER_MOMTOR    0xA0
#define PRINTER_TEM	  0xA1

static FILE *fp_zk;//fp_zk字库文件指针
static int fp_fd;  //fp_fd打印机设备标识

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
    fclose(fp_zk);//关闭字库文件
    close(fp_fd);//关闭打印机设备
    return 0;
}

int readline(const char *filename,int line,char *date)
{
    FILE*fp ;
    int zch;
    int xlin = -1;
    if(access(filename, 0) == 0)/*要是不存在就建立一个新test.db数据库文件，并创建record记录表*/ 
    {
        fp = fopen(filename,"r");
        fseek(fp, 0, SEEK_SET);
        zch = fgetc(fp);
        while(zch != (-1))
        {
            if(zch == '\n')xlin++;
            if(xlin == line)break;
            zch = fgetc(fp);
        }
        if(zch < 0)
        {
            fseek(fp, -1, SEEK_CUR);
        }
        fgets(date,100,fp);//用fgets读第一行到数组s中
        fclose(fp);
    }
    return xlin;
}

int PrintText(char *pzText)
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
    write(fp_fd,string,(cur_l+1)*1150);//填写打印缓冲区
    return 0;
}



int Big_PrintText(char *pzText)
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
    ioctl(fp_fd,PRINTER_MOMTOR,7000);//加快打印速度
    ioctl(fp_fd,PRINTER_TEM,10000);   //加热时间增加
    write(fp_fd,string,(cur_l+1)*1150);//填写打印缓冲区
    ioctl(fp_fd,PRINTER_MOMTOR,7000);//加快打印速度
    ioctl(fp_fd,PRINTER_TEM,7000);
    return 0;
}

static int print_test(HWND hWnd)//print测试程序
{
    unsigned char Content[50];
    char buf[1024];
    open_printer();
    PrintText(".............................");
    PrintText(".........1234567890..........");
    PrintText("");
    PrintText("ABCDEFG...............abcdefg");
    PrintText("");
    PrintText(".........打印测试程序.........");
    PrintText("");
    Big_PrintText(".........打印测试程序.........");
    Big_PrintText("...............................");
    PrintText("");
    PrintText("..................................");
    PrintText("");
    PrintText("");
    close_printer();
    printf("打印测试结束！\n");
    MessageBox (hWnd, "打印测试结束！", "提示", MB_OK | MB_ICONINFORMATION);
    return 0;
}
//##############################################------------sd_test----------###################
int MmcCardDetect(void)
{
    struct  dirent	*pDir;
    DIR	*dir;
    struct  stat   ftype;
    char fullpath[PATH_MAX + NAME_MAX + 1] = {0, };	
    char dirpath[] = "/dev";
    char systemcall[40] = {0};

    dir = opendir (dirpath);					
    while ((pDir = readdir(dir)) != NULL) 				
    {
        strncpy (fullpath, dirpath, PATH_MAX);			
        if (fullpath[ strlen(fullpath) - 1 ] != '/')
            strcat (fullpath, "/");
        strcat (fullpath, pDir->d_name);
        if (lstat (fullpath, &ftype) < 0 )
        {
            continue;
        }
        if(strstr(pDir->d_name, "mmcblk0p1") != NULL)		
        {
            strcpy(systemcall, "mount ");
            strcat(strcat(systemcall,fullpath)," /mnt/mmc");
            printf(systemcall);
            printf("\n");
            system(systemcall);				
            system("ls /mnt/mmc/ -l");			
            return 1;
        }
    }
    return -1;
}

int MmcCardClose(void)
{
    system("umount /mnt/mmc");
    return 0;
}

static int sd_test(HWND hWnd)//sd测试程序
{
    FILE *HZK; 
    char buf[1024];

    int ret, lenth;
    int fd;
    char *newfile = "/mnt/mmc/sdtest.txt";
    char w_data[1024] = "This is MMC-SD test.";
    char r_data[1024];

    printf("MMC-SD TEST\n");	
    ret = MmcCardDetect();
    if (ret < 0) {
        printf("MMC-SD not be mounted\n");
        MessageBox (hWnd, "请检查SD卡是否放入机器内！", "提示", MB_OK | MB_ICONINFORMATION);
        return -1;
    }
    printf("MMC-SD have been mounted\n");
    if((HZK=fopen(newfile,"w+"))==NULL) 
    {
        printf("Error open!\n");
        return -1;
    }
    memset(buf,0,sizeof(buf));
    printf("写入数据：深圳市卡联科技有限公司\r\n技术支持：\r\n TEST MMC SD OK\r\n");
    strcpy(buf,"深圳市卡联科技有限公司\r\n技术支持：\r\n TEST MMC SD OK\r\n");
    if(fwrite(buf,strlen(buf),1,HZK) != 1)
    {
        printf("文件写入错误\n");
        MessageBox (hWnd, "文件写入错误！", "提示", MB_OK | MB_ICONINFORMATION);
    }
    else 
    {
        MessageBox (hWnd, "文件写入正确！", "提示", MB_OK | MB_ICONINFORMATION);
        printf("文件写入正确\n");
    }    
    fclose(HZK);
    printf("MMC-SD test Ok.\n");
    MmcCardClose();	    
    printf("sd测试结束！\n");
    MessageBox (hWnd, "SD测试结束！", "提示", MB_OK | MB_ICONINFORMATION);
    return 0;
}
//##############################################------------beep_test----------###################
static int beep_test(HWND hWnd)//beep测试程序
{	
    int count = 2;
    char buf[1024];
    int fd;

    fd=open("/dev/beep",O_RDWR);
    if(fd<0)  
    {
        printf("Can't open /dev/beep \n");
        return -1;
    }
    while(count--) 
    {	
        ioctl(fd,0);
        printf("蜂鸣器: 开\n");
        sleep(1);//sleep 1s
        ioctl(fd,1);
        printf("蜂鸣器: 关\n");
        sleep(1);
    }
    ioctl(fd,0);
    close(fd);
    printf("蜂鸣器测试结束！\n");
    MessageBox (hWnd, "蜂鸣器测试结束！", "提示", MB_OK | MB_ICONINFORMATION);
    return 0;
}
//##############################################------------all_test----------###################
static int all_test(HWND hWnd)//all测试程序
{	
    blue_test(hWnd);
    bat_test(hWnd);
    beep_test(hWnd);
    gprs_test(hWnd);
    RTC_Server(hWnd);
    print_test(hWnd);
    sd_test(hWnd);
    key_test(hWnd);
    mifare_test(hWnd);
    return 0;
}

//##############################################------------MIAN_Switch----------###################
void Main_Switch(HWND hWnd,int Id)
{
    switch(Id)
    {
        case BLUE_ID:
            blue_test(hWnd);
            break;
        case BAT_ID:
            bat_test(hWnd);
            break;
        case BEEP_ID:
            beep_test(hWnd);
            break;
        case GPRS_ID:
            gprs_test(hWnd);
            break;
        case RTC_ID:
            RTC_Server(hWnd);
            break;
        case PRINT_ID:
            print_test(hWnd);
            break;
        case SD_ID:
            sd_test(hWnd);
            break;
        case KEY_ID:
            key_test(hWnd);
            break;
        case MIFIRE_ID:
            mifare_test(hWnd);
            break;
        case ALL_ID:
            all_test(hWnd);
            break;
        case DELAY_ID1:
            MessageBox (hWnd, "正在建设中。。。。。。", "提示", MB_OK | MB_ICONINFORMATION);
            break;
        case DELAY_ID2:
            MessageBox (hWnd, "正在建设中。。。。。。", "提示", MB_OK | MB_ICONINFORMATION);
            break;
        default:
            MessageBox (hWnd, "正在建设中。。。。。。", "提示", MB_OK | MB_ICONINFORMATION);
            break;
    }
}
/*********************************************************************************************************/
void signal_handler(int num)//处理函数
{
    int status = -1;
    int asyiono = -1;


    read(asyncio_fd, &status, 4);//判断是那个驱动发出的signo 信号

    printf("line[45]	status = %d\n", status);

    asyiono = (char)status;	//低8位保存的为那个异步IO
    printf("asyiono = %X,--------------------status = 0x%X\n",asyiono,status);

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
        default:break;
    }
}
int InitAsyncio(void)//初始化异步io驱动
{
    int oflags;

    if ((asyncio_fd = open("/dev/asyncio",O_RDWR)) < 0) 
    {
        exit(1);
    }	
    signal(SIGIO, signal_handler);			//绑定action到	SIGIO signal
    fcntl(asyncio_fd, F_SETOWN, getpid());		//设置STDIN_FILENOD的owner为当前进程
    oflags = fcntl(asyncio_fd, F_GETFL);		//获取文件打开的方式
    fcntl(asyncio_fd, F_SETFL, oflags | FASYNC);	//设置STDIN_FILENO的文件状态标志（增加了FASYNC标志）.
    return 0;
}
static BITMAP bmp;

static char* mk_time (char* buff)   
{   
    time_t t;   
    struct tm * tm;   

    time (&t);   
    tm = localtime (&t);   
    sprintf(buff,"时间：%d年%d月%d日       %d时%d分%d秒",   
            tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,   
            tm->tm_hour,tm->tm_min,tm->tm_sec);   
    return buff;   
} 

    static int
BookProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    char time_buf[50];

    switch (message)
    {
        case MSG_INITDIALOG:
            {
                IVITEMINFO ivii;
                static int i = 0;
                hIconView = GetDlgItem (hDlg, IDC_ICONVIEW);
                SetWindowBkColor (hIconView, PIXEL_lightwhite); 
                SendMessage (hIconView, IVM_SETITEMSIZE, 74, 65);
                for (i = 0; i < TABLESIZE(myicons); i++) 
                {
                    memset (&ivii, 0, sizeof(IVITEMINFO));
                    ivii.bmp = &myicons[i];
                    ivii.nItem = i;
                    ivii.label = iconlabels[i];
                    ivii.addData = (DWORD)iconlabels[i];
                    SendMessage (hIconView, IVM_ADDITEM, 0, (LPARAM)&ivii);
                }
                SendMessage (hIconView, IVM_SETCURSEL, 0, 0);
                LoadBitmapFromFile(HDC_SCREEN,&wchange,"./res/wchange.jpg");
                break;
            }
        case MSG_CREATE:
            {
                logfontttf1 = CreateLogFont ("type1", "song", "GB2312", 
                        FONT_WEIGHT_LIGHT,
                        FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                        FONT_OTHER_TTFNOCACHE, FONT_UNDERLINE_NONE, 
                        FONT_STRUCKOUT_NONE, 
                        12, 0);
                logfontttf2 = CreateLogFont( NULL, "song", "GB2312",
                        FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                        16, 0); 
                InitAsyncio();//初始化异步io驱动
                wavecom_power_no();
                usleep(10000);
                system("pppd call gprs &");		
                SetTimer(hDlg,DATE_TIMER,50);  
            }
            break;
        case MSG_KEYDOWN:
            {
                int sel = SendMessage (hIconView, IVM_GETCURSEL, 0, 0);
                if (sel >= 0)
                {
                    int count = SendMessage (hIconView, IVM_GETITEMCOUNT, 0, 0);
                    int x,y;
                    switch(wParam)
                    {
                        case SCANCODE_CURSORBLOCKUP:
                            {
                                x = sel / 4;
                                y = sel % 4;
                                x--;
                                sel = x * 4 + y;

                                SendMessage (hIconView, IVM_SETCURSEL, sel, 1);
                            }
                            break;
                        case SCANCODE_CURSORBLOCKDOWN:
                            {
                                x = sel / 4;
                                y = sel % 4;
                                x++;
                                sel = x * 4 + y;
                                SendMessage (hIconView, IVM_SETCURSEL, sel, 1);
                            }
                            break;
                        case SCANCODE_CURSORBLOCKRIGHT:
                            {
                                if(++sel == count )sel = 0;
                                SendMessage (hIconView, IVM_SETCURSEL, sel, 1);
                            }
                            break;
                        case SCANCODE_CURSORBLOCKLEFT:
                            {
                                if(--sel < 0)sel = count - 1;
                                SendMessage (hIconView, IVM_SETCURSEL, sel, 1);
                            }
                            break;	
                        case SCANCODE_ENTER:
                            {
                                Main_Switch(hDlg,sel);
                            }
                            break;	
                    }
                }
            }
            break;
        case MSG_TIMER:   
            {   
                if(wParam== DATE_TIMER)   
                {   
                    SetDlgItemText(hDlg,IDC_MAIN_EDIT,mk_time(time_buf));   
                }      
                break;   
            } 
            break;
        case MSG_CLOSE:
            {
                DestroyLogFont (logfontttf1);
                DestroyLogFont (logfontttf2);
                EndDialog (hDlg, 0);
                return 0;
            }

    } /* end switch */
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    int i;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "iconview" , 0 , 0);
#endif
    /* load pictures */
    for (i=0; i<TABLESIZE(myicons); i++) 
    {
        LoadBitmap (HDC_SCREEN, &myicons[i], iconfiles[i]);
    }

    /* init mini gui, do not know */
    if (!InitMiniGUIExt()) 
    {
        return 2;
    }

    DialogBoxIndirectParam (&DlgIcon, HWND_DESKTOP, BookProc, 0L);

    MiniGUIExtCleanUp ();

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif
