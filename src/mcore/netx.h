#pragma once


#ifndef __netx_h__
#define __netx_h__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#ifdef _MSC_VER
#define netx_errno              GetLastError()
#define netx_ewouldblock        WSAEWOULDBLOCK
#define netx_econnreset         WSAECONNRESET
	typedef int					socklen_t;

#else
    // close
#include <unistd.h>
    // sockaddr_in
#include <netinet/in.h>
    // inet_nota
#include <sys/socket.h>
#include <arpa/inet.h>
    // errno
#include <errno.h>
#define netx_errno              errno
#define netx_ewouldblock        EWOULDBLOCK
#define netx_econnreset         ECONNRESET
#endif

#ifndef _len_str_defined_
#define _len_str_defined_
    typedef struct len_str
    {
        char *data;
        int len;
    } _len_str;
#endif

int netx_init();
int netx_uninit();
int netx_http_req(struct len_str *url, int port, int get, struct len_str *req, char *obuf, int obuf_siz);

#define netx_open_flag_reuse_addr   0x01

int netx_open(long type/* same as socket()'s type, SOCK_STREAM... */, struct in_addr *ip, int port, unsigned int flag);
/*
@local_port[in]     if 0, ignore local info 
@addr[out]          if NULL, ignore*/

int netx_connect(char *host, unsigned long port, struct in_addr *local_ip, int local_port, struct sockaddr_in *addr);

int netx_set_keepalive(int fd, int idle_ms, int interval_ms, int times);
int netx_set_nonblock(int fd, int non);

#if defined(WIN32)
#define netx_close(fd)      closesocket(fd)
#else
#define netx_close(fd)      close(fd)
#endif



#ifdef __cplusplus
}
#endif

#endif
