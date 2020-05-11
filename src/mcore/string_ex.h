
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __string_ex_h__
#define __string_ex_h__

#ifndef _len_str_defined_
#define _len_str_defined_
    typedef struct len_str
    {
        char *data;
        int len;
    } _len_str;
#endif

    int len_str_dup(struct len_str *dst, const char *src, int len);
    int len_str_clear(struct len_str *src);

#define len_str_cmp(_str1, _str2)   (int)(((_str1)->len != (_str2)->len)?((_str1)->len - (_str2)->len):memcmp((_str1)->data, (_str2)->data, (_str1)->len))
#define len_str_cmp_str(ls, str)    (((ls) && ((ls)->len) && str) ? strcmp ((ls)->data, str):-1)

#define count_of_array(_a)          (sizeof(_a) / sizeof (_a[0]))

#endif

#ifdef __cplusplus
}
#endif
