
#include "cgi.h"
#include "db.h"
/*
 一开始没参数，或者参数不对，都丢个问题给它
 如果参数正确，显示通讯录给它
 如果
*/

void show_login()
{
    printf("我们的班主任是谁啊...");
}

int main()
{
    xs_init();
    html_start();

    head_start();
    head_codeset("UTF-8");
    head_title("91504班级通讯录");
    head_end();

    if(is_post())
        printf("POST\n"); 
    else
        show_login();


    html_end();
    xs_fini();
    return 0;
}
