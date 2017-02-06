#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

char* xs_popen_with_output(const char* c, ...)
{
    /* get cmd */
    static char buf[2048];
    FILE* f;
    int ret;
    char cmd[1024];
    va_list ap;

    va_start(ap, c);
    vsprintf(cmd, c, ap);
    va_end(ap);

    f = popen(cmd, "r");

    if(!f)
        return NULL;

    ret = fread(buf, 1, 2048, f);
    fclose(f);

    if(ret <= 0)
        return NULL;

    buf[ret-1] = 0;

    return buf;
}

void xs_popen(const char* c, ...)
{
    /* get cmd */
    char cmd[1024];
    va_list ap;
    FILE* f;
    va_start(ap, c);
    vsprintf(cmd, c, ap);
    va_end(ap);

    f = popen(cmd, "r");
    if(!f)
    {
        return;
    }

    fclose(f);
}

int xs_system(const char* c, ...)
{
    /* get cmd */
    char cmd[1024];
    va_list ap;

    va_start(ap, c);
    vsprintf(cmd, c, ap);
    va_end(ap);

    return system(cmd);
}
#ifdef __cplusplus
}
#endif
