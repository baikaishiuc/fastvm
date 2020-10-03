
#include "vm.h"
#include "slghpattern.h"

PatternBlock*       PatternBlock_newB(bool tf)
{
    PatternBlock* pb = vm_mallocz(sizeof(pb[0]));

    if (tf)
        pb->nonzerosize = 0;
    else
        pb->nonzerosize = -1;

    return pb;
}

PatternBlock*       PatternBlock_new3(int off, uintm mask, uintm val)
{ 
    PatternBlock *a = vm_mallocz(sizeof(a[0]));
    a->offset = off;
    dynarray_add(&a->maskvec, (void *)mask);
    dynarray_add(&a->valvec, (void *)val);
    a->nonzerosize = sizeof(uintm); // Assume all non-zero bytes before normalization
    PatternBlock_normalize(a);

    return a;
}

PatternBlock*       PatternBlock_clone(PatternBlock *pb)
{
    PatternBlock *res = vm_mallocz(sizeof(res[0]));

    res->offset = pb->offset;
    res->nonzerosize = pb->nonzerosize;
    res->maskvec = pb->maskvec;
    res->valvec = pb->valvec;

    return res;
}

void                PatternBlock_delete(PatternBlock *a)
{
    vm_free(a);
}


PatternBlock*       PatternBlock_intersect(PatternBlock *a, PatternBlock *b)
{ // Construct the intersecting pattern
    if (PatternBlock_alwaysFalse(a) || PatternBlock_alwaysFalse(b))
        return PatternBlock_newB(false);

    PatternBlock *res = PatternBlock_newB(true);
    int4 maxlength = (PatternBlock_getLength(a) > PatternBlock_getLength(b)) ?
        PatternBlock_getLength(a) : PatternBlock_getLength(b);

    res->offset = 0;
    int4 offset = 0;
    uintm mask1, val1, mask2, val2, commonmask;
    uintm resmask, resval;
    while (offset < maxlength)
    {
        mask1 = PatternBlock_getMask(a, offset * 8, sizeof(uintm) * 8);
        val1 = PatternBlock_getValue(a, offset * 8, sizeof(uintm) * 8);
        mask2 = PatternBlock_getMask(b, offset * 8, sizeof(uintm) * 8);
        val2 = PatternBlock_getValue(b, offset * 8, sizeof(uintm) * 8);
        commonmask = mask1 & mask2; // Bits in mask shared by both patterns
        if ((commonmask & val1) != (commonmask & val2))
        {
            res->nonzerosize = -1; // Impossible pattern
            PatternBlock_normalize(res);
            return res;
        }
        resmask = mask1 | mask2;
        resval = (mask1 & val1) | (mask2 & val2);
        dynarray_add(&res->maskvec, (void *)resmask);
        dynarray_add(&res->valvec, (void *)resval);
        offset += sizeof(uintm);
    }
    res->nonzerosize = maxlength;
    PatternBlock_normalize(res);
    return res;
}

bool                PatternBlock_identical(PatternBlock *op1, PatternBlock *op2)
{
    int tmplength;
    int length = 8 * PatternBlock_getLength(op2);
    tmplength = 8 * PatternBlock_getLength(op1);
    if (tmplength > length)
        length = tmplength;
    uintm mask1, mask2, value1, value2;
    int sbit = 0;
    while (sbit < length) {
        tmplength = length - sbit;
        if (tmplength > 8 * sizeof(uintm))
            tmplength = 8 * sizeof(uintm);

        mask1 = PatternBlock_getMask(op1, sbit, tmplength);
        value1 = PatternBlock_getValue(op1, sbit, tmplength);
        mask2 = PatternBlock_getMask(op2, sbit, tmplength);
        value2 = PatternBlock_getValue(op2, sbit, tmplength);
        if (mask1 != mask2)
            return false;

        if ((mask1 & value1) != (mask2 & value2))
            return false;

        sbit += tmplength;
    }

    return true;
}

uintm               PatternBlock_getMask(PatternBlock *pb, int startbit, int size)
{
    startbit -= 8 * pb->offset;
    uintm res;

    int wordnum1 = startbit / (8 * sizeof(uintm));
    int shift = startbit % (8 * sizeof(uintm));
    int wordnum2 = (startbit + size - 1) / (8 * sizeof(uintm));

    if ((wordnum1 < 0) || (wordnum1 >= pb->maskvec.len))
        res = 0;
    else
        res = (uintm)pb->maskvec.ptab[wordnum1];

    res <<= shift;
    if (wordnum1 != wordnum2) {
        uintm tmp;
        if ((wordnum2 < 0) || (wordnum2 >= pb->maskvec.len))
            tmp = 0;
        else
            tmp = (uintm)pb->maskvec.ptab[wordnum2];

        res |= (tmp >> (8 * sizeof(uintm) - shift));
    }
    res >>= (8 * sizeof(uintm) - size);

    return res;
}

