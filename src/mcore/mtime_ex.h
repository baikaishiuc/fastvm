
#ifdef __cplusplus
extern "C" {
#endif


#ifndef __mtime_ex_h__
#define __mtime_ex_h__

    char *mtime2s(char *buf);
    wchar_t *mtime2sW(wchar_t *buf);

    unsigned int mtime_tick();

#endif

#ifdef __cplusplus
}
#endif