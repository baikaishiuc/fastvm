

#include <stdio.h>
#include <string.h>
#include "mutil.h"

unsigned char *hex2str(unsigned char *hex, int len, unsigned char *obuf)
{
    int i;
    const char *hexstr = "0123456789ABCDEF";

    for (i = 0; i < len; i++) {
        obuf[i * 2] = hexstr [hex[i] >> 4];
        obuf[i * 2 + 1] = hexstr [hex[i] & 0x0f];
    }

    obuf[i*2] = 0;

    return obuf;
}

#if defined(_MSC_VER)
//#define PATH_SEP            '\\'
#define PATH_SEP            '/'
#else
#define PATH_SEP            '/'
#endif

char *basename(char *fullpath, char *base)
{
    // 找最后一个路径分割符
    char *s = strrchr (fullpath, PATH_SEP);
    int len;

    base[0] = 0;

    // 假如找不到，直接认为当前的路径就是全文件名
    if (!s)
        s = fullpath;
    else 
        s++;
    
    // 假如路径分割符后为空，认为没有basename
    if (!s[0])
        return base;

    char *dot = strchr (s, '.');
    if (dot) {
        len = dot - s;
    }
    else {
        len = strlen (s);
    }

    strcpy(base, s);
    base[len] = 0;

    return base;
}
