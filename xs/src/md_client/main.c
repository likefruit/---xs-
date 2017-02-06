
#include <stdio.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <strings.h>
#include "md_uart.h"
#include "xs.h"

int uart_set_proper(int fd, int baud, int databits, int stopbits, int parity);

int send_buf_block(int fd, char* buf, int size)
{
    int ret;
    while(size > 0)
    {
        xs_logd("size=%d", size);
        ret = write(fd, buf, size);
        if(ret < 0)
            return ret;
        else if(ret > 0)
            size -= ret;
    }
    return size;
}

int _recv_buf_block(int fd, char* buf, int size)
{
    int ret;
    int cur = 0;
    while(size)
    {
        ret = read(fd, buf+cur, size);
        if(ret > 0)
        {
            size -= ret;
            cur += ret;
        }
        else if(ret < 0)
            return ret;
    } 
    return size;
}

int recv_buf_block(int fd, char* buf, int* size)
{
    int ret;

    ret = _recv_buf_block(fd, (char*)size, 4);
    if(ret != 0)
        return -1;
    *size = ntohl(*size);
    if(*size > 1024)
    {
        xs_logd("recv buf block size > 1024 =%d", *size);
        return -1;
    }

    ret = _recv_buf_block(fd, buf, *size);
    if(ret != 0)
        return -1;
    return ret;
}

int recv_response(int fd)
{
    int ret;
    char tmp[1024];
    int size;
    xs_model_t* model;
    ret = recv_buf_block(fd, tmp, &size);
    if(ret != 0)
    {
        xs_logd("recv ack error");
        return ret;
    }

    model = xs_model_from_buf(tmp, size);
    if(!xs_success(model->argv[0]))
    {
        xs_logd("err, reason is:%s", model->argv[1]);
        xs_model_delete(model);
        return -1;
    }
    xs_model_delete(model);

    return 0;
}

int send_file_content(int fd, char* filename, int filelen)
{

    int filefd;
    char buf[1024];
    int ret = 0, readlen;
    xs_logd("start send file content");

    filefd = open(filename, O_RDWR);
    if(filefd < 0)
    {
        xs_logd("error open local file");
        return -1;
    }

    while(filelen)
    {
        xs_logd("filelen=%d", filelen);
        readlen = ret = read(filefd, buf, 1024);
        xs_logd("ret=%d", ret);
        if(ret < 0)
        {
            xs_logd("read error");
            break;
        }
        ret = htonl(ret);
        send_buf_block(fd, (char*)&ret, 4);
        send_buf_block(fd, buf, ntohl(ret));

        ret = recv_response(fd);
        if(ret != 0)
        {
            xs_logd("response error");
            break;
        }

        filelen -= readlen;
        xs_logd("now filelen=%d", filelen);
    }

    close(filefd);
    return filelen;
}

int send_file(int fd, char* filename)
{
    int filelen;
    char tmp[1024];
    char* buf;
    int size;
    int ret;
    filelen = xs_file_length(filename);
    xs_model_t* model = xs_model_alloc(4);
    model->argv[0] = xs_strdup("pos");
    model->argv[1] = xs_strdup("trans");
    model->argv[2] = xs_strdup(filename);
    model->argv[3] = xs_strdup(xs_itoa(filelen, tmp));

    buf = xs_model_to_buf(model, &size);
    xs_model_delete(model);

    ret = send_buf_block(fd, buf, size);
    if(ret != 0)
    {
        xs_logd("send req error");
        return ret;
    }

    ret = recv_buf_block(fd, tmp, &size);
    if(ret != 0)
    {
        xs_logd("recv ack error");
        return ret;
    }

    model = xs_model_from_buf(tmp, size);
    if(!xs_success(model->argv[0]))
    {
        xs_logd("err, reason is:%s", model->argv[1]);
        xs_model_delete(model);
        return -1;
    }
    xs_logd("success: len=%s", model->argv[1]);
    xs_model_delete(model);

    return send_file_content(fd, filename, filelen);
}

int main(int argc, char* argv[])
{
    int ret;
    xs_init();
    if(argc != 3)
    {
        xs_logd("argc not 3");
        return -1;
    }

    int fd = open(argv[1], O_RDWR);
    if(fd < 0)
    {
        xs_logd("error open serial");
        return 0;
    }

    if( 0 != uart_set_proper(fd, 115200, 8, 1, 'N'))
    {
        xs_logd("error set proper");
        close(fd);
        return 0;
    }

    xs_logd("ready to send file");

    ret = send_file(fd, argv[2]);
    if(ret != 0)
    {
        xs_logd("send file error");
    }

    close(fd);
    xs_fini();
    return 0;
}

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
