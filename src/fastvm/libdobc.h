
#if defined(__cplusplus)
extern "C" {
#endif

#ifndef __libdobc_h__
#define __libdobc_h__

    struct VMState;

    typedef struct VMState VMState;

    int         dobc_parse_args(VMState *s, int argc, char **argv);
    int         dobc_run(VMState *s);
    VMState*    dobc_new(void);
    void        dobc_delete(VMState *s);

#define DOBC_OUTPUT_MEMORY          1
#define DOBC_OUTPUT_EXE             2
#define DOBC_OUTPUT_DLL             3
#define DOBC_OUTPUT_OBJ             4
#define DOBC_OUTPUT_PREPROCESS      5


#endif

#if defined(__cplusplus)
}
#endif