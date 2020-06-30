

#ifndef __dynarray_h__
#define __dynarray_h__

#ifdef __cplusplus
extern "C" {
#endif

    /* 动态数组，这个地方的length和size不是字节数，而是元素个数 */
    struct dynarray
    {
        void    **ptab;
        int     len;
        int     size;
        void    *ref;
        int(*compare_func)(void *lhs, void *rhs, void *ref);
    };

    int dynarray_add(struct dynarray *darray, void *data);
    int dynarray_reset(struct dynarray *darray);
    int dynarray_reverse(struct dynarray *darray);
	int dynarray_copy(struct dynarray *dst, struct dynarray *src);
	int dynarray_cmp(struct dynarray *lhs, struct dynarray *rhs);
    void* dynarray_find(struct dynarray *arr, void *a);
    int dynarray_exist(struct dynarray *d, int i);

#define dynarray_is_empty(_a)		((_a)->len == 0)

#ifdef __cplusplus
}
#endif

#endif