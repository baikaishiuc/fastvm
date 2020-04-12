

#include "mcore/mcore.h"
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
