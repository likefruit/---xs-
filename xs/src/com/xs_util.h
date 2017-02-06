#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_UTIL_H__
#define __XS_UTIL_H__

static inline int xs_power2(int x)
{
    if(x < 0)
        return 0;
    int i=1;
    x --;
    while((x+1) & x)
    {
        x |= (x>>i);
        i <<= 1;
    }
    return x+1;
}

#endif
#ifdef __cplusplus
}
#endif
