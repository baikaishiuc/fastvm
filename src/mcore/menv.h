
#ifndef __menv_h__
#define __menv_h__


#ifdef __cplusplus
extern "C" {
#endif

    int menvset(const char *key, const char *val);
    char *menvget(const char *key);

#ifdef __cplusplus
}
#endif

#endif

