#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_NET_H__
#define __XS_NET_H__


#ifdef WIN32
/* select model in win32 */

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET XSOCKET;
#define EINPROGRESS WSAEWOULDBLOCK
char *strtok_r(char *s, const char *delim, char **save_ptr);
#define EPOLLIN 1
#define EPOLLOUT 2
#define EPOLL_CTL_DEL 0
#define EPOLL_CTL_MOD 1
#define EPOLL_CTL_ADD 2
struct epoll_event
{
    union
    {
        void* ptr;
    } data;
    int events;
};

int epoll_ctl(int p, int m, XSOCKET fd, struct epoll_event* ptr);
int epoll_create(int c);
int epoll_wait(int p, struct epoll_event* ev, int c, int tw);
int xs_close_epoll(int epollfd);
#define xs_close_socket closesocket
int inet_pton(int af, const char *src, void *dst);

int socketpair(int af, int type, int protocol, XSOCKET socks[2]);



#else

#include <sys/epoll.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#define xs_close_epoll close
#define xs_close_socket(_fd) do{xs_logd("close socket");close(_fd);}while(0)
typedef int XSOCKET;
#endif

#endif
#ifdef __cplusplus
}
#endif
