

#ifndef __mutil_h__
#define __mutil_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
# define IS_DIRSEP(c) (c == '/' || c == '\\')
# define IS_ABSPATH(p) (IS_DIRSEP(p[0]) || (p[0] && p[1] == ':' && IS_DIRSEP(p[2])))
# define PATHCMP stricmp
# define PATHSEP ";"
#else
# define IS_DIRSEP(c) (c == '/')
# define IS_ABSPATH(p) IS_DIRSEP(p[0])
# define PATHCMP strcmp
# define PATHSEP ":"
#endif

/* 使用这个函数时自己注意obuf的大小，是前面的len * 2 + 1，因为要加0 */
unsigned char *hex2str(unsigned char *hex, int len, unsigned char *obuf);

char *basename(const char *name);

#ifdef __cplusplus
}
#endif

#endif

