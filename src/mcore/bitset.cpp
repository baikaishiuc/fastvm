
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitset.h"
#include "mtime_ex.h"
#include "print_util.h"

static int      bitset__expand(struct bitset *bs, int len)
{
    int len4, new_len = len;
    unsigned int *p;

    if (len <= bs->len4 * 32) {
        bs->len = len;
        bs->siz4 = (bs->len + 31) / 32;
        return 0;
    }

    if (bs->len * 2 > len)
        new_len = bs->len * 2;

    len4 = (new_len + 31) / 32;

    p = (unsigned int *)calloc(1, sizeof (int) * len4);
    if (NULL == p)
        return -1;

    memcpy(p, bs->data, sizeof (bs->data[0]) * bs->len4);
    free(bs->data);
    bs->data = p;
    bs->len4 = len4;
    bs->len = len;
    bs->siz4 = (bs->len + 31) / 32;

    return 0;
}

struct bitset*  bitset_new(int size)
{
    struct bitset *bs = (struct bitset *)calloc(1, sizeof (bs[0]));
    if (!bs)
        return NULL;

    return bitset_init(bs, size);
}

void            bitset_delete(struct bitset *bs)
{
    if (bs->data)   free(bs->data);

    free(bs);
}

struct bitset*  bitset_init(struct bitset *bs, int size)
{
    bs->len4 = (size + 31) / 32;
    bs->len = size;
    bs->siz4 = (bs->len + 31) / 32;
    bs->data = (unsigned int *)calloc(1, bs->len4 * sizeof (int));
    if (NULL == bs->data) {
        free(bs);
        return NULL;
    }

    return bs;
}

void            bitset_uninit(struct bitset *bs)
{
    bitset_reset(bs);
}

int             bitset_set(struct bitset *bset, int bit, int val)
{
    int i;
    if (bit < 0)
        return 0;

    if (bit >= bset->len) {
        bitset__expand(bset, bit + 1);
    }

    val = !!val;
    i = bit / 32;

    if (val) 
        bset->data[i] |= 1 << (bit % 32);
    else
        bset->data[i] &= ~(1 << (bit % 32));

    return 0;
}

int             bitset_get(struct bitset *bset, int bit)
{
	if (!bset || (bit >= bset->len)) {
        printf("bitset_get() failed with overflow[%d:%d]\n", bit, bset->len);
        return -1;
	}

    return !!(bset->data[bit / 32] & (1 << (bit % 32)));
}

void            bitset_clear(struct bitset *bs)
{
    int i;

    for (i = 0; i < bs->len4; i++)
        bs->data[i] = 0;
}

void            bitset_reset(struct bitset *bs)
{
    if (bs->data)
        free(bs->data);

    memset(bs, 0, sizeof (bs[0]));
}

struct bitset*  bitset_or(struct bitset *dst, struct bitset *src)
{
    int i;

    if (dst->len4 != src->len4) {
        bitset__expand((dst->len > src->len) ? src:dst, (dst->len > src->len)?dst->len:src->len);
    }

    for (i = 0; i < dst->siz4; i++) {
        dst->data[i] |= src->data[i];
    }

    return dst;
}

struct bitset*  bitset_clone(struct bitset *dst, struct bitset *src)
{
    int i;

    if (dst->len4 != src->len4) {
        bitset__expand((dst->len > src->len) ? src:dst, (dst->len > src->len)?dst->len:src->len);
    }

    for (i = 0; i < dst->siz4; i++) {
        dst->data[i] = src->data[i];
    }

    return dst;
}

struct bitset*  bitset_and(struct bitset *dst, struct bitset *src)
{
    int i;

    if (dst->len != src->len) {
        bitset__expand((dst->len > src->len) ? src:dst, (dst->len > src->len)?dst->len:src->len);
    }

    for (i = 0; i < dst->siz4; i++) {
        dst->data[i] &= src->data[i];
    }

    return dst;
}

struct bitset*  bitset_not(struct bitset *dst)
{
    int i;

    for (i = 0; i < dst->siz4; i++) {
        dst->data[i] = ~dst->data[i];
    }

    return dst;
}

struct bitset*  bitset_sub(struct bitset *dst, struct bitset *src)
{
    int i, a, b;

    if (dst->len4 != src->len4) {
        bitset__expand((dst->len > src->len) ? src:dst, (dst->len > src->len)?dst->len:src->len);
    }

    for (i = 0; i < dst->siz4; i++) {
        a = dst->data[i];
        b = src->data[i];
        dst->data[i] = ~(a & b) & a;
    }

    return dst;
}

int             bitset_is_equal(struct bitset *dst, struct bitset *src)
{
    int i;

    if (dst->len4 != src->len4) {
        bitset__expand((dst->len > src->len) ? src:dst, (dst->len > src->len)?dst->len:src->len);
    }

    for (i = 0; i < dst->siz4; i++) {
        if (dst->data[i] != src->data[i])
            return 0;
    }

    return 1;
}

int             bitset_is_empty(struct bitset *bs)
{
    int i;

    for (i = 0; i < bs->len4; i++) {
        if (bs->data[i])
            return 0;
    }

    return 1;
}

int         bitset_next_bit_pos(struct bitset *bs, int pos)
{
    int i, j, k;
    if (pos >= bs->len || pos < 0)
        return -1;

    while (pos < bs->len) {
        i = pos / 32;
        if (!bs->data[i])
            pos = (i + 1) * 32;
        else {
            for (j = pos % 32; j < 32; j++) {
                if (bs->data[i] & (1 << j))
                    return ((k = (i * 32 + j)) >= bs->len) ? -1:k;
            }

            pos = (i + 1) * 32;
        }
    }

    return -1;
}

static inline int bit_count(int v)
{
    int c;

    c = (v & 0x55555555) + ((v >> 1) & 0x55555555);
    c = (c & 0x33333333) + ((c >> 2) & 0x33333333);
    c = (c & 0x0F0F0F0F) + ((c >> 4) & 0x0F0F0F0F);
    c = (c & 0x00FF00FF) + ((c >> 8) & 0x00FF00FF);
    c = (c & 0x0000FFFF) + ((c >> 16) & 0x0000FFFF);

    return c;
}

int             bitset_count(struct bitset *bs)
{
    int c = 0, i;
    for (i = 0; i < bs->len4; i++)
        c += bit_count(bs->data[i]);

    return c;
}

void            bitset_expand(struct bitset *bs, int len)
{
    bitset__expand(bs, len);
}

void            bitset_dump(struct bitset *bs)
{
    int i;

    printf("{");
    bitset_foreach(bs, i) {
        printf("%d ", i);
    }
    printf("}");
}

