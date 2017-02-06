#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_SOCK_H__
#define __XS_SOCK_H__

extern int xs_sock_connect(uint16_t port, const char* ip);
extern struct in_addr xs_sock_getip(const char* name);

static inline int xs_sock_set_nonblock(XSOCKET fd)
{
#ifdef WIN32
    BOOL flg = TRUE;
    if (ioctlsocket(fd, FIONBIO, (unsigned long *)&flg) != 0)
      return -1;
    return 0;

#else
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
#endif
}
static inline int xs_sock_bind(XSOCKET fd, uint16_t port, const char* ip)
{
    int ret;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if(ip)
        inet_pton(AF_INET, ip, &addr.sin_addr); 
    else
        addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));

    assert(ret == 0);

    return ret;
}

static inline int xs_sock_listen(XSOCKET fd, int buf)
{
    int ret;
#ifdef WIN32
#else
    int timeout = 5;
#endif
    ret = listen(fd, buf);
    if(ret < 0)
        return ret;

    xs_sock_set_nonblock(fd);
#ifdef WIN32
#else
    ret = setsockopt(fd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &timeout, sizeof(timeout));
#endif
    return 0;
}

void xs_sock_close(xs_ev_sock_t* sev);

static inline void xs_sock_wait_peer_close(XSOCKET fd)
{
    xs_ev_add_sock_ev(fd, EPOLLIN, xs_sock_close, NULL);
}

int xs_sock_send_block(XSOCKET fd, char* buf, int size, int timeout);
int xs_sock_recv_block(XSOCKET fd, void* buf, int size, int timeout);

#endif
#ifdef __cplusplus
}
#endif
