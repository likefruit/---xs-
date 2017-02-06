
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Content-Type:text/html\n\n");
    
    int contentLength = atoi(getenv("CONTENT_LENGTH"));
    int filesize = contentLength;

    char buf[1024];
    int ret;
    char filename[256];

    fgets(buf, sizeof(buf), stdin);
    filesize -= strlen(buf);
    filesize -= strlen(buf);
    filesize -= 4;

    fgets(buf, sizeof(buf), stdin);
    filesize -= strlen(buf);
    char* p = strstr(buf, "filename=");
    if(p == NULL)
    {
        printf("%d<br>\n", __LINE__);
        exit(0);
    }

    p = strtok(p, "\"");
    p = strtok(NULL, "\"");

    /*
     * for windows
     * */
    char* q1;
    char* q2;
    q1 = strtok(p, "\\");
    while((q2 = strtok(NULL, "\\")))
    {
        q1 = q2;
    }
    /* for windows end */

    sprintf(filename, "/var/www/download/%s", q1);

    fgets(buf, sizeof(buf), stdin);
    filesize -= strlen(buf);

    fgets(buf, sizeof(buf), stdin);
    filesize -= strlen(buf);

    FILE* f = fopen(filename, "w");
    if(f == NULL)
    {
        printf("%d<br>\n", __LINE__);
        exit(0);
    }

    while(filesize > 0)
    {
        if(filesize > sizeof(buf))
            ret = fread(buf, 1, sizeof(buf), stdin);
        else
            ret = fread(buf, 1, filesize, stdin);
        if(ret > 0)
        {
            filesize -= ret;
        }
        fwrite(buf, ret, 1, f);
    }

    fclose(f);
    printf("<meta http-equiv=\"Refresh\" content=\"0;URL=/download\" />");

    return 0;
}
