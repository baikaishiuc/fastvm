

#ifndef __mdir_h__
#define __mdir_h__

#ifdef __cplusplus
extern "C" {
#endif

struct mdir*    mdir_open(const char *dir);
void            mdir_close(struct mdir *d);
char*           mdir_next(struct mdir *d);

#ifdef __cplusplus
}
#endif

#endif
