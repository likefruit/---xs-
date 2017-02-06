/*
 * =====================================================================================
 *
 *       Filename:  md_uart.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  28/03/13 13:51:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tsihang (), isoledadcache@gmail.com
 *        COMPANY:  
 *
 * =====================================================================================
 */




#ifndef UART_H
#define UART_H

#define UART0_DEV		   		"/dev/ttyS1"		
#define UART1_DEV		   		"/dev/ttyS2"
#define UART2_DEV		   		"/dev/ttyS3"
#define UART3_DEV		  		"/dev/ttyS4"
#define UART4_DEV		   		"/dev/ttyS5"
#define UART5_DEV		   		"/dev/ttyS6"


#define BUFFER_LEN				512
#define UART_RBUFF_LEN			BUFFER_LEN
#define UART_DELAY				1000


#define NONE_PARITY				'N'
#define EVEN_PARITY				'E'
#define ODD_PARITY				'O'

#define DEFAULT_FD				0xFFFFFFFF
#define TTYS 						0
#define 							_DEBUG_
#define VOID						void

#define TRUE						1
#define FALSE					(!TRUE)

typedef struct 
{	
	int 			fd;
	int 			baud;        		/* 1200, 2400, ... 115200 */
	unsigned char 			data_bit;     		/* 7 or 8 */
	unsigned char 			stop_bit ;     		/* 01-1; 00-1.5; 10-2; other reverse*/
	unsigned char 			parity_bit;		/* 'N', 'E' or 'O' */
	unsigned char 			valid;			/* 1, valid; 0, invalid */
	unsigned short int 			com_num;  		/* com1, com2, ... com6 */
 }arm_uart_para_info_t;

arm_uart_para_info_t 	g_uart_para_info;

/***************************** UART BAUD ********************************/

#define BAUD_115200				115200
#define BAUD_38400  				38400
#define BAUD_19200  				19200
#define BAUD_9600    				9600
#define BAUD_4800    				4800
#define BAUD_2400    				2400
#define BAUD_1200    				1200

/************************************************************************/



/****************************** DATA BIT ********************************/
typedef enum
{
	DATA_BIT_7 = 				7,
	DATA_BIT_8,
        INVALID_DATA_BIT=		-1
}DATA_BIT;
/************************************************************************/

/***************************** STOP BIT *********************************/
typedef enum
{
	STOP_BIT_1= 				1,
	STOP_BIT_2,
       INVALID_STOP_BIT=			-1
}STOP_BIT;
/************************************************************************/

/****************************** COM NUMBER ******************************/
typedef enum
{
	COM_ZERO =					0,
	COM_ONE,
	COM_TWO,
	COM_THREE,
	COM_FOUR,
	COM_FIVE,
	COM_SIX,
       INVALID_COM =				0xFFFF
}COM_NUM;
/************************************************************************/

/**************************** VALID BIT *********************************/

typedef enum
{
	INVALID =					0,
        VALID
}VALID_BIT;

typedef enum
{
	EMPTY =						0,
        FULL
}BUFFER_STATUS;

typedef enum
{
        UART_EXE_OKAY =			0,
        UART_EXE_FAILE,
        UART_EXE_BAUD_FAILE,
        UART_EXE_PARITY_FAILE,
        UART_EXE_DATABIT_FAILE,
        UART_EXE_STOPBIT_FAILE
}UART_RET;


typedef enum
{
    MOD_SNMP,
    MOD_UART,
    MOD_OLT,
    MOD_NET,
    MOD_PPC,
    MOD_STORE,
    MOD_THREAD,
    MOD_DEBUG,
    MOD_TIMER,
    MOD_MSGQUE,
}ENUM_MODULE;    

typedef enum
{
    LV_DEBUG,          
    LV_INFO,          
    LV_WARNING,       
    LV_ERROR,         
    LV_EMERG,         
    LV_LOG,             
}ENUM_LEVEL;    

/************************************************************************/


/************************* SEND QUEUE STATUS ****************************/


#define	get_tty_num(com, dev) 					\
	switch(com)						\
	{							\
		case 0: strcpy(dev,"/dev/ttyS1"); break;	\
		case 1: strcpy(dev,"/dev/ttyS2"); break;	\
		case 2: strcpy(dev,"/dev/ttyS3"); break;	\
		case 3: strcpy(dev,"/dev/ttyS4"); break;	\
		case 4: strcpy(dev,"/dev/ttyS5"); break;	\
		case 5: strcpy(dev,"/dev/ttyS6"); break;	\
		default: strcpy(dev,"/dev/ttyS1"); break;	\
	}



#define OPEN_DEV(dev)						\
	open(dev,O_RDWR|O_NOCTTY|O_NONBLOCK);



#define CLOSE_DEV(fd) 						\
	close(fd);						\
	tcflush(fd, TCOFLUSH);					\
	printf("Com%d closeed\n", uart_para_info.com_num);	


extern ssize_t uart_recvn(int fd, void *vptr, size_t n);
extern ssize_t uart_sendn(int fd, const void *vptr, size_t n);
extern ssize_t uart_recv(int fd, void *ptr, size_t nbytes);
extern ssize_t uart_send(int fd, const void *ptr, size_t nbytes);
extern int uart_packet_send(int fd, void*ptr, size_t nbytes);
extern int uart_packet_recv(int fd, void*ptr, size_t nbytes);

extern VOID *uart_send_task(VOID);
extern VOID *uart_recv_task(VOID);

#endif


