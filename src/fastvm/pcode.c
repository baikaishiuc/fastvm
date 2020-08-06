#include "vm.h"
#include "pcode.h"

int                 pctx_init(VMState *s)
{
    return 0;
}

void                pctx_uninit(VMState *s)
{
}

struct pcode_blk*   pblk_find(struct pcode_ctx *pctx, char *name)
{
    int i;
    struct pcode_blk *pblk;

    for (i = 0; i < pctx->allblks.len; i++) {
        pblk = pctx->allblks.ptab[i];
        if (!strcmp(name, pblk->name))
            return pblk;
    }

    return NULL;
}

struct pcode_blk*   pblk_new(struct pcode_ctx *pctx, char *name)
{
    struct pcode_blk *pblk;

    if ((pblk = pblk_find(pctx, name))) return pblk;

    pblk = vm_mallocz(sizeof (pblk[0]));

    pblk->name = strdup(name);

    dynarray_add(&pctx->allblks, pblk);

    return pblk;
}

pcode_op*   pcode_new(struct pcode_blk *blk, enum pcode_type type)
{
    pcode_op *pcode_op = vm_mallocz(sizeof (pcode_op[0]));

    return pcode_op;
}

pcode_op*   pcode_new_copy(struct pcode_blk *blk, int size, struct varnode *dst, struct varnode *src)
{
    pcode_op *pcode = vm_mallocz(sizeof (pcode[0]));

    pcode->type = pt_copy;
    pcode->size = size;
    pcode_set_output(pcode, dst);
    pcode_set_input(pcode, src, 0);

    return pcode;
}

pcode_op*       pcode_new_load(struct pcode_blk *blk, int size, int64_t dst, int64_t addr, int64_t offset)
{
    return NULL;
}

void                pcode_set_output(pcode_op *pcode, struct varnode *vn)
{
}

void                pcode_set_input(pcode_op *pcode, struct varnode *vn, int index)
{
}

void                pcode_clear_input(pcode_op *pcode)
{
}
