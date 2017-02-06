#include "mwlib.h"
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif

#define xs_malloc malloc
#define xs_free free

static inline void xs_encrypt_swap(unsigned char* key, int i, int j)
{
    if(i==j)
        return;
    unsigned char ch = key[i];
    key[i] = key[j];
    key[j] = ch;
}

static void xs_encrypt_gen_key(unsigned char* key, int size)
{
    int i, j;
    if(key[size-1] > key[size-2])
    {
        xs_encrypt_swap(key, size-1, size-2);
        return;
    }

    for(i=size-2; i>0; i--)
    {
        if(key[i] > key[i-1])
        {
            for(j=size-1; j>=i; j--)
            {
                if(key[i-1] < key[j])
                {
                    xs_encrypt_swap(key, j, i-1);
                    goto RET;
                }
            }
        }
    }
RET:
    while(1)
    {
        --size;
        if(i>=size)
            return;
        xs_encrypt_swap(key, i, size);
        i++;
    }
}

void xs_encrypt(unsigned char* buf, unsigned int len, const unsigned char* enKey, unsigned int keyLen)
{
    unsigned int index = 0;
    unsigned char* key1;
    unsigned int dwBlockSize;
    unsigned int i;

    if(enKey == NULL || keyLen == 0)
    {
        return;
    }
    else
    {
        dwBlockSize = keyLen;
        key1 = (unsigned char*)xs_malloc((size_t)dwBlockSize);
        memcpy(key1, enKey, dwBlockSize* sizeof(unsigned char));
    }

    for(;;)
    {
        xs_encrypt_gen_key(key1, dwBlockSize);

        for(i=0; i<dwBlockSize; i++)
        {
            buf[index++] ^= key1[i];
            if(index == len)
                break;
        }

        if(index == len)
            break;

    }
    xs_free(key1);
}

void mwlib_encrypt(unsigned char *buf, unsigned int len)
{
    unsigned char key[] = {0x13, 0x28, 0xf9, 0x87, 0x47, 0x98, 0xa8};
    xs_encrypt(buf, len, key, 7);
}

void mwlib_decrypt(unsigned char *buf, unsigned int len)
{
    unsigned char key[] = {0x13, 0x28, 0xf9, 0x87, 0x47, 0x98, 0xa8};
    xs_encrypt(buf, len, key, 7);
}

#ifdef __cplusplus
}
#endif
