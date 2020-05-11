
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print_util.h"
#include "string_ex.h"

int len_str_dup(struct len_str *dst, const char *src, int len)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "len_str_dup(dst:%p, src:%15s, len:%d) "
#define func_format()   dst, src, len

    if (!dst || !src)
    {
        print_err("[%s] err:" func_format_s " failed with invalid param. %s:%d\n",
            mtime2s(NULL), func_format(), __FILE__, __LINE__);
        return -1;
    }

    if (!len)
        len = strlen(src);

    dst->data = (char *)malloc(len + 1);
    if (NULL == dst->data)
    {
        print_err("[%s] err:" func_format_s " failed with malloc. %s:%d\n",
            mtime2s(NULL), func_format(), __FILE__, __LINE__);
        return -1;
    }
    dst->len = len;
    memcpy(dst->data, src, len);
    dst->data[len] = 0;

    return 0;
}

int len_str_clear(struct len_str *src)
{
    if (src)
    {
        free(src->data);
        src->data = NULL;
        src->len = 0;
    }
    return 0;
}

