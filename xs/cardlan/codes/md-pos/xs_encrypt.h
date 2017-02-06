/*
 * encrypt.h 
 * author: xueguoliang(xueguoliang@gmail.com)
 * date: 11/4/2012
 *
 * */
#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_ENCRYPT_H__
#define __XS_ENCRYPT_H__

void xs_encrypt(unsigned char* buf, unsigned int len, const unsigned char* enKey, unsigned int keyLen);
#define xs_decrypt xs_encrypt

#endif
#ifdef __cplusplus
}
#endif
