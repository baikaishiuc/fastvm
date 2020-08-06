
#ifndef __PCODE_H__
#define __PCODE_H__

/* IR */

#include "mcore/mcore.h"

/* copy from https://ghidra.re/courses/languages/html/pcoderef.html */
enum pcode_op_type {
    pt_null,
    pt_copy,
    pt_load,
    pt_store,
    pt_branch,
    pt_cbranch,     // condition branch
    pt_call,        // call addr
    pt_callnd,      // call [addr]
    pt_userdefined,
    pt_return,
    pt_piece,       // This is a concatenation operation
    pt_subpiece,    // 
    pt_int_equal,
    pt_int_notequal,
    pt_int_less,
    pt_int_sless,
    pt_int_lessequal,
    pt_int_slessequal,
    pt_int_zext,    // zero extend
    pt_int_sext,    // 
    pt_int_add,
    pt_int_sub,
    pt_int_carray,
    pt_int_scarry,
    pt_int_sborrow,
    pt_int_2comp,       // This is the twos complement or arithmetic negation operation.    output = -input0
    pt_int_negate,      // output = ~input0
    pt_int_xor,
    pt_int_and,
    pt_int_or,
    pt_int_leftshfit,
    pt_int_rightshift,
    pt_int_srightshift,
    pt_int_mul,
    pt_int_div,
    pt_int_rem,         // output = input0 % input1
    pt_int_sdiv,        // output = input0 s/ nput1
    pt_int_srem,        // output = input0 s% input1
    pt_bool_negate,     // output = !input0
    pt_bool_xor,        // output = input0 ^^ input1
    pt_bool_and,        //
    pt_bool_or,
    pt_float_equal,
    pt_float_notequal,
    pt_float_less,
    pt_float_lessequal,
    pt_float_add,
    pt_float_sub,
    pt_float_mult,
    pt_float_div,
    pt_float_neg,
    pt_float_abs,
    pt_float_sqrt,
    pt_float_ceil,
    pt_float_floor,
    pt_float_round,
    pt_float_nan,
    pt_int2float,
    pt_float2float,     // output = float2float(input0); input0 difference size of output
    pt_trunc,           // output = trunc(input0);

    /* object-oriented instruction */
    pt_cpoolref,
    pt_new,
    pt_num_of_type,
};

typedef int         pt_long;
typedef struct address {
    int64_t     base;
    int         offset;
} address_t;

/* varnode.hh from Ghidra */
typedef struct varnode {
    struct {
        unsigned constant : 1;
    } flags;
    int size;
    int create_index;

    union {
        int8_t      i8;
        int16_t     i16;
        int32_t     i32;
        int64_t     i64;
    };

    struct pcode_op     *def;       // the defining operation of this varnode
} varnode_t;

typedef struct pcode_op {
    int id;
    /* operation size */
    int size;
    enum pcode_op_type type;
    varnode_t *output;
    struct dynarray inrefs;
} pcode_op;

/* pcode block, define a function */
struct pcode_blk {
    char *name;

    struct dynarray allpcode;
    struct dynarray allcfg;
    struct dynarray allvn;     // all varnode

    int pcode_id;
    int pcfg_id;
};

/* pcode cfg */
struct pcode_cfg {
    int id;
};

/* 当导入一个文件时，生成一个pcode_ctx 结构，包含关系如下
   pcode_ctx -> pcode_blk -> pcode_cfg -> pcode
   */
struct pcode_ctx {
    const char *filename;

    struct dynarray allblks;
};

int                 pctx_init(VMState *s);
void                pctx_uninit(VMState *s);

struct pcode_blk*   pblk_find(struct pcode_ctx *pctx, char *name);
struct pcode_blk*   pblk_new(struct pcode_ctx *pctx, char *name);

pcode_op*           pcode_new_copy(struct pcode_blk *blk, int size, struct varnode *dst, struct varnode *src);
pcode_op*           pcode_new_load(struct pcode_blk *blk, int size, int64_t dst, int64_t addr, int64_t offset);

struct varnode*     varnode_new(int size, int val);
void                varnode_delete(struct varnode *v);

void                pcode_set_output(pcode_op *pcode, struct varnode *vn);
void                pcode_set_input(pcode_op *pcode, struct varnode *vn, int index);
void                pcode_clear_input(pcode_op *pcode);


#endif