#include <inttypes.h>
#include "opcard.h"
#include "mwlib/mwlib.h"

#ifdef __cplusplus
extern "C"{
#endif

#include "xs.h"
void mwcom_trace_output(const char* file, int line)
{
    FILE* f = fopen("mw.log", "a+");
    if(f)
    {
#ifdef WIN32
        fprintf(f, "%s %d\r\n", file, line);
#else
        fprintf(f, "%s %d\n", file, line);
#endif

        fclose(f);
    }
}


#ifdef WIN32
#else
#include <sys/ioctl.h>
#endif
static unsigned char _key[7] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0};
static char _filter[] = {1, 2, 3, 4, -1, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
    33, 34, 35, 36, 37, 0x7f, -1, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};
#ifdef WIN32


int read_card(unsigned long* cardno, unsigned char* buf, unsigned char* key, char* filter)
{
    HANDLE hDev;
    int ret;
    uint16_t cardType;
    int i;
    unsigned char size;
    if(key == NULL) key = _key;
    if(filter == NULL) filter = _filter;

    hDev = Open_USB();
    if((int)hDev < 0)
        return -1;

    ret = rf_request(hDev, 1, &cardType);
    if(ret != 0)
    {
        Close_USB(hDev);
        return -1;
    }

    ret = rf_anticoll(hDev, 0, cardno);
    if(ret != 0)
    {
        Close_USB(hDev);
        return -1;
    }

    ret = rf_select(hDev, *cardno, &size);
    if(ret != 0)
    {
        Close_USB(hDev);
        return -1;
    }

    for(i=0; i<64; i++)
    {
        if(filter[i] == 0x7f)
            break;

        if(i % 4 == 3) continue;
        if(filter[i] == 0)
            continue;


        ret = rf_authentication_key(hDev, 0, i, key);
        if(ret != 0)
        {
            Close_USB(hDev);
            return -1;
        }

        ret = rf_read(hDev, i, buf + i * 16);
        if(ret != 0)
        {
            Close_USB(hDev);
            return -1;
        }
    }

    rf_halt(hDev);
    Close_USB(hDev);
    return 0;
}

int write_card(unsigned long* cardno, unsigned char* buf, unsigned char* key, char* filter)
{
    HANDLE hDev;
    int i;
    int ret;
    unsigned long cardno1 = *cardno;
    uint16_t cardType;
    unsigned char size;
    if(key == NULL) key = _key;
    if(filter == NULL) filter = _filter;

    mwcom_trace();

    hDev = Open_USB();
    if((int)hDev < 0)
        return -1;

    mwcom_trace();

    ret = rf_request(hDev, 1, &cardType);
    if(ret != 0)
    {
        Close_USB(hDev);
        return -2;
    }

    mwcom_trace();

    ret = rf_anticoll(hDev, 0, cardno);
    if(ret != 0)
    {
        Close_USB(hDev);
        return -3;
    }

    mwcom_trace();

    if(*cardno != cardno1)
    {
        Close_USB(hDev);
        return -4;
    }

    mwcom_trace();

    ret = rf_select(hDev, *cardno, &size);
    if(ret != 0)
    {
        Close_USB(hDev);
        return -5;
    }

    mwcom_trace();

    for(i=0; i<64; i++)
    {
        if(filter[i] == 0x7f)
            break;

        if(i==0) continue;
        if(i % 4 == 3) continue;
        if(filter[i] == 0)
            continue;

        mwcom_trace();

        ret = rf_authentication_key(hDev, 0, i, key);
        if(ret != 0)
        {
            Close_USB(hDev);
            mwcom_trace();
            return -6;
        }

        mwcom_trace();

        ret = rf_write(hDev, i, buf + i * 16);
        if(ret != 0)
        {
            mwcom_trace();
            Close_USB(hDev);
            return -7;
        }
    }

    mwcom_trace();

    rf_halt(hDev);
    Close_USB(hDev);

    mwcom_trace();
    return 0;
}

#else /* ifdef WIN32 */
void close_card(int fd)
{
    close(fd);
}
int open_card()
{
    int fd = open("/dev/typea", O_RDWR);
    return fd;
}

