
#include "mcore/mcore.h"
#include "vm.h"
#include "arm_emu.h"
#include "minst.h"
#include <math.h>


#define ARM_SP_VAL(e)   e->regs[ARM_REG_SP]     
#define ARM_PC_VAL(e)   e->regs[ARM_REG_PC]
#define ARM_APSR_PTR(e)     ((struct arm_cpsr *)&e->regs[ARM_REG_APSR])

#define REG_SET_KNOWN(e, reg)       (e->known |= 1 << reg)
#define REG_SET_UNKNOWN(e, reg)     (e->knwwn &= ~(1 << reg))
#define REG_IS_KNOWN(e,reg)         (e->knwon & (1 << reg)

#define PROCESS_STACK_SIZE          (256 * KB)
#define PROCESS_STACK_BASE          0x10000000
#define IN_PROCESS_STACK_RANGE(a)   (((a) < PROCESS_STACK_BASE) && (a >= (PROCESS_STACK_BASE - PROCESS_STACK_SIZE)))

#define MEM_ERROR           -1
#define MEM_CODE            0       // [0 - 64M)
#define MEM_STACK           1       // [254 - 256M)
#define MEM_HEAP            2       // ?

#define MEM_STACK_TOP(e)        (PROCESS_STACK_BASE - e->regs[ARM_REG_SP])
#define MEM_STACK_TOP1(e)       (PROCESS_STACK_BASE - e->prev_regs[ARM_REG_SP])


#define BITS_GET(a,offset,len)   ((a >> (offset )) & ((1 << len) - 1))
#define BITS_GET_SHL(a, offset, len, sh)      (BITS_GET(a, offset, len) << sh)

#define ARM_UNPREDICT()   vm_error("arm unpredictable. %s:%d", __FILE__, __LINE__)

#define IDUMP_ADDR              0x01
#define IDUMP_BINCODE           0x02
#define IDUMP_STACK_HEIGHT      0x04    
#define IDUMP_STATUS            0x08
#define IDUMP_TRACE             0x10
#define IDUMP_LIVE              0x20
#define IDUMP_REACHING_DEFS     0x40
#define IDUMP_DEFAULT           -1

#define EMU_SEQ_MODE            0
#define EMU_CONST_MODE          1
#define EMU_TRACE_MODE          2

/* arm指令中，某部分指令是上下文相关的 */
#define EMU_IS_SEQ_MODE(e)      (e->seq_mod == EMU_SEQ_MODE)
#define EMU_IS_CONST_MODE(e)    (e->seq_mod == EMU_CONST_MODE)
#define EMU_IS_TRACE_MODE(e)    (e->seq_mod == EMU_TRACE_MODE)

#define EMU_SET_SEQ_MODE(e)     (e->seq_mod = EMU_SEQ_MODE)
#define EMU_SET_CONST_MODE(e)   (e->seq_mod = EMU_CONST_MODE)
#define EMU_SET_TRACE_MODE(e)   (e->seq_mod = EMU_TRACE_MODE)

#define MDO_SUCCESS             0
#define MDO_CANT_TRACE          1
#define MDO_TRACE_FLAT          2
#define MDO_TRACE_FLAT_OUT_CSM  3


struct emu_temp_var
{
    /* temp variable id */
    int     t;
    /* stack address */
    int     top;
};

struct arm_emu {
    struct {
        unsigned char*  data;
        int             len;
        int             pos;

        struct arm_inst_ctx ctx;
    } code;

    char inst_fmt[64];
    unsigned int prev_regs[32];
    unsigned int regs[32];
    unsigned int known;
    int seq_mod;

    int(*inst_func)(unsigned char *inst, int len,  char *inst_str, void *user_ctx);
    void *user_ctx;

    /* emulate stack */
    struct {
        /* 堆栈分配的地址 */
        unsigned char*      data;
        /* 我们分配的内存写的时候从低往高写，但是真实的程序堆栈是从高往低写，所以我们让base指向末尾 */
        unsigned char*      base;
        int                top;
        int                size;
    } stack;

    struct {
        unsigned nfa : 1;
        unsigned dfa :1;
        unsigned mblk : 1;
        unsigned cfg : 1;
    } dump;

    /* target machine内的函数起始位置 */
    int             baseaddr;
    int             thumb;
    int             meet_blx;
    int             decode_inst_flag;
    /* P26 */
    //struct arm_cpsr apsr;

    /* 生成IR时，会产生大量的临时变量，这个是临时变量计数器 */
    struct dynarray tvars;
    int             tvar_id;

    struct {
        int     inblock;
        int     num;
        int     cond;
        char    et[8];
    } it;

    struct minst_blk        mblk;
    struct minst            *prev_minst;
};

static const char *regstr[] = {
    "r0", "r1", "r2", "r3",
    "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11",
    "r12", "sp", "lr", "pc",
    "", "", "apsr", "",
    "", "", "", "",
};

static const char *condstr[] = {
    "eq", "ne", "cs", "cc",
    "mi", "pl", "vs", "vc",
    "hi", "ls", "ge", "lt",
    "gt", "le", "{al}"
};

#define FLAG_DISABLE_EMU            0x1            
#define IS_DISABLE_EMU(e)           (e->decode_inst_flag & FLAG_DISABLE_EMU)

typedef int(*arm_inst_func)    (struct arm_emu *emu, struct minst *minst, uint16_t *inst, int inst_len);

#include "arm_op.h"

static struct reg_node*     arm_insteng_parse(uint8_t *code, int len, int *olen);
static int arm_minst_do(struct arm_emu *emu, struct minst *minst);

char *it2str(int firstcond, int mask, char *buf)
{
    int i, j;
    int c0 = firstcond & 1;

    buf[0] = 0;
    i = RightMostBitPos(mask, 4);
    if (i == -1)
        ARM_UNPREDICT();

    strcat(buf, "t");

    for (j = 0; j < (3 - i); j++) {
        strcat(buf, ((mask >> (3 - j)) & 1) == c0 ? "t":"e");
    }

    return buf;
}

const int cur_inst_it(struct arm_emu *e)
{
    if (!e->it.inblock)
        vm_error("not in it block");

    return e->it.et[e->it.num - e->it.inblock] == 't';
}

const char *cur_inst_it_cond(struct arm_emu *e)
{
    if (!e->it.inblock)
        return "";

    return (e->it.et[e->it.num - e->it.inblock] == 't') ? condstr[e->it.cond]:condstr[(e->it.cond & 1) ? (e->it.cond - 1):(e->it.cond + 1)];
}

static char* reglist2str(int reglist, char *buf)
{
    int i, start_reg = -1, len;
    buf[0] = 0;

    strcat(buf, "{");
    for (i = 0; i < 16; ) {
        if (reglist & (1 << i)) {
            strcat(buf, regstr[i]);

            start_reg = i++;
            while (reglist & (1 << i)) i++;

            if (i != (start_reg + 1)) {
                strcat(buf, "-");
                strcat(buf, regstr[i - 1]);
            }

            strcat(buf, ",");
        }
        else
            i++;
    }
    len = strlen(buf);
    if (buf[len - 1] == ',')  buf[len - 1] = 0;
    strcat(buf, "}");

    return buf;
}

static void arm_prepare_dump(struct arm_emu *emu, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(emu->inst_fmt, sizeof (emu->inst_fmt), fmt, ap);
    va_end(ap);
}

static int arm_dump_bitset(const char *desc, struct bitset *v, char *obuf)
{
    char *o = obuf;
    int olen = 0, pos = -1;
    /* dump liveness calculate result */
    olen = sprintf(o += olen, "[%s: ", desc);
    while ((pos = bitset_next_bit_pos(v, pos + 1)) >= 0) {
        olen = sprintf(o += olen, "%d  ", pos);
    }
    if (o[olen - 1] == ' ') olen--;
    olen = sprintf(o += olen, "] ");

    return (o + olen) - obuf;
}

static int arm_dump_temp_reglist(const char *desc, struct bitset *v, char *obuf)
{
    char *o = obuf;
    int i, j, olen = 0;
    /* dump liveness calculate result */
    olen = sprintf(o += olen, "[%s: ", desc);
    if (v->len4) {
        for (i = 0; i < 32; i++) {
            if (v->data[0] & (1 << i))
                olen = sprintf(o += olen, "%s ", regstr[i]);
        }

        for (i = 1; i < v->len4; i++) {
            for (j = 0; j < 32; j++) {
                if (v->data[i] & (1 << j))
                    olen = sprintf(o += olen, "t%d ", i * 32 + j);
            }
        }
    }
    if (o[olen - 1] == ' ') olen--;
    olen = sprintf(o += olen, "] ");

    return (o + olen) - obuf;
}

static int arm_inst_print_format(struct arm_emu *emu, struct minst *minst, unsigned int flag, char *obuf)
{
    int i, len, olen = 0;
    char *buf, *param, *o = obuf;

    if (!minst->len) {
        obuf[0] = 0;
        return 0;
    }

    buf = emu->inst_fmt;
    len = strlen(buf);
    for (i = 0; i < len; i++) {
        buf[i] = toupper(buf[i]);
    }

    param = strchr(buf, ' ');
    if (param) {
        *param++ = 0;
        while (isblank(*param)) param++;
    }

    olen = sprintf(o += olen, "%04d ", minst->id);

    if (flag & IDUMP_ADDR)
        olen = sprintf(o += olen, "%08x ", emu->baseaddr + (minst->addr - emu->code.data));

    if (flag & IDUMP_STACK_HEIGHT)
        olen = sprintf(o += olen, "%03x ", MEM_STACK_TOP1(emu));

    if (flag & IDUMP_BINCODE) {
        for (i = 0; i < minst->len; i++) {
            olen = sprintf(o += olen, "%02x ", (unsigned char)minst->addr[i]);
        }
    }

    for (; i < 6; i++) {
        olen = sprintf(o += olen, "   ");
    }

    olen = sprintf(o += olen, "%-10s %-18s  ", buf, param);

    if (flag & IDUMP_STATUS) {
        olen = sprintf(o += olen, "[");
        olen = sprintf(o += olen, "%c", minst->flag.dead_code ? 'D':' ');
        olen = sprintf(o += olen, "]");
    }

    if (flag & IDUMP_TRACE) {
        if (emu->code.ctx.ld != -1) {
            olen = sprintf(o += olen, "[%-3s=%08x", regstr[emu->code.ctx.ld], minst->ld_imm);
        }
        else {
            olen = sprintf(o += olen, "[");
        }

        olen = sprintf(o += olen, ",APSR=%c%c%c%c]", minst->apsr.z ? 'Z':' ', 
            minst->apsr.c ? 'C':' ', minst->apsr.n ? 'N':' ', minst->apsr.v ? 'V':' ');
    }

    /* dump liveness calculate result */
    if (flag & IDUMP_LIVE) {
        olen = arm_dump_temp_reglist("def", &minst->def, o += olen);
        olen = arm_dump_temp_reglist("use", &minst->use, o += olen);
        //olen = arm_dump_temp_reglist("in", &minst->in, o += olen);
        //olen = arm_dump_temp_reglist("out", &minst->out, o+= olen);
    }

    if (flag & IDUMP_REACHING_DEFS) {
        olen = arm_dump_bitset("r_in", &minst->rd_in, o += olen);
        olen = arm_dump_bitset("r_out", &minst->rd_out, o+= olen);
        olen = arm_dump_bitset("kills", &minst->kills, o += olen);
    }

    o += olen;
    emu->inst_fmt[0] = 0;

    return o - obuf;
}

