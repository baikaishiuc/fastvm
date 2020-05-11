
#include <string.h>
#include "merror.h"

#if defined(_MSC_VER)
#include <Windows.h>
char*	merr_msg(int code, char *buf, int size)
{
	static char sbuf[128];

	char *s = buf ? buf : sbuf;
	int len = size ? size : (sizeof(sbuf) / sizeof(sbuf[0]));

	len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		s, len, NULL);

	/* 删除尾部换行符 \r\n */
	len -= 2;
	s[len] = 0;

	return s;
}
#else
char*	merr_msg(int code, char *buf, int size)
{
    return  strerror(code);
}
#endif
