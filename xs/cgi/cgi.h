#ifndef __CGI_H__
#define __CGI_H__

#include <stdio.h>
#include <ctype.h>
#include "xs.h"

static inline void html_start()
{
    printf("Content-Type:text/html\n\n");
    printf("<html>\n");
}

static inline void html_end()
{
    printf("</html>\n");
}

static inline void head_start()
{
    printf("<head>");
}
static inline void head_end()
{
    printf("</head>");
}

static inline void head_codeset(const char* codeset)
{
    printf("<meta charset=\"%s\">", codeset);
}
static inline void head_title(const char* title)
{
    printf("<title>%s</title>", title);
}

static inline char* read_env(const char* key)
{
    return  getenv(key);
}

static inline int read_post(char* buf, int size)
{
    return fread( buf, 1, size, stdin);
}

static inline int is_method(const char* m)
{
    char* method = xs_strdup(read_env("REQUEST_METHOD"));
    if(method == NULL)
    {
        return 0;
    }
    int i = 0;

    while(method[i])
    {
        method[i] = toupper(method[i]);
        ++i;
    }

    i = strcmp(method, m);
    xs_free(method);
    return i==0;
}

static inline int is_post()
{
    return is_method("POST");
}
static inline int is_get()
{
    return is_method("GET");
}


#endif
