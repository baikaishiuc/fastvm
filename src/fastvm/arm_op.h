
enum SRType {
    SRType_None,
    SRType_LSL,
    SRType_LSR,
    SRType_ASR,
    SRType_ROR,
    SRType_RRX,
};

struct bits {
    int v;
    char n;
    char carry_out;
    char overflow;
    char resv;
};

#define ZeroExtend(a)           a

#define NOT(a)              (~(a))
#define INT_TOPMOSTBIT(a)   ((a >> 31) & 1)
#define IsZeroBit(a)        (a == 0)

static inline int BadReg(int n)
{
    return n == 13 || n == 15;
}

static inline int RightMostBitPos(int a, int size)
{
    int i;

    if (a == 0)
        return -1;

    for (i = 0; i < size; i++) {
        if (a & (1 << i))
            return i;
    }

    return -1;
}

static inline int SignExtend(int a, int size)
{
    if (a & (1 << (size - 1)))
        return (~((1 << size) - 1)) | a;

    return a;
}

static inline int InITBlock(struct arm_emu *e)
{
    return e->it.inblock;
}

static inline int LastInITBlock(struct arm_emu *e)
{
    return e->it.inblock == 1;
}

static inline int _ConditionPassed(struct arm_cpsr *c, int cond)
{
    switch (cond) {
    case ARM_COND_EQ:
        return c->z == 1;

    case ARM_COND_NE:  
        return c->z == 0;

    case ARM_COND_CS:  
        return c->c == 1;

    case ARM_COND_CC:  
        return c->c == 0;

    case ARM_COND_MI:  
        return c->n == 1;

    case ARM_COND_PL:  
        return c->n == 0;

    case ARM_COND_VS:  
        return c->v == 1;

    case ARM_COND_VC:  
        return c->v == 0;

    case ARM_COND_HI:  
        return (c->c == 1) && (c->z == 0);

    case ARM_COND_LS:  
        return (c->c == 0) || (c->z == 1);

    case ARM_COND_GE:  
        return c->n == c->v;

    case ARM_COND_LT:
        return c->n != c->v;

    case ARM_COND_GT:  
        return (c->z == 0) && (c->n == c->v);

    case ARM_COND_LE:  
        return (c->z == 1) || (c->n != c->v);

    case ARM_COND_AL:  
        return 1;

    default:
        return 1;
    }
}

static inline int ConditionPassed(struct arm_emu *e)
{
    if (!e->it.inblock)
        return 1;

    int cond = (e->it.et[e->it.num - e->it.inblock] == 't') ? e->it.cond:((e->it.cond & 1) ? (e->it.cond - 1):(e->it.cond + 1));

    return _ConditionPassed(ARM_APSR_PTR(e), cond);
}

static inline int BitCount(int v)
{
    int c;

    c = (v & 0x55555555) + ((v >> 1) & 0x55555555);
    c = (c & 0x33333333) + ((c >> 2) & 0x33333333);
    c = (c & 0x0F0F0F0F) + ((c >> 4) & 0x0F0F0F0F);
    c = (c & 0x00FF00FF) + ((c >> 8) & 0x00FF00FF);
    c = (c & 0x0000FFFF) + ((c >> 16) & 0x0000FFFF);

    return c;
}

static inline struct bits LSL_C(struct bits x, int n)
{
    int t = x.v << n;

    x.carry_out = t & (1 << n);
    x.v = t & ((1 << n) - 1);

    return x;
}

static inline struct bits LSR_C(struct bits x, int n)
{
    if (n > x.n)
        x.carry_out = 0;
    else
        x.carry_out = (x.v >> (n - 1)) & 1;

    x.v = (unsigned)x.v >> n;

    return x;
}

static inline struct bits ASR_C(struct bits x, int n)
{
    int t = SignExtend(x.v, x.n);

    x.v = t >> n;
    x.carry_out = (t >> (n - 1)) & 1;

    return x;
}

static inline struct bits LSL(struct bits x, int n)
{
    if (n == 0)
        return x;
    else
        return LSL_C(x, n);
}

static inline struct bits LSR(struct bits x, int n)
{
    if (n == 0)
        return x;
    else
        return LSR_C(x, n);
}