int card_setkey(int card, unsigned char* key, int block)
{
        int ret;
        struct card_buf keybuf;
        keybuf.mode = KEYA;
        memcpy(keybuf.key, key, sizeof(keybuf.key));
        memset(keybuf.rwbuf, 0xff, 16);
        memset(keybuf.money, 0x00, 4);

        // if it is M1 card
        ret = ioctl(card, RC531_M1_CSN);
        if(ret != MI_SUCCESS)
        {
                xs_logd("M1 CSN error");
                ret = ioctl(card, RC531_M1_CSN);
                if(ret != MI_SUCCESS)
                {
                    ret = ioctl(card, RC531_M1_CSN);
                    if(ret != MI_SUCCESS)
                    {
                        xs_logd("M1 CSN error again");
                        return -2 ;
                    }
                }
        }
        xs_logd("M1 CSN ok");

        /* write key to driver */
        ret = ioctl(card, WRITE_TYPE, W_CHAR);
        if(ret != MI_SUCCESS)
        {
                xs_logd("ready write key error");
                return -1;
        }
        ret = write(card, &keybuf, sizeof(struct card_buf));
        if(ret != MI_SUCCESS)
        {
                xs_logd("write key error");
                return -1;
        }

        /* check the key */
        ret =  ioctl(card, RC531_AUTHENT, block);
        if(ret != MI_SUCCESS)
        {
                xs_logd("M1 RC531_AUTHENT error");
                return -1;
        }

        return 0;
}

int read_block(int card, int block, char* buf, unsigned char* key)
{
        int authBlock = (block/4+1)*4-1;
        int ret;

        /* set key and check */
        ret = card_setkey(card, key, authBlock);
        if(ret < 0)
                return ret;
        /* tell driver, ready to read */
        ret = ioctl(card, RC531_READ, block);
        if(ret != MI_SUCCESS)
        {
                xs_logd("M1 RC531_READ error");
                return -1;
        }

        /* do read */
        ret = read(card, buf, 16);
        xs_logd("read length is =%d", ret);
        if(ret < 0)
                return ret;
        return 0;
}

int read_card(unsigned long *cardno, unsigned char *buf, unsigned char *key, char *filter)
{
    int card = open_card();
    int ret = -1;
    int i;
    if(key == NULL) key = _key;
    if(filter == NULL) filter = _filter;
    if(card < 0)
    {
            xs_loge("open card error");
            return -1;
    }

    for(i=0; i<64; i++)
    {
            if(filter[i] == 0x7f)
                break;

            if(i % 4 == 3) continue;
            if(filter[i] == 0)
                continue;

            ret = read_block(card, i, (char*)buf+i*16, key);
            if(ret < 0)
                    break;
    }

    *cardno = *(unsigned long*)buf;
    close_card(card);
    return ret;
}

int check_is_old_card(unsigned long cardno)
{
        unsigned long now_cardno;
        unsigned char buf[256];
        char filter[10] = {1, 2, 3, 4, 0x7f, -1};
        int ret;
        ret = read_card(&now_cardno, buf, NULL, filter);
        if(ret < 0)
        {
                xs_logd("card removed");
                return ret;
        }
        if(now_cardno != cardno)
        {
                xs_logd("card changed, now_cardno = %lld", (long long int)now_cardno);
                return -3;
        }
        return 0;
}

int write_block(int card, int block, char* buf, unsigned char* key)
{
        int authBlock = (block/4+1)*4-1;
        int ret;

        /* set key and check */
        ret = card_setkey(card, key, authBlock);
        if(ret < 0)
                return ret;

        ret = ioctl(card, WRITE_TYPE, W_CARD);
        if(ret != MI_SUCCESS)
        {
                xs_logd("set write type error");
                return -1;
        }
        /* tell driver, ready to read */
        ret = ioctl(card, RC531_WRITE, block);
        if(ret != MI_SUCCESS)
        {
                xs_logd("M1 RC531_WRITE error");
                return -1;
        }

        /* do read */
        ret = write(card, buf, 16);
        xs_logd("write length is =%d", ret);
        if(ret < 0)
                return ret;
        return 0;
}

