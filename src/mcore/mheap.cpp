

#include "mheap.h"
#include "print_util.h"
#include <stdlib.h>
#include <search.h>

#define LCHILD(x)	(2 * x + 1)
#define RCHILD(x)	(2 * x + 2)
#define PARENT(x)	((i-1)/2)

static int compare_int(void *ref, const void *lhs, const void *rhs) {
	return (long)lhs - (long)rhs;
}

struct mheap*	mheap_new(int size, void *ref, int(*compare_func)(void *ref, const void *lhs, const void *rhs))
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "mheap_new(size:%d, ref:%d, compare_func:%p) "
#define func_format()	size, ref, compare_func
	struct mheap *h;

	if (!size)
		return NULL;

	h = (struct mheap *)calloc(1, sizeof (h[0]) + size * sizeof (h->ptab[0]));
	if (!h)
		return NULL;

	h->ptab = (void **)((char *)h + sizeof(h[0]));
	h->len = 0;
	h->size = size;
	h->ref = ref;
	h->compare_func = compare_func?compare_func:compare_int;

	return h;
}

void			mheap_delete(struct mheap *h)
{
	if (h)		free(h);
}

void*			mheap_min(struct mheap *h)
{
	return h->len?h->ptab[0]:NULL;
}

#define swap(x,y) do { \
	void *tmp = x; \
	x = y; \
	y = tmp; \
	} while (0)

void*			mheap_delmin(struct mheap *h)
{
	void *first;
	int left, right, parent, t;

	if (!h->len)
		return NULL;

	if (h->len == 1) {
		h->len--;
		return h->ptab[0];
	}

	first = h->ptab[0];
	h->ptab[0] = h->ptab[h->len - 1];
	h->len--;

	parent = t = 0;
	while (1) {
		left = LCHILD(parent);
		right = RCHILD(parent);

		t = parent;

		if ((left < h->len) && h->compare_func(h->ref, h->ptab[left], h->ptab[t]) < 0) {
			t = left;
		}

		if ((right < h->len) && h->compare_func(h->ref, h->ptab[right], h->ptab[t]) <= 0) {
			t = right;
		}

		if (t != parent) {
			swap(h->ptab[t], h->ptab[parent]);
			parent = t;
		}
		else
			break;
	}

	return first;
}

int				mheap_insert(struct mheap *h, void *elm)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "mheap_insert(h:%p, elm:%p) "
#define func_format()	h, elm
	int parent, i;

	if ((h->len + 1) > h->size) {
		print_err("failed with overflow.");
		return -1;
	}

	i = h->len;
	h->ptab[h->len] = elm;
	h->len++;

	while (i && (h->compare_func(h->ref, h->ptab[parent = PARENT(i)], h->ptab[i]) > 0)) {
	//while (i && (h->ptab[parent = PARENT(i)] > h->ptab[i])) {
		swap(h->ptab[parent], h->ptab[i]);
		i = parent;
	}

	return 0;
}

int				mheap_isempty(struct mheap *h)
{
	return h->len == 0;
}

void				mheap_deckey(struct mheap *h, void *key)
{
	int i, parent;

	if (h->len <= 1) return;

	for (i = 0; i < h->len; i++) {
		if (h->ptab[i] == key)
			break;
	}

	if (i == h->len)	return;

	while (i && (h->compare_func(h->ref, h->ptab[parent = PARENT(i)], h->ptab[i]) > 0)) {
	//while (i && (h->ptab[parent = PARENT(i)] > h->ptab[i])) {
		swap(h->ptab[parent], h->ptab[i]);
		i = parent;
	}
}

int				mheap_sort(struct mheap *h)
{
	if (h->len <= 1)
		return 0;

	//qsort_s(h->ptab, h->len, sizeof (h->ptab[0]), h->compare_func, h->ref);

	return 0;
}
