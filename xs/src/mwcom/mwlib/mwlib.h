#ifdef __cplusplus
extern "C"{
#endif

#ifndef MWLIB_H
#define MWLIB_H


void mwlib_encrypt(unsigned char *buf, unsigned int len);
void mwlib_decrypt(unsigned char *buf, unsigned int len);

#endif // MWLIB_H

#ifdef __cplusplus
}
#endif
