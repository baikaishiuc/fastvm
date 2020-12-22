
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER)
#include <WinSock2.h>
#include <windows.h>
#include <Shlwapi.h>
#include <winhttp.h>

        //parseURL
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ws2_32.lib")

#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#endif
#include "print_util.h"
#include "string_ex.h"
#include "mtime_ex.h"
#include "netx.h"

#define JSYX_CONFIG_URL_HOST            "down.yshigame.com"
#define JSYX_CONFIG_URL_OBJ             "/ndc-nc/serverlist.config"
#define JSYX_CONFIG_URL                 


#if defined(_MSC_VER)
#define NETX_HTTP_SUPPORT       1
    static long netx__socket_lib_inited = 0;
#else
#define NETX_HTTP_SUPPORT       0
#endif

#if defined(_WIN32) /* windows */
#   define netx__socket_lib_try_load() \
    do\
    {\
        if(0 == netx__socket_lib_inited) \
        {\
            WSADATA data;\
            if(0 != WSAStartup(MAKEWORD(2,0),&data))\
            {/* load winsock dll failed */\
                print_err("err: " func_format_s " failed when WSAStartup() with syserrno[%d]. %s:%d\r\n",\
                          func_format(), netx_errno, __FILE__, __LINE__);\
                return -2;\
            }\
            netx__socket_lib_inited = 1;\
        }\
    }while(0)
#   define netx__socket_lib_try_unload() \
    do{\
        if((0 == netx__polls.counts) && netx__socket_lib_inited)\
        {\
            WSACleanup();\
            netx__socket_lib_inited = 0; \
        }\
    }while(0)
#else
#   define netx__socket_lib_try_load()      do{}while(0)
#   define netx__socket_lib_try_unload()    do{}while(0)
#endif

#if defined(NETX_HTTP_SUPPORT) && NETX_HTTP_SUPPORT
int netx_http_req(struct len_str *url, int port, int get, struct len_str *req, char *obuf, int obuf_siz)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "netx_http_req(url[%s], get:%d, req:[%20s] obuf[%p], obuf_siz[%d]) (%d)"
#define func_format()   url?url->data:NULL, get, req?req->data:NULL, obuf, obuf_siz, GetLastError()
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    int ret = 0;
    BOOL bResult;
    DWORD dwSize, dwDownloaded;
    char head_buf[1024];
    char *s, url_buf[128], *hostname, *suffix;
    wchar_t wbuf[128];

    if (!url || !strstr (url->data, "http://"))
    {
        print_err("err: " func_format_s " failed with invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        ret = -1; goto exit_label;
    }

    strcpy(s = url_buf, url->data);
    hostname = url_buf + sizeof("http://") - 1;
    if ((suffix = strchr(hostname, '/')))
    {
        suffix[0] = 0;
        suffix++;
    }

    hSession = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_NO_PROXY, NULL, NULL, 0);
    if (NULL == hSession)
    {
        printf("cjsyx_server_list_get() failed with WinHttpOpen(). %s:%d\n", __FILE__, __LINE__);
        ret = -1; goto exit_label;
    }

    mbstowcs(wbuf, hostname, strlen(hostname) + 1);
    hConnect = WinHttpConnect(hSession, wbuf, port?port:INTERNET_DEFAULT_HTTP_PORT, 0);
    if (NULL == hConnect)
    {
        print_err("err: " func_format_s " failed with WinHttpConnect(). %s:%d\r\n", func_format(), __FILE__, __LINE__);
        ret = -1; goto exit_label;
    }

    mbstowcs(wbuf, suffix, strlen(suffix) + 1);
    hRequest = WinHttpOpenRequest(hConnect, get?L"Get":L"Post", wbuf, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (NULL == hRequest)
    {
        print_err("err: " func_format_s " failed with WinHttpOpenRequest(). %s:%d\r\n", func_format(), __FILE__, __LINE__);
        ret = -1; goto exit_label;
    }

    bResult = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, 
        get?WINHTTP_NO_REQUEST_DATA:req->data, req?req->len:0, req?req->len:0, 0);
    if (FALSE == bResult)
    {
        print_err("err: " func_format_s " failed with WinHttpSendRequest(). %s:%d\r\n", 
            func_format(), __FILE__, __LINE__);
        ret = -1; goto exit_label;
    }

    bResult = WinHttpReceiveResponse(hRequest, NULL);
    if (FALSE == bResult)
    {
        print_err("err: " func_format_s " failed with WinHttpReceiveResponse(). %s:%d\r\n", func_format(), __FILE__, __LINE__);
        ret = -1; goto exit_label;
    }

    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS,
        WINHTTP_HEADER_NAME_BY_INDEX, head_buf, &dwSize, WINHTTP_NO_HEADER_INDEX);

    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        print_err("err: " func_format_s " failed with WinHttpQueryHeaders() buf too small. %s:%d\r\n", 
            func_format(), __FILE__, __LINE__);
        ret = -1; goto exit_label;
    }

    do
    {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
        {
            print_err("err: " func_format_s " failed with WinHttpQueryDataAvailable() buf too small. %s:%d\r\n", 
                func_format(), __FILE__, __LINE__);
            ret = -1; goto exit_label;
        }

        if (!dwSize) break;

        if (!WinHttpReadData(hRequest, obuf, obuf_siz - 1, &dwDownloaded))
        {
            print_err("err: " func_format_s " failed with WinHttpReadData() . %s:%d\r\n", 
                func_format(), __FILE__, __LINE__);
            ret = -1; goto exit_label;
        }
        if (!dwDownloaded)
            break;
        ret = dwDownloaded;
    } while (dwSize > 0);

    if (ret > 0)
    { 
        obuf[ret] = 0;
        //printf("http got[%s]\n", obuf);
    }

