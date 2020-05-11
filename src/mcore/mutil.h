

#ifndef __mutil_h__
#define __mutil_h__

#ifdef __cplusplus
extern "C" {
#endif

/* 使用这个函数时自己注意obuf的大小，是前面的len * 2 + 1，因为要加0 */
unsigned char *hex2str(unsigned char *hex, int len, unsigned char *obuf);

char *basename(char *fullpath, char *base);

#ifdef __cplusplus
}
#endif

#endif