uintm           PatternBlock_getValue(PatternBlock *pb, int startbit, int size)
{
    startbit -= 8 * pb->offset;
    int wordnum1 = startbit / (8 * sizeof(uintm));
    int shift = startbit % (8 * sizeof(uintm));
    int wordnum2 = (startbit + size - 1) / (8 * sizeof(uintm));
    uintm res;

    if ((wordnum1 < 0) || (wordnum1 >= pb->valvec.len))
        res = 0;
    else
        res = (uintm)pb->valvec.ptab[wordnum1];

    if (wordnum1 != wordnum2) {
        uintm tmp;
        if ((wordnum2 < 0) || (wordnum2 >= pb->valvec.len))
            tmp = 0;
        else
            tmp = (uintm)pb->valvec.ptab[wordnum2];
    }
    res >>= (8 * sizeof(uintm) - size);

    return res;
}

void                PatternBlock_normalize(PatternBlock *pb)
{
    if (pb->nonzerosize <= 0) {
        pb->offset = 0;
        dynarray_reset(&pb->maskvec);
        dynarray_reset(&pb->valvec);
    }
    int i1, i2, i;

    i1 = i2 = 0;
    while ((i1 != pb->maskvec.len) && (pb->maskvec.ptab[i1] == NULL)) {
        i1++;
        pb->offset += sizeof(uintm);
    }
    dynarray_erase(&pb->maskvec, 0, i1);
    dynarray_erase(&pb->valvec, 0, i1);

    if (pb->maskvec.len) {
        int suboff = 0;         // cutoff unaligned zeros from begining of mask
        uintm tmp = (uintm)pb->maskvec.ptab[0];
        while (tmp) {
            suboff += 1;
            tmp >>= 8;
        }

        suboff = sizeof(uintm) - suboff;
        if (suboff) {
            pb->offset += suboff;
            for (i = 0; i < (pb->maskvec.len - 1); i++) {
                tmp = ((uintm)pb->maskvec.ptab[i]) << (suboff * 8);
                tmp |= ((uintm)pb->maskvec.ptab[i + 1]) >> ((sizeof(uintm) - suboff) * 8);
                (uintm)pb->maskvec.ptab[i] = tmp;
            }
            (uintm)pb->maskvec.ptab[i] <<= suboff * 8;
            for (i = 0; i < (pb->valvec.len - 1); ++i) {
                tmp = ((uintm)pb->valvec.ptab[i]) << (suboff * 8);
                tmp |= ((uintm)pb->valvec.ptab[i + 1]) >> ((sizeof(uintm) - suboff) * 8);
                ((uintm)pb->valvec.ptab[i]) = tmp;
            }
            (uintm)pb->valvec.ptab[i] <<= suboff * 8;
        }

        i = pb->maskvec.len - 1;
        while (i) {
            --i;
            if (pb->maskvec.ptab[i])
                break;
        }

        if (i != pb->maskvec.len - 1) i++;

        dynarray_erase(&pb->maskvec, i, pb->maskvec.len - 1);
        dynarray_erase(&pb->valvec, i, pb->valvec.len - 1);
    }

    if (!pb->maskvec.len) {
        pb->offset = 0;
        pb->nonzerosize = 0;
        return;
    }

    pb->nonzerosize = pb->maskvec.len * sizeof(uintm);
    uintm tmp = (uintm)dynarray_back(&pb->maskvec);
    while ((tmp & 0xff) == 0) {
        pb->nonzerosize -= 1;
        tmp >>= 8;
    }
}

bool PatternBlock_isInstructionMatch(PatternBlock *pb, ParserWalker *walker)
{
    if (pb->nonzerosize <= 0)
        return (pb->nonzerosize == 0);

    int off = pb->offset, i;
    for (i = 0; i < pb->maskvec.len; i++) {
        uintm data = ParserWalker_getInstructionBytes(walker, off, sizeof(uintm));
        uintm mask = (uintm)pb->maskvec.ptab[i];
        if ((mask & data) != (uintm)pb->valvec.ptab[i])
            return false;

        off += sizeof(uintm);
    }

    return true;
}