exit_label:

    if (hRequest)   WinHttpCloseHandle(hRequest);
    if (hConnect)   WinHttpCloseHandle(hConnect);
    if (hSession)   WinHttpCloseHandle(hSession);

    return ret;
}
#else
int netx_http_req(struct len_str *url, int port, int get, struct len_str *req, char *obuf, int obuf_siz)
{
    return 0;
}
#endif

int netx_init()
{
#undef func_format_s
#undef func_format
#define func_format_s   "netx_init(%s)"
#define func_format()   ""

#if defined(_MSC_VER)
	WSADATA data;
	if(0 != WSAStartup(MAKEWORD(2,0),&data))
	{/* load winsock dll failed */
		print_err("err: " func_format_s " failed when WSAStartup() with syserrno[%d]. %s:%d\r\n",
				  func_format(), netx_errno, __FILE__, __LINE__);
		return -2;
	}
#endif

	return 0;
}

int netx_uninit()
{
#if defined(_MSC_VER)
	WSACleanup();
#endif

	return 0;
}

int netx_open(long type/* same as socket()'s type, SOCK_STREAM... */, struct in_addr *ip, int port, unsigned int flag)
{
#undef func_format_s
#undef func_format
#define func_format_s   "netx_open(type[%d], ip[%p{%s}], port[%d], flag[0x%x])"
#define func_format()   type, ip, ip?inet_ntoa(*ip):"", port, flag

    long                fd;
    struct sockaddr_in  addr;
#if defined(WIN32)
    u_long          block_flag = 1;
#endif
    print_debug("debug: " func_format_s ". %s:%d\r\n", func_format(), __FILE__, __LINE__);

    netx__socket_lib_try_load();

    /* create socket */
    if(0 > (fd = socket(AF_INET, type, 0)))
    {/* create socket failed */
        print_err("err: " func_format_s " failed when socket() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        return -1;
    }

    /* bind */
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs((unsigned short)port);
    addr.sin_addr.s_addr = ip?ip->s_addr:0;
    if(port && (flag & netx_open_flag_reuse_addr))
    {
        unsigned long reuse_addr = 1;
        if(0 > setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_addr, sizeof(reuse_addr)))
        {/* set socket reuse opt failed */
            print_err("err: " func_format_s " failed when setsockopt(SO_REUSEADDR) with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
    }
    /* set non-block */
#if defined(_WIN32)
    if(ioctlsocket(fd, FIONBIO, &block_flag))
#else /* linux */
    if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK))
#endif
    {/* set sock to non-block mode failed. */
        print_err("err: " func_format_s " failed when set with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }

    if(port || addr.sin_addr.s_addr)
    {
        if(0 > bind(fd, (const struct sockaddr*)&addr, sizeof(addr)))
        {/* bind socket failed */
            print_err("err: " func_format_s " failed when bind() with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
    }

    if((SOCK_STREAM == type) && (0 > listen(fd, 32)))
    {/* listen failed */
        print_err("err: " func_format_s " failed when listen() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }
    return fd;
}

int netx_connect_by_addr(struct sockaddr_in *addr, struct sockaddr_in *local_addr/* local-addr, if NULL ignore */)
{
#undef func_format_s
#undef func_format
#define func_format_s   "netx_connect_by_addr(addr[%p{%s:%d}], local_addr[%p{%s:%ld})"
#define func_format()   addr, addr?strcpy(s1, inet_ntoa(addr->sin_addr)):"", (int)(addr?ntohs(addr->sin_port):0), \
                        local_addr, local_addr?strcpy(s2, inet_ntoa(local_addr->sin_addr)):"",  (int)(local_addr?ntohs(local_addr->sin_port):0)

#if defined(WIN32)
    u_long            block_flag = 1;
#endif
    char s1[32], s2[32];
    int fd;

    netx__socket_lib_try_load();

    fd = socket(AF_INET, SOCK_STREAM, 0);
    print_debug("debug: " func_format_s " ret-fd[%ld]. %s:%d\r\n", func_format(), fd, __FILE__, __LINE__);
    /* create socket */
    if(0 > fd)
    {/* create socket failed */
        print_err("err: " func_format_s " failed when socket() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        return -1;
    }

    /* set non-block */
#if defined(_WIN32)
    if(ioctlsocket(fd, FIONBIO, &block_flag))
#else /* linux */
    if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK))
#endif
    {/* set sock to non-block mode failed. */
        print_err("err: " func_format_s " failed when fcntl(NONBLOCK) with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }

    if(local_addr)
    {/* need setting local port */
        unsigned reuse_addr = 1;
        if(0 > setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_addr, sizeof(reuse_addr)))
        {/* set socket reuse opt failed */
            print_err("err: " func_format_s " failed when setsockopt(SO_REUSEADDR) with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
        else if(0 > bind(fd, (const struct sockaddr*)local_addr, sizeof(*local_addr)))
        {/* bind socket failed */
            print_err("err: " func_format_s " failed when bind() with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
    }

    /* connect */
    if((0 > connect(fd, (struct sockaddr*)addr, sizeof(*addr)))
#if defined(_WIN32)
       && (WSAEWOULDBLOCK != WSAGetLastError())
#else
       && (EINPROGRESS != errno)
#endif
      )
    {/* connect failed */
        print_info("info: " func_format_s " failed when connect() with syserrno[%d]. %s:%d\r\n",
                   func_format(), netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }
#if defined(__APPLE__)
    do
    {
        int set = 1;
        setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
    }while(0);
#endif
    return fd;
}

int netx_connect(char *host, unsigned long port, struct in_addr *local_ip, int local_port/* if 0, ignore local info */, struct sockaddr_in *addr/* [out] if NULL ignore*/)
{
#undef func_format_s
#undef func_format
#define func_format_s   "netx_connect(host[%s], port[%d], local_ip[%p{%s}], local_port[%d], addr[%p])"
#define func_format()   host, port, local_ip, local_ip?inet_ntoa(*local_ip):"", local_port, addr

    struct sockaddr_in  addr_local;
    struct sockaddr_in  addr_tmp;
    struct hostent      *hostet = NULL;
    struct in_addr      ip = {0};

    print_debug("debug: " func_format_s ". %s:%d\r\n", func_format(), __FILE__, __LINE__);
#ifdef WINCE
    /* gethostbyname in wince doest not support IP address strings  */
    ip.s_addr = inet_addr( host );
    if((INADDR_NONE == ip.s_addr) || (INADDR_ANY == ip.s_addr))
    {
        print_err("err: "func_format_s" failed when inet_addr() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        return -1;
    }
#else
#   if defined(WIN32)
    ip.s_addr = inet_addr(host);
    if(INADDR_NONE == ip.s_addr)
#   else
    if(0 == inet_aton(host, &ip))
#   endif
    {/* not a ip host */
        hostet = (struct hostent *)gethostbyname( host );
        if( hostet == NULL )
        {
            print_err("err: " func_format_s " failed when gethostbyname() with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            return -1;
        }
        ip = *((struct in_addr *)*(hostet->h_addr_list));
    }
#endif
    addr = addr?addr:&addr_tmp;
    addr->sin_family = AF_INET;
    addr->sin_port = htons((unsigned short)port);
    addr->sin_addr = ip;
    addr_local.sin_family = AF_INET;
    addr_local.sin_addr.s_addr = local_ip?local_ip->s_addr:0;
    addr_local.sin_port = ntohs((unsigned short)local_port);
    return netx_connect_by_addr(addr, local_port?&addr_local:NULL);
}

int netx_set_keepalive(int fd, int idle_ms, int interval_ms, int times)
{
#undef func_format_s
#undef func_format
#define func_format_s   "netx_set_keepalive(fd[%ld], idle_ms[%ld], interval_ms[%ld], times[%ld])"
#define func_format()   fd, idle_ms, interval_ms, times

#if defined(WIN32)
    int             keeplive_flag = 1;
    struct
    {
         unsigned long onoff;
         unsigned long keepalivetime;  /* first keep-live packet send after ms */
         unsigned long keepaliveinterval; /* after first keep-live packet send, how many ms passed then second packet will be send */
    }keeplive_in = {0}, keeplive_out = {0};
    unsigned long   bytes_ret, in_len = sizeof(keeplive_in), out_len = sizeof(keeplive_out);
    long            err1 = 0, err2 = 0;

    keeplive_in.onoff = 1;
    keeplive_in.keepaliveinterval = interval_ms?interval_ms:5000;
    keeplive_in.keepalivetime = idle_ms?idle_ms:7200000;/* 2hour */

#   if !defined(SIO_KEEPALIVE_VALS)
#       define   SIO_KEEPALIVE_VALS         _WSAIOW(IOC_VENDOR,4)
#   endif


    if((SOCKET_ERROR == (err1 = setsockopt((SOCKET)fd, (int)SOL_SOCKET, (int)SO_KEEPALIVE,
                                            (char*)&keeplive_flag, sizeof(keeplive_flag))))
        ||((interval_ms || idle_ms) && (SOCKET_ERROR == (err2 = WSAIoctl((SOCKET)fd, SIO_KEEPALIVE_VALS,
                                            (void*)&keeplive_in, in_len,
                                            (void*)&keeplive_out, out_len, &bytes_ret, NULL, NULL)))))
    {
        print_err("failed when %s(%s) with err[%ld].",
                  err1?"setsockopt":"WSAIoctl", err1?"SO_KEEPLIVE":"SIO_KEEPALIVE_VALS",
                  (long)netx_errno);
        return -1;
    }
#elif __APPLE__
    return -1;
#else
    int keeplive_flag = 1;
    int keeplive_idle = (idle_ms?idle_ms:7200000) / 1000;
    int keeplive_interval = (interval_ms?interval_ms:5000) / 1000;
    int keeplive_times = times?times:5;
    long err1 = 0, err2 = 0, err3 = 0, err4 = 0, err5 = 0;
    if((0 > (err1 = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keeplive_flag, sizeof(keeplive_flag))))
        ||(keeplive_idle && (0 > (err2 = setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE,(void *)&keeplive_idle, sizeof(keeplive_idle)))))
        ||(keeplive_interval && (0 > (err3 = setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL,(void *)&keeplive_interval, sizeof(keeplive_interval)))))
        ||(keeplive_times && (0 > (err4 = setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keeplive_times, sizeof(keeplive_times)))))
        )
    {
        print_err("err: " func_format_s " failed when setsockopt(%s) with err[%ld]. %s:%d\r\n", func_format(),
                  (err1?"SO_KEEPALIVE":(err2?"TCP_KEEPIDLE":(err3?"TCP_KEEPINTVL":"TCP_KEEPCNT"))),
                  (long)netx_errno, __FILE__, __LINE__);
        return -1;
    }
#endif

    return 0;
}

int netx_set_nonblock(int fd, int none)
{
#if defined(_MSC_VER)
    return ioctlsocket(fd, FIONBIO, (u_long *)&none);
#else
    if (none) 
        return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    else
        return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);
#endif
}


