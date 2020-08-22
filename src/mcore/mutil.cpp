

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

char *basename(const char *name)
{
    char *p = strchr((char *)name, 0);
    while (p > name && !IS_DIRSEP(p[-1]))
        --p;
    return p;
}
