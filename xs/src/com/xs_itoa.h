#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_ITOA_H__
#define __XS_ITOA_H__

static inline char* xs_itoa_r(int64_t v, char* buf)
{
#ifdef WIN32
    sprintf(buf, "%I64d", (long long int)v);
#else
    sprintf(buf, "%lld", ( long long int)v);
#endif
    return buf;
}

static inline char* xs_itoa(int64_t v)
{
    static char buf[128];
    return xs_itoa_r(v, buf);
}

#endif
#ifdef __cplusplus
}
#endif