int write_card(unsigned long *cardno, unsigned char *buf, unsigned char *key, char *filter)
{
    int ret;
        int card, i;
    ret = check_is_old_card(*cardno);
    if(ret < 0)
            return ret;

    if(key == NULL) key = _key;
    if(filter == NULL) filter = _filter;

    card = open_card();
    for(i=1; i<64; i++)
    {
            if(filter[i] == 0x7f)
                break;

            if(i % 4 == 3) continue;
            if(filter[i] == 0)
                continue;

            ret = write_block(card, i, (char*)buf+i*16, key);
            if(ret < 0)
                    break;
    }

    close_card(card);
    return 0;
}


#endif /* #ifdef WIN32 */

int is_valid_card(unsigned char *buf)
{
    mwlib_encrypt(buf+16, 16);
    return memcmp(buf, buf+16, 16);
}

void init_card(unsigned char* buf)
{
    memcpy(buf+16, buf, 16);
    mwlib_encrypt(buf+16, 16);
}



int write_card_data(unsigned long cardno, unsigned char* buf, char* balance, char* name, char* zone,
        char* dept, char* num, char* id, char* orderid, char* daily)
{
    unsigned char* pos = buf;
    /* name */
    mwcom_trace();

    pos += 64;
    if(name)
    {
        strncpy((char*)pos,    name,       48);
        mwlib_encrypt(pos, 48);
    }

    mwcom_trace();

    pos += 64;
    if(balance)
    {
        strncpy((char*)pos,    balance,    48);
        mwlib_encrypt(pos, 48);
    }

    mwcom_trace();

    pos += 64;
    if(zone )
    {
        strncpy((char*)pos, zone,       48);
        mwlib_encrypt(pos, 48);
    }

    mwcom_trace();

    pos += 64;
    if(dept )
    {
        strncpy((char*)pos, dept,       48);
        mwlib_encrypt(pos, 48);
    }

    mwcom_trace();

    pos += 64;
    if(num )
    {
        strncpy((char*)pos, num,        48);
        mwlib_encrypt(pos, 48);
    }

    mwcom_trace();

    pos += 64;
    if(id )
    {
        strncpy((char*)pos, id,         24);
        mwlib_encrypt(pos, 24);


    }

    mwcom_trace();
    if(daily)
    {
        strncpy((char*)pos+24, daily, 24);
        mwlib_encrypt(pos+24, 24);
    }

    mwcom_trace();


    pos += 64;
    if(orderid )
    {
        strncpy((char*)pos, orderid,    48);
        mwlib_encrypt(pos, 48);
    }

    mwcom_trace();

    static char filter[] = {0, 0, 0, 0, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, 36, 37, 0x7f, -1, 40, 41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};

    return write_card(&cardno, buf, NULL, filter);
}

void read_card_data(unsigned char* buf, char* balance, char* name, char* zone,
        char* dept, char* num, char* id, char* orderid, char* daily)
{
    unsigned char* pos = buf;

    pos += 64;
    if(name)
    {
        memcpy(name, (char*)pos, 48);
        mwlib_encrypt((unsigned char*)name, 48);
    }
    pos += 64;
    if(balance)
    {
        memcpy(balance , (char*)pos,48);
        mwlib_encrypt((unsigned char*)balance, 48);
    }
    pos += 64;
    if(zone)
    {
        memcpy(zone , (char*)pos, 48);
        mwlib_encrypt((unsigned char*)zone, 48);
    }
    pos += 64;
    if(dept)
    {
        memcpy(dept , (char*)pos,48);
        mwlib_encrypt((unsigned char*)dept, 48);
    }
    pos += 64;
    if(num)
    {
        memcpy(num, (char*)pos,48);
        mwlib_encrypt((unsigned char*)num, 48);
    }
    pos += 64;
    if(id)
    {
        memcpy(id, (char*)pos, 24);
        mwlib_encrypt((unsigned char*)id, 24);

    }
    if(daily)
    {
        memcpy(daily, (char* )pos+24, 24);
        mwlib_encrypt((unsigned char*)daily, 24);
        daily[23] = 0;
    }

    pos += 64;
    if(orderid)
    {
        memcpy(orderid, (char*)pos, 48);
        mwlib_encrypt((unsigned char*)orderid, 48);
    }
}

#ifdef __cplusplus
}
#endif
