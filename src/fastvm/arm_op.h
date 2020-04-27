
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
};

#define ZeroExtend(a)           a

static inline int SignExtend(int a, int size)
{
    if (a & (1 << (size - 1)))
        return (-1 & ((1 << size) - 1)) | a;

    return a;
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
static inline int ThumbExpandImmWithC(struct arm_emu *e, int imm)
{
    struct bits bs;
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
    }
    else {
        t = BITS_GET(imm, 0, 7) | 0x80;
        bs.v = t;
        bs.n = 8;
        imm32 = (ROR_C(bs, BITS_GET(imm, 7, 5))).v;
    }

    return imm32;
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
