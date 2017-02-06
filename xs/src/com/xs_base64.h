#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_BASE64_H__
#define __XS_BASE64_H__

int xs_base64_encode(uint8_t* src, int ssize, uint8_t** dst);
int xs_base64_decode(uint8_t* src, int ssize, uint8_t** dst);

#endif
#ifdef __cplusplus
}
#endif
