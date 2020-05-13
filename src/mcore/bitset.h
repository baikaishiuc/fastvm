

#ifndef __bitset_h__
#define __bitset_h__

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */

struct bitset
{
    int len;
    int len4;
    unsigned int *data;
};

#define BITSET_INIT(a)      struct bitset a = {0}

    struct bitset;
    struct bitset*  bitset_new(int size);
    void            bitset_delete(struct bitset *bs);

    struct bitset*  bitset_init(struct bitset *bs, int size);
    void            bitset_uninit(struct bitset *bs);

    void            bitset_reset(struct bitset *bs);
    /* bitset 设置 
    @bit    位索引，从0开始
    @val    值， 0， 1
    */
    int             bitset_set(struct bitset *bset, int bit, int val);
    int             bitset_get(struct bitset *bset, int bit);
    void            bitset_clear(struct bitset *bs);
    struct bitset*  bitset_or(struct bitset *dst, struct bitset *src);
    struct bitset*  bitset_clone(struct bitset *dst, struct bitset *src);
    struct bitset*  bitset_and(struct bitset *dst, struct bitset *src);
    /* dst = dst - src*/
    struct bitset*  bitset_sub(struct bitset *dst, struct bitset *src);
    int             bitset_is_equal(struct bitset *lhs, struct bitset *src);
    int             bitset_is_empty(struct bitset *bs);
    int             bitset_next_bit_pos(struct bitset *bs, int pos);
    int             bitset_count(struct bitset *bs);

#define u4_bit_set(a, pos)      (a |= 1 << pos)

#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */

#endif