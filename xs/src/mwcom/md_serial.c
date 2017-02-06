#ifdef __cplusplus
extern "C"{
#endif

#ifndef WIN32
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
typedef enum
{
    UART_EXE_OKAY =			0,
    UART_EXE_FAILE,
    UART_EXE_BAUD_FAILE,
    UART_EXE_PARITY_FAILE,
    UART_EXE_DATABIT_FAILE,
    UART_EXE_STOPBIT_FAILE
}UART_RET;
int uart_set_proper(int fd, int baud, int databits, int stopbits, int parity)
{
    struct termios options, oldoptions;
    int tc_ret = 0;

    tc_ret = tcgetattr(fd, &oldoptions);
    if(tc_ret != 0)
    {
        printf("error errno=%d\n", errno);
        perror("set_proper");
        return(UART_EXE_FAILE);
    }

    bzero(&options, sizeof(options));
    options.c_cflag  |=  CLOCAL | CREAD;
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
        case 7:
            options.c_cflag |= CS7; break;
        case 8:
            options.c_cflag |= CS8; break;
        default:
            fprintf(stderr,"Unsupported data bit !\n");
            return (UART_EXE_DATABIT_FAILE);
    }
    switch (parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            options.c_cflag |= PARENB;
            options.c_cflag |= PARODD;
            //options.c_iflag |= INPCK;
            options.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            //options.c_iflag |= INPCK;
            options.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'S':
        case 's':
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            fprintf(stderr,"Unsupported parity!\n");
            return (UART_EXE_PARITY_FAILE);
    }

    switch(baud)
    {
        case 2400:
            cfsetispeed(&options, B2400);
            cfsetospeed(&options, B2400);
            break;
        case 4800:
            cfsetispeed(&options, B4800);
            cfsetospeed(&options, B4800);
            break;
        case 9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
        case 115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
        default:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
    }

    switch (stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;	break;
        case 2:
            options.c_cflag |= CSTOPB; 	break;
        default:
            fprintf(stderr,"Unsupported stop bits ! \n");
            return (UART_EXE_STOPBIT_FAILE);
    }

    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;

    tcflush(fd,TCIFLUSH);
    if ((tcsetattr(fd,TCSANOW,&options)) != 0)
    {
        perror("tcsetattr failed!\n");
        return (UART_EXE_FAILE);
    }
    printf("set uart done!\n");

    return (UART_EXE_OKAY);
}
#endif


#ifdef __cplusplus
}
#endif