static struct emu_temp_var *emu_alloc_temp_var(struct arm_emu *emu, unsigned long addr)
{
    struct emu_temp_var *var;

    /* 所有对进程堆栈的操作，都要转换成临时变量，假如这个地址不是堆栈内，直接报错 */
    if (!IN_PROCESS_STACK_RANGE(addr))
        vm_error("%ul not in process range", addr);

    var = calloc(1, sizeof (var[0]));
    if (!var)
        vm_error("stack_var calloc failure");

    var->top = addr;
    var->t = SYS_REG_NUM + emu->tvar_id++;

    dynarray_add(&emu->tvars, var);

    return var;
}

static struct emu_temp_var *emu_find_temp_var(struct arm_emu *emu, unsigned long addr)
{
    struct emu_temp_var *tvar;
    int i;

    for (i = emu->tvars.len - 1; i >= 0; i--) {
        tvar = emu->tvars.ptab[i];
        if (tvar->top == addr)
            return tvar;
    }

    return NULL;
}

static struct emu_temp_var* emu_stack_push(struct arm_emu *emu, int val)
{

    int top = MEM_STACK_TOP(emu);;

    if (top >= emu->stack.size)
        vm_error("arm emulator stack size overflow");

    emu->regs[ARM_REG_SP] -= 4;

    *(int *)(emu->stack.base - MEM_STACK_TOP(emu)) = val;

    return emu_alloc_temp_var(emu, ARM_SP_VAL(emu));
}

static struct emu_temp_var* emu_stack_top(struct arm_emu *emu)
{
    int top = MEM_STACK_TOP(emu);
    
    if ((top == 0) || !emu->tvars.len)
        vm_error("arm emulator stack empty");

    return emu->tvars.ptab[emu->tvars.len - 1];
}

static void emu_stack_pop(struct arm_emu *emu)
{
    int top = MEM_STACK_TOP(emu);

    if ((top == 0) || !emu->tvars.len)
        vm_error("arm emulator stack empty");

    free(emu->tvars.ptab[emu->tvars.len - 1]);
    emu->tvars.len--;

    emu->regs[ARM_REG_SP] += 4;
}

