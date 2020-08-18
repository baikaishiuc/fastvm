

#include <stdlib.h>
#include "dynarray.h"

int dynarray_add(struct dynarray *darray, void *data)
{
    if (!darray)
        return -1;

    int nb, nb_alloc;
    void **pp;

    nb = darray->len;
    pp = darray->ptab;
    /* every power of two we double array size */
    if ((nb & (nb - 1)) == 0) {
        if (!nb)
            nb_alloc = 1;
        else
            nb_alloc = nb * 2;
        pp = (void **)realloc(pp, nb_alloc * sizeof(void *));
        darray->ptab = pp;
        darray->size = nb_alloc;
    }
    darray->ptab[nb++] = data;
    darray->len = nb;

    return 0;
}

int dynarray_reset(struct dynarray *darray)
{
    if (!darray)
        return 0;
    if (darray->ptab)
        free(darray->ptab);
    darray->len = 0;
    darray->size = 0;
    darray->ptab = NULL;
    return 0;
}

int dynarray_reverse(struct dynarray *darray)
{
    int i;
    void *t;

    if (!darray || !darray->len)
        return -1;

    for (i = 0; i < (darray->len / 2); i++) {
        t = darray->ptab[i];
        darray->ptab[i] = darray->ptab[darray->len - i -1];
        darray->ptab[darray->len - i - 1] = t;
    }

    return 0;
}

int dynarray_copy(struct dynarray *dst, struct dynarray *src)
{
	int i;
	dynarray_reset(dst);

	for (i = 0; i < src->len; i++)
		dynarray_add(dst, src->ptab[i]);

	return 0;
}

int dynarray_cmp(struct dynarray *lhs, struct dynarray *rhs)
{
	int i;
	if (lhs->len != rhs->len)
		return -1;

	for (i = 0; i < lhs->len; i++) {
		if (lhs->ptab[i] != rhs->ptab[i])
			return -1;
	}

	return 0;
}

void* dynarray_find(struct dynarray *arr, void *a)
{
    int left = 0, right = arr->len - 1;
    int mid, ret;

    while (left <= right) {
        mid = (left + right) / 2;

        ret = arr->cmp(a, arr->ptab[mid], arr->ref);
        if (0 == ret)
            return arr->ptab[mid];

        if (ret < 0) {
            right = mid - 1;
        } else  {
            left = mid + 1;
        }
    }

    return NULL;
}

int dynarray_exist(struct dynarray *d, int k)
{
    int i;

    for (i = 0; i < d->len; i++) {
        if ((int)d->ptab[i] == k)
            return 1;
    }

    return 0;
}

struct dynarray*    dynarray_new(cmp_fn cmp)
{
    struct dynarray *d = (struct dynarray *)calloc(1, sizeof (d[0]));
    if (!d)
        return NULL;

    d->cmp = cmp;

    return d;
}

void                dynarray_delete(struct dynarray *d)
{
    free(d->ptab);
    free(d);
}

int int64_cmp(void *lhs, void *rhs, void *ref)
{
    return (int)(*(int64_t *)lhs - *(int64_t *)rhs);
}

int64_t *int64_new(int64_t a)
{
    int64_t *to = (int64_t *)malloc(sizeof(to[0]));

    to[0] = a;

    return to;
}
