

#ifndef __merror_h__
#define __merror_h__

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */

#define ERR_OK					0
#define ERR_SYSTEM				-1
#define ERR_INVALID_PARAM		-2
#define ERR_MEM_ALLOC			-3
#define ERR_MOD_EXIST			-4		// 注入远程dll时，dll已经存在

	char*	merr_msg(int code, char *buf, int size);

#ifdef _MSC_VER
#define merr_msg1()	merr_msg(GetLastError(), NULL, 0)
#else
#define merr_msg1()	strerror(errno)
#endif

#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */

#endif
