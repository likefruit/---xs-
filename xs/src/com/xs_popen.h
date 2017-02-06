#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_POPEN_H__
#define __XS_POPEN_H__

char* xs_popen_with_output(const char* c, ...);
void xs_popen(const char* cmd, ...);
static inline char* xs_get_ip(const char* eth)
{
    return xs_popen_with_output("/sbin/ifconfig %s | awk '/inet addr/ {print $2}' | cut -f2 -d \":\"", eth);
}
int xs_system(const char* c, ...);

#endif
#ifdef __cplusplus
}
#endif
