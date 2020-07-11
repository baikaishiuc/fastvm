
#if defined(__cplusplus)
extern "C" {
#endif

#ifndef __libdobc_h__
#define __libdobc_h__


    int         dobc_parse_args(VMState *s, int argc, char **argv);
    int         dobc_run(VMState *s);
    VMState*    dobc_new(void);
    void        dobc_delete(VMState *s);


#endif

#if defined(__cplusplus)
}
#endif