static int t1_inst_lsl(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_lsr(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_asr(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int thumb_inst_push(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    char buf[32];
    int i, reglist;

    if (len == 1) {
        reglist = emu->code.ctx.register_list | (emu->code.ctx.m << ARM_REG_LR);
        arm_prepare_dump(emu, "push %s", reglist2str(reglist, buf));
    } else {
        reglist = emu->code.ctx.register_list | (emu->code.ctx.m << ARM_REG_LR);
        arm_prepare_dump(emu, "push.w %s", reglist2str(emu->code.ctx.register_list, buf));
    }

    for (i = 0; i < 16; i++) {
        if (reglist & (1 << i))
            live_use_set(&emu->mblk, i);
    }

#if 0
    struct emu_temp_var *var;
    for (i = 0; i < 16; i++) {
        if (reglist & (1 << i)) {
            var = emu_stack_push(emu, emu->regs[i]);
            live_def_set(&emu->mblk, var->t);
        }
    }
#endif

    return 0;
}

static int thumb_inst_pop(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    char buf[32];
    int reglist, i;

    if (len == 2) {
        reglist = code[1] & 0xdfff;
        arm_prepare_dump(emu, "pop%s.w %s", cur_inst_it_cond(emu), reglist2str(reglist, buf));
    }
    else {
        reglist = emu->code.ctx.register_list | (emu->code.ctx.m << ARM_REG_PC);
        arm_prepare_dump(emu, "pop%s %s", cur_inst_it_cond(emu), reglist2str(reglist, buf));
    }

    for (i = 0; i < 16; i++) {
        if (reglist & (1 << i)) {
            live_def_set(&emu->mblk, i);
        }
    }

#if 0
    struct emu_temp_var *var;
    if (ConditionPassed(emu)) {
        for (i = 0; i < 16; i++) {
            if (reglist && (1 << i)) {
                /* FIXME: */
                var = emu_stack_top(emu);
                live_use_set(&emu->mblk, var->t);
                emu_stack_pop(emu);
            }
        }
    }
#endif

    return 0;
}

static int t1_inst_pop(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_add1(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_add(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    int lm, imm32;
    if (((code[0] >> 12) & 0xf) == 0b1010) {
        arm_prepare_dump(emu, "add %s, sp, #0x%x", regstr[emu->code.ctx.ld], imm32 = emu->code.ctx.imm * 4);
        liveness_set2(&emu->mblk, emu->code.ctx.ld, emu->code.ctx.ld, ARM_REG_SP);
        lm = ARM_REG_SP;
    }
    else {
        arm_prepare_dump(emu, "add %s, %s", regstr[emu->code.ctx.ld], regstr[lm = emu->code.ctx.lm]);
        liveness_set2(&emu->mblk, emu->code.ctx.ld, emu->code.ctx.ld, emu->code.ctx.lm);
    }

    if (ConditionPassed(emu) && emu->code.ctx.setflags) {
        live_def_set(&emu->mblk, ARM_REG_APSR);
    }

    if (IS_DISABLE_EMU(emu))
        return 0;

    /*add sp */
    if (((code[0] >> 12) & 0xf) == 0b1010) {
        if (ConditionPassed(emu)) {
            struct bits b = AddWithCarry(ARM_SP_VAL(emu), imm32, 0);
            emu->regs[emu->code.ctx.ld] = b.v;

            if (emu->code.ctx.setflags) {
                ARM_APSR_PTR(emu)->n = INT_TOPMOSTBIT(b.v);
                ARM_APSR_PTR(emu)->z = IsZeroBit(b.v);
                ARM_APSR_PTR(emu)->c = b.carry_out;
                ARM_APSR_PTR(emu)->v = b.overflow;
            }
        }
    }
    else {
    }

    return 0;
}

static int t1_inst_sub(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

/* P454 */
static int thumb_inst_sub(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    arm_prepare_dump(emu, "sub%s sp, sp, #0x%x", cur_inst_it_cond(emu), emu->code.ctx.imm * 4);

    emu->code.ctx.ld = ARM_REG_SP;
    liveness_set(&emu->mblk, emu->code.ctx.ld, emu->code.ctx.ld);
    if (emu->code.ctx.setflags)
        live_def_set(&emu->mblk, ARM_REG_APSR);

    if (IS_DISABLE_EMU(emu))
        return 0;

    if (ConditionPassed(emu)) {
        struct bits bits = AddWithCarry(ARM_SP_VAL(emu), NOT(emu->code.ctx.imm * 4), 1);

        emu->regs[emu->code.ctx.ld] = bits.v;

        if (emu->code.ctx.setflags) {
            ARM_APSR_PTR(emu)->n = INT_TOPMOSTBIT(bits.v);
            ARM_APSR_PTR(emu)->z = IsZeroBit(bits.v);
            ARM_APSR_PTR(emu)->c = bits.carry_out;
            ARM_APSR_PTR(emu)->v = bits.overflow;
        }
    }

    REG_SET_KNOWN(emu, emu->code.ctx.ld);

    return 0;
}

static int thumb_inst_sub_reg(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    arm_prepare_dump(emu, "sub%s %s, %s, %s", InITBlock(emu) ? "c":cur_inst_it_cond(emu), 
        regstr[emu->code.ctx.ld], regstr[emu->code.ctx.ln], regstr[emu->code.ctx.lm]);

    liveness_set2(&emu->mblk, emu->code.ctx.ld, emu->code.ctx.ln, emu->code.ctx.lm);

    emu->code.ctx.setflags = !InITBlock(emu);
    if (emu->code.ctx.setflags)
        live_def_set(&emu->mblk, ARM_REG_APSR);

    return 0;
}

static int t1_inst_mov(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_mov_0100(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    struct minst *const_minst;

    arm_prepare_dump(emu, "mov%s %s, %s", cur_inst_it_cond(emu), regstr[emu->code.ctx.ld], regstr[emu->code.ctx.lm]);

    minst->type = mtype_mov_reg;

    if (EMU_IS_CONST_MODE(emu)) {
        const_minst = minst_get_last_const_definition(&emu->mblk, minst, emu->code.ctx.lm);
        if (const_minst) {
            minst_set_const(minst, const_minst->ld_imm);
        }
    }
    else if (EMU_IS_TRACE_MODE(emu)) {
        const_minst = minst_get_trace_def(&emu->mblk, emu->code.ctx.lm, NULL, 0);

        if (minst_is_tconst(const_minst))
            minst_set_trace(minst, const_minst->ld_imm);
    }

    return 0;
}

static int t1_inst_mov1(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    arm_prepare_dump(emu, "mov%s %s, #0x%x",  InITBlock(emu)? "c":cur_inst_it_cond(emu), regstr[emu->code.ctx.ld], emu->code.ctx.imm);

    if (!minst->flag.in_it_block)
        minst_set_const(minst, emu->code.ctx.imm);


    return 0;
}

static int t1_inst_cmp_imm(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    arm_prepare_dump(emu, "cmp %s, 0x%x", regstr[emu->code.ctx.lm], emu->code.ctx.imm);

    liveness_set(&emu->mblk, ARM_REG_APSR, emu->code.ctx.lm);

    return 0;
}

static int thumb_inst_cmp(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    arm_prepare_dump(emu, "cmp %s, %s", regstr[emu->code.ctx.ln], regstr[emu->code.ctx.lm]);

    minst->cmp.lm = emu->code.ctx.lm;
    minst->cmp.ln = emu->code.ctx.ln;
    minst->type = mtype_cmp;

    live_def_set(&emu->mblk, ARM_REG_APSR);

    if (EMU_IS_CONST_MODE(emu)) {
        struct minst *ln_minst = minst_get_last_const_definition(&emu->mblk, minst, emu->code.ctx.ln);
        struct minst *lm_minst = minst_get_last_const_definition(&emu->mblk, minst, emu->code.ctx.lm);

        if (!ln_minst && !lm_minst)
            return 0;

        /* cmp比较的2个寄存器都是常量，可以直接常量转换 */
        if (ln_minst && lm_minst && ln_minst->flag.is_const && lm_minst->flag.is_const) {
            struct bits bs = AddWithCarry(ln_minst->ld_imm, ~lm_minst->ld_imm, 1);

            minst->apsr.n = INT_TOPMOSTBIT(bs.v);
            minst->apsr.z = IsZeroBit(bs.v);
            minst->apsr.c = bs.carry_out;
            minst->apsr.v = bs.overflow;
            minst->flag.is_const = 1;
        }
        /* 假如比较的2个寄存器不是直接常量，
        比如 cmp r5, r0 
        我们分开确认每个寄存器比如 r5，是由哪个值定义的，他的上方是否有形如
            mov r5, r6
        这样的指令，如果是的话，因为r6一定不是常量(假如是常量，那么在常量传播中
        r6和r5一定已经被常数化了)，所以我们尝试分析r6为什么不是常量，有2种可能
        1. 本身的值就是模糊定义的，可能是外部参数进来的，可能是某函数返回值，或者内存里的某个定义
        2. 这条指令所在的cfg节点有多个前驱节点，每个前驱节点都有对r6的定值

        我们先不分析1的情况，2中的情况，假如2中的每个前驱节点对r6的定值，对 状态寄存器 的影响
        是一样的，那么我们认为这个地方的cmp指令是可以优化的。

        我们把这种优化称之胃 多路径常量优化

        先不处理2个指令都要做多路径常量优化的情况
        */
        else  if (0) {
            struct minst *p_minst = minst->preds.minst, *d_minst = NULL;

            int reg = (ln_minst && ln_minst->flag.is_const) ? emu->code.ctx.lm : emu->code.ctx.ln;

            for (; p_minst; p_minst = p_minst->preds.minst) {
                if (p_minst->preds.next)
                    return 0;

                if ((minst_get_def(p_minst) == reg) && (p_minst->type == mtype_mov_reg))
                    break;
            }

            if (!p_minst)
                return 0;

            int reg_use = minst_get_use(p_minst);

            BITSET_INIT(defs);
            struct minst *const_minst = NULL;
            int pos = -1, p_apsr = -1, ok = 1, passed, i;
            struct bits bits;
            struct dynarray d = { 0 }, id = { 0 };
            union {
                struct arm_cpsr apsr;
                int ld_imm;
            } v = { 0 };

            struct minst *b_minst = minst->succs.minst;

            /* 获取当前cmp指令的下一个b<cond> jmp指令 */
            while (b_minst && !minst_is_b0(b_minst)) {
                b_minst = b_minst->succs.minst;
            }

            /* 到了末尾退出 */
            if (!b_minst)
                return 0;

            bitset_clone(&defs, &p_minst->rd_in);
            bitset_and(&defs, &emu->mblk.defs[reg_use]);

            /* 假如这个常量传播在这个控制流节点内是起始路径可达的，则以这个常量的计算作为根据，查看
            假如以这个常量走入的所有分支给出的对reg_use的定值是否造成的影响一样 */
            pos = bitset_next_bit_pos(&defs, 0);
            if (pos < 0)
                return 0;

#define minst_cmp_calc(apsr, imm1, imm2) do { \
        struct bits bits = AddWithCarry(imm1, ~imm2, 1); \
        (apsr).n = INT_TOPMOSTBIT(bits.v); \
        (apsr).z = IsZeroBit(bits.v); \
        (apsr).c = bits.carry_out; \
        (apsr).v = bits.overflow; \
    } while (0)

            for (ok = 0; pos >= 0; pos = bitset_next_bit_pos(&defs, pos + 1)) {
                const_minst = emu->mblk.allinst.ptab[pos];
                if (const_minst->flag.is_const
                    && minst_blk_is_on_start_unique_path(&emu->mblk, const_minst, p_minst)) {

                    if ((ln_minst && ln_minst->flag.is_const))
                        bits = AddWithCarry(ln_minst->ld_imm, ~const_minst->ld_imm, 1);
                    else
                        bits = AddWithCarry(const_minst->ld_imm, ~lm_minst->ld_imm, 1);

                    v.apsr.n = INT_TOPMOSTBIT(bits.v);
                    v.apsr.z = IsZeroBit(bits.v);
                    v.apsr.c = bits.carry_out;
                    v.apsr.v = bits.overflow;

                    passed = _ConditionPassed(&v.apsr, b_minst->flag.b_cond);

                    if (minst_blk_get_all_branch_reg_const_def(&emu->mblk, minst, passed, reg_use, &d, &id))
                        goto exit;
                    ok = 1;

                    for (i = 0; i < id.len; i++) {
                        bitset_set(&defs, ((struct minst *)id.ptab[i])->id, 0);
                    }

                    break;
                }
            }

            if (!ok)
                return 0;

            /* 确认所有从某个cfg(a)节点走出的分支最后回到cfg(a)节点的循环入边某个reg_use值都是常量，在分别计算
            这些常量的值在cmp指令上对aspr的影响是否一样  */
            pos = bitset_next_bit_pos(&defs, 0);
            if (pos < 0)
                return 0;

            for (ok = 0; pos >= 0; pos = bitset_next_bit_pos(&defs, pos + 1)) {
                const_minst = d.ptab[pos];
                v.ld_imm = 0;
                
                if (!const_minst->flag.is_const) {
                    ok = 0;
                    break;
                }

                if ((ln_minst && ln_minst->flag.is_const))
                    bits = AddWithCarry(ln_minst->ld_imm, ~const_minst->ld_imm, 1);
                else
                    bits = AddWithCarry(const_minst->ld_imm, ~lm_minst->ld_imm, 1);

                v.apsr.n = INT_TOPMOSTBIT(bits.v);
                v.apsr.z = IsZeroBit(bits.v);
                v.apsr.c = bits.carry_out;
                v.apsr.v = bits.overflow;

                if (-1 == p_apsr)   p_apsr = v.ld_imm;
                else if (v.ld_imm != p_apsr) {
                    ok = 0;
                    break;
                }
            }

            /* 计算 cfg(a)中，它的左右分支是独立的，而且他的左右分支产生的循环入边，构成了 */

exit:
            /**/
            bitset_uninit(&defs);
            dynarray_reset(&d);
            dynarray_reset(&id);
            if (ok) {
                printf("woow, found a cmp instruction[%d] multi-path const propgation\n", minst->id);
                minst_set_const(minst, p_apsr);
            }
        }
    } else if (EMU_IS_TRACE_MODE(emu)) {
        if (minst->flag.is_const)   return 0;

        struct minst *ln_def = minst_get_trace_def(&emu->mblk, emu->code.ctx.ln, NULL, 0);
        struct minst *lm_def = minst_get_trace_def(&emu->mblk, emu->code.ctx.lm, NULL, 0);

        if (!minst_is_tconst(ln_def) || !minst_is_tconst(lm_def))
            return 0;

        minst_cmp_calc(minst->apsr, ln_def->ld_imm, lm_def->ld_imm);
        minst->flag.is_trace = 1;
    }

    return 0;
}

static int t1_inst_and(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int thumb_inst_it(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    int firstcond = emu->code.ctx.cond;
    int mask = emu->code.ctx.imm;

    if (0 == mask) ARM_UNPREDICT();
    if (0xf == firstcond) ARM_UNPREDICT();
    if ((0xe == firstcond) && (BitCount(mask) != 1)) ARM_UNPREDICT();
    if (InITBlock(emu)) ARM_UNPREDICT();

    arm_prepare_dump(emu, "i%s %s", it2str(firstcond, mask, emu->it.et), condstr[firstcond]);

    emu->it.cond = firstcond;

    if (EMU_IS_SEQ_MODE(emu)) {
        emu->it.inblock = 5 - RightMostBitPos(mask, 4);
        emu->it.num = emu->it.inblock - 1;
    }

    live_use_set(&emu->mblk, ARM_REG_APSR);

    return 0;
}

static int t1_inst_eor(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_adc(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_sbc(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_ror(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_tst(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_neg(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_cmn(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_orr(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_mul(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_bic(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_mvn(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_cpy(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

/* */
static unsigned long emu_addr_target2host(struct arm_emu *emu, unsigned long addr)
{
    return 0;
}

static unsigned long emu_addr_host2target(struct arm_emu *emu, unsigned long addr)
{
    addr -= emu->baseaddr;

    return (unsigned long)emu->code.data + addr;
}

static int thumb_inst_ldr(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    int sigcode = (code[0] >> 12) & 0xf, lm;
    unsigned long addr = 0;

    if (len == 1) {
        if (sigcode == 0b0100) {
            arm_prepare_dump(emu, "ldr %s, [pc, #0x%x] ", regstr[emu->code.ctx.ld], emu->code.ctx.imm * 4);

            liveness_set(&emu->mblk, emu->code.ctx.ld, ARM_REG_PC);
        }
        else if (sigcode == 0b1001) {
            if (emu->code.ctx.imm)
                arm_prepare_dump(emu, "ldr %s, [sp,#0x%x]", regstr[emu->code.ctx.ld], emu->code.ctx.imm * 4);
            else
                arm_prepare_dump(emu, "ldr %s, [sp]");

            addr = ARM_SP_VAL(emu) + emu->code.ctx.imm * 4;
            lm = ARM_REG_SP;
        }
        else {
            if (emu->code.ctx.imm)
                arm_prepare_dump(emu, "ldr %s, [%s, #0x%x] ", regstr[emu->code.ctx.ld], regstr[emu->code.ctx.lm], emu->code.ctx.imm * 4);
            else
                arm_prepare_dump(emu, "ldr %s, [%s] ", regstr[emu->code.ctx.ld], regstr[emu->code.ctx.lm]);

            addr = emu->regs[emu->code.ctx.lm] + emu->code.ctx.imm * 4;
            lm = emu->code.ctx.lm;
        }
    }
    else { 
        if (emu->code.ctx.lm == 15) {
            arm_prepare_dump(emu, "ldr.w %s, [pc, #0x%c%x]", regstr[emu->code.ctx.ld], emu->code.ctx.u ? '+':'-', emu->code.ctx.imm);
            liveness_set(&emu->mblk, emu->code.ctx.ld, ARM_REG_PC);
        }
        else {
            arm_prepare_dump(emu, "ldr.w %s, [%s,0x%x]", regstr[emu->code.ctx.ld], regstr[emu->code.ctx.lm], emu->code.ctx.imm);

            addr = emu->regs[emu->code.ctx.lm] + emu->code.ctx.imm;
            lm = emu->code.ctx.lm;
        }
    }

    if (IS_DISABLE_EMU(emu))
        return 0;
    
#if 0
    if (addr) {
        struct emu_temp_var *var;
        if ((var = emu_find_temp_var(emu, addr))) 
            liveness_set(&emu->mblk, emu->code.ctx.ld, var->t);
        else 
            liveness_set(&emu->mblk, emu->code.ctx.ld, lm);
    }
#endif

    return 0;
}

static int t1_inst_str_01101(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int t1_inst_str_10010(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    if (emu->code.ctx.imm) 
        arm_prepare_dump(emu, "str %s, [sp,#0x%x]", regstr[emu->code.ctx.lm], emu->code.ctx.imm * 4);
    else
        arm_prepare_dump(emu, "str %s, [sp]", regstr[emu->code.ctx.lm]);

    if (IS_DISABLE_EMU(emu))
        return 0;

#if 0
    unsigned long addr;
    struct emu_temp_var *var;
    addr = ARM_SP_VAL(emu) + emu->code.ctx.imm * 4;
    if ((var = emu_find_temp_var(emu, addr)))
        liveness_set(&emu->mblk, var->t, emu->code.ctx.lm);
    else if (IN_PROCESS_STACK_RANGE(addr)) {
        var = emu_alloc_temp_var(emu, addr);
        liveness_set(&emu->mblk, var->t, emu->code.ctx.lm);
    }
    else
        liveness_set(&emu->mblk, -1, emu->code.ctx.lm);
#endif

    return 0;
}

static int thumb_inst_mov(struct arm_emu *emu, struct minst *minst, uint16_t *inst, int inst_len)
{
    int imm = BITS_GET_SHL(inst[0], 10, 1, 11) + BITS_GET_SHL(inst[0], 0, 4, 12) + BITS_GET_SHL(inst[1], 12, 3, 8) + BITS_GET_SHL(inst[1], 0, 8, 0);
    int is_movt = 0;

    if ((inst[0] >> 7) & 1) {
        arm_prepare_dump(emu, "movt%s %s, #0x%x", cur_inst_it_cond(emu), regstr[emu->code.ctx.ld], imm);
        liveness_set(&emu->mblk, emu->code.ctx.ld, emu->code.ctx.ld);

        struct minst *cminst = minst_get_last_const_definition(&emu->mblk, minst, emu->code.ctx.ld);
        if (cminst)
            minst_set_const(minst, imm << 16 | (cminst->ld_imm & 0xffff));
    }
    else {
        arm_prepare_dump(emu, "mov%sw %s, #0x%x", cur_inst_it_cond(emu), regstr[emu->code.ctx.ld], imm);

        minst_set_const(minst, imm);
    }

    return 0;
}

static int t1_inst_mov_w(struct arm_emu *emu, struct minst *minst, uint16_t *inst, int inst_len)
{
    int imm = BITS_GET_SHL(inst[0], 10, 1, 11) + BITS_GET_SHL(inst[1], 12, 3, 8) + BITS_GET_SHL(inst[1], 0, 8, 0), imm1;

    arm_prepare_dump(emu, "mov%s.w %s, #0x%x", cur_inst_it_cond(emu), regstr[emu->code.ctx.ld], imm1 = ThumbExpandImmWithC(emu, imm));

    if (!minst->flag.in_it_block)
        minst_set_const(minst, imm1);

    return 0;
}

static int t1_inst_bx_0100(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    arm_prepare_dump(emu, "bx %s", regstr[emu->code.ctx.lm]);

    return 0;
}

static int t1_inst_blx_0100(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    int i;
    arm_prepare_dump(emu, "blx %s", regstr[emu->code.ctx.lm]);

    live_def_set(&emu->mblk, ARM_REG_R0);
    for (i = 0; i < 4; i++)
        live_use_set(&emu->mblk, ARM_REG_R0 + i);

    return 0;
}

static int t1_inst_b(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    arm_prepare_dump(emu, "b 0x%x", minst->host_addr = (ARM_PC_VAL(emu) + SignExtend(emu->code.ctx.imm, 11) * 2));

    if (IS_DISABLE_EMU(emu))
        return 0;

    minst->type = mtype_b;
    if (!minst->flag.b_need_fixed)
        minst->flag.b_need_fixed = 1;

    return 0;
}

static int thumb_inst_add_sp_imm(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    arm_prepare_dump(emu, "add%s sp, #0x%x", cur_inst_it_cond(emu), emu->code.ctx.imm * 4);

    liveness_set(&emu->mblk, ARM_REG_SP, ARM_REG_SP);

    return 0;
}

static int t_swi(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int thumb_inst_b(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    struct minst *t = NULL;
    struct bitset defs = { 0 };
    struct arm_cpsr apsr;
    char bincode[4];
    int binlen, index[4];

    int i;

    if (emu->code.ctx.cond == 0xe)
        ARM_UNPREDICT();
    else if (emu->code.ctx.cond == 0xf)
        return t_swi(emu, minst, code, len);

    minst->host_addr = (ARM_PC_VAL(emu) + SignExtend(emu->code.ctx.imm, 8) * 2);
    arm_prepare_dump(emu, "b%s 0x%x", condstr[emu->code.ctx.cond], minst->host_addr);

    if (InITBlock(emu))
        ARM_UNPREDICT();

    live_use_set(&emu->mblk, ARM_REG_APSR);

    minst->type = (emu->code.ctx.cond == ARM_COND_AL) ? mtype_b : mtype_bcond;
    minst->flag.b_cond = emu->code.ctx.cond;
    if (!minst->flag.b_need_fixed)
        minst->flag.b_need_fixed = 1;

    if (EMU_IS_CONST_MODE(emu)) {
        struct minst *cminst = NULL;
        struct minst *tminst;

        if (minst->flag.is_const || (cminst = minst_get_last_const_definition(&emu->mblk, minst, ARM_REG_APSR))->flag.is_const) {
            if (minst->flag.is_const)
                tminst = minst->flag.b_cond_passed ? minst_get_true_label(minst) : minst_get_false_label(minst);
            else {
                /* 要删除一个边，所以要反取不符合的 */
                minst_set_const(minst, cminst->ld_imm);
                tminst = (_ConditionPassed(&minst->apsr, emu->code.ctx.cond)) ? minst_get_false_label(minst) : minst_get_true_label(minst);
            }

            minst_succ_del(minst, tminst);
            minst_pred_del(tminst, minst);

            /* 删除一条边以后，bcond指令就要改成b指令了 */
            arm_asm2bin("b", bincode, &binlen);
            live_use_clear(&emu->mblk, ARM_REG_APSR);
            t = minst_change(minst, mtype_b, arm_insteng_parse(bincode, binlen, NULL),  bincode, binlen);
        }
    }
    else if (EMU_IS_TRACE_MODE(emu)) {
        if (minst_is_b(minst)) return 0;

        t = minst_get_trace_def(&emu->mblk, ARM_REG_APSR, index, 0);

        if (minst_is_tconst(t)) {
            minst_set_trace(minst, t->ld_imm);

            minst->flag.b_cond_passed = _ConditionPassed(&minst->apsr, emu->code.ctx.cond);

            if (t->type != mtype_cmp)
                vm_error("only support cmp instruction before branch");

            struct minst *lm_minst = minst_get_trace_def(&emu->mblk, t->cmp.lm, &index[1], index[0]);
            struct minst *ln_minst = minst_get_trace_def(&emu->mblk, t->cmp.ln, &index[2], index[0]);

            /* FIXME: 是否在常量传播中直接处理掉这种情况 ? */
            if (lm_minst->flag.is_const && ln_minst->flag.is_const)
                return 0;

            struct minst *def_reg = lm_minst->flag.is_const ? ln_minst : lm_minst;
            struct minst *def2;

            if (def_reg->type = mtype_mov_reg) {
                def2 = minst_get_trace_def(&emu->mblk, minst_get_use(def_reg), NULL, lm_minst->flag.is_const ? index[2]:index[1]);
                bitset_clone(&defs, &emu->mblk.defs[minst_get_use(def_reg)]);
                bitset_and(&defs, &def_reg->rd_in);
                int ok = MDO_TRACE_FLAT;

                bitset_foreach(&defs, i) {
                    struct minst *const_minst = emu->mblk.allinst.ptab[i];

                    if (!const_minst->flag.is_const || (const_minst == def2))
                        continue;

                    if (lm_minst->flag.is_const)
                        minst_cmp_calc(apsr, lm_minst->ld_imm, const_minst->ld_imm);
                    else
                        minst_cmp_calc(apsr, const_minst->ld_imm, ln_minst->ld_imm);

                    if (minst->flag.b_cond_passed == _ConditionPassed(&apsr, emu->code.ctx.cond)) {
                        ok = 0;
                        break;
                    }
                }

                bitset_uninit(&defs);
                return ok;
            }
        }
    }

    return 0;
}

static int thumb_inst_ldr_reg(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    return 0;
}

static int thumb_inst_blx(struct arm_emu *emu, struct minst *minst, uint16_t *code, int len)
{
    if (InITBlock(emu) && !LastInITBlock(emu)) ARM_UNPREDICT();

    arm_prepare_dump(emu, "blx");

    emu->meet_blx = 1;

    return 0;
}

/*

definition about inst reg rule
o[num]:      operate 
i[num]:      immediate
m[num]:      link register
rl[num]:     register list
lm[num]:     src register
hm[num]:     high register
*/
struct arm_inst_desc {
    const char *regexp;
    arm_inst_func    funclist[4];
    const char        *desc[4];
} desclist[]= {
    {"0000    o1    i5  lm3 ld3",           {t1_inst_lsl, t1_inst_lsr}, {"lsl", "lsr"}},
    {"0001    0     i5  lm3 ld3",           {t1_inst_asr}, {"asr"}},
    {"0001    10 o1 lm3 ln3 ld3",           {t1_inst_add, thumb_inst_sub_reg}, {"add", "sub"}},
    {"0001    11 o1 i3  ln3 ld3",           {t1_inst_add, t1_inst_sub}, {"add", "sub"}},
    {"0010    0 ld3    i8",                 {t1_inst_mov1}, {"mov"}},
    {"0010    1 lm3    i8",                 {t1_inst_cmp_imm}, {"cmp"}},
    {"0011    o1 ld3    i8",                {t1_inst_add, t1_inst_sub}, {"add", "sub"}},
    {"0100    0000 o2 lm3 ld3",             {t1_inst_and, t1_inst_eor, t1_inst_lsl, t1_inst_lsr}, {"and", "eor", "lsl2", "lsr2"}},
    {"0100    0001 o2 lm3 ld3",             {t1_inst_asr, t1_inst_adc, t1_inst_sbc, t1_inst_ror}, {"asr", "adc", "sbc", "ror"}},
    {"0100    0010 0o1 lm3 ld3",            {t1_inst_tst, t1_inst_neg}, {"tst", "neg"}},
    {"0100    0010 1o1 lm3 ln3",            {thumb_inst_cmp, t1_inst_cmn}, {"cmp", "cmn"}},
    {"0100    0011 o2 lm3 ld3",             {t1_inst_orr, t1_inst_mul, t1_inst_bic, t1_inst_mvn}, {"orr", "mul", "bic", "mvn"}},
    {"0100    0110 00 lm3 ld3",             {t1_inst_mov_0100}, {"mov"}},
    {"0100    01 o1 0 01 hm3 ld3",          {t1_inst_add, t1_inst_mov_0100}, {"add", "mov"}},
    {"0100    01 o1 0 10 lm3 hd3",          {t1_inst_add, t1_inst_mov_0100}, {"add", "mov"}},
    {"0100    01 o1 0 11 hm3 hd3",          {t1_inst_add, t1_inst_mov_0100}, {"add", "mov"}},
    {"0100    0101 01 lm3 hn3 ",            {thumb_inst_cmp}, {"cmp"}},
    {"0100    0101 10 hm3 ln3 ",            {thumb_inst_cmp}, {"cmp"}},
    {"0100    0101 11 hm3 hn3 ",            {thumb_inst_cmp}, {"cmp"}},
    {"0100    1 ld3 i8",                    {thumb_inst_ldr}, {"ldr"}},
    {"0100    0111 o1 lm4 000",             {t1_inst_bx_0100, t1_inst_blx_0100}, {"bx", "blx"}},
    {"0110    o1 i5 lm3 ld3",               {t1_inst_str_01101, thumb_inst_ldr},      {"str", "ldr"}},
    {"1001    0 lm3 i8",                    {t1_inst_str_10010}, {"str"}},
    {"1001    1 ld3 i8",                    {thumb_inst_ldr}, {"ldr"}},
    {"1010    o1 ld3 i8",                   {t1_inst_add1, t1_inst_add}, {"add", "add"}},
    {"1011    o1 10 m1 rl8",                {thumb_inst_push, thumb_inst_pop}, {"push", "pop"}},
    {"1011    0000 o1 i7",                  {thumb_inst_add_sp_imm, thumb_inst_sub}, {"add", "sub"}},
    {"1011    1111 c4 i4",                  {thumb_inst_it}, {"it"}},
    {"1101    c4 i8",                       {thumb_inst_b}, {"b<cond>"}},
    {"1110    0 i11",                       {t1_inst_b}, {"b"}},

    {"1110 1001 0010 1101 0 m1 0 rl13",     {thumb_inst_push}, "push.w"},
    {"1110 1000 1011 1101 i1 m1 0 rl13",    {thumb_inst_pop}, "pop.w"},

    {"1111 0i1 i10 11 i1 0 i1 i10 0",       {thumb_inst_blx}, "blx"},

    {"1111 0i1 00010 s1 11110 i3 ld4 i8",   {t1_inst_mov_w}, "mov.w"},
    {"1111 0i1 101100 i4 0 i3 ld4 i8",      {thumb_inst_mov}, "movt"},
    {"1111 0i1 100100 i4 0 i3 ld4 i8",      {thumb_inst_mov}, "movw"},
    {"1111 1000 u1 101 lm4 ld4 i12",        {thumb_inst_ldr}, {"ldr.w"}},
};

static int init_inst_map = 0;

struct reg_node {
    int id;
    struct reg_node *parent;
    struct reg_node *childs[3];

    arm_inst_func func;
    struct dynarray set;
    char *exp;
    char *desc;
};

struct reg_tree {
    int counts;
    struct dynarray arr;
    struct reg_node root;
};

struct arm_inst_engine {
    struct reg_tree    enfa;
    struct reg_tree    dfa;

    int width;
    int height;
    int *trans2d;
};

struct reg_node *reg_node_new(struct reg_tree *tree, struct reg_node *parent)
{
    struct reg_node *node = calloc(1, sizeof (node[0]));

    if (!node)
        vm_error("reg_node_new() failed calloc");

    node->parent = parent;
    node->id = ++tree->counts;
    dynarray_add(&tree->arr, node);

    return node;
}

void reg_node_copy(struct reg_node *dst, struct reg_node *src)
{
    dst->func = src->func;
    dst->exp = strdup(src->exp);
    dst->desc = strdup(src->desc);
}

int reg_node_height(struct reg_node *node)
{
    int i = 0;
    while (node->parent) {
        i++;
        node = node->parent;
    }

    return i;
}

void    reg_node_delete(struct reg_node *root)
{
    int i;
    if (!root)
        return;

    for (i = 0; i < count_of_array(root->childs); i++) {
        reg_node_delete(root->childs[i]);
    }

    if (root->exp)    free(root->exp);
    if (root->desc) free(root->desc);

    free(root);
}

void reg_node__dump_dot(FILE *fp, struct reg_tree *tree)
{
    struct reg_node *node, *root;
    int i, j;

    for (j = 0; j < tree->arr.len; j++) {
        root = (struct reg_node *)tree->arr.ptab[j];

        for (i = 0; i < count_of_array(root->childs); i++) {
            if (!root->childs[i])
                continue;

            fprintf(fp, "%d -> %d [label = \"%d\"]\n", root->id, root->childs[i]->id, i);
        }

        if (root->set.len) {
            fprintf(fp, "%d [label=\"%d (", root->id, root->id);
            for (i = 0; i < root->set.len; i++) {
                node = (struct reg_node *)root->set.ptab[i];
                fprintf(fp, "%d ", node->id);
            }
            fprintf(fp, ")\"]\n");
        }


        if (root->func) {
            fprintf(fp, "%d [shape=Msquare, label=\"%s\"];\n", root->id, root->desc);
        }
    }
}

void reg_node_dump_dot(const char *filename, struct reg_tree *tree)
{
    FILE *fp = fopen(filename, "w");

    fprintf(fp, "digraph G {\n");

    reg_node__dump_dot(fp, tree);

    fprintf(fp, "%d [shape=Mdiamond];\n", tree->root.id);

    fprintf(fp, "}\n");

    fclose(fp);
}

struct arm_inst_engine *inst_engine_create()
{
    struct arm_inst_engine *engine = NULL;

    engine = calloc(1, sizeof(engine[0]));
    if (!engine)
        vm_error("failed calloc()");

    return engine;
}

int arm_insteng_add_exp(struct arm_inst_engine *en, const char *exp, const char *desc, arm_inst_func func)
{
    struct reg_node *root = &en->enfa.root;
    int j, len = strlen(exp), rep, height, idx;
    const char *s = exp;

    for (; *s; s++) {
        if (isblank(*s))    continue;
        switch (*s) {
        case '0':
        case '1':
            idx = *s - '0';
            if (!root->childs[idx]) {
                root->childs[idx] = reg_node_new(&en->enfa, root);
            }

            root = root->childs[idx];
            break;

            /* setflags */
        case 's':
            /* immediate */
        case 'i':
            /* condition */
        case 'c':

            /* more register，一般是对寄存器列表的补充 */
        case 'm':
        case 'u':
loop_label:
            rep = atoi(&s[1]);
            if (!rep) 
                goto fail_label;

            for (j = 0; j < rep; j++) {
                if (!root->childs[2])
                    root->childs[2] = reg_node_new(&en->enfa, root);
                root = root->childs[2];
            }

            while (s[1] && isdigit(s[1])) s++;
            break;

        case 'r':
            if (s[1] != 'l')
                goto fail_label;

            s++;
            goto loop_label;

            /* register */
        case 'l':
            if (s[1] != 'm' && s[1] != 'd' && s[1] != 'n')
                goto fail_label;

            s++;
            goto loop_label;

        case 'h':
            if (s[1] != 'm' && s[1] != 'd' && s[1] != 'n')
                goto fail_label;

            s++;
            goto loop_label;

    fail_label:
        default:
            vm_error("inst expression [%s], position[%d:%s]\n", exp, (s - exp), s);
            break;
        }
    }

    root->exp = strdup(exp);
    root->func = func;
    root->desc = strdup(desc);

    // FIXME:测试完毕可以关闭掉，验证层数是否正确
    height = reg_node_height(root);
    if (height != 16 && height != 32) {
        vm_error("inst express error[%s], not size 16 or 32\n", exp);
    }

    return 0;
}

static struct arm_inst_engine *g_eng = NULL;

static struct reg_node *reg_node_find(struct reg_tree *tree, struct dynarray *arr)
{
    struct reg_node *node;
    int i;
    for (i = 0; i < tree->arr.len; i++) {
        node = (struct reg_node *)tree->arr.ptab[i];
        if (0 == dynarray_cmp(arr, &node->set))
            return node;
    }

    return NULL;
}

static int arm_insteng_gen_dfa(struct arm_inst_engine *eng)
{
    struct reg_node *nfa_root = &eng->enfa.root;

    struct reg_node *dfa_root = &eng->dfa.root;
    struct reg_node *node_stack[512], *droot, *nroot, *dnode1, *nnode;
    struct dynarray set = {0};
    int stack_top = -1, i, j, need_split;

#define istack_push(a)            (node_stack[++stack_top] = a)
#define istack_pop()            node_stack[stack_top--]        
#define istack_is_empty()        (stack_top == -1)

    dynarray_add(&dfa_root->set, nfa_root);
    istack_push(dfa_root);
    dynarray_reset(&set);

    while (!istack_is_empty()) {
        droot = istack_pop();

        for (i = need_split = 0; i < droot->set.len; i++) {
            nroot = (struct reg_node *)droot->set.ptab[i];
            if (nroot->childs[0] || nroot->childs[1]) {
                need_split = 1;
                break;
            }
        }

        for (i = need_split ? 0:2; i < (3 - need_split); i++) {
            dnode1 = NULL;

            for (j = 0; j < droot->set.len; j++) {
                if (!(nroot = (struct reg_node *)droot->set.ptab[j]))
                    continue;

                if (nroot->childs[i])    dynarray_add(&set, nroot->childs[i]);
                if ((i != 2) && nroot->childs[2])    dynarray_add(&set, nroot->childs[2]);
            }

            if (!dynarray_is_empty(&set)) {
                dnode1 = reg_node_find(&eng->dfa, &set);
                if (!dnode1) {
                    dnode1 = reg_node_new(&eng->dfa, droot);
                    dynarray_copy(&dnode1->set, &set);
                    istack_push(dnode1);
                }

                droot->childs[i] = dnode1;
            }

            /* 检查生成的DFA节点中包含的NFA节点，是否有多个终结态，有的话报错，没有的话把NFA的
            状态拷贝到DFA中去 */
            if (dnode1) {
                for (j = 0; j < dnode1->set.len; j++) {
                    if (!(nnode = (struct reg_node *)dnode1->set.ptab[j]) || !nnode->func)    continue;

                    if (dnode1->func) {
                        if (!strcmp(dnode1->desc, nnode->desc))
                            continue;

                        vm_error("conflict end state[%s] [%s]\n", dnode1->desc, nnode->desc);
                    }

                    reg_node_copy(dnode1, nnode);
                }
            }

            dynarray_reset(&set);
        }
    }

    return 0;
}

static int arm_insteng_gen_trans2d(struct arm_inst_engine *eng)
{
    int i;
    struct reg_tree *tree = &eng->dfa;
    struct reg_node *node;
    eng->width = eng->dfa.counts;
    eng->height = 2;

    eng->trans2d = calloc(1, eng->width * eng->height * sizeof (eng->trans2d[0]));
    if (!eng->trans2d)
        vm_error("trans2d calloc failure");

    for (i = 0; i < tree->arr.len; i++) {
        node = (struct reg_node *)tree->arr.ptab[i];

        if (node->childs[2]) {
            eng->trans2d[eng->width + node->id] = node->childs[2]->id;
            eng->trans2d[node->id] = node->childs[2]->id;
            continue;
        }

        if (node->childs[0]) {
            eng->trans2d[node->id] = node->childs[0]->id;
        }

        if (node->childs[1]) {
            eng->trans2d[eng->width + node->id] = node->childs[1]->id;
        }
    }

    return 0;
}

static int arm_insteng_init(struct arm_emu *emu)
{
    int i, j, k, m, n, len;
    const char *exp;
    char buf[128];

    if (g_eng)
        return 0;

    g_eng = calloc(1, sizeof (g_eng[0]));
    if (!g_eng)
        vm_error("arm_insteng_init() calloc failure");

    dynarray_add(&g_eng->enfa.arr, &g_eng->enfa.root);
    dynarray_add(&g_eng->dfa.arr, &g_eng->dfa.root);

    for (i = 0; i < count_of_array(desclist); i++) {
        exp = desclist[i].regexp;
        len = strlen(exp);
        for (j = k = 0; j < len; k++, j++) {
            if (exp[j] == 'o') {
                int bits = atoi(exp + j + 1);
                if (!bits)
                    vm_error("express error, unknown o token, %s\n", exp);

                while (isdigit(exp[++j]));

                int pow1 = (int)pow(2, bits);
                for (n = 0; n < pow1; n++) {
                    for (m = 0; m < bits; m++) {
                        buf[k + m] = !!(n & (1 <<  (bits - m - 1))) + '0';
                    }

                    strcpy(buf + k + bits, exp + j);
                    arm_insteng_add_exp(g_eng, buf, desclist[i].desc[n], desclist[i].funclist[n]);
                }
                break;
            }
            else {
                buf[k] = exp[j];
            }
        }
        buf[k] = 0;

        if (j == len)
            arm_insteng_add_exp(g_eng, buf, desclist[i].desc[0], desclist[i].funclist[0]);
    }

    if (emu->dump.nfa)
        reg_node_dump_dot("nfa.dot", &g_eng->enfa);

    arm_insteng_gen_dfa(g_eng);

    if (emu->dump.dfa)
        reg_node_dump_dot("dfa.dot", &g_eng->dfa);

    arm_insteng_gen_trans2d(g_eng);

    init_inst_map = 1;

    return 0;
}

static int arm_insteng_uninit()
{
}

static void arm_inst_ctx_init(struct arm_inst_ctx *ctx)
{
    memset(ctx, 0, sizeof (ctx[0]));
    ctx->ld = -1;
    ctx->lm = -1;
    ctx->ln = -1;
}

static void arm_liveness_init(struct arm_emu *emu, struct minst *minst, struct arm_inst_ctx *ctx)
{
    if (ctx->ld >= 0) {
        live_def_set(&emu->mblk, ctx->ld);
    }

    if (ctx->lm >= 0) {
        live_use_set(&emu->mblk, ctx->lm);
    }

    if (ctx->ln >= 0) {
        live_use_set(&emu->mblk, ctx->ln);
    }

    if (InITBlock(emu)) {
        live_use_set(&emu->mblk, ARM_REG_APSR);
    }
}

static struct reg_node*     arm_insteng_parse(uint8_t *code, int len, int *olen)
{
    struct reg_node *node = NULL, *p_end_node = NULL;
    int i, j, from = 0, to, bit;

    /* arm 解码时，假如第一个16bit没有解码到对应的thumb指令终结态，则认为是thumb32，开始进入
    第2轮的解码 */

    /* 搜索指令时采取最长匹配原则 */
    //printf("start state:%d\n", from);
    for (i = from = 0; i < 2; i++) {
        uint16_t inst = ((uint16_t *)code)[i];
        for (j = 0; j < 16; j++) {
            bit = !!(inst & (1 << (15 - j)));
            to = g_eng->trans2d[g_eng->width * bit + from];

            if (!to)
                goto exit;

            node = ((struct reg_node *)g_eng->dfa.arr.ptab[to]);
            from = node->id;

            //printf("%d ", from);
        }

        if (node->func)
            p_end_node = node;
    }
    exit:
    //printf("\n");

    node = p_end_node;

    if (!node || !node->func) {
        vm_error("arm_insteng_decode() meet unkown instruction, code[%02x %02x]", code[0], code[1]);
    }

    if (olen)
        *olen = i;

    return node;
}

/*

@return     0       normal success
            1       cant trace
*/
static int arm_minst_do(struct arm_emu *emu, struct minst *minst)
{
    int ret;
    struct reg_node *reg_node = minst->reg_node;

    if (minst->flag.prologue || minst->flag.epilogue)
        return 0;

    emu->regs[ARM_REG_PC] = emu->baseaddr + (minst->addr - emu->code.data) + 4;

    memcpy(emu->prev_regs, emu->regs, sizeof (emu->regs));

    arm_inst_extract_ctx(&emu->code.ctx, reg_node, minst->addr, minst->len);

    arm_liveness_init(emu, minst, &emu->code.ctx);

    /* 不要挪动位置，假如你把他移动到 reg_node->func 后面，会导致 it 也被判断为在 it_block 中 */
    if (InITBlock(emu)) {
        minst->flag.in_it_block = 1;
        minst->flag.is_t = cur_inst_it(emu);
    }

    ret = reg_node->func(emu, minst, (uint16_t *)minst->addr, minst->len / 2);

    if (emu->it.inblock> 0)
        emu->it.inblock--;

    return ret;
}

static int arm_insteng_decode(struct arm_emu *emu, uint8_t *code, int len)
{
    int i;
    struct minst *minst;

    if (!g_eng)     arm_insteng_init(emu);

    struct reg_node *node = arm_insteng_parse(code, len, &i);
    minst = minst_new(&emu->mblk, code, i * 2, node);

    arm_minst_do(emu, minst);

    if (!emu->prev_minst || !minst_is_b(emu->prev_minst)) {
        minst_succ_add(emu->prev_minst, minst);
        minst_pred_add(minst, emu->prev_minst);
    }

    emu->prev_minst = minst;

    return i * 2;
}

int arm_inst_extract_ctx(struct arm_inst_ctx *ctx, struct reg_node *reg_node, uint8_t *code, int code_len)
{
    int i, len, c;
    char *exp = reg_node->exp;

    arm_inst_ctx_init(ctx);

    uint16_t inst = *(uint16_t *)code;

    i = 0;
    while(*exp) {
        switch ((c = *exp)) {
        case '0': case '1':
            exp++; i++; 
            break;
        case ' ':
            exp++;
            break;

        case 's':
        case 'i':
        case 'c':
        case 'u':
            len = atoi(++exp);

            if (c == 's')
                ctx->setflags = BITS_GET(inst, 16 - i - len, len);
            else if (c == 'i')
                ctx->imm = BITS_GET(inst, 16 -i - len, len);
            else if (c == 'u')
                ctx->u = BITS_GET(inst, 16 -i - len, len);
            else
                ctx->cond = BITS_GET(inst, 16 -i - len, len);
            while (isdigit(*exp)) exp++;
            i += len;
            break;


        case 'm':
            len = 1;
            ctx->m = BITS_GET(inst, 16 - i - len, len);
            exp += 2; i += len;
            break;

        case 'h':
        case 'l':
            exp++;
            len = atoi(exp + 1);
            switch (*exp++) {
            case 'n':
                ctx->ln = BITS_GET(inst, 16 - i - len, len) + (c == 'h') * 8;
                break;

            case 'm':
                ctx->lm = BITS_GET(inst, 16 - i - len, len) + (c == 'h') * 8;
                break;

            case 'd':
                ctx->ld = BITS_GET(inst, 16 - i - len, len) + (c == 'h') * 8;
                break;

            default:
                goto fail_label;
            }
            i += len;

            while (isdigit(*exp)) exp++;
            break;

        case 'r':
            exp++;
            if (*exp == 'l') {
                len = atoi(++exp);
                ctx->register_list |= BITS_GET(inst, 16 - i - len, len);
            }
            else
                goto fail_label;
            while (isdigit(*exp)) exp++;
            i += len;
            break;

        fail_label:
        default:
            vm_error("inst exp [%s], un-expect token[%s]\n", reg_node->exp, exp);
            break;
        }

        if ((i == 16) && (code_len == 4)) {
            i = 0;
            inst = *(uint16_t *)(code + 2);
        }
    }

    return 0;
}

static int arm_emu_cpu_reset(struct arm_emu *emu)
{
    memset(emu->regs, 0 , sizeof (emu->regs));
    memset(emu->prev_regs, 0, sizeof (emu->prev_regs));

    emu->regs[ARM_REG_SP] = PROCESS_STACK_BASE;
    emu->code.pos = 0;

    emu->known = 0;

    memset(&emu->it, 0, sizeof (emu->it));

    REG_SET_KNOWN(emu, ARM_REG_SP);
    REG_SET_KNOWN(emu, ARM_REG_PC);

    EMU_SET_SEQ_MODE(emu);

    return 0;
}

struct arm_emu   *arm_emu_create(struct arm_emu_create_param *param)
{
    struct arm_emu *emu;

    emu = calloc(1, sizeof (emu[0]));
    if (!emu)
        vm_error("arm_emu_create() failed with calloc");
    
    emu->code.data = param->code;
    emu->code.len = param->code_len;
    emu->baseaddr = param->baseaddr;
    emu->dump.cfg = 1;
    emu->dump.nfa = 1;
    emu->dump.dfa = 1;

    minst_blk_init(&emu->mblk, NULL, arm_minst_do, emu);

    emu->stack.size = PROCESS_STACK_SIZE;
    emu->stack.data = calloc(1, emu->stack.size);
    if (NULL == emu->stack.data)
        vm_error("arm alloc stack size[%d] failure", emu->stack.size);

    emu->stack.base = emu->stack.data + emu->stack.size;

    arm_emu_cpu_reset(emu);

    return emu;
}

void        arm_emu_destroy(struct arm_emu *e)
{
    if (e->stack.data)
        free(e->stack.data);

    minst_blk_uninit(&e->mblk);
    free(e);
}

static int arm_emu_dump_mblk(struct arm_emu *emu)
{
    struct minst *minst;
    int i;
    char buf[4096];

    arm_emu_cpu_reset(emu);
    emu->decode_inst_flag = FLAG_DISABLE_EMU;
    for (i = 0; i < emu->mblk.allinst.len; i++) {
        minst = emu->mblk.allinst.ptab[i];

        arm_minst_do(emu, minst);

        arm_inst_print_format(emu, minst, -1, buf);

        printf("%s\n", buf);
    }

    return 0;
}

#define CFG_NODE_ID(addr)       (emu->baseaddr + (addr - emu->code.data))

static void arm_emu_dump_cfg(struct arm_emu *emu)
{
    int i;
    struct minst *succ;
    struct minst_node *tnode;
    struct minst_blk *blk = &emu->mblk;
    struct minst_cfg *cfg;
    char obuf[512];

    FILE *fp = fopen("cfg.dot", "w");

    fprintf(fp, "digraph G {");
    fprintf(fp, "node [fontname = \"helvetica\"]\n");

    arm_emu_cpu_reset(emu);

    for (i = 0; i < blk->allcfg.len; i++) {
        cfg = blk->allcfg.ptab[i];
        if (cfg->flag.dead_code) continue;

        fprintf(fp, "sub_%x [shape=MSquare, label=<<font color='red'><b>sub_%x(%d, %d)</b></font><br/>", 
            CFG_NODE_ID(cfg->start->addr), CFG_NODE_ID(cfg->start->addr), cfg->id, cfg->csm);
        for (succ = cfg->start; succ; succ = succ->succs.minst) {
            arm_minst_do(emu, succ);

            arm_inst_print_format(emu, succ, IDUMP_STATUS, obuf);

            fprintf(fp, "%s<br/>", obuf);

            if (succ == cfg->end) break;
        }
        fprintf(fp, ">]\n");

        for (tnode = &cfg->end->succs; tnode; tnode = tnode->next) {
            fprintf(fp, "sub_%x -> sub_%x\n", CFG_NODE_ID(cfg->start->addr), CFG_NODE_ID(tnode->minst->addr));
        }
    }
    fprintf(fp, "}");

    fclose(fp);
}

static int arm_emu_mblk_fix_pos(struct arm_emu *emu)
{
    int i, same = 0;
    struct minst *minst, *b_minst, *succ;
    struct minst_node *succ_node;

    for (i = 0; i < emu->mblk.allinst.len; i++) {
        minst = emu->mblk.allinst.ptab[i];
        if (minst_is_b0(minst) && minst->flag.b_need_fixed) {
            unsigned long target_addr = emu_addr_host2target(emu, minst->host_addr);

            b_minst = minst_blk_find(&emu->mblk, target_addr);
            if (!b_minst)
                vm_error("arm emu host_addr[0x%x] target_addr[0x%x] not found", minst->host_addr, target_addr);

            minst_succ_add(minst, b_minst);
            minst_pred_add(b_minst, minst);

            minst->flag.b_need_fixed = 2;
        }

        if (minst->flag.in_it_block) {
            for (same = 1, succ = minst->succs.minst; succ && succ->flag.in_it_block; succ = succ->succs.minst) {
                if (minst->flag.is_t != succ->flag.is_t) same = 0;
            }

            if (same) {
                minst_succ_add(minst->preds.minst, succ);
                minst_pred_add(succ, minst->preds.minst);
                for (; ((i+1) < emu->mblk.allinst.len) && ((struct minst *)emu->mblk.allinst.ptab[i+1])->flag.in_it_block; i++);
            }
            else {
                for (succ_node = &minst->succs; succ_node; succ_node = succ_node->next) {
                    minst_succ_add(minst->preds.minst, succ_node->minst);
                    minst_pred_add(succ_node->minst, minst->preds.minst);

                    if (!succ_node->minst->flag.in_it_block)
                        break;
                }
            }

        }
    }

    minst_blk_gen_cfg(&emu->mblk);

    return 0;
}

int         minst_blk_const_propagation(struct arm_emu *emu);

static int  arm_emu_dump_defs1(struct arm_emu *emu, int inst_id, int reg_def)
{
    struct minst *minst = emu->mblk.allinst.ptab[inst_id], *def_minst;
    BITSET_INIT(defs);
    int pos;

    bitset_clone(&defs, &emu->mblk.defs[reg_def]);
    bitset_and(&defs, &minst->rd_in);

    printf("%s def list\n", regstr[reg_def]);
    for (pos = bitset_next_bit_pos(&defs, 0); pos >= 0; pos = bitset_next_bit_pos(&defs, pos + 1)) {
        def_minst = emu->mblk.allinst.ptab[pos];
        if (def_minst->flag.is_const)
            printf("%d const=0x%x\n", pos, def_minst->ld_imm);
        else
            printf("%d unknown\n", pos);
    }
    printf("\n");

    bitset_uninit(&defs);
    return 0;
}

int         arm_emu_trace_flat(struct arm_emu *emu);

int         arm_emu_run(struct arm_emu *emu)
{
    int ret;

    emu->meet_blx = 0;
    /* first pass */
    arm_emu_cpu_reset(emu);
    minst_blk_live_prologue_add(&emu->mblk);
    for (emu->code.pos = 0; emu->code.pos < emu->code.len; ) {
        if (emu->meet_blx)
            break;

        ret = arm_insteng_decode(emu, emu->code.data + emu->code.pos, emu->code.len - emu->code.pos);
        if (ret < 0) {
            return -1;
        }
        emu->code.pos += ret;
    }
    minst_blk_live_epilogue_add(&emu->mblk);
    /* second pass */
    arm_emu_mblk_fix_pos(emu);

    /* third pass */
    minst_blk_liveness_calc(&emu->mblk);

    minst_blk_gen_reaching_definitions(&emu->mblk);

    minst_blk_const_propagation(emu);

    //minst_cfg_classify(&emu->mblk);
    arm_emu_trace_flat(emu);

#if 0
    //minst_blk_out_of_order(&emu->mblk);

    minst_blk_copy_propagation(&emu->mblk);

    minst_blk_dead_code_elim(&emu->mblk);

    minst_blk_const_propagation(emu);
#endif

    arm_emu_dump_mblk(emu);

    //arm_emu_dump_defs1(emu, 44, ARM_REG_R5);
    //arm_emu_dump_defs1(emu, 44, ARM_REG_R6);

    if (emu->dump.cfg)
        arm_emu_dump_cfg(emu);

    return 0;
}

int         arm_emu_run_once(struct arm_emu *vm, unsigned char *code, int code_len)
{
    return 0;
}

void        arm_emu_dump(struct arm_emu *emu)
{
}

#define F_EMU_SEQ_ANALY             0x01
#define F_EMU_CONST                 0x02
#define F_TRACE                     0x04

int         arm_emu_trace_flat(struct arm_emu *emu)
{
    struct minst_blk *blk = &emu->mblk;
    struct minst *minst, *t, *n;
    struct minst_cfg *cfg = blk->allcfg.ptab[0], *last_cfg, *state_cfg;
    char bincode[8];
    int ret, i, trace_start, binlen, state_reg;
    BITSET_INIT(defs);
    BITSET_INITS(cfg_visitall, blk->allcfg.len);
    BITSET_INITS(visitall, blk->allinst.len);

#define MTRACE_PUSH_CFG(_cfg) do { \
        struct minst *start = (_cfg)->start; \
        for (; start != _cfg->end; start = start->succs.minst) \
            MSTACK_PUSH(blk->trace, start); \
        MSTACK_PUSH(blk->trace, start); \
        bitset_set(&cfg_visitall, _cfg->id, 1); \
    } while (0)

#define MTRACE_POP_CFG() do { \
        struct minst *end = MSTACK_TOP(blk->trace); \
        struct minst_cfg *end_cfg = end->cfg; \
        while (end != end_cfg->start) \
            MSTACK_POP(blk->trace); \
        MSTACK_POP(blk->trace); \
    } while (0)

    int changed = 1;

    minst_cfg_classify(blk);

    while (changed) {
        changed = 0;
        for (i = 0; i < blk->allcfg.len; i++) {
            state_cfg = blk->allcfg.ptab[i];
            if (minst_cfg_is_const_state_machine(state_cfg, &state_reg)) {
                changed = 1;
                break;
            }
        }

        if (!changed) continue;

        cfg = blk->allcfg.ptab[0];
        bitset_expand(&cfg_visitall, blk->allcfg.len);
        MTRACE_PUSH_CFG(cfg);

        if (MSTACK_IS_EMPTY(blk->trace))
            vm_error("not found start path to const state machine cfg node");

        EMU_SET_TRACE_MODE(emu);
        printf("start trace\n");
        while (!MSTACK_IS_EMPTY(blk->trace)) {
            minst = MSTACK_TOP(blk->trace);

            if (minst->cfg->csm  == CSM_OUT)
                ret = MDO_TRACE_FLAT_OUT_CSM;
            else
                ret = arm_minst_do(emu, minst);

            printf("minst= %d \n", minst->id);

            switch (ret) {
            case MDO_SUCCESS:
                if (minst_succs_count(minst) == 2) {
                    if (minst_is_tconst(minst)) {
                        if (minst->flag.b_cond_passed) 
                            MSTACK_PUSH(blk->trace, minst_get_true_label(minst));
                        else 
                            MSTACK_PUSH(blk->trace, minst_get_false_label(minst));
                    }
                    else {
                        if (minst_get_false_label(minst)->cfg->csm != CSM_OUT)
                            MSTACK_PUSH(blk->trace, minst_get_false_label(minst));
                        else
                            MSTACK_PUSH(blk->trace, minst_get_true_label(minst));
                    }
                }
                else 
                    MSTACK_PUSH(blk->trace, minst_get_false_label(minst));
                break;

            case MDO_CANT_TRACE:
                vm_error("prog cant deobfuse");
                break;

            case MDO_TRACE_FLAT:
            case MDO_TRACE_FLAT_OUT_CSM:
                bitset_clear(&visitall);
                /* 开始对trace进行平坦化，第一步先申请 cfg 节点 */
                last_cfg = ((struct minst *)MSTACK_TOP(blk->trace))->cfg;

                while (MSTACK_TOP(blk->trace) != last_cfg->start) MSTACK_POP(blk->trace);
                MSTACK_POP(blk->trace);


                /* 因为我们是trace到最后一个唯一状态节点中，所以
                1. 删除最后b指令到唯一状态分支的连接
                2. FIXME:增加被删除b指令到唯一状态分支的真分支的边
                */
                if (ret == MDO_TRACE_FLAT) {
                    t = MSTACK_TOP(blk->trace);
                    n = (minst_is_bcond(t) && t->flag.b_cond_passed) ? minst_get_true_label(t):minst_get_false_label(t);
                    minst_del_edge(t, n);
                    minst_add_edge(t, minst_get_true_label(last_cfg->end));

                    minst_get_trace_def(blk, state_reg, &i, 0);
                }
                else {
                    for (i = blk->trace_top; i >= 0; i--) {
                        t = blk->trace[i];
                        if (t->cfg->csm == CSM_IN)
                            break;
                    }
                }

                for (i++; i < blk->trace_top; i++) {
                    t = blk->trace[i];
                    /* 路径节点有多个前驱节点时，把trace流上的节点从前驱节点的后继中删除 */
                    if (minst_preds_count(t) > 1) {
                        minst_del_edge(blk->trace[i - 1], blk->trace[i]);
                        break;
                    }
                }

                EMU_SET_TRACE_MODE(emu);
                cfg = minst_cfg_new(&emu->mblk, NULL, NULL);
                /* 然后复制从开始trace的地方，到当前的所有指令，所有的jmp指令都要抛弃 */
                for (trace_start = i; i <= blk->trace_top; i++) {
                    t = blk->trace[i];

                    if (minst_is_b0(t)) continue;

                    n = minst_new_copy(cfg, blk->trace[i]);

                    arm_minst_do(emu, n);

                    if (!cfg->start)    cfg->start = n;
                }
                /* cfg末尾需要添加一条实际的jmp指令 */
                arm_asm2bin("b", bincode, &binlen);
                t = minst_new_t(cfg, mtype_b, arm_insteng_parse(bincode, binlen, NULL),  bincode, binlen);
                cfg->end = t;

                /* 
                1. 删除start节点到状态机节点连接
                2. 连接start节点到trace节点
                3. 连接新的cfg到唯一状态节点中
                */
                minst_add_edge(blk->trace[trace_start - 1], cfg->start);
                minst_add_edge(cfg->end, last_cfg->start);

                /* 恢复被trace过的指令内容 */
                for (i = trace_start; i <= blk->trace_top; i++) {
                    minst_restore(blk->trace[i]);
                }

                /* 调整trace const的指令为const指令*/ 
                for (t = cfg->start; t != cfg->end; t = t->succs.minst) {
                    t->flag.is_trace = 0;
                    t->flag.is_const = 1;
                }
                /* 弹出所有trace指令 */
                while (!MSTACK_IS_EMPTY(blk->trace)) 
                    MSTACK_POP(blk->trace);

                /* FIXME:这里其实应该是恢复以前的模式 */
                minst_blk_const_propagation(emu);
                minst_cfg_classify(blk);
                static int const_time = 0;
                if (++const_time == 4)
                    return 0;
                break;
            }

            if (ret == MDO_TRACE_FLAT)
                break;
        }
    }

    return 0;
}

/* FIXME: 后面需要改成半符号执行的方式来应对各种情况 */
static int arm_emu_bcond_symbo_exec(struct arm_emu *emu, struct minst_cfg *cfg, struct minst *def)
{
    struct minst_blk *blk = cfg->blk;
    struct minst *end = cfg->end, *pred;
    struct arm_cpsr apsr = { 0 };

    for (pred = end->preds.minst; pred; pred = pred->preds.minst) {
        if (pred->type == mtype_cmp)
            break;

        if (minst_preds_count(pred) != 1)
            return -1;
    }
    
    if (pred->type != mtype_cmp)
        return -1;

    struct minst *lm_minst = minst_get_last_const_definition(blk, pred, pred->cmp.lm);
    struct minst *ln_minst = minst_get_last_const_definition(blk, pred, pred->cmp.ln);

    if ((lm_minst && ln_minst) || (!lm_minst && !lm_minst))
        return -1;

    if (ln_minst)
        minst_cmp_calc(apsr, ln_minst->ld_imm, def->ld_imm);
    else
        minst_cmp_calc(apsr, def->ld_imm, lm_minst->ld_imm);

    return _ConditionPassed(&apsr, cfg->end->flag.b_cond);
}

int         minst_blk_const_propagation(struct arm_emu *emu)
{
    struct bitset *uses;
    struct minst *minst, *use_minst, *def_minst;
    struct minst_blk *blk = &emu->mblk;
    struct minst_cfg *cfg;
    BITSET_INIT(defs);
    int i, changed = 1, pos, use_reg, ret, pret;

    EMU_SET_CONST_MODE(emu);

    for (i = 0; i < blk->allinst.len; i++) {
        minst = blk->allinst.ptab[i];
        if (minst->flag.is_const)
            dynarray_add(&blk->const_insts, minst);
    }

    /* MCIC P446 */
    while (changed) {
        changed = 0;

        minst_blk_liveness_calc(&emu->mblk);
        minst_blk_gen_reaching_definitions(&emu->mblk);

        /* copy progagation */

        /* constant folding */
        for (i = 0; i < blk->const_insts.len; i++) {
            minst = blk->const_insts.ptab[i];
            if (minst->flag.dead_code) continue;
            if (minst_get_def(minst) < 0) continue;

            /* 遍历所有使用minst定值的 use 列表*/
            uses = &blk->uses[minst_get_def(minst)];
            for (pos = bitset_next_bit_pos(uses, 0); pos > 0; pos = bitset_next_bit_pos(uses, pos + 1)) {
                use_minst = blk->allinst.ptab[pos];
                if (use_minst->flag.dead_code) continue;
                if (use_minst->flag.is_const) continue;

                /* 
                1. 查看使用列表中的指令的 reaching definitions in 集合中是否有这条指令，
                假如有的话，确认in集合中的def指令时唯一一条对 minst_get_def(minst) 中进行
                定值的指令.
                2. 假如多个定值都一样时
                 */
                if (minst_get_last_const_definition(blk, use_minst, minst_get_def(minst))) {
                    arm_minst_do(emu, use_minst);

                    if (use_minst->flag.is_const) {
                        dynarray_add(&blk->const_insts, use_minst);
                        changed = 1;
                    }
                }
            }
        }

        /* constant conditions */
        for (i = 0; i < blk->allcfg.len; i++) {
            cfg = blk->allcfg.ptab[i];

            if (minst_succs_count(cfg->end) <= 1) continue;

            minst = minst_cfg_apsr_get_overdefine_reg(cfg, &use_reg);
            if (!minst) continue;

            bitset_clone(&defs, &blk->defs[use_reg]);
            bitset_and(&defs, &minst->rd_in);
            pret = -1;
            bitset_foreach(&defs, pos) {
                def_minst = blk->allinst.ptab[pos];
                if (!def_minst->flag.is_const) continue;

                ret = arm_emu_bcond_symbo_exec(emu, cfg, def_minst);
                if (ret == -1) break;
                if (pret == -1) pret = ret;
                else if (pret != ret) break;
            }

            if ((pret == ret) && (ret != -1)) {
                cfg->end->flag.is_const = 1;
                cfg->end->flag.b_cond_passed = ret;
                arm_minst_do(emu, cfg->end);
                printf("delete cfg [%d:%d-%d]", cfg->id, cfg->start->id, cfg->end->id);
                changed = 1;
            }
        }

        /* delete unreachable code 
        除了起始cfg节点，其余前驱节点为0的cfg都是不可达的
        */
        for (i = 1; i < blk->allcfg.len; i++) {
            cfg = blk->allcfg.ptab[i];
            if (cfg->flag.dead_code) continue;
            if (minst_preds_count(cfg->start) > 0) continue;

            minst_blk_del_unreachable(blk, cfg);
            changed = 1;
        }
    }

    return 0;
}

char *arm_asm2bin(const char *asm, char *bin, int *len)
{
    static char bin2[8];
    short t1;
    int c;

    if (!bin)
        bin = bin2;

    c = tolower(*asm);
    switch (c) {
    case 'b':
        t1 = 0xe000;
        break;

    default:
        break;
    }

    memcpy(bin, &t1, 2);

    if (len)
        *len = 2;

    return bin;
}
