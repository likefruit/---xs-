#ifdef __cplusplus
extern "C"{
#endif

#ifndef __OPCARD_H__
#define __OPCARD_H__

#ifdef WIN32
#include <windows.h>
#include "mt/mwrf32.h"

#endif

typedef struct card_buf
{
    unsigned char key[6];
    unsigned char mode;
    unsigned char rwbuf[16];
    unsigned char money[4];
}card_buf;

#define mwcom_trace() mwcom_trace_output(__FILE__, __LINE__)
void mwcom_trace_output(const char* file, int line);


#define		KEYA				0x0A
#define		KEYB				0x0B

#define 		DO_TYPEA_M1    		0xB0
#define		DO_TYPEA_CPU         0xB1
#define		WRITE_TYPE		0xB2
#define         FIFO_RCV_LEN           0xB3
#define		DO_TYPEB_CPU		0xB4

//WRITE type
#define	W_CARD                  		0x01
#define 	W_CHAR					0x02
#define 	W_CPU                   		0x03


//TYPEA--------------M1 Command
#define	RC531_MFOUTSELECT       0xA0
#define 	RC531_REQIDL            	0xA1
#define 	RC531_REQALL            	0xA2
#define 	RC531_ANTICOLL          	0xA3
#define 	RC531_SELECT            	0xA4
#define 	RC531_AUTHENT           	0xA5
#define 	RC531_READ              	0xA6
#define 	RC531_WRITE             	0xA7
#define 	RC531_INC               		0xA8
#define 	RC531_DEC               		0xA9
#define 	RC531_HALT              	0xAA

#define 	RC531_M1_CSN            	0xAB

//TYPEA--------------FM1208------------------
#define	TYPEA_CPU_REST		0xAC
#define	TYPEA_CPU_PPS			0xAD
//TYPEB--------------SFEN------------------
#define	GET_SFEN_CSN	        	0xAE



#define md_mifare_card_check_csn_type() 			ioctl(md_mifare_card_fd, RC531_M1_CSN)
#define md_mifare_card_is_validation( sector )		ioctl(md_mifare_card_fd, RC531_AUTHENT, (4*sector + 3))
#define SECTOR_SIZE				16
#define _MIF_					128
#define MD_MIFARE_CARD_PATH	"/dev/typea"
typedef enum MI_CARD_OP_R
{
    MI_SUCCESS	= 0x00,
    MI_ERROR
}__MI_CARD_OP_RESULT;

extern char mifare_buff[1024] ;

extern int md_mifare_card_fd;

extern int md_mifare_card_init();

extern int md_mifare_card_read(int , char *, int );
extern int md_mifare_card_write(int , char *, int );


int read_card(unsigned long* cardno, unsigned char* buf, unsigned char* key, char* filter);
int write_card(unsigned long* cardno, unsigned char* buf, unsigned char* key, char* filter);

void read_card_data(unsigned char* buf, char* balance, char* name, char* zone,
        char* dept, char* num, char* id, char* orderid, char* daily);
int write_card_data(unsigned long cardno, unsigned char* buf, char* balance, char* name, char* zone,
        char* dept, char* num, char* id, char* orderid, char* daily);
int is_valid_card(unsigned char *buf);
void init_card(unsigned char* buf);

#endif


#ifdef __cplusplus
}
#endif