static inline struct bits ASR(struct bits x, int n)
{
    if (n == 0)
        return x;
    else
        return ASR_C(x, n);
}

static inline struct bits ROR_C(struct bits x, int n)
{
    struct bits t1, t2;
    int m = n % x.n;

    if (m) {
        t1 = LSR(x, m);
        t2 = LSL(x, x.n - m);

        x.v = t1.v | t2.v;
    }

    x.carry_out = (x.v >> (x.n - 1)) & 1;

    return x;
}

static inline struct bits ROL_C(struct bits x, int n)
{
    struct bits t1, t2;
    int m = n % x.n;

    if (m) {
        t1 = LSL(x, m);
        t2 = LSR(x, x.n - m);

        x.v = t1.v | t2.v;
    }

    x.carry_out = x.v & 1;

    return x;
}

static inline struct bits RRX_C(struct bits x, int cin)
{
    x.v = ((unsigned)x.v >> 1) | (cin << (x.n - 1));
    x.carry_out = x.v & 1;

    return x;
}

static inline struct bits ROR(struct bits x, int n)
{
    return ROR_C(x, n);
}

static inline struct bits ROL(struct bits x, int n)
{
    return ROL_C(x, n);
}

static inline struct bits RRX(struct bits x, int cin)
{
    return RRX_C(x, cin);
}

/* Thumb-2SupplementReferceManual P93 */
static inline struct bits ThumbExpandImmWithC(struct arm_emu *e, int imm)
{
    struct bits bs, ret = {0};
    int t, m, imm32, c = 0;
    if (BITS_GET(imm, 10, 2) == 0) {
        t = BITS_GET(imm, 0, 8);
        m = BITS_GET(imm, 8, 2);

        if (m && !t)
            vm_error("arm unpredictable");

        switch (m) {
        case 0:
            imm32 = t;
            break;

        case 1:
            imm32 = t << 16 | t;
            break;

        case 2:
            imm32 = t << 24 | t << 8;
            break;

        case 3:
            imm32 = t << 24 | t << 16 | t << 8 | t ;
            break;
        }

        ret.v = imm32;
        ret.carry_out = e->aspr.c;
    }
    else {
        t = BITS_GET(imm, 0, 7) | 0x80;
        bs.v = t;
        bs.n = 8;
        ret = (ROR_C(bs, BITS_GET(imm, 7, 5)));
    }

    return ret;
}

static inline int ThumbExpandImm(struct arm_emu *e, int imm)
{
    struct bits b = ThumbExpandImmWithC(e, imm);

    return b.v;
}

static inline int DecodeImmShift(int type, int imm, enum SRType *srtype)
{
    switch (type) {
    case 0:
        *srtype = SRType_LSL;
        return imm;

    case 1:
        *srtype = SRType_LSR;
        return imm ? imm : 32;
    case 2:
        *srtype = SRType_ASR;
        return imm ? imm : 32;

    case 3:
        if (imm) {
            *srtype = SRType_ROR;
            return imm;
        }
        else {
            *srtype = SRType_RRX;
            return 1;
        }
    }
    return 0;
}

static inline struct bits Shift_C(struct bits value, enum SRType type, int amount, char carry_in)
{
    struct bits ret;
    switch (type) {
    case SRType_None:
        ret = value;
        ret.carry_out = carry_in;
        return value;

    case SRType_LSL:
        if (amount == 0)
            return value;
        else
            return LSL_C(value, amount);

    case SRType_LSR:
        return LSR_C(value, amount);

    case SRType_ASR:
        return ASR_C(value, amount);

    case SRType_ROR:
        return ROR_C(value, amount);

    case SRType_RRX:
        return RRX_C(value, amount);
    }
}

/* http://www.c-jump.com/CIS77/CPU/Overflow/lecture.html */
static inline struct bits AddWithCarry(int x, int y, int carry_in)
{
    struct bits result = { 0 };

    result.v = x + y + carry_in;

    if ((x > 0 && y > 0 && result.v < 0) || (x < 0 && y < 0 && result.v > 0))
        result.overflow = 1;

    unsigned int s = (unsigned)x + (unsigned)y + carry_in;
    if (s < (unsigned)x || s < (unsigned)y || s < (unsigned)carry_in)
        result.carry_out = 1;

    return result;
}