bool    PatternBlock_isContextMatch(PatternBlock *pb, ParserWalker *walker) 
{
    if (pb->nonzerosize <= 0)
        return (pb->nonzerosize == 0);

    int off = pb->offset, i;
    for (i = 0; i < pb->maskvec.len; i++) {
        uintm data = ParserWalker_getContextBytes(walker, off, sizeof(uintm));
        uintm mask = (uintm)pb->maskvec.ptab[i];
        if ((mask & data) != (uintm)pb->valvec.ptab[i])
            return false;

        off += sizeof(uintm);
    }

    return true;
}

Pattern*            Pattern_new(int type)
{
    Pattern *p = vm_mallocz(sizeof(p[0]));

    p->type = type;

    return p;
}

InstructionPattern*     InstructionPattern_new(void)
{
    return Pattern_new(a_instructionPattern);
}

InstructionPattern*     InstructionPattern_newP(PatternBlock *pb)
{
    Pattern*    p = Pattern_new(a_instructionPattern);

    p->instruction.maskvalue = pb;

    return p;
}

InstructionPattern*     InstructionPattern_newB(bool tf)
{
    Pattern*    p = Pattern_new(a_instructionPattern);

    p->instruction.maskvalue = PatternBlock_newB(tf);

    return p;
}

void                    Pattern_delete(Pattern *p)
{
    vm_free(p);
}

bool            Pattern_isMatch(Pattern *pat, ParserWalker *walker)
{
    int i;
    switch (pat->type) {
    case a_combinePattern:
        if (!Pattern_isMatch(pat->combine.instr, walker))
            return false;

        if (!Pattern_isMatch(pat->combine.context, walker))
            return false;

        return true;

    case a_instructionPattern:
        return PatternBlock_isInstructionMatch(pat->instruction.maskvalue, walker);

    case a_contextPattern:
        return PatternBlock_isContextMatch(pat->context.maskvalue, walker);

    case a_orPattern:
        for (i = 0; i < pat->or.orlist.len; i++) {
            if (Pattern_isMatch(pat->or.orlist.ptab[i], walker)) 
                return true;
        }
        return false;

    default:
        vm_error("unsupport pattern[%d]", pat->type);
        return false;
    }
}

int     Pattern_numDisjoint(Pattern *pat)
{
    if (pat->type == a_orPattern) return pat->or.orlist.len;

    return 0;
}

Pattern*        Pattern_simplifyClone(Pattern *pat)
{
    switch (pat->type) {
    case a_instructionPattern:
        return InstructionPattern_newP(PatternBlock_clone(pat->instruction.maskvalue));
    }

    return NULL;
}

Pattern*        Pattern_commonSubPattern(Pattern *a, Pattern * b, int sa)
{
    int i;
    Pattern *p, *res, *next;

    switch (a->type) {
    case a_orPattern:
        p = a-> or .orlist.ptab[0];
        res = Pattern_commonSubPattern(p, b, sa);
        if (sa > 0) sa = 0;
        for (; i < a-> or .orlist.len; i++) {
            p = a-> or .orlist.ptab[i];
            next = Pattern_commonSubPattern(p, res, sa);
            res = next;
        }
        return res;

    default:
        vm_error("un-support type[%d]", a->type);
        return NULL;
    }
}

bool            Pattern_alwaysTrue(Pattern *a)
{
    int i;
    switch (a->type) {
    case a_instructionPattern:
        return PatternBlock_alwaysTrue(a->instruction.maskvalue);

    case a_contextPattern:
        return PatternBlock_alwaysTrue(a->context.maskvalue);

    case a_orPattern:
        for (i = 0; i < a-> or .orlist.len; i++) {
            Pattern *p = a-> or .orlist.ptab[i];
            if (Pattern_alwaysTrue(p))
                return true;
        }
        return false;

    default:
        vm_error("not support pattern type [%d]", a->type);
        return false;
    }
}

bool            Pattern_alwaysFalse(Pattern *a)
{
    int i;
    switch (a->type) {
    case a_instructionPattern:
        return PatternBlock_alwaysFalse(a->instruction.maskvalue);

    case a_contextPattern:
        return PatternBlock_alwaysFalse(a->context.maskvalue);

    case a_orPattern:
        for (i = 0; i < a-> or .orlist.len; i++) {
            Pattern *p = a-> or .orlist.ptab[i];
            if (Pattern_alwaysFalse(p))
                return false;
        }
        return true;

    default:
        vm_error("not support pattern type [%d]", a->type);
        return false;
    }
}
