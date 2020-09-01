

#ifndef __dynarray_h__
#define __dynarray_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    /* 动态数组，这个地方的length和size不是字节数，而是元素个数 */

    typedef int(*cmp_fn)(void *lhs, void *rhs, void *ref);
    typedef void(*free_fn)(void *data);
    struct dynarray
    {
        void    **ptab;
        int     len;
        int     size;
        void    *ref;
        cmp_fn  cmp;
        free_fn free1;
    };

    struct dynarray*    dynarray_new(cmp_fn cmp, free_fn);
    void                dynarray_delete(struct dynarray *d);

    int dynarray_add(struct dynarray *darray, void *data);
    int dynarray_reset(struct dynarray *darray);
    int dynarray_reverse(struct dynarray *darray);
	int dynarray_copy(struct dynarray *dst, struct dynarray *src);
	int dynarray_cmp(struct dynarray *lhs, struct dynarray *rhs);
    void* dynarray_find(struct dynarray *arr, void *a);
    int dynarray_exist(struct dynarray *d, int i);
    void dynarray_insert(struct dynarray *d, struct dynarray *s);

#define dynarray_is_empty(_a)		((_a)->len == 0)
#define dynarray_push               dynarray_add
#define dynarray_back(_a)           ((_a)->ptab[(_a)->len - 1])
#define dynarray_top(_a)            dynarray_back(_a)
#define dynarray_pop(_a)            ((_a)->ptab[--(_a)->len])

    int int64_cmp(void *lhs, void *rhs, void *ref);
    int64_t *int64_new(int64_t a);
    void    int64_delete(int64_t *);

#ifdef __cplusplus
}
#endif

#endif