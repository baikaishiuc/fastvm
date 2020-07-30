

#include "vm.h"

#define ERROR_WARN      0
#define ERROR_NOABORT   1
#define ERROR_ERROR     2

static struct VMState *vm_state;

static void strcat_vprintf(char *buf, int buf_size, const char *fmt, va_list ap)
{
    int len;
    len = strlen(buf);
    vsnprintf(buf + len, buf_size - len, fmt, ap);
}

static void strcat_printf(char *buf, int buf_size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    strcat_vprintf(buf, buf_size, fmt, ap);
    va_end(ap);
}

void error1(int mode, const char *fmt, va_list ap)
{
    char buf[2048];
    VMState *s1 = vm_state;

	buf[0] = 0;
    if (mode == ERROR_WARN) {
        strcat_printf(buf, sizeof (buf), "warnings: ");
    }
    else {
        strcat_printf(buf, sizeof (buf), "error: ");
    }

    strcat_vprintf(buf, sizeof (buf), fmt, ap);
    if (!s1 || !s1->error_func) {
        fflush(stdout);
        fprintf(stderr, "%s\n", buf);
        fflush(stderr);
    }
    else {
        s1->error_func(s1->error_opaque, buf);
    }

    if (s1) {
        if (mode != ERROR_ERROR)
            return;
    }

    exit(1);
}

void _vm_error_noabort(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    error1(ERROR_NOABORT, fmt, ap);
    va_end(ap);
}

void _vm_error_warning(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    error1(ERROR_WARN, fmt, ap);
    va_end(ap);
}

void _vm_error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    error1(ERROR_ERROR, fmt, ap);
    va_end(ap);
}

void _vm_warning(const char *fmt, ...)
{
}

void *vm_realloc(void *ptr, unsigned long size)
{
    void *ptr1;

    ptr1 = realloc(ptr, size);
    if (!ptr1 && size)
        _vm_error("memory full (realloc)");
    return ptr1;
}

void *vm_malloc(unsigned long size)
{
	void *ptr;
	ptr = malloc(size);
	if (!ptr && size)
		_vm_error("memory full (malloc)");
	return ptr;
}

void *vm_mallocz(unsigned long size)
{
	void *ptr;
	ptr = vm_malloc(size);
	memset(ptr, 0, size);
	return ptr;
}

void vm_free(void *ptr)
{
    free(ptr);
}

VMState *vm_new(void)
{
	VMState *s;

	s = (VMState *)calloc(1, sizeof (s[0]));
	if (!s)
		vm_error("calloc failure");

	return s;
}

void vm_delete(VMState *s)
{
	free(s);
}

typedef struct DOBCOption {
    const char *name;
    uint16_t    index;
    uint16_t    args;
} DOBCOption;

enum {
    DOBC_OPTION_HELP,
    DOBC_OPTION_v,
    DOBC_OPTION_ds,
    DOBC_OPTION_dh,
    DOBC_OPTION_dl,
    DOBC_OPTION_dS,

    DOBC_OPTION_d,
    DOBC_OPTION_df,
    DOBC_OPTION_t,
};

#define DOBC_OPTION_HAS_ARGS            0x01
#define DOBC_OPTION_NOSEP               0x02

static const DOBCOption dobc_options[] = {
    { "h",  DOBC_OPTION_HELP, 0 },
    { "v",  DOBC_OPTION_v, 0 },
    { "ds", DOBC_OPTION_ds, DOBC_OPTION_HAS_ARGS },
    { "dh", DOBC_OPTION_dh, 0 },
    { "dl", DOBC_OPTION_dl, 0 },
    { "dS", DOBC_OPTION_dS, 0 },
    { "d",  DOBC_OPTION_dS, 0 },
    { "df", DOBC_OPTION_df, DOBC_OPTION_HAS_ARGS },
    { "t",  DOBC_OPTION_df, DOBC_OPTION_HAS_ARGS },
    { NULL, 0, 0},
};

int dobc_parse_args(VMState *s, int argc, char **argv)
{
    const DOBCOption *popt;
    const char *r;
    int i;

    for (i = 1; i < argc; i++) {
        r = argv[i];
        for (popt = dobc_options; ; ++popt) {
            const char *opname = popt->name;
            const char *r1 = r + 1;

            if (!strcmp(opname, r1)) break;
        }

        if (popt->args & DOBC_OPTION_HAS_ARGS) {
            if (popt->index != DOBC_OPTION_df) {
                s->filename = strdup(argv[i+1]);
            }
        }

        switch (popt->index) {
        case DOBC_OPTION_HELP:          return OPT_HELP;
        case DOBC_OPTION_v:             return OPT_V;
        case DOBC_OPTION_dh:            return OPT_DUMP_ELF_HEADER;
        case DOBC_OPTION_dl:            return OPT_DUMP_ELF_PROG_HEADER;
        case DOBC_OPTION_ds:            return OPT_DUMP_ELF_DYNSYM;

        case DOBC_OPTION_df:
            s->funcaddr = strtol(argv[i+1], NULL, 16);
            s->filename = strdup(argv[i+2]);
            i += 2;
            return OPT_DECODE_FUNC;

        case DOBC_OPTION_d:
            return OPT_DECODE_ELF;

        default:
            break;
        }
    }

    return OPT_HELP;
}

VMState *dobc_new(void)
{
    VMState *s;

    s = calloc(1, sizeof (s[0]));
    if (!s)
        vm_error("dobc_new() failed when calloc()");

    dobcelf_new(s);

    return s;
}

void dobc_delete(VMState *s)
{
    dobcelf_delete(s);

    if (s->filename)
        free(s->filename);

    free(s);
}

int dobc_load_file(VMState *s1)
{
    int obj_type;
    s1->filedata = file_load(s1->filename, &s1->filelen);

    obj_type = dobc_object_type((ElfW(Ehdr) *)s1->filedata);
    if (obj_type != AFF_BINTYPE_DYN)
        vm_error("Sorry, this version only support dll file, %s\n", s1->filename);

#if 0
    s1->output_type = DOBC_OUTPUT_DLL;
    s1->output_format = DOBC_OUTPUT_FORMAT_ELF;
    dobc_load_dll(s1);
    dobc_output_file(s1, "test.so");
    exit(1);
#endif

    return 0;
}
