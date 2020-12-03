

#include "sleigh_arch.hh"
#include "sleigh.hh"
#include "funcdata.hh"
#include "flow.hh"
#include "dobc.hh"
#include <iostream>
#include <assert.h>

#define strdup _strdup

#define GEN_SH          "#!/bin/bash\n"  \
    "for filename in `find . -type f -name \"*.dot\" | xargs`\n"  \
    "do\n" \
    "   echo `date +\"%T.%3N\"` gen $filename png \n" \
    "   dot -Tsvg -o ${filename%.*}.svg $filename\n" \
    "done\n" 

#define ENABLE_DUMP_INST                1
#define ENABLE_DUMP_PCODE               0

static char help[] = {
    "dobc [.sla filename] [filename]"
};

static dobc *g_dobc = NULL;

#define COLOR_ASM_INST_MNEM             "#3933ff"               
#define COLOR_ASM_INST_BODY             "#3933ff"               
#define COLOR_ASM_ADDR                  "#33A2FF"               
#define COLOR_ASM_STACK_DEPTH           "green"

class AssemblyRaw : public AssemblyEmit {

public:
    char *buf = NULL;
    FILE *fp = NULL;
    int sp = 0;

    virtual void dump(const Address &addr, const string &mnem, const string &body) {
        if (!fp) fp = stdout;

        if (buf) {
            sprintf(buf, "<tr>"
                "<td><font color=\"" COLOR_ASM_STACK_DEPTH "\">%03x:</font></td>"
                "<td><font color=\"" COLOR_ASM_ADDR "\">0x%04x:</font></td>"
                "<td align=\"left\"><font color=\"" COLOR_ASM_INST_MNEM "\">%s </font></td>"
                "<td align=\"left\"><font color=\"" COLOR_ASM_INST_BODY "\">%s</font></td></tr>",
                sp, (int)addr.getOffset(), mnem.c_str(), body.c_str());
            //sprintf(buf, "0x%08x:%10s %s", (int)addr.getOffset(), mnem.c_str(), body.c_str());
        }
        else {
            fprintf(fp, "0x%04x: %s %s\n", (int)addr.getOffset(), mnem.c_str(), body.c_str());
        }
    }

    void set_buf(char *b) { buf = b; }
    void set_fp(FILE *f) { fp = f; }
    void set_sp(int s) { sp = s;  };
};

static void print_vardata(Translate *trans, FILE *fp, VarnodeData &data)
{
    string name = trans->getRegisterName(data.space, data.offset, data.size);

    if (name == "")
        fprintf(fp, "(%s,0x%llx,%d)", data.space->getName().c_str(), data.offset, data.size);
    else
        fprintf(fp, "(%s,%s,%d)", data.space->getName().c_str(), name.c_str(), data.size);
}

static int print_vartype(Translate *trans, char *buf, varnode *data)
{
    if (!data) {
        return sprintf(buf, " ");
    }

    if (data->type.height == a_constant) {
        if (data->size == 8)
            return sprintf(buf, "%llx", data->type.v);
        else 
            return sprintf(buf, "%x", (u4)data->type.v);
    }
    else if (data->type.height == a_rel_constant) {
        Address &addr = data->get_rel();
        string name = trans->getRegisterName(addr.getSpace(), addr.getOffset(), data->size);
        name = g_dobc->get_abbrev(name);

        return sprintf(buf, "%c%s%c%llx", addr.getShortcut(), name.c_str(), data->type.v > 0 ? '+':'-', abs(data->type.v));
    }
    else 
        return sprintf(buf, "T");
}

static int print_udchain(char *buf, pcodeop *op, uint32_t flags)
{
    varnode *out = op->output;
    int i = 0, j, defs = 0, uses_limit = 10000, defs_limit = 10000;

    if (flags & PCODE_OMIT_MORE_USE)
        uses_limit = 7;

    if (out && out->uses.size()) {
        list<pcodeop *>::iterator iter = out->uses.begin();
        i += sprintf(buf + i, " [u:");
        for (j = 0; iter != out->uses.end(); iter++, j++) {
            /* 最多打印limit个use */
            if (j == uses_limit)
                break;

            i += sprintf(buf + i, "%d ", (*iter)->start.getTime());
        }
        if (iter != out->uses.end())
        /* 遇见没打印完的use，以省略号代替，*/
            i += sprintf(buf + i, "...]");
        else {
            /* 删除末尾多余的空格 */
            if (j > 0) i--;

            i += sprintf(buf + i, "]");
        }
    }

    for (j = 0; j < op->inrefs.size(); j++) {
        if (op->inrefs[j]->def)
            defs++;
    }

    if (defs) {
        if (flags & PCODE_OMIT_MORE_DEF)
            defs_limit = 7;

        i += sprintf(buf + i, " [d:");
        for (j = 0; j < op->inrefs.size(); j++) {
            if (j == defs_limit) break;

            if (op->inrefs[j]->def)
                i += sprintf(buf + i, "%d ", op->inrefs[j]->def->start.getTime());
        }

        if (j == defs_limit)
            i += sprintf(buf + i, "...]");
        else {
            if (j > 0) i--;
            i += sprintf(buf + i, "]");
        }
    }

    return i;
}

static int print_varnode(Translate *trans, char *buf, varnode *data)
{
    Address addr = data->get_addr();
    string name = trans->getRegisterName(addr.getSpace(), addr.getOffset(), data->size);

    name = g_dobc->get_abbrev(name);

    if ((addr.getSpace()->getType() == IPTR_CONSTANT) && ((intb)addr.getOffset() == (intb)trans->getDefaultCodeSpace()))
        return sprintf(buf, "ram");
    else if (name == "") {
        if (addr.getSpace()->getIndex() == IPTR_CONSTANT)
            return sprintf(buf, "(%c%llx:%d)", addr.getSpace()->getShortcut(), addr.getOffset(), data->size);
        else
            return sprintf(buf, "(%c%llx:%d)", addr.getSpace()->getShortcut(), addr.getOffset(), data->size);
    }
    else
        return sprintf(buf, "(%c%s.%d:%d)", addr.getSpace()->getShortcut(), name.c_str(), data->version, data->size);
}

void pcodeemit2::dump(const Address &addr, OpCode opc, VarnodeData *outvar, VarnodeData *vars, int4 isize)
{
    int i;
    pcodeop *op;
    varnode *vn;

    if (outvar != (VarnodeData *)0) {
        Address oaddr(outvar->space, outvar->offset);
        op = fd->newop(isize, addr);
        fd->new_varnode_out(outvar->size, oaddr, op);
    }
    else
        op = fd->newop(isize, addr);

    fd->op_set_opcode(op, opc);

    i = 0;
    /* CPUI_CBRANCH的第一个节点，指明当条件为真时的跳转地址，不清楚为什么在IPTR_PROCESSOR内
    而不是在 IPTR_CONSTANT 内 ，这里直接改掉 */
#if 0
    if (opc == CPUI_CBRANCH) {
        Address addrcode(fd->d->trans->getConstantSpace(), vars[0].offset);

        fd->op_set_input(op, fd->new_coderef(addrcode), 0);

        i++;
    }
#endif

    for (; i < isize; i++) {
        vn = fd->new_varnode(vars[i].size, vars[i].space, vars[i].offset);
        fd->op_set_input(op, vn, i);
    }

#if ENABLE_DUMP_PCODE
    fprintf(fp, "    ");

    if (outvar) {
        print_vardata(fd->d->trans, fp, *outvar); fprintf(fp, " = ");
    }

    fprintf(fp, "%s", get_opname(opc));
    // Possibly check for a code reference or a space reference
    for (i = 0; i < isize; ++i) {
        fprintf(fp, " ");
        print_vardata(fd->d->trans, fp, vars[i]);
    }
    fprintf(fp, "\n");
#endif

}

#define zext(in,insiz,outsiz)               in

intb sext(intb in, int insiz, int outsz)
{
    if (in & ((intb)1 << (insiz - 1)))
        return (~(((intb)1 << insiz) - 1)) | in;

    return in;
}

int valuetype::cmp(const valuetype &b)
{
    if (height == b.height) {
        /* a_top代表的是 unknown或者undefined，这个值无法做比较， 例如数里面的 无限大 和 无限大 无法做比较。
        他们都是不相等
        */
        if (height == a_top) return 1;

        if (height == a_constant)
            return v - b.v;

        if (height == a_rel_constant) {
            if (rel == b.rel) return v - b.v;

            assert(0);
        }
    }

    return height - b.height;
}


// FIXME:loader的类型本来应该是LoadImageB的，但是不知道为什么loader的getNextSymbol访问的是
// LoadImageB的，而不是ElfLoadImage的···
void dobc::run()
{

#if 0
    LoadImageFunc sym;
    while (loader->getNextSymbol(sym)) {
        if (!sym.size) continue;

        dump_function(sym, 1);
    }
#else
    //dump_function("_Z10__fun_a_18Pcj");
    //dump_function("_Z9__arm_a_0v");
    //dump_function("_Z10__arm_a_21v");
    plugin_dvmp360();
#endif
}

void dobc::init_abbrev()
{
    abbrev["mult_addr"] = "ma";
    abbrev["shift_carry"] = "sh_ca";
}

struct pltentry {
    char *name;
    intb addr;
    int input;
    int output;
    int exit;
    int side_effect;
    u4 flags;
} pltlist[] = {
    { "__stack_chk_fail",   0x1a34, 0, 0, 1, 0 },
    { "time",               0x1adc, 1, 1, 0, 0 },
    { "srand48",            0x1ad0, 0, 1, 0, 0 },
    { "vmp360_op1",         0x67ac, 3, 0, 0, 1 },
    { "vmp360_mathop",      0x6248, 3, 0, 0, 1 },
    { "vmp360_op3",         0x66ac, 3, 0, 0, 1 },
    { "vmp360_op4",         0x61c0, 3, 0, 0, 1 },
    { "vmp360_op5",         0x6204, 3, 0, 0, 1 },
};

void dobc::init_plt()
{
    funcdata *fd;
    int i;

    for (i = 0; i < count_of_array(pltlist); i++) {
        struct pltentry *entry = &pltlist[i];

        Address addr(trans->getDefaultCodeSpace(), entry->addr);
        fd = new funcdata(entry->name, addr, 0, this);
        fd->set_exit(entry->exit);
        fd->funcp.set_side_effect(entry->side_effect);
        fd->funcp.inputs = entry->input;
        fd->funcp.output = entry->output;
        add_func(fd);
    }
}

const string& dobc::get_abbrev(const string &name)
{
    map<string, string>::iterator iter = abbrev.find(name);

    return (iter != abbrev.end()) ? (*iter).second : name;
}

#define STACKBASE           0x4000000

funcdata* test_vmp360_cond_inline(dobc *d, intb addr)
{
    int i;

    for (i = 0; i < count_of_array(pltlist); i++) {
        pltentry *e = pltlist + i;
        if ((e->addr == addr) && strstr(e->name, "vmp360")) {
            Address addr(d->get_code_space(), addr);
            return d->find_func(addr);
        }
    }

    return NULL;
}

void dobc::plugin_dvmp360()
{
    int changed;

    funcdata *fd_main = find_func("_Z10__arm_a_21v");
    fd_main->set_alias("vm_func1");

    set_func_alias("_Z9__arm_a_0v", "vm_enter");
    set_func_alias("_Z10__fun_a_18Pcj", "vm_run");

    set_test_cond_inline_fn(test_vmp360_cond_inline);

    fd_main->set_safezone_base(STACKBASE);
    fd_main->set_safezone(-0x148, 4);

    fd_main->follow_flow();

    fd_main->inline_call("", 1);
    fd_main->inline_call("", 1);

#if 1
    changed = 1;
    fd_main->heritage();
    fd_main->dump_cfg(fd_main->name, "orig", 1);
    while (changed) {
        changed = 0;

        fd_main->constant_propagation(0);

        // 发现CBRANCH列表不为空，说明有条件判断语句为常量
        if (!fd_main->cbrlist.empty()) {
            fd_main->dump_cfg(fd_main->name, "before_ccp", 1);
            fd_main->cond_constant_propagation();
            /* 发生了条件常量传播以后，整个程序的结构发生了变化，整个结构必须得重来 */
            fd_main->heritage_clear();
            fd_main->heritage();
            changed = 1;
        }

        // 发现新地址
        if (!fd_main->addrlist.empty()) {
            fd_main->generate_ops();
            fd_main->generate_blocks();
            changed = 1;
        }
    }
#endif
    fd_main->loop_unrolling(fd_main->get_vm_loop_header(), 1);

    fd_main->dump_pcode("1");
    fd_main->dump_cfg(fd_main->name, "1", 1);
    fd_main->dump_djgraph("1", 1);
    fd_main->dump_store_info("1");
}

void    dobc::vmp360_dump(pcodeop *p)
{
    if (p->opcode == CPUI_LOAD) {
        varnode *in1 = p->get_in(1);

        if ((in1->type.height == a_rel_constant) && (in1->get_val() == -0x1bc)) {
            p->flags.vm_eip = 1;
        }
    }
    else if (p->opcode == CPUI_STORE) {
        varnode *in1 = p->get_in(1);
        varnode *in2 = p->get_in(2);

        /* 加入跟新 VMEIP 指针的值不是常量直接报错 */
        if ((in1->type.height == a_rel_constant) && (in1->get_val() == -0x1bc)) {
            p->flags.vm_eip = 1;
            if (!p->flags.vm_vis && (in2->type.height == a_constant)) {
                printf("addr = %llx, p%d, store VMEIP = %lld\n", p->get_dis_addr().getOffset(), p->start.getTime(), in2->get_val());
                p->flags.vm_vis = 1;
            }
        }
    }
}

funcdata* dobc::find_func(const Address &addr)
{
    funcdata *fd;
    int i;
    mlist_for_each(funcs, fd, node, i) {
        if (fd->get_addr() == addr)
            return fd;
    }

    return NULL;
}

funcdata* dobc::find_func(const char *name)
{
    funcdata *fd;
    int i;
    mlist_for_each(funcs, fd, node, i) {
        if (!strcmp(fd->name.c_str(), name))
            return fd;
    }

    return NULL;
}

void dobc::init()
{
    LoadImageFunc sym;

    init_abbrev();
    init_plt();

    while (loader->getNextSymbol(sym)) {
        Address addr(sym.address);
        Address lastaddr(trans->getDefaultCodeSpace(), sym.address.getOffset() + sym.size);

        funcdata *func;

        func = new funcdata(sym.name.c_str(), addr, sym.size, this);

        Address baddr(get_code_space(), 0);
        Address eaddr(get_code_space(), ~((uintb)0));

        //func->set_range(addr, lastaddr);
        func->set_range(baddr, eaddr);

        mlist_add(funcs, func, node);
    }
}

void dobc::gen_sh(void)
{
    char buf[MAX_PATH];

    sprintf(buf, "%s/gen.sh", filename.c_str());
    file_save(buf, GEN_SH, strlen(GEN_SH));
}

dobc::dobc(const char *sla, const char *bin) 
    : fullpath(bin)
{
    slafilename.assign(sla);
    filename.assign(basename(bin));

    loader = new ElfLoadImage(bin);
    context = new ContextInternal();
    trans = new Sleigh(loader, context);

    DocumentStorage docstorage;
    Element *sleighroot = docstorage.openDocument(slafilename)->getRoot();
    docstorage.registerTag(sleighroot);
    trans->initialize(docstorage); // Initialize the translator

    loader->setCodeSpace(trans->getDefaultCodeSpace());

    mdir_make(filename.c_str());
    gen_sh();

    sp_addr = trans->getRegister("sp").getAddr();
    lr_addr = trans->getRegister("lr").getAddr();
    r0_addr = trans->getRegister("r0").getAddr();
    r1_addr = trans->getRegister("r1").getAddr();
    r2_addr = trans->getRegister("r2").getAddr();
    r3_addr = trans->getRegister("r3").getAddr();
    cy_addr = trans->getRegister("CY").getAddr();

    init();
}

dobc::~dobc()
{
}

void dobc::dump_function(char *symname)
{
    funcdata *func;

    func = find_func(symname);
    if (!func) {
        printf("not found function %s", symname);
        exit(-1);
    }

    printf("function:%s\n", symname);

    func->follow_flow();

    func->dump_cfg(func->name, "1", 0);

    printf("\n");
}

void dobc::add_func(funcdata *fd)
{
    mlist_add(funcs, fd, node);
}

void        dobc::set_func_alias(const string &func, const string &alias)
{
    funcdata *fd = find_func(func.c_str());

    fd->set_alias(alias);
}

funcdata* dobc::find_func_by_alias(const string &alias)
{
    funcdata *fd;
    int i;
    mlist_for_each(funcs, fd, node, i) {
        if (alias == fd->alias)
            return fd;
    }

    return NULL;
}

#define SLA_FILE            "../../../Processors/ARM/data/languages/ARM8_le.sla"
#define PSPEC_FILE          "../../../Processors/ARM/data/languages/ARMCortex.pspec"
#define CSPEC_FILE          "../../../Processors/ARM/data/languages/ARM.cspec"
#define TEST_SO             "../../../data/vmp/360_1/libjiagu.so"

#if defined(DOBC)
int main(int argc, char **argv)
{
    if (argc != 3) {
        puts(help);
        return 0;
    }

    dobc d(argv[1], argv[2]);

    g_dobc = &d;

    d.run();

    return 0;
}

#endif

varnode::varnode(int s, const Address &m)
    : loc(m)
{
    if (!m.getSpace())
        return;

    size = s;

    spacetype tp = m.getSpace()->getType();

    type.height = a_top;

    if (tp == IPTR_CONSTANT) {
        nzm = m.getOffset();

        set_val(m.getOffset());
    }
    else if ((tp == IPTR_FSPEC) || (tp == IPTR_IOP)) {
        flags.annotation = 1;
        flags.covertdirty = 1;
        nzm = ~((uintb)0);
    }
    else {
        flags.covertdirty = 1;
        nzm = ~((uintb)0);
    }

}

varnode::~varnode()
{
}

void            varnode::set_def(pcodeop *op)
{
    def = op;
    if (op) {
        flags.written = 1;
    }
    else
        flags.written = 0;
}

void            varnode::add_use(pcodeop *op)
{
    uses.push_back(op);
}

void            varnode::del_use(pcodeop *op)
{
    list<pcodeop *>::iterator iter;

    iter = uses.begin();
    while (*iter != op)
        iter ++;

    uses.erase(iter);
    flags.covertdirty = 1;
}

bool            varnode::in_liverange(pcodeop *p)
{
    if (!def) return false;

    if ((p->output->get_addr() == get_addr()) && (p->output->version - 1) == version) return true;

    if (def->parent != p->parent) return false;

    list<pcodeop *>::iterator it = def->basiciter;

    for (; it != p->parent->ops.end(); it++) {
        pcodeop *o = *it;

        if (o == p) return true;

        if (o->output && o->output->get_addr() == get_addr())
            return false;
    }

    assert(0);
    return true;
}

intb            varnode::get_val(void)
{
    return type.v;
}

inline bool varnode_cmp_loc_def::operator()(const varnode *a, const varnode *b) const
{
    uint4 f1, f2;

    if (a->get_addr() != b->get_addr()) return (a->get_addr() < b->get_addr());
    if (a->size != b->size) return (a->size < b->size);

    f1 = ((int)a->flags.input << 1) + a->flags.written;
    f2 = ((int)b->flags.input << 1) + b->flags.written;

    /* 这样处理过后，假如一个节点是free的(没有written和Input标记)，那么在和一个带标记的
    节点比较时，就会变大，因为 0 - 1 = 0xffffffff, f1是Unsigned类型，导致他被排后 */
    if (f1 != f2) return (f1 - 1) < (f2 - 1);

    if (a->flags.written) {
        if (a->def->start != b->def->start)
            return a->def->start < b->def->start;
    }
    else if (f1 == 0)
        return a->create_index < b->create_index;

    return false;
}

inline bool varnode_cmp_def_loc::operator()(const varnode *a, const varnode *b) const
{
    uint4 f1, f2;

    f1 = ((int)a->flags.input << 1) + a->flags.written;
    f2 = ((int)b->flags.input << 1) + b->flags.written;

    if (f1 != f2) return (f1 - 1) < (f2 - 1);

    if (a->flags.written) {
        if (a->def->start != b->def->start)
            return a->def->start < b->def->start;
    }
    if (a->get_addr() != b->get_addr()) return (a->get_addr() < b->get_addr());
    if (a->size != b->size) return (a->size < b->size);

    if (f1 == 0)
        return a->create_index < b->create_index;

    return false;
}

inline bool pcodeop_cmp_def::operator()(const pcodeop *a, const pcodeop *b) const
{
    return a->output->get_addr() < b->output->get_addr();
}

inline bool varnode_cmp_gvn::operator()(const varnode *a, const varnode *b) const
{
    pcodeop *op1, *op2;
    if (a->type.height != b->type.height)  return a->type.height < b->type.height;

    if (a->type.height == a_top) {
        op1 = a->def;
        op2 = b->def;

        if (op1->opcode != op2->opcode) return op1->opcode < op2->opcode;

        switch (op1->opcode) {
        case CPUI_INT_ADD:
        case CPUI_INT_MULT:
            if (((op1->get_in(0) == op2->get_in(0)) && (op1->get_in(1) == op2->get_in(1)))
                || ((op1->get_in(0) == op2->get_in(1)) && (op1->get_in(1) == op2->get_in(0))))
                return false;

        default:
            return a->create_index < b->create_index;
        }
    }
    else if (a->type.height == a_rel_constant) {
        return (a->type.v < b->type.v);
    }
    else if (a->type.height == a_constant) {
        return (a->type.v < b->type.v);
    }

    return false;
}

pcodeop::pcodeop(int s, const SeqNum &sq)
    :start(sq), inrefs(s)
{
    flags = { 0 };
    flags.dead = 1;
    parent = 0;

    output = 0;
    opcode = CPUI_NULL;
}
pcodeop::~pcodeop()
{
}

void    pcodeop::set_opcode(OpCode op)
{
    if (opcode != CPUI_NULL)
        flags.changed = 1;

    opcode = op;
}

void    pcodeop::remove_input(int slot)
{
    for (int i = slot + 1; i < inrefs.size(); i++)
        inrefs[i - 1] = inrefs[i];

    inrefs.pop_back();
}

void    pcodeop::insert_input(int slot)
{
    int i;
    inrefs.push_back(NULL);
    for (i = inrefs.size() - 1; i > slot; i--)
        inrefs[i] = inrefs[i - 1];
    inrefs[slot] = NULL;
}

// 扩展outbuf的内容区，用来做对齐用 
#define expand_line(num)        while (i < num) buf[i++] = ' '
int             pcodeop::dump(char *buf, uint32_t flags)
{
    int i = 0, j, in_limit = 10000;
    Translate *trans = parent->fd->d->trans;

    i += sprintf(buf + i, "    p%d:", start.getTime());

    if (output) {
        i += print_varnode(trans, buf + i, output);
        i += sprintf(buf + i, " = ");
    }

    i += sprintf(buf + i, "%s", get_opname(opcode));
    // Possibly check for a code reference or a space reference

    if (flags & PCODE_OMIT_MORE_IN) in_limit = 4;

    for (j = 0; j < inrefs.size(); ++j) {
        if (j == in_limit) break;
        i += sprintf(buf + i, " ");
        i += print_varnode(trans, buf + i, inrefs[j]);
    }

    if (j == in_limit)
        i += sprintf(buf + i, "[...]");

    expand_line(48);

    if (flags & PCODE_DUMP_VAL) {
        if (flags & PCODE_HTML_COLOR)   i += sprintf(buf + i, "<font color=\"red\"> ");
        i += print_vartype(trans, buf + i, output);
        if (flags & PCODE_HTML_COLOR)   i += sprintf(buf + i, " </font>");
    }

    if (flags & PCODE_DUMP_UD)
        i += print_udchain(buf + i, this, flags);

    if (is_call() && callctx) {
        if (flags & PCODE_HTML_COLOR)   i += sprintf(buf + i, "<font color=\"red\"> ");
#if 0
        i += sprintf(buf + i, "[r0:");
        i += print_vartype(trans, buf + i, callctx->r0);

        i += sprintf(buf + i, ",r1:");
        i += print_vartype(trans, buf + i, callctx->r1);

        i += sprintf(buf + i, ",r2:");
        i += print_vartype(trans, buf + i, callctx->r2);

        i += sprintf(buf + i, ",r3:");
        i += print_vartype(trans, buf + i, callctx->r3);
#endif

        i += sprintf(buf + i, "[sp:");
        i += print_vartype(trans, buf + i, callctx->sp);
        i += sprintf(buf + i, "]");
        if (flags & PCODE_HTML_COLOR)   i += sprintf(buf + i, " </font>");
    }

    if (this->flags.vm_eip) {
        i += sprintf(buf + i, "<font color=\"green\">**VMEIP</font>");
    }

    buf[i] = 0;

    return i;
}

int             pcodeop::compute(int inslot, flowblock **branch, list<pcodeop *> *wlist)
{
    varnode *in0, *in1, *in2, *out;
    funcdata *fd = parent->fd;
    dobc *d = fd->d;
    uint1 buf[8];
    int i, ret = 0;
    pcodeop *store, *op;

    out = output;
    in0 = get_in(0);

    switch (opcode) {
    case CPUI_COPY:
        if (in0->is_constant()) {
            out->set_val(in0->get_val());
        }
        else if (fd->is_sp_rel_constant(in0)) {
            out->set_rel_constant(in0->get_rel(), in0->get_val());

            /* 识别这种特殊形式 
            
            sp = ma + 4;

            ma = sp

            转换成 
            ma = ma + 4;

            不处理load是怕会影响别名分析
            */
            op = in0->def;
            if ((in0->uses.size() == 1) 
                && !in0->flags.input 
                && (op->opcode == CPUI_INT_ADD)) {
                varnode *_in0 = op->get_in(0);
                varnode *_in1 = op->get_in(1);

                /* 后面那个判断都是用来确认，活跃范围的 */
                //if (_in1->is_constant() && (_in0->get_addr() == output->get_addr()) && ((_in0->version + 1) == (output->version))) {
                if (_in1->is_constant() && _in0->in_liverange(this)) {
                    fd->op_remove_input(this, 0);
                    fd->op_set_opcode(this, op->opcode);

                    for (int i = 0; i < op->num_input(); i++) {
                        fd->op_set_input(this, op->get_in(i), i);
                    }

                    fd->op_destroy(op);
                }
            }
        }
        else
            out->type.height = in0->type.height;

        break;

        //      out                            in0              in1            
        // 66:(register,r1,4) = LOAD (const,0x11ed0f68,8) (const,0x840c,4)
    case CPUI_LOAD:
        d->vmp360_dump(this);

        in1 = get_in(1);
        in2 = (inrefs.size() == 3) ? get_in(2):NULL;
        if (fd->is_code(in0) && in1->is_constant()) {
            Address addr(d->trans->getDefaultCodeSpace(), in1->type.v);

            memset(buf, 0, sizeof(buf));
            d->loader->loadFill(buf, out->size, addr);

            if (out->size == 1)
                out->set_val(*(int1 *)buf);
            else if (out->size == 2)
                out->set_val(*(int2 *)buf);
            else if (out->size == 4)
                out->set_val(*(int *)buf);
            else if (out->size == 8)
                out->set_val(*(intb *)buf);

#if 0
            while (num_input() > 0)
                fd->op_remove_input(this, 0);

            fd->op_set_opcode(this, CPUI_COPY);
            fd->op_set_input(this, fd->create_constant_vn(out->get_val(), out->size), 0);
#endif

            //printf("addr=%llx, pcode=%d, load ram, pos = %llx, val = %llx\n", get_dis_addr().getOffset(), start.getTime(), in1->type.v, out->get_val());
        }
        else if (in2) { // 别名分析过
            varnode *def = in2->def->output;
            output->type = def->type;

            /* load - store 严格别名测试下Ok */
            if (fd->test_strict_alias(this, in2->def)) {
                /* 假如是类似于
                1. mem[sp] = r0;
                ... 
                ...
                n. r0 = mem[sp];
                假如1 和 n之间没有更多的对r0的定义，我们认为可以删除

                n处的r0的版本号只能比 1出的r0版本号大1
                */
                varnode *ref = in2->def->get_in(2);
                if ((output->get_addr() == ref->get_addr()) 
                    && (output->version == (ref->version + 1))) {
#if 0
                    while (num_input() > 0)
                        fd->op_remove_input(this, 0);
                    fd->op_set_opcode(this, CPUI_COPY);
                    fd->op_set_input(this, ref, 0);

                    if (def->uses.size() == 0) 
                        fd->destroy_varnode(in2->def->output);
#endif
                }
            }
#if 0
            if (in2->uses.size() == 1) {
                fd->op_remove_input(this, 0);
                fd->op_remove_input(this, 1);
                fd->op_remove_input(this, 2);
                fd->op_set_opcode(this, CPUI_COPY);
                fd->op_set_input(this, in2->def->get_in(2), 0);
            }
#endif
        }
        else if ((inslot >= 0) && (store = fd->trace_store_query(in1))) { // trace流中
            out->type = store->get_in(2)->type;
        }
        /* 假如这个值确认来自于外部，不要跟新他 */
        else if (!flags.input)
            out->type.height = a_top;

        break;

        //
    case CPUI_STORE:
        d->vmp360_dump(this);

        in1 = get_in(1);
        in2 = get_in(2);
        if (output) {
            output->type = in2->type;
        }

        break;

    case CPUI_BRANCH:
        in0 = get_in(0);

        if (!flags.branch_call) {
            *branch = parent->get_out(0);
        }
        ret = ERR_MEET_CALC_BRANCH;
        break;

    case CPUI_CBRANCH:
        in1 = get_in(1);
        if (in1->is_constant()) {
            blockedge *edge = in1->get_val() ? parent->get_true_edge() : parent->get_false_edge();

            *branch = edge->point;
            ret = ERR_MEET_CALC_BRANCH;

            if (!fd->in_cbrlist(this))
                fd->cbrlist.push_back(this);
        }
        break;

    case CPUI_BRANCHIND:
        if (in0->is_constant()) {
            Address addr(d->get_code_space(), in0->get_val());

            if (!(op = fd->find_op(addr))) {
                printf("we found a new address[%llx] to analaysis\n", addr.getOffset());
                fd->addrlist.push_back(addr);
            }
            else {
                *branch = op->parent;
                ret = ERR_MEET_CALC_BRANCH;
            }
        }
        break;

    case CPUI_INT_EQUAL:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            output->set_val(in0->get_val() == in1->get_val());
        }
        else
            output->type.height = a_top;
        break;

    case CPUI_INT_NOTEQUAL:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            output->set_val(in0->get_val() != in1->get_val());
        }
        else
            output->type.height = a_top;
        break;

    case CPUI_INT_SLESS:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            output->set_val(in0->get_val() < in1->get_val());
        }
        else
            output->type.height = a_top;
        break;

    case CPUI_INT_SLESSEQUAL:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            output->set_val(in0->get_val() <= in1->get_val());
        }
        else
            output->type.height = a_top;
        break;

    case CPUI_INT_LESS:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            output->set_val((uint8)in0->get_val() < (uint8)in1->get_val());
        }
        else
            output->type.height = a_top;
        break;

    case CPUI_INT_LESSEQUAL:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            output->set_val((uint8)in0->get_val() <= (uint8)in1->get_val());
        }
        else
            output->type.height = a_top;
        break;

    case CPUI_INT_ZEXT:
        if (in0->is_constant()) {
            if (in0->size < output->size) {
                output->set_val(zext(in0->get_val(), in0->size, out->size));
            }
            else if (in0->size > output->size) {
                throw LowlevelError("zext in size > ouput size");
            }
        }
        else
            output->type.height = a_top;
        break;

    case CPUI_INT_SEXT:
        if (in0->is_constant()) {
            if (in0->size < output->size) {
                output->set_val(sext(in0->get_val(), in0->size, out->size));
            }
            else if (in0->size > output->size) {
                throw LowlevelError("zext in size > ouput size");
            }
        }
        else
            output->type.height = a_top;
        break;

    case CPUI_INT_ADD:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            if (in0->size == 1)
                out->set_val((int1)in0->type.v + (int1)in1->type.v);
            else if (in0->size == 2)
                out->set_val((int2)in0->type.v + (int2)in1->type.v);
            else if (in0->size == 4)
                out->set_val((int4)in0->type.v + (int4)in1->type.v);
            else 
                out->set_val(in0->type.v + in1->type.v);
        }
        else if (fd->is_sp_rel_constant(in0) && in1->is_constant()) {
            op = in0->def;

            if (op && ((op->opcode == CPUI_INT_ADD) || (op->opcode == CPUI_INT_SUB))) {
                varnode *_in0 = op->get_in(0);
                varnode *_in1 = op->get_in(1);

                /*
                ma = ma + 4;
                x = ma + 4;

                转换成
                x = ma + 8;
                */
#if 0
                while ((in0->uses.size() == 1) 
                    && _in1->is_constant() && (op->output->get_addr() == _in0->get_addr())) {
                    intb v = in1->get_val();

                    if (op->opcode == CPUI_INT_ADD)
                        v += _in1->get_val();
                    else
                        v -= _in1->get_val();

                    while (num_input() > 0)
                        fd->op_remove_input(this, 0);

                    fd->op_set_input(this, in0 = _in0, 0);
                    fd->op_set_input(this, in1 = fd->create_constant_vn(v, in1->size), 1);
                    fd->op_destroy(op);

                    op = _in0->def;
                    if ((op->opcode != CPUI_INT_ADD) && (op->opcode != CPUI_INT_SUB))
                        break;
                    _in0 = op->get_in(0);
                    _in1 = op->get_in(1);
                }
#endif
            }

            out->set_rel_constant(in0->get_rel(), in0->type.v + in1->type.v);
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_INT_SUB:
        in1 = get_in(1);

        if (in0->is_constant() && in1->is_constant()) {
            if (in0->size == 1)
                out->set_val((int1)in0->type.v - (int1)in1->type.v);
            else if (in0->size == 2)
                out->set_val((int2)in0->type.v - (int2)in1->type.v);
            else if (in0->size == 4)
                out->set_val((int4)in0->type.v - (int4)in1->type.v);
            else 
                out->set_val(in0->type.v - in1->type.v);
        }
        /*      out                             0                   1       */
        /* 0:(register,mult_addr,4) = INT_SUB (register,sp,4) (const,0x4,4) */
        else if (fd->is_sp_rel_constant(in0) && in1->is_constant()) {
            out->set_rel_constant(in0->get_rel(), in0->type.v - in1->type.v);
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_INT_LEFT:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val((uintb)in0->get_val() << (uintb)in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_INT_RIGHT:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val((uintb)in0->get_val() >> (uintb)in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_INT_XOR:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->get_val() ^ in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_INT_AND:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->get_val() & in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_INT_OR:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->get_val() | in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_INT_MULT:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->get_val() * in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_BOOL_NEGATE:
        if (in0->is_constant()) {
            out->set_val(in0->get_val() ? 0:1);
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_BOOL_XOR:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->get_val() ^ in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_BOOL_AND:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->get_val() & in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_BOOL_OR:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->get_val() | in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_MULTIEQUAL:
        if (inslot >= 0) {
            output->type = get_in(inslot)->type;
        }
        else {
            for (i = 1; i < inrefs.size(); i++) {
                in1 = get_in(i);
                if (in0->type != in1->type)
                    break;
            }

            if (i == inrefs.size())
                output->type = in0->type;
            else
                output->type.height = a_top;
        }
        break;

    case CPUI_SUBPIECE:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            int v = in1->get_val();
            if (v > 0)
                out->set_val(in0->get_val() & ((1 << v) - 1));
        }
        break;

    default:
        break;
    }

    /* 返回跳转地址 */
    if ((this == parent->last_op()) && (parent->out.size() == 1))
        *branch = parent->get_out(0);

    /* 计算sp */

    return ret;
}

bool            pcodeop::is_safe_inst()
{
    funcdata *fd = parent->fd;

    VisitStat &stat(fd->visited[start.getAddr()]);

    return stat.inst_type == a_stmdb;
}

bool            pcodeop::is_prev_op(pcodeop *p) 
{
    list<pcodeop *>::iterator it = basiciter;

    if ((parent->ops.begin() != basiciter) && ((*it)->parent == p->parent)) {
        --it;
        return (*it) == this;
    }

    return false;
}


flowblock::flowblock(funcdata *f)
{
    fd = f;
}

flowblock::~flowblock()
{
}

op_edge::op_edge(pcodeop *f, pcodeop *t)
{
    from = f;
    to = t;
}

op_edge::~op_edge()
{
}

jmptable::jmptable(pcodeop *o)
{
    op = o;
    opaddr = o->get_addr();
}

jmptable::jmptable(const jmptable *op2)
{
    op = op2->op;
    opaddr = op2->opaddr;
    defaultblock = op2->defaultblock;
    lastblock = op2->lastblock;
    size = op2->size;
    addresstable = op2->addresstable;
}

jmptable::~jmptable()
{
}

void    jmptable::update(funcdata *fd)
{
    op = fd->find_op(opaddr);
}

void blockgraph::add_block(blockbasic *b)
{
    int min = b->index;

    if (blist.empty())
        index = min;
    else {
        if (min < index) index = min;
    }

    b->parent = this;
    blist.push_back(b);
}

void flowblock::find_spanning_tree(vector<flowblock *> &preorder, vector<flowblock *> &rootlist)
{
    if (blist.size() == 0) return;

    int i, origrootpos;
    vector<flowblock *> rpostorder;
    vector<flowblock *> state;
    int *visitcount;
    int rpostcount = blist.size();
    flowblock *tmpbl, *child;

    preorder.reserve(blist.size());
    rpostorder.resize(blist.size());
    visitcount = (int *)calloc(1, sizeof(int) * blist.size());

    for (i = 0; i < blist.size(); i++) {
        tmpbl = blist[i];
        tmpbl->index = -1;
        tmpbl->dfnum = -1;
        tmpbl->copymap = tmpbl;
        if ((tmpbl->in.size() == 0))
            rootlist.push_back(tmpbl);
    }
    assert(rootlist.size() == 1);

    origrootpos = rootlist.size() - 1;

    visitcount[0] = 0;
    state.push_back(blist[0]);
    preorder.push_back(blist[0]);
    blist[0]->dfnum = 0;

    while (!state.empty()) {
        flowblock *bl = state.back();

        int index = visitcount[bl->dfnum];

        /* 当前节点的子节点都遍历完成 */
        if (index == bl->out.size()) {
            state.pop_back();
            bl->index = --rpostcount;
            rpostorder[rpostcount] = bl;
            if (!state.empty())
                state.back()->numdesc += bl->numdesc;
        }
        else {
            blockedge &e = bl->out[index];
            child = e.point;
            visitcount[bl->dfnum] += 1;

            /* */
            if (child->dfnum == -1) {
                bl->set_out_edge_flag(index, a_tree_edge);
                state.push_back(child);

                child->dfnum = preorder.size();
                /* dfs顺序的就是先序遍历 */
                preorder.push_back(child);
                visitcount[child->dfnum] = 0;
                child->numdesc = 1;
            }
            /* 假如发现out边上的节点指向的节点，是已经被访问过的，那么这是一条回边 */
            else if (child->index == -1) {
                bl->set_out_edge_flag(index, a_back_edge | a_loop_edge);
            }
            /**/
            else if (bl->dfnum < child->dfnum) {
                bl->set_out_edge_flag(index, a_forward_edge);
            }
            else
                bl->set_out_edge_flag(index, a_cross_edge);
        }
    }

    free(visitcount);
    blist = rpostorder;
}

/*
1. 找到不可规约边
2. 找到 spanning tree(计算df需要)
3. 设置flowblock的索引为反向支配顺序
4. 标记 tree-edge, forward-edges, cross-edges, 和 back-edge
    初步怀疑: tree-edge 是spanning tree
              forward-edge 是
*/
void flowblock::structure_loops(vector<flowblock *> &rootlist)
{
    vector<flowblock *> preorder;
    int irreduciblecount = 0;

    find_spanning_tree(preorder, rootlist);
    /* vm360的图是不可规约的，还不确认不可规约的图会对优化造成什么影响 */
    find_irrereducible(preorder, irreduciblecount);
}

void flowblock::dump_spanning_tree(const char *filename, vector<flowblock *> &rootlist)
{
    FILE *fp;
    int i;

    fp = fopen(filename, "w");

    fprintf(fp, "digraph G {\n");
    fprintf(fp, "node [fontname = \"helvetica\"]\n");

    for (i = 0; i < rootlist.size(); i++) {
    }

    fclose(fp);
}

/*

paper: A Simple, Fast Dominance Algorithm
http://web.cse.ohio-state.edu/~rountev.1/788/papers/cooper-spe01.pdf
*/
void  flowblock::calc_forward_dominator(const vector<flowblock *> &rootlist)
{
    vector<flowblock *>     postorder;
    flowblock *b, *new_idom, *rho;
    bool changed;
    int i, j, finger1, finger2;

    if (blist.empty())
        return;

    if (rootlist.size() > 1)
        throw LowlevelError("we are not support rootlist.size() exceed 1");

    int numnodes = blist.size() - 1;
    postorder.resize(blist.size());
    for (i = 0; i < blist.size(); i++) {
        blist[i]->immed_dom = NULL;
        postorder[numnodes - i] = blist[i];
    }

    b = postorder.back();
    if (b->in.size()) {
        throw LowlevelError("entry node in edge error");
    }

    b->immed_dom = b;
    for (i = 0; i < b->out.size(); i++)
        b->get_out(i)->immed_dom = b;

    changed = true;
    new_idom = NULL;

    while (changed) {
        changed = false;
        for (i = postorder.size() - 2; i >= 0; --i) {
            b = postorder[i];

            /* 感觉这个判断条件是不需要的，但是Ghdira源代码里有 */
            if (b->immed_dom == postorder.back()) {
                continue;
            }

            for (j = 0; j < b->in.size(); j++) {
                new_idom = b->get_in(j);
                if (new_idom->immed_dom)
                    break;
            }

            j += 1;
            for (; j < b->in.size(); j++) {
                rho = b->get_in(j);
                if (rho->immed_dom) {
                    finger1 = numnodes - rho->index;
                    finger2 = numnodes - new_idom->index;
                    while (finger1 != finger2) {
                        while (finger1 < finger2)
                            finger1 = numnodes - postorder[finger1]->immed_dom->index;
                        while (finger2 < finger1)
                            finger2 = numnodes - postorder[finger2]->immed_dom->index;
                    }
                    new_idom = postorder[finger1];
                }
            }
            if (b->immed_dom != new_idom) {
                b->immed_dom = new_idom;
                changed = true;
            }
        }
    }

    postorder.back()->immed_dom = NULL;
}

blockbasic* blockgraph::new_block_basic(funcdata *f)
{
    blockbasic *ret = new blockbasic(f);
    add_block(ret);
    return ret;
}

void        blockgraph::set_start_block(flowblock *bl)
{
    int i;
    if (blist[0]->flags.f_entry_point) {
        if (bl == blist[0]) return;
    }

    for (i = 0; i < blist.size(); i++)
        if (blist[i] == bl) break;

    for (; i > 0; --i)
        blist[i] = blist[i - 1];

    blist[0] = bl;
    bl->flags.f_entry_point = 1;
}

void        blockgraph::set_initial_range(const Address &b, const Address &e)
{
    cover.clear();
    cover.insertRange(b.getSpace(), b.getOffset(), e.getOffset());
}


void        flowblock::insert(list<pcodeop *>::iterator iter, pcodeop *inst)
{
    list<pcodeop *>::iterator newiter;
    inst->parent = this;
    newiter = ops.insert(iter, inst);
    inst->basiciter = newiter;
}

int         flowblock::sub_id() 
{ 
    list<pcodeop *>::const_iterator iter = ops.begin();
    return (*iter)->start.getTime();
}

flowblock*  flowblock::get_entry_point(void)
{
    int i;

    for (i = 0; i < blist.size(); i++) {
        if (blist[i]->is_entry_point())
            return blist[i];
    }

    return NULL;
}

int         flowblock::get_in_index(const flowblock *bl)
{
    int i;

    for (i = 0; i < in.size(); i++) {
        if (in[i].point == bl)
            return i;
    }

    return -1;
}

int         flowblock::get_out_index(const flowblock *bl)
{
    int i;

    for (i = 0; i < out.size(); i++) {
        if (out[i].point == bl)
            return i;
    }

    return -1;
}

void        flowblock::clear(void)
{
    vector<flowblock *>::iterator iter;

    for (iter = blist.begin(); iter != blist.end(); ++iter)
        delete *iter;

    blist.clear();
}

void        flowblock::clear_marks(void)
{
    int i;

    for (i = 0; i < blist.size(); i++)
        blist[i]->clear_mark();
}

void        flowblock::remove_edge(flowblock *begin, flowblock *end)
{
    int i;
    for (i = 0; i < end->in.size(); i++) {
        if (end->in[i].point == begin)
            break;
    }

    end->remove_in_edge(i);
}

void        flowblock::add_edge(flowblock *begin, flowblock *end)
{
    end->add_in_edge(begin, 0);
}

void        flowblock::add_in_edge(flowblock *b, int lab)
{
    int outrev = b->out.size();
    int brev = in.size();
    in.push_back(blockedge(b, lab, outrev));
    b->out.push_back(blockedge(this, lab, brev));
}

void        flowblock::remove_in_edge(int slot)
{
    flowblock *b = in[slot].point;
    int rev = in[slot].reverse_index;

    half_delete_in_edge(slot);
    b->half_delete_out_edge(rev);
}

void        flowblock::remove_out_edge(int slot)
{
    flowblock *b = out[slot].point;
    int rev = out[slot].reverse_index;

    half_delete_out_edge(slot);
    b->half_delete_in_edge(rev);
}

void        flowblock::half_delete_out_edge(int slot)
{
    while (slot < (out.size() - 1)) {
        blockedge &edge(out[slot]);
        edge = out[slot + 1];

        blockedge &edge2(edge.point->in[edge.reverse_index]);
        edge2.reverse_index -= 1;
        slot += 1;
    }

    out.pop_back();
}

void        flowblock::half_delete_in_edge(int slot)
{
    while (slot < (in.size() - 1)) {
        blockedge &edge(in[slot]);
        edge = in[slot + 1];

        blockedge &edge2(edge.point->out[edge.reverse_index]);
        edge2.reverse_index -= 1;
        slot += 1;
    }
    in.pop_back();
}

int         flowblock::get_back_edge_count(void)
{
    int i, count = 0;

    for (i = 0; i < in.size(); i++) {
        if (in[i].label & a_back_edge) count++;
    }

    return count;
}

blockedge* flowblock::get_true_edge(void)
{
    pcodeop *op = last_op();
    int i;

    if (op->opcode != CPUI_CBRANCH)
        throw LowlevelError("get_true_edge() only support CPUI_CBRANCH");


    for (i = 0; i < out.size(); i++) {
        if (out[i].label & a_true_edge)
            return &out[i];
    }

    throw LowlevelError("not found true edge in flowblock");
}

blockedge*  flowblock::get_false_edge(void)
{
    pcodeop *op = last_op();
    int i;

    if (op->opcode != CPUI_CBRANCH)
        throw LowlevelError("get_false_addr() only support CPUI_CBRANCH");

    for (i = 0; i < out.size(); i++) {
        if (!(out[i].label & a_true_edge))
            return &out[i];
    }

    throw LowlevelError("not found false edge in flowblock");
}

void        flowblock::set_out_edge_flag(int i, uint4 lab)
{
    flowblock *bbout = out[i].point;
    out[i].label |= lab;
    bbout->in[out[i].reverse_index].label |= lab;
}

void        flowblock::clear_out_edge_flag(int i, uint4 lab)
{
    flowblock *bbout = out[i].point;
    out[i].label &= ~lab;
    bbout->in[out[i].reverse_index].label &= ~lab;
}

void        flowblock::remove_from_flow(flowblock *bl)
{
    if (bl->in.size() > 0)
        throw LowlevelError("only support remove block which in-size is 0");

    flowblock *bbout;

    while (bl->out.size() > 0) {
        bbout = bl->get_out(bl->out.size() - 1);
        bl->remove_out_edge(bl->out.size() - 1);
    }
}

void        flowblock::remove_op(pcodeop *inst)
{
    inst->parent = NULL;
    ops.erase(inst->basiciter);
}

void        flowblock::remove_block(flowblock *bl)
{
    vector<flowblock *>::iterator iter;

    if (bl->in.size())
        throw LowlevelError("only support remove block in-size is 0");

    for (iter = blist.begin(); iter != blist.end(); iter++) {
        if (*iter == bl) {
            blist.erase(iter);
            break;
        }
    }

    delete bl;
}

void        flowblock::collect_reachable(vector<flowblock *> &res, flowblock *bl, bool un) const
{
    flowblock *blk, *blk2;

    bl->set_mark();
    res.push_back(bl);
    int total = 0, i, j;

    while (total < res.size()) {
        blk = res[total++];
        for (j = 0; j < blk->out.size(); j++) {
            blk2 = blk->get_out(j);
            if (blk2->is_mark())
                continue;

            blk2->set_mark();
            res.push_back(blk2);
        }
    }

    if (un) {
        res.clear();
        for (i = 0; i < blist.size(); i++) {
            blk = blist[i];
            if (blk->is_mark())
                blk->clear_mark();
            else
                res.push_back(blk);
        }
    }
    else {
        for (i = 0; i < res.size(); i++)
            res[i]->clear_mark();
    }
}

void        flowblock::splice_block(flowblock *bl)
{
    flowblock *outbl = (flowblock *)0;
    char f[3];
    if (bl->out.size() == 1) {
        outbl = bl->get_out(0);
        if (outbl->in.size() != 1)
            outbl = NULL;
    }
    if (outbl == NULL)
        throw LowlevelError("Can only splice block with 1 output to ");

    f[0] = bl->flags.f_entry_point;
    bl->remove_out_edge(0);

    int sizeout = outbl->out.size();
    for (int i = 0; i < sizeout; i++) 
        move_out_edge(outbl, 0, bl);

    remove_block(outbl);
    bl->flags.f_entry_point = f[0];
}

void        flowblock::move_out_edge(flowblock *blold, int slot, flowblock *blnew)
{
    flowblock *outbl = blold->get_out(slot);
    int i = blold->get_out_rev_index(slot);
    outbl->replace_in_edge(i, blnew);
}

void        flowblock::replace_in_edge(int num, flowblock *b)
{
    flowblock *oldb = in[num].point;
    oldb->half_delete_out_edge(in[num].reverse_index);
    in[num].point = b;
    in[num].reverse_index = b->out.size();
    b->out.push_back(blockedge(this, in[num].label, num));
}

list<pcodeop *>::reverse_iterator flowblock::get_rev_iterator(pcodeop *op)
{
    list<pcodeop *>::reverse_iterator it = ops.rbegin();

    for (; it != ops.rend(); it++) {
        if (*it == op)
            return it;
    }

    throw LowlevelError("get_rev_iterator failure");
}

flowblock*  flowblock::add_block_if(flowblock *b, flowblock *cond, flowblock *tc)
{
    add_edge(b, cond);
    b->out[0].set_true();

    add_edge(b, tc);
    b->out[1].set_false();

    return b;
}

bool        flowblock::is_dowhile(flowblock *b)
{
    int i;

    for (i = 0; i < b->out.size(); i++) {
        flowblock *o = b->get_out(i);

        if (o == b)
            return true;
    }

    return false;
}

pcodeop*    flowblock::first_callop(void)
{
    list<pcodeop *>::iterator it;

    for (it = ops.begin(); it != ops.end(); it++) {
        pcodeop *p = *it;

        if (p->callfd) return p;
    }

    return NULL;
}

pcodeop*    flowblock::first_callop_vmp(flowblock *end)
{
    list<pcodeop *>::iterator it;
    pcodeop *op;
    dobc *d = fd->d;

    for (int i = 0; i < blist.size(); i++) {
        flowblock *b = blist[i];

        if (b == end)
            return NULL;

        for (it = b->ops.begin(); it != b->ops.end(); it++) {
            op = *it;
            if (op->is_call() && d->test_cond_inline(d, op->get_call_offset()))
                return op;
        }
    }

    return NULL;
}

bool        in_loop(flowblock *h);

void        flowblock::build_dom_tree(vector<vector<flowblock *>> &child)
{
    int i;
    flowblock *bl;

    child.clear();
    child.resize(blist.size() + 1);
    for (i = 0; i < blist.size(); ++i) {
        bl = blist[i];
        if (bl->immed_dom)
            child[bl->immed_dom->index].push_back(bl);
        else
            child[blist.size()].push_back(bl);
    }
}

int         flowblock::build_dom_depth(vector<int> &depth)
{
    flowblock *bl;
    int max = 0, i;

    depth.resize(blist.size() + 1);

    for (i = 0; i < blist.size(); i++) {
        bl = blist[i]->immed_dom;
        if (bl)
            depth[i] = depth[bl->index] + 1;
        else
            depth[i] = i;

        if (max < depth[i])
            max = depth[i];
    }

    depth[blist.size()] = 0;
    return max;
}

Address     flowblock::get_start(void)
{
    const Range *range = cover.getFirstRange();
    if (range == NULL) {
        assert(0);
    }

    return range->getFirstAddr();
}

/* 
Testing Flow Graph Reducibility
https://core.ac.uk/download/pdf/82032035.pdf */
bool        flowblock::find_irrereducible(const vector<flowblock *> &preorder, int &irreduciblecount)
{
    vector<flowblock *> reachunder;
    bool needrebuild = false;
    int xi = preorder.size() - 1, i;

    while (xi >= 0) {
        flowblock *x = preorder[xi];
        xi -= 1;
        int sizein = x->in.size();
        for (i = 0; i < sizein; ++i) {
            if (!x->is_back_edge_in(i))
                continue;

            flowblock *y = x->get_in(i);
            if (y == x)
                continue;

            reachunder.push_back(y->copymap);
            y->copymap->set_mark();
        }
        int q = 0;
        while (q < reachunder.size()) {
            flowblock *t = reachunder[q];
            q += 1;

            int sizein_t = t->in.size();
            for (i = 0; i < sizein_t; i++) {
                flowblock *y = t->get_in(i);
                flowblock *yprime = y->copymap;         // y' = FIND(y)

                if ((x->dfnum > yprime->dfnum) || ((x->dfnum + x->numdesc) <= yprime->dfnum)) {
                    printf("warn: dfnum[%d] irreducible to dfnum[%d]\n", x->dfnum, yprime->dfnum);
                    clear_marks();
                    return true;
                }
                else if (!yprime->is_mark() && (yprime != x)) {
                    reachunder.push_back(yprime);
                    yprime->set_mark();
                }
            }
        }

        for (i = 0; i < reachunder.size(); i++) {
            flowblock *s = reachunder[i];
            s->clear_mark();
            s->copymap = x;
        }

        reachunder.clear();
    }

    return false;
}

void        flowblock::calc_loop()
{
    int i;

    for (i = 0; i < blist.size(); i++) {
    }
}

void        flowblock::add_op(pcodeop *op)
{
    insert(ops.end(), op);
}

funcdata::funcdata(const char *nm, const Address &a, int size, dobc *d1)
    : startaddr(a),
    bblocks(this),
    name(nm),
    alias(nm),
    searchvn(0, Address(Address::m_minimal))
{
    d = d1;

    emitter.fd = this;

    memstack.size = 256 * 1024;
    memstack.bottom = (u1 *)malloc(sizeof (u1) * memstack.size);
    memstack.top = memstack.bottom + memstack.size;

    vbank.uniqbase = 0x100000;
    vbank.uniqid = vbank.uniqbase;
    vbank.create_index = 0;
}

funcdata::~funcdata(void)
{
}

pcodeop*    funcdata::newop(int inputs, const SeqNum &sq)
{
    pcodeop *op = new pcodeop(inputs, sq);
    if (sq.getTime() >= op_uniqid)
        op_uniqid = sq.getTime() + 1;

    optree[op->start] = op;
    op->insertiter = deadlist.insert(deadlist.end(), op);

    return op;
}

pcodeop*    funcdata::newop(int inputs, const Address &pc)
{
    pcodeop *op = new pcodeop(inputs, SeqNum(pc, op_uniqid++));
    optree[op->start] = op;
    op->insertiter = deadlist.insert(deadlist.end(), op);

    return op;
}

varnode*    funcdata::new_varnode_out(int s, const Address &m, pcodeop *op)
{
    varnode *vn = create_def(s, m, op);
    op->output = vn;

    return vn;
}

varnode*    funcdata::new_varnode(int s, AddrSpace *base, uintb off)
{
    return new_varnode(s, Address(base, off));
}

varnode*    funcdata::new_varnode(int s, const Address &m)
{
    varnode *vn = create_vn(s, m);

    return vn;
}

varnode*    funcdata::create_vn(int s, const Address &m)
{
    varnode *vn = new varnode(s, m);

    vn->create_index = vbank.create_index++;
    vn->lociter = loc_tree.insert(vn).first;
    vn->defiter = def_tree.insert(vn).first;
    return vn;
}

varnode*    funcdata::create_def(int s, const Address &m, pcodeop *op)
{
    varnode *vn = new varnode(s, m);
    vn->create_index = vbank.create_index++;
    vn->set_def(op);

    return xref(vn);
}

varnode*    funcdata::xref(varnode *vn)
{
    pair<varnode_loc_set::iterator, bool> check;

    check = loc_tree.insert(vn);
    if (!check.second) {
        throw LowlevelError("vn already be inserted");
    }

    vn->lociter = check.first;
    vn->defiter = def_tree.insert(vn).first;

    return vn;
}

varnode*    funcdata::set_def(varnode *vn, pcodeop *op)
{
    if (!vn->is_free()) 
        throw LowlevelError("Defining varnode which is not free at " + op->get_addr().getShortcut());
    

    if (vn->is_constant())
        throw LowlevelError("Assignment to constant at ");

    loc_tree.erase(vn->lociter);
    def_tree.erase(vn->defiter);

    vn->set_def(op);

    return xref(vn);
}

varnode*    funcdata::create_def_unique(int s, pcodeop *op)
{
    Address addr(uniq_space, vbank.uniqid);

    vbank.uniqid += s;

    return create_def(s, addr, op);
}

varnode*    funcdata::create_constant_vn(intb val, int size)
{
    Address addr(d->trans->getConstantSpace(), val);

    return create_vn(size, addr);
}

void        funcdata::op_set_opcode(pcodeop *op, OpCode opc)
{
    if (opc)
        remove_from_codelist(op);
    op->set_opcode(opc);
    add_to_codelist(op);
}

void        funcdata::op_resize(pcodeop *op, int size)
{
    op->inrefs.resize(size);
}

void        funcdata::op_set_input(pcodeop *op, varnode *vn, int slot)
{
    while (op->inrefs.size() < (slot + 1))
        op->inrefs.push_back(NULL);

    if (vn == op->get_in(slot))
        return; 

    if (vn->is_constant()) {
    }

    if (op->get_in(slot))
        op_unset_input(op, slot);

    vn->add_use(op);
    op->inrefs[slot] = vn;
}

void        funcdata::op_set_output(pcodeop *op, varnode *vn)
{
    if (vn == op->get_out())
        return;

    if (op->get_out())
        op_unset_output(op);

    if (vn->get_def())
        op_unset_output(vn->get_def());

    vn = set_def(vn, op);

    op->set_output(vn);
}

void        funcdata::op_unset_input(pcodeop *op, int slot)
{
    varnode *vn = op->get_in(slot);

    vn->del_use(op);
    op->clear_input(slot);
}

void        funcdata::op_unset_output(pcodeop *op)
{
    varnode *vn = op->get_out();

    if (vn == NULL) return;

    op->set_output(NULL);
}

void        funcdata::op_remove_input(pcodeop *op, int slot)
{
    op_unset_input(op, slot);
    op->remove_input(slot);
}

void        funcdata::op_insert_input(pcodeop *op, varnode *vn, int slot)
{
    op->insert_input(slot);
    op_set_input(op, vn, slot);
}

void        funcdata::op_zero_multi(pcodeop *op)
{
    if (op->num_input() == 0) {
        assert(0);
    }
    else if (op->num_input() == 1) {
        op_set_opcode(op, CPUI_COPY);
        op->flags.phi = 1;
    }
}

void        funcdata::op_unlink(pcodeop *op)
{
    int i;

    op_unset_output(op);

    for (i = 0; i < op->inrefs.size(); i++)
        op_unset_input(op, i);
    if (op->parent)
        op_uninsert(op);
}

void        funcdata::op_uninsert(pcodeop *op)
{
    mark_dead(op);
    op->parent->remove_op(op);
}

void        funcdata::clear_block_phi(flowblock *b)
{
    pcodeop *p;
    list<pcodeop *>::iterator it;

    while ((p = b->ops.front())) {
        if ((p->opcode != CPUI_MULTIEQUAL) && !p->flags.phi) break;

        op_destroy_ssa(p);
    }
}


/* 1. 返回这个地址对应的instruction的第一个pcode的地址
   2. 假如这个地址上的instruction没有产生pcode，返回顺序的下一个instruction的首pcode的地址 */
pcodeop*    funcdata::target(const Address &addr) const
{
    map<Address, VisitStat>::const_iterator iter;

    iter = visited.find(addr);
    while (iter != visited.end()) {
        const SeqNum &seq(iter->second.seqnum);
        if (!seq.getAddr().isInvalid()) {
            pcodeop *retop = find_op(seq);
            if (retop)
                return retop;
            break;
        }

        iter = visited.find(iter->first + iter->second.size);
    }

    throw LowlevelError("Could not find op at target address");
}

/* 设置当前解析地址到哪个地址为止
假如发现 栈顶 位置已经被访问过了，弹出当前地址，并返回
假如发现 栈顶 位置小于某个已访问到的地址b，把地址b设置为边界
假如发现 栈顶 位置就是访问过的地址中最大的，设置eaddr为bound
*/
bool        funcdata::set_fallthru_bound(Address &bound)
{
    map<Address, VisitStat>::const_iterator iter;
    const Address &addr(addrlist.back());

    iter = visited.upper_bound(addr);
    if (iter != visited.begin()) {
        --iter;
        if (addr == iter->first) {
            addrlist.pop_back();
            pcodeop *op = target(addr);
            op->flags.startblock = 1;
            return false;
        }
        /* 这个是对同一个地址有不同的解析结果，在一些保护壳中有用 */
        if (addr < (iter->first + iter->second.size)) {
            throw LowlevelError("different interpreted instruction in address");
        }

        ++iter;
    }

    if (iter != visited.end())
        bound = iter->first;
    else
        bound = eaddr;

    return true;
}

pcodeop*    funcdata::find_op(const Address &addr)
{
    map<Address, VisitStat>::iterator iter;
    iter = visited.find(addr);
    if (iter == visited.end()) return NULL;
    return find_op(iter->second.seqnum);
}

pcodeop*    funcdata::find_op(const SeqNum &num) const
{
    pcodeop_tree::const_iterator iter = optree.find(num);
    if (iter == optree.end()) return NULL;
    return iter->second;
}

int         funcdata::inst_size(const Address &addr)
{
    map<Address, VisitStat>::iterator iter;
    iter = visited.find(addr);
    return iter->second.size;
}

/* 
Algorithms for Computing the Static Single Assignment Form

GIANFRANCO BILARDI

https://www.cs.utexas.edu/users/pingali/CS380C/2007fa/papers/ssa.pdf

up-edge: 
*/
void        funcdata::build_adt(void)
{
    flowblock *x, *v, *u;
    int i, j, k, l;
    int size = bblocks.get_size();
    vector<int>     a(size);
    vector<int>     b(size, 0);
    vector<int>     t(size, 0);
    vector<int>     z(size, 0);

    /*
    Definition 4. Given a CFG G = (V, E), (u -> v) in E is an up-edge if
u != idom(v). The subgraph (V, Eup) of G containing only the up-edges is called
the a-DF graph.
*/
    vector<flowblock *> upstart, upend;

    augment.clear();
    augment.resize(size);
    phiflags.clear();
    phiflags.resize(size, 0);

    bblocks.build_dom_tree(domchild);
    maxdepth = bblocks.build_dom_depth(domdepth);
    for (i = 0; i < size; i++) {
        x = bblocks.get_block(i);
        for (j = 0; j < domchild[i].size(); ++j) {
            v = domchild[i][j];
            for (k = 0; k < v->in.size(); ++k) {
                u = v->get_in(k);
                if (u != v->immed_dom) {
                    upstart.push_back(u);
                    upend.push_back(v);
                    b[u->index] += 1;
                    t[x->index] += 1;
                }
            }
        }
    }

    for (i = size - 1; i >= 0; --i) {
        k = 0;
        l = 0;
        for (j = 0; j < domchild[i].size(); ++j) {
            k += a[domchild[i][j]->index];
            l += z[domchild[i][j]->index];
        }

        a[i] = b[i] - t[i] + k;
        z[i] = 1 + l;

        if ((domchild[i].size() == 0) || (z[i] > (a[i] + 1))) {
            phiflags[i] |= boundary_node;
            z[i] = 1;
        }
    }
    z[0] = -1;
    for (i = 1; i < size; ++i) {
        j = bblocks.get_block(i)->immed_dom->index;
        if (phiflags[j] & boundary_node)
            z[i] = j;
        else
            z[i] = z[j];
    }

    for (i = 0; i < upstart.size(); ++i) {
        v = upend[i];
        j = v->immed_dom->index;
        k = upstart[i]->index;
        while (j < k) {
            augment[k].push_back(v);
            k = z[k];
        }
    }
}

/* FIXME:似乎是一个pcode产生了跳转，但是这个跳转发生在归属instruction中
但是看名字，它又是像一个相对跳转 */
pcodeop*    funcdata::find_rel_target(pcodeop *op, Address &res) const
{
    const Address &addr(op->get_in(0)->get_addr());
    uintm id = op->start.getTime() + addr.getOffset();
    SeqNum seqnum(op->start.getAddr(), id);
    pcodeop *retop = find_op(seqnum);
    if (retop)
        return op;

    SeqNum seqnum1(op->get_addr(), id - 1);
    retop = find_op(seqnum1);
    if (retop) {
        map<Address, VisitStat>::const_iterator miter;
        miter = visited.upper_bound(retop->get_addr());
        if (miter != visited.begin()) {
            --miter;
            res = miter->first + miter->second.size;
            if (op->get_addr() < res)
                return NULL;
        }
    }

    throw LowlevelError("Bad relative branch at instruction");
}

/* 当扫描某个branch指令时，查看他的to地址，是否已经再visited列表里，假如已经再的话

把目的地址设置为startblock，假如不是的话，推入扫描地址列表 

*/
void     funcdata::new_address(pcodeop *from, const Address &to)
{
    if (visited.find(to) != visited.end()) {
        pcodeop *op = target(to);
        op->flags.startblock = 1;
        return;
    }

    addrlist.push_back(to);
}

void        funcdata::del_varnode(varnode *vn)
{
}

varnode_loc_set::const_iterator     funcdata::begin_loc(const Address &addr)
{
    searchvn.loc = addr;
    return loc_tree.lower_bound(&searchvn);
}

varnode_loc_set::const_iterator     funcdata::end_loc(const Address &addr)
{
    if (addr.getOffset() == addr.getSpace()->getHighest()) {
        AddrSpace *space = addr.getSpace();
        searchvn.loc = Address(d->trans->getNextSpaceInOrder(space), 0);
    }
    else
        searchvn.loc = addr + 1;

    return loc_tree.lower_bound(&searchvn);
}

varnode_loc_set::const_iterator     funcdata::begin_loc(AddrSpace *spaceid)
{
    searchvn.loc = Address(spaceid, 0);
    return loc_tree.lower_bound(&searchvn);
}

varnode_loc_set::const_iterator     funcdata::end_loc(AddrSpace *spaceid)
{
    searchvn.loc = Address(d->trans->getNextSpaceInOrder(spaceid), 0);
    return loc_tree.lower_bound(&searchvn);
}

void        funcdata::del_op(pcodeop *op)
{
    int i;
    for (i = 0; i < op->inrefs.size(); i++) {
    }
}

/* 当一个指令内产生了多个pcode以后，比如

inst1: p1, p2, p3, p4, p5

p2产生了跳转，p3, p4, p5全部都要删除，到下一个指令为止
*/
void        funcdata::del_remaining_ops(list<pcodeop *>::const_iterator oiter)
{
    while (oiter != deadlist.end()) {
        pcodeop *op = *oiter;
        ++oiter;
        del_op(op);
    }
}

void        funcdata::add_callspec(pcodeop *p, funcdata *fd)
{
    int sug_input;
    p->callfd = fd;
    qlst.push_back(new func_call_specs(p, fd));

    sug_input = (fd->funcp.inputs == -1) ? 3 : fd->funcp.inputs;

    // call的地址也在参数0
    sug_input += 1;

    while (p->num_input() < sug_input) {
        if (p->num_input() < 2) {
            varnode *vn = new_varnode(4, d->r0_addr);
            op_set_input(p, vn, 1);
        }
        else if (p->num_input() < 3) {
            varnode *vn = new_varnode(4, d->r1_addr);
            op_set_input(p, vn, 2);
        }
        else if (p->num_input() < 4) {
            varnode *vn = new_varnode(4, d->r2_addr);
            op_set_input(p, vn, 3);
        }
        else if (p->num_input() < 5) {
            varnode *vn = new_varnode(4, d->r3_addr);
            op_set_input(p, vn, 4);
        }
    }
}

pcodeop*    funcdata::xref_control_flow(list<pcodeop *>::const_iterator oiter, bool &startbasic, bool &isfallthru)
{
    funcdata *fd;
    pcodeop *op = NULL;
    isfallthru = false;
    uintm maxtime = 0;
    int exit = 0;

    while (oiter != deadlist.end()) {
        op = *oiter++;
        if (startbasic) {
            op->flags.startblock = 1;
            startbasic = false;
        }

        switch (op->opcode) {
        case CPUI_CBRANCH:
        case CPUI_BRANCH: {
            const Address &destaddr(op->get_in(0)->get_addr());
            startbasic = true;

            /* 没看懂，destaddr指向了常量空间? */
            if (destaddr.isConstant()) {
                Address fallThruAddr;
                pcodeop *destop = find_rel_target(op, fallThruAddr);
                if (destop) {
                    destop->flags.startblock = 1;
                    uintm newtime = destop->start.getTime();
                    if (newtime > maxtime)
                        maxtime = newtime;
                }
                else
                    isfallthru = true;
            }
            /*
在某些加壳程序中, branch指令会模拟call的效果，它会branch到一个函数里面去，这个时候
我们需要检查目的地址是否是某个函数的地址，假如是的话，就不加入扫描列表，同时把这个
branch指令打上call_branch标记

它不同于一般的call，一般的call指令，还有隐含的把pc压入堆栈行为，call完还会回来，
但是branch跳过去就不回来了
            */
            else if ((fd = d->find_func(destaddr))) {
                if (op->opcode == CPUI_CBRANCH)
                    throw LowlevelError("not support cbranch on " + fd->name);

                startbasic = false;
                op->flags.branch_call = 1;
                op->flags.exit = 1;

                add_callspec(op, fd);
            }
            else
                new_address(op, destaddr);
        }
            break;

        case CPUI_BRANCHIND:
            if (op->get_in(0)->is_constant()) {
                Address destaddr(d->trans->getConstantSpace(), op->get_in(0)->get_val());
                new_address(op, destaddr);
            }
            else
                tablelist.push_back(op);
        case CPUI_RETURN:

            if (op->start.getTime() >= maxtime) {
                del_remaining_ops(oiter);
                oiter = deadlist.end();
            }
            startbasic = true;
            break;

        case CPUI_CALL:
            const Address &destaddr(op->get_in(0)->get_addr());
            /* 假如发现某些call的函数有exit标记，则开始新的block，并删除当前inst的接下去的所有pcode，因为走不到了 */
            if ((fd = d->find_func(destaddr))) {
                if (exit = fd->flags.exit) startbasic = true;

                add_callspec(op, fd);
            }
            op->flags.exit = exit;
            break;
        }

        if ((op->opcode == CPUI_BRANCH
            || op->opcode == CPUI_BRANCHIND
            || op->opcode == CPUI_RETURN
            || exit) && (op->start.getTime() >= maxtime)) {
            del_remaining_ops(oiter);
            oiter = deadlist.end();
        }
    }

    if (exit)
        isfallthru = false;
    else if (isfallthru)
        startbasic = true;
    else if (!op)
        isfallthru = true;
    else {
        switch (op->opcode) {
        case CPUI_BRANCH:
        case CPUI_BRANCHIND:
        case CPUI_RETURN:
            break;

        default:
            isfallthru = true;
            break;
        }
    }

    return op;
}

bool        funcdata::process_instruction(const Address &curaddr, bool &startbasic)
{
    bool emptyflag;
    bool isfallthru = true;
    AssemblyRaw assem;

    list<pcodeop *>::const_iterator oiter;
    int step;

    if (inst_count >= inst_max) {
        throw LowlevelError("Flow exceeded maximum allowable instruction");
    }

    inst_count++;

    /* 这里做了一堆花操作，其实就是让 oiter，一定要指向instruction生成的第一个pcode */
    if (optree.empty())
        emptyflag = true;
    else {
        emptyflag = false;
        oiter = deadlist.end();
        --oiter;
    }

    //d->trans->printAssembly(assem, curaddr);
    step = d->trans->oneInstruction(emitter, curaddr);

    VisitStat &stat(visited[curaddr]);
    stat.size = step;

    if (curaddr < minaddr)
        minaddr = curaddr;
    if (maxaddr < (curaddr + step))
        maxaddr = curaddr + step;

    if (emptyflag)
        oiter = deadlist.begin();
    else
        ++oiter;

    /* 这个时候oiter指向的是新生成的instruction的第一个pcode的位置, 这个判断是为了防止
    某些instruction没有生成pcode */
    if (oiter != deadlist.end()) {
        stat.seqnum = (*oiter)->start;
        (*oiter)->flags.startinst = 1;

        xref_control_flow(oiter, startbasic, isfallthru);
    }

    if (isfallthru)
        addrlist.push_back(curaddr + step);

    return isfallthru;
}

/* 指令有跳转和无跳转之分，fallthru的意思就是直达往下，我们从一个地址开始分析，
 假如碰到条件跳转指令，把要跳转的地址压入，然后继续往下分析，一直到无法往下为止，如碰到return
*/
void        funcdata::fallthru()
{
    Address bound;

    /* 设置 直达 边界 */
    if (!set_fallthru_bound(bound))
        return;

    Address curaddr;
    bool startbasic = true;
    bool fallthruflag;

    while (!addrlist.empty()) {
        curaddr = addrlist.back();
        addrlist.pop_back();
        fallthruflag = process_instruction(curaddr, startbasic);
        if (!fallthruflag)
            break;

        if (bound <= addrlist.back()) {
            if (bound == addrlist.back()) {
                if (startbasic) {
                    pcodeop *op = target(addrlist.back());
                    op->flags.startblock = 1;
                }

                addrlist.pop_back();
                break;
            }

            if (!set_fallthru_bound(bound))
                return;
        }
    }
}

jmptable*   funcdata::find_jmptable(pcodeop *op)
{
    vector<jmptable *>::const_iterator iter;
    jmptable *jt;

    for (iter = jmpvec.begin(); iter != jmpvec.end(); ++iter) {
        jt = *iter;
        if (jt->opaddr == op->get_addr()) return jt;
    }

    return NULL;
}

void        funcdata::recover_jmptable(pcodeop *op, int elmsize)
{
    Address addr(op->start.getAddr());
    jmptable *newjt = new jmptable(op);

    int i;

    for (i = 0; i < (elmsize + 2); i++) {
        addrlist.push_back(addr + 4 + 4 * i);

        newjt->addresstable.push_back(addr + 4 + 4 * i);
    }
    newjt->defaultblock = elmsize + 1;
    jmpvec.push_back(newjt);
}

void        funcdata::fix_jmptable()
{
    int i, j;
    flowblock *bb;

    for (i = 0; i < jmpvec.size(); i++) {
        jmptable *jt = jmpvec[i];
        for (j = 0; j < jt->addresstable.size(); j++) {
            Address &addr = jt->addresstable[j];
            pcodeop *op = find_op(addr);

            if (!op->flags.startblock)
                throw LowlevelError("indirect jmp not is start block");

            bb = op->parent;
            bb->jmptable = jt;
            bb->flags.f_switch_out = 1;
        }

        jt->op->parent->type = a_switch;
        jt->op->parent->jmptable = jt;
    }
}

void        funcdata::analysis_jmptable(pcodeop *op)
{
    varnode *vn = op->get_in(0);
    pcodeop *def = vn->def;
    int reg, reg1;
    unsigned int data, data1;

    if (d->trans->getRegisterName(vn->loc.getSpace(), vn->loc.getOffset(), vn->loc.getAddrSize()) == "pc") {
        d->loader->loadFill((uint1 *)&data, 4, op->start.getAddr());
        d->loader->loadFill((uint1 *)&data1, 4, op->start.getAddr() - 4);
        
#define ARM_ADD_MASK        0x00800000
#define ARM_CMP_MASK        0x03500000

        if ((data & ARM_ADD_MASK) == ARM_ADD_MASK
            && (data1 & ARM_CMP_MASK) == ARM_CMP_MASK) {
            reg = data & 0xf;
            reg1 = (data1 >> 16) & 0xf;
            if (reg == reg1) {
                recover_jmptable(op, data1 & 0xfff);
                return;
            }
        }
        /* 不是switch table */
    }
}

void        funcdata::generate_ops_start()
{
    addrlist.push_back(startaddr);

    generate_ops();
}

void        funcdata::generate_ops(void)
{
    vector<pcodeop *> notreached;       // 间接跳转是不可达的?

    /* 修改了原有逻辑，可以多遍op_generated*/
    //printf("%s generate ops %d times\n", name.c_str(), op_generated+1);

    while (!addrlist.empty())
        fallthru();

    while (!tablelist.empty()) {
        pcodeop *op = tablelist.back();
        tablelist.pop_back();

        analysis_jmptable(op);

        while (!addrlist.empty())
            fallthru();
    }

    op_generated++;
}

pcodeop*    funcdata::branch_target(pcodeop *op) 
{
    const Address &addr(op->get_in(0)->get_addr());

    if (addr.isConstant()) {
        Address res;
        pcodeop *retop = find_rel_target(op, res);
        if (retop)
            return retop;

        return target(res);
    }

    return target(addr);
}

pcodeop*    funcdata::fallthru_op(pcodeop *op)
{
    pcodeop*    retop;
    list<pcodeop *>::const_iterator iter = op->insertiter;
    ++iter;
    if (iter != deadlist.end()) {
        retop = *iter;
        if (!retop->flags.startinst)
            return retop;
    }

    map<Address, VisitStat>::const_iterator miter;
    miter = visited.upper_bound(op->get_addr());
    if (miter == visited.begin())
        return NULL;
    --miter;
    if ((*miter).first + (*miter).second.size <= op->get_addr())
        return NULL;
    return target((*miter).first + (*miter).second.size);
}

void        funcdata::collect_edges()
{
    list<pcodeop *>::const_iterator iter, iterend;
    list<op_edge *>::const_iterator iter1;
    jmptable *jt;
    pcodeop *op, *target_op, *target_op1;
    bool nextstart;
    int i;

    if (flags.blocks_generated)
        throw LowlevelError(name + "blocks already generated");

    iter = deadlist.begin();
    iterend = deadlist.end();
    while (iter != iterend) {
        op = *iter++;
        if (iter == iterend)
            nextstart = true;
        else
            nextstart = (*iter)->flags.startblock;

        switch (op->opcode) {
        case CPUI_BRANCH:
            if (!op->flags.branch_call) {
                target_op = branch_target(op);
                block_edge.push_back(new op_edge(op, target_op));
            }
            break;

        case CPUI_BRANCHIND:
            if (op->get_in(0)->is_constant()) {
                Address addr(d->get_code_space(), op->get_in(0)->get_val());

                target_op = find_op(addr);
                if (!target_op)
                    throw LowlevelError("not found address ");
                block_edge.push_back(new op_edge(op, target_op));
                break;
            }

            jt = find_jmptable(op);
            if (jt == NULL) break;

            for (i = 0; i < jt->addresstable.size(); i++) {
                target_op = target(jt->addresstable[i]);
                if (target_op->flags.mark)
                    continue;
                target_op->flags.mark = 1;

                block_edge.push_back(new op_edge(op, target_op));
            }

            iter1 = block_edge.end();
            while (iter1 != block_edge.begin()) {
                --iter1;
                if ((*iter1)->from == op)
                    (*iter1)->to->flags.mark = 0;
                else
                    break;
            }
            break;

        case CPUI_RETURN:
            break;

        case CPUI_CBRANCH:
            target_op = fallthru_op(op);
            block_edge.push_back(new op_edge(op, target_op));

            target_op1 = branch_target(op);
            block_edge.push_back(new op_edge(op, target_op1));
            block_edge.back()->t = 1;

            /* arm中的单行条件判断指令，如 addls pc, pc, $r3, lsl, #3 */
            if ((target_op1->get_addr() == op->get_addr())) {
                VisitStat  &stat(visited[op->get_addr()]);

                stat.flags.condinst = 1;
            }
            break;

        default:
            if (op->flags.exit)
                break;

            if (nextstart) {
                target_op = fallthru_op(op);
                block_edge.push_back(new op_edge(op, target_op));
            }
            break;
        }
    }
}

void        funcdata::mark_alive(pcodeop *op)
{
    op->flags.dead = 0;
}

void        funcdata::mark_dead(pcodeop *op)
{
    op->flags.dead = 1;
}

void        funcdata::mark_free(varnode *vn)
{
    loc_tree.erase(vn->lociter);
    def_tree.erase(vn->defiter);

    vn->set_def(NULL);
    vn->flags.insert = 0;
    vn->flags.input = 0;

    vn->lociter = loc_tree.insert(vn).first;
    vn->defiter = def_tree.insert(vn).first;
}

void        funcdata::op_insert(pcodeop *op, blockbasic *bl, list<pcodeop *>::iterator iter)
{
    mark_alive(op);
    bl->insert(iter, op);
}

void        funcdata::op_insert_begin(pcodeop *op, blockbasic *bl)
{
    list<pcodeop *>::iterator iter = bl->ops.begin();

    if (op->opcode != CPUI_MULTIEQUAL) {
        while (iter != bl->ops.end()) {
            if ((*iter)->opcode != CPUI_MULTIEQUAL)
                break;

            ++iter;
        }
    }

    op_insert(op, bl, iter);
}

void        funcdata::op_insert_end(pcodeop *op, blockbasic *bl)
{
    list<pcodeop *>::iterator iter = bl->ops.end();

    if (iter != bl->ops.begin()) {
        --iter;
    }

    op_insert(op, bl, iter);
}

void        funcdata::connect_basic()
{
    flowblock *from;
    op_edge *edge;
    list<op_edge *>::const_iterator iter;

    iter = block_edge.begin();
    while (iter != block_edge.end()) {
        edge = *iter++;
        from = edge->from->parent;
        bblocks.add_edge(from, edge->to->parent);

        if (edge->t)
            from->set_out_edge_flag(from->out.size() - 1, a_true_edge);

        //printf("0x%x -> 0x%x\n", (int)edge->from->start.getAddr().getOffset(), (int)edge->to->start.getAddr().getOffset());
    }
}

void        funcdata::split_basic()
{
    pcodeop *op;
    blockbasic *cur;
    list<pcodeop *>::const_iterator iter, iterend;

    iter = deadlist.begin();
    iterend = deadlist.end();
    if (iter == iterend)
        return;

    op = *iter++;

    cur = bblocks.new_block_basic(this);
    op_insert(op, cur, cur->ops.end());
    bblocks.set_start_block(cur);

    Address start = op->get_addr();
    Address stop = start;

    while (iter != iterend) {
        op = *iter++;

        if (op->flags.startblock) {
            cur->set_initial_range(start, stop);
            cur = bblocks.new_block_basic(this);
            start = op->start.getAddr();
            stop = start;
        }
        else {
            const Address &nextaddr(op->get_addr());
            if (stop < nextaddr)
                stop = nextaddr;
        }

        op_insert(op, cur, cur->ops.end());
    }
    cur->set_initial_range(start, stop);
}

void        funcdata::generate_blocks()
{
    clear_blocks();

    collect_edges();
    split_basic();
    connect_basic();

    if (bblocks.blist.size()) {
        flowblock *startblock = bblocks.blist[0];
        if (startblock->in.size()) {
            // 保证入口block没有输入边
            blockbasic *newfront = bblocks.new_block_basic(this);
            bblocks.add_edge(newfront, startblock);
            bblocks.set_start_block(newfront);
            newfront->set_initial_range(startaddr, startaddr);
        }
    }

    fix_jmptable();

    flags.blocks_generated = 1;

    op_gen_iter = --deadlist.end();

    structure_reset();
}

void        funcdata::dump_inst()
{
    map<Address, VisitStat>::iterator it, prev_it;
    AssemblyRaw assememit;
    char buf[128];
    FILE *fp;

    sprintf(buf, "%s/%s/inst.txt", d->filename.c_str(), name.c_str());
    fp = fopen(buf, "w");

    assememit.set_fp(fp);

    for (it = visited.begin(); it != visited.end(); it++) {
        if (it != visited.begin()) {
            /* 假如顺序的地址，加上size不等于下一个指令的地址，代表中间有缺漏，多打印一个 空行 */
            if ((prev_it->first + prev_it->second.size) != it->first) {
                fprintf(fp, "\n");
            }
        }

        d->trans->printAssembly(assememit, it->first);

        prev_it = it;
    }

    fclose(fp);
}

void        funcdata::dump_pcode(const char *postfix)
{
    FILE *fp;
    char buf[2048];
    Address prev_addr;
    AssemblyRaw assememit;
    pcodeop *p;

    sprintf(buf, "%s/pcode_%s.txt", get_dir(buf), postfix);
    fp = fopen(buf, "w");

    list<pcodeop *>::iterator iter = deadlist.begin();

    assememit.set_fp(fp);

    for (int i = 0; i < d->trans->numSpaces(); i++) {
        AddrSpace *spc = d->trans->getSpace(i);
        fprintf(fp, "Space[%s, 0x%llx, %c]\n", spc->getName().c_str(), spc->getHighest(), spc->getShortcut());
    }
    fprintf(fp, "ma = mult_addr\n");
    fprintf(fp, "\n");

    for (; iter != deadlist.end(); iter++) {
        p = *iter;
        if (p->flags.dead) continue;

        if (p->get_dis_addr() != prev_addr) {
            if (p->opcode == CPUI_MULTIEQUAL) {
                fprintf(fp, "<tr>"
                    "<td><font color=\"" COLOR_ASM_STACK_DEPTH "\">000</font></td>"
                    "<td><font color=\"" COLOR_ASM_ADDR "\">0000</font></td>"
                    "<td align=\"left\"><font color=\"" COLOR_ASM_INST_MNEM "\">phi--------------</font></td>"
                    "<td align=\"left\"><font color=\"" COLOR_ASM_INST_BODY "\"></font></td></tr>");
            }
            else {
                d->trans->printAssembly(assememit, p->get_dis_addr());
            }
        }

        p->dump(buf, PCODE_DUMP_ALL & ~PCODE_HTML_COLOR);
        fprintf(fp, "%s\n", buf);

        prev_addr = (*iter)->get_dis_addr();
    }

    fclose(fp);
}

void        funcdata::dump_block(FILE *fp, blockbasic *b, int flag)
{
    Address prev_addr;
    pcodeop *p;
    list<pcodeop *>::iterator iter;
    AssemblyRaw assem;
    char obuf[2048];

    assem.set_buf(obuf);

    // 把指令都以html.table的方式打印，dot直接segment fault了，懒的调dot了
    fprintf(fp, "loc_%x [style=\"filled\" fillcolor=%s label=<<table bgcolor=\"white\" align=\"left\" border=\"0\"><tr><td><font color=\"red\">sub_%llx(%d,%d)</font></td></tr>",
        b->sub_id(),
        block_color(b),
        b->get_start().getOffset(),
        b->dfnum,
        b->index);

    iter = b->ops.begin();

    for (p = NULL;  iter != b->ops.end() ; iter++) {
        p = *iter;

        if (p->flags.startinst) {
            assem.set_sp(p->sp);
            d->trans->printAssembly(assem, p->get_dis_addr());
            fprintf(fp, "%s", obuf);
        }

        if (flag) {
            p->dump(obuf, PCODE_DUMP_SIMPLE | PCODE_HTML_COLOR);
            fprintf(fp, "<tr><td></td><td></td><td colspan=\"2\" align=\"left\">%s</td></tr>", obuf);
        }

        prev_addr = p->get_dis_addr();
    }
    fprintf(fp, "</table>>]\n");
}

void        funcdata::dump_djgraph(const char *postfix, int flag)
{
    char obuf[512];

    sprintf(obuf, "%s/djgraph_%s.dot", get_dir(obuf), postfix);

    FILE *fp = fopen(obuf, "w");
    if (NULL == fp) {
        printf("fopen failure %s", obuf);
        exit(0);
    }

    fprintf(fp, "digraph G {\n");
    fprintf(fp, "node [fontname = \"helvetica\"]\n");

    int i, j;
    for (i = 0; i < bblocks.blist.size(); ++i) {
        blockbasic *b = bblocks.blist[i];

        dump_block(fp, b, flag);
    }

    for (i = 0; i < bblocks.blist.size(); ++i) {
        blockbasic *b = bblocks.blist[i];
        blockbasic *idom = b->immed_dom;

        if (idom)
            fprintf(fp, "loc_%x ->loc_%x [color=\"red\" penwidth=3]\n", idom->sub_id(), b->sub_id());

        if (!idom || idom->flags.f_mark) continue;
        idom->flags.f_mark = 1;
        for (j = 0; j < idom->out.size(); ++j) {
            blockedge *e = &idom->out[j];

            if (e->point->immed_dom != idom)
                fprintf(fp, "loc_%x ->loc_%x [label = \"J\" color=\"blue\" penwidth=2]\n",
                    idom->sub_id(), e->point->sub_id());
        }
    }
    clear_blocks_mark();

    fprintf(fp, "}");
    fclose(fp);
}

char*       funcdata::edge_color(blockedge *e)
{
    if (e->label & a_tree_edge)     return "red";
    if (e->label & a_back_edge)     return "blue";
    if (e->label & a_cross_edge)    return "black";
    if (e->label & a_forward_edge)  return "green";

    return "black";
}

int         funcdata::edge_width(blockedge *e)
{
    if (e->label & a_tree_edge)     return 1;
    if (e->label & a_back_edge)     return 1;
    if (e->label & a_cross_edge)    return 1;
    if (e->label & a_forward_edge)  return 1;

    return 1;
}

char*       funcdata::block_color(flowblock *b)
{
    list<pcodeop *>::iterator iter = b->ops.end();
    if (b->flags.f_entry_point)     return "red";

    iter--;
    if ((*iter)->opcode == CPUI_RETURN)     return "blue";

    return "white";
}

void        funcdata::dump_cfg(const string &name, const char *postfix, int dumppcode)
{
    char obuf[512];

    sprintf(obuf, "%s/cfg_%s_%s.dot", get_dir(obuf), name.c_str(), postfix);

    FILE *fp = fopen(obuf, "w");
    if (NULL == fp) {
        printf("fopen failure %s", obuf);
        exit(0);
    }

    fprintf(fp, "digraph G {\n");
    fprintf(fp, "node [fontname = \"helvetica\"]\n");

    int i, j;
    for (i = 0; i < bblocks.blist.size(); ++i) {
        blockbasic *b = bblocks.blist[i];

        dump_block(fp, b, dumppcode);
    }

    for (i = 0; i < bblocks.blist.size(); ++i) {
        blockbasic *b = bblocks.blist[i];

        for (j = 0; j < b->out.size(); ++j) {
            blockedge *e = &b->out[j];

                fprintf(fp, "loc_%x ->loc_%x [label = \"%s\" color=\"%s\" penwidth=%d]\n",
                    b->sub_id(), e->point->sub_id(),  e->is_true() ? "true":"false", edge_color(e), edge_width(e));
        }
    }

    fprintf(fp, "}");

    fclose(fp);
}

varnode*    funcdata::new_coderef(const Address &m)
{
    varnode *vn;

    vn = new_varnode(1, m);
    vn->flags.annotation = 1;
    return vn;
}

varnode*    funcdata::new_unique(int s)
{
    Address addr(d->get_uniq_space(), vbank.uniqid);

    vbank.uniqid += s;

    return create_vn(s, addr);
}

varnode*    funcdata::new_unique_out(int s, pcodeop *op)
{
    varnode* vn = create_def_unique(s, op);

    op->set_output(vn);

    return vn;
}

varnode*    funcdata::clone_varnode(const varnode *vn)
{
    varnode *newvn = new_varnode(vn->size, vn->loc);

    newvn->flags.annotation = vn->flags.annotation;
    newvn->flags.readonly = vn->flags.readonly;
    newvn->flags.virtualnode = vn->flags.virtualnode;

    return newvn;
}

void        funcdata::destroy_varnode(varnode *vn)
{
    list<pcodeop *>::const_iterator iter;

    for (iter = vn->uses.begin(); iter != vn->uses.end(); ++iter) {
        pcodeop *op = *iter;
        /* 
        1. clear_block_phi在清理multi节点的use时，会把out的use中被处理过的置空
        */
        if (!op) continue;

        op->inrefs[op->get_slot(vn)] = NULL;
    }

    if (vn->def) {
        vn->def->output = NULL;
        vn->def = NULL;
    }

    vn->uses.clear();
    delete_varnode(vn);
}

void        funcdata::delete_varnode(varnode *vn)
{
    if (vn->def) {
        printf("warn:try to remove varnode have def[%d] forbidden. %s:%d\n", vn->def->start.getTime(), __FILE__,__LINE__);
        return;
    }

    loc_tree.erase(vn->lociter);
    def_tree.erase(vn->defiter);
    delete(vn);
}

varnode*    funcdata::set_input_varnode(varnode *vn)
{
    varnode *v1;
    if (vn->flags.input) return vn;

    if (caller && (v1 = callop->callctx->get_vn(vn->get_addr()))) {
        vn->type = v1->type;
    }
    else if (vn->get_addr() == d->sp_addr) {
        vn->set_rel_constant(d->sp_addr, 0);
    }

    vn->flags.input = 1;
    return vn;
}

pcodeop*    funcdata::cloneop(pcodeop *op, const SeqNum &seq)
{
    int i;
    pcodeop *newop1 = newop(op->inrefs.size(), seq);
    op_set_opcode(newop1, op->opcode);

    newop1->flags.startinst = op->flags.startinst;
    newop1->flags.startblock = op->flags.startblock;
    if (op->output)
        op_set_output(newop1, clone_varnode(op->output));
    
    for (i = 0; i < op->inrefs.size(); i++)
        op_set_input(newop1, clone_varnode(op->get_in(i)), i);

    newop1->callfd = op->callfd;

    return newop1;
}

void        funcdata::op_destroy_raw(pcodeop *op)
{
    int i;

    for (i = 0; i < op->inrefs.size(); i++)
        destroy_varnode(op->inrefs[i]);
    if (op->output)
        destroy_varnode(op->output);

    optree.erase(op->start);
    deadlist.erase(op->insertiter);
}

void        funcdata::op_destroy(pcodeop *op)
{
    int i;

    if (op->start.getTime() == 8978) {
        printf("a\n");
    }

    if (op->output)
        destroy_varnode(op->output);

    for (i = 0; i < op->num_input(); ++i) {
        varnode *vn = op->get_in(i);
        if (vn)
            op_unset_input(op, i);
    }

    if (op->parent) {
        mark_dead(op);
        op->parent->remove_op(op);
    }
}

void        funcdata::op_destroy_ssa(pcodeop *p)
{
    varnode *out = p->output;
    list<pcodeop *>::iterator it;
    list<pcodeop *> copy = out->uses;
    pcodeop *use;
    int slot;

    it = copy.begin();
    for (; it != copy.end(); it++) {
        use = *it;
        if (use == p) continue;

        slot = use->get_slot(out);
        assert(slot >= 0);
        op_set_input(use, new_varnode(out->size, out->get_addr()), slot);
    }

    op_destroy(p);
}

void        funcdata::inline_clone(funcdata *inlinefd, const Address &retaddr)
{
    list<pcodeop *>::const_iterator iter;

    for (iter = inlinefd->deadlist.begin(); iter != inlinefd->deadlist.end(); ++iter) {
        pcodeop *op = *iter;
        pcodeop *cloneop1;
        if ((op->opcode == CPUI_RETURN) && !retaddr.isInvalid()) {
            cloneop1 = newop(1, op->start);
            op_set_opcode(cloneop1, CPUI_BRANCH);
            varnode *vn = new_coderef(retaddr);
            op_set_input(cloneop1, vn, 0);
        }
        else
            cloneop1 = cloneop(op, op->start);
    }

    visited.insert(inlinefd->visited.begin(), inlinefd->visited.end());
}

void        funcdata::inline_call(string name, int num)
{
    list<func_call_specs *>::iterator iter = qlst.begin();

    for (; iter != qlst.end(); iter++) {
        if (name.empty() || (*iter)->get_name() == name) {
            inline_call((*iter)->get_addr(), num);
            break;
        }
    }
}

void        funcdata::inline_call(const Address &addr, int num)
{
    int del;
    while (num--) {
        list<func_call_specs *>::iterator iter = qlst.begin();

        for (del = 0; iter != qlst.end(); iter++) {
            func_call_specs *cs = *iter;
            if ((*iter)->get_addr() == addr) {
                qlst.erase(iter);
                inline_flow(cs->fd, cs->op);
                delete cs;
                del = 1;
                break;
            }
        }

        if (!del && (iter == qlst.end()))
            break;
    }
}

void        funcdata::inline_ezclone(funcdata *fd, const Address &calladdr)
{
    list<pcodeop *>::const_iterator iter;
    for (iter = fd->deadlist.begin(); iter != fd->deadlist.end(); iter++) {
        pcodeop *op = *iter;
        if (op->opcode == CPUI_RETURN)
            break;

        // 这里原先的Ghidra把inline的所有地址都改成了calladdr里的地址，这个地方我不理解，
        // 这里inline函数的地址统统不改，使用原先的
        //SeqNum seq(calladdr, op->start.getTime());
        SeqNum seq(op->start);
        cloneop(op, seq);
    }

    visited.insert(fd->visited.begin(), fd->visited.end());
}

void        funcdata::inline_flow(funcdata *fd1, pcodeop *callop)
{
    funcdata fd(fd1->name.c_str(), fd1->get_addr(), fd1->size, fd1->d);
    pcodeop *firstop;

    if (callop->flags.inlined)
        throw LowlevelError("callop already be inlined");

    callop->flags.inlined = 1;

    /* inline的fd，从父函数里的op_uniq开始，这样可以保证再inline后，uniq是连续的 */
    fd.set_op_uniqid(get_op_uniqid());
    fd.set_range(fd1->baddr, fd1->eaddr);
    fd.generate_ops_start();

    if (fd.check_ezmodel()) {
        list<pcodeop *>::const_iterator oiter = deadlist.end();
        --oiter;
        inline_ezclone(&fd, callop->get_addr());
        ++oiter;    // 指向inline的第一个pcode

        if (oiter != deadlist.end()) {
            firstop = *oiter;
            oiter = deadlist.end();
            --oiter; // 指向inline的最后一个pcode
            pcodeop *lastop = *oiter;
            if (callop->flags.startblock)
                firstop->flags.startblock = 1;
            else
                firstop->flags.startblock = 0;
        }
        /* FIXME:对单block的节点，要不要也加上对inline节点的跳转，在后期的merge阶段合并掉？ */
    }
    else {
        Address retaddr;
        if (!test_hard_inline_restrictions(&fd, callop, retaddr))
            return;

        inline_clone(&fd, retaddr);

        vector<jmptable *>::const_iterator iter;
        for (iter = fd.jmpvec.begin(); iter != fd.jmpvec.end(); iter++) {
            jmptable *jtclone = new jmptable(*iter);
            jmpvec.push_back(jtclone);
            jtclone->update(this);
        }

        while (callop->num_input() > 1)
            op_remove_input(callop, callop->num_input() - 1);

        op_set_opcode(callop, CPUI_BRANCH);
        varnode *inlineaddr = new_coderef(fd.get_addr());
        op_set_input(callop, inlineaddr, 0);
    }

    /* qlst用来存放所有函数call的链表，当inline一个函数时，需要把它的call列表inline进来 */
    list<func_call_specs *>::const_iterator it = fd.qlst.begin();
    for (; it != fd.qlst.end(); it++) {
        pcodeop *op = find_op((*it)->op->start);
        func_call_specs *cs = new func_call_specs(op, (*it)->fd);

        qlst.push_back(cs);
    }

    /* 原始的Ghidra中，inline只能inline call，但是因为branch也可以模拟call，所以branch也能inline的 */
    if (callop->opcode == CPUI_CALL) {
    }

    generate_blocks();
}

void        funcdata::cond_inline(funcdata *inlinefd, pcodeop *callop)
{
    funcdata fd(inlinefd->name.c_str(), inlinefd->get_addr(), 0, d);
    char buf[32];
    bool ez = false;

    if (callop->flags.inlined)
        throw LowlevelError("callop already be inlined");

    printf("try cond inline p%d[%llx, %s]\n", callop->start.getTime(), callop->callfd->get_addr().getOffset(), callop->callfd->name.c_str());

    /* inline的fd，从父函数里的op_uniq开始，这样可以保证再inline后，uniq是连续的 */
    fd.set_caller(this, callop);
    fd.set_op_uniqid(get_op_uniqid());
    fd.set_user_offset(get_user_offset());
    fd.set_virtualbase(get_virtualbase());
    fd.set_range(inlinefd->baddr, inlinefd->eaddr);
    fd.generate_ops_start();
    fd.generate_blocks();
    fd.heritage();

    fd.cond_pass();

    sprintf(buf, "cond_%d_2", callop->start.getTime());
    fd.dump_cfg(fd.name, buf, 1);

    assert(1 == fd.bblocks.get_size());

    flowblock *b = fd.bblocks.get_block(0);
    list<pcodeop *>::iterator it = b->ops.begin();

    for (; it != b->ops.end(); it++) {
        pcodeop *p = *it;
        pcodeop *op;

        if (p->opcode == CPUI_RETURN) break;

        Address addr2(d->get_code_space(), user_offset + p->get_addr().getOffset());
        SeqNum seq(addr2, op_uniqid++);
        op = cloneop(p, seq);
        op->disaddr = new Address (p->get_addr());

        op_insert(op, callop->parent, callop->basiciter);
    }

    op_destroy(callop);
}


void        funcdata::cond_pass(void)
{
    int changed = 1;
    pcodeop *p;

    constant_propagation(0);

    int g_time = 0;

    while (changed) {
        changed = 0;

        if (!cbrlist.empty()) {
            cond_constant_propagation();
            /* 发生了条件常量传播以后，整个程序的结构发生了变化，整个结构必须得重来 */
            changed = 1;

            heritage_clear();
            heritage();
            dead_code_elimination(bblocks.blist);

#if 1
            if (g_time > 3) {
                char buf[128];
                sprintf(buf, "_inline%d", g_time);
                dump_cfg(name, buf, 1);
                exit(0);
            }
#endif

            continue;
        }

        if ((p = bblocks.first_callop_vmp(NULL))) {
            changed = 1;
            /* FIXME:这里判断的过于粗糙，还需要判断整个p->parent是否再循环内*/
            if (bblocks.is_dowhile(p->parent)) {
                vector<flowblock *> v;
                v.push_back(p->parent);
                dowhile2ifwhile(v);
                p = bblocks.first_callop_vmp(NULL);

                heritage_clear();
                heritage();
            }

            cond_inline(p->callfd, p);

            g_time++;

            heritage_clear();
            heritage();

            continue;
        }
    }

    dead_code_elimination(bblocks.blist);

    if (this->callop->parent->fd->callop == NULL) {
        dump_cfg(name, "check", 1);
        exit(1);
    }
}

void        funcdata::set_caller(funcdata *caller1, pcodeop *callop1)
{
    caller = caller1;
    callop = callop1;
}

bool        funcdata::check_ezmodel(void)
{
    list<pcodeop *>::const_iterator iter = deadlist.begin();

    while (iter != deadlist.end()) {
        pcodeop *op = *iter;
        //if (op->flags.call || op->flags.branch)
        if ((op->opcode == CPUI_BRANCH) || (op->opcode == CPUI_CBRANCH))
            return false;

        ++iter;
    }

    return true;
}

/* 对于当前的cfg，重新计算 loop结构和支配关系 
cfg发生改变以后，整个loop结构和支配关系都需要重新计算，
并且一次改变，可能需要重新计算多次
*/
void funcdata::structure_reset()
{
    vector<flowblock *> rootlist;

    flags.blocks_unreachable = 0;

    bblocks.structure_loops(rootlist);
    bblocks.calc_forward_dominator(rootlist);
}

/* build dominator tree*/
void funcdata::build_dom_tree()
{
}

void        funcdata::clear_blocks()
{
    if (!flags.blocks_generated)
        return;

    bblocks.clear();

    flags.blocks_generated = 0;
}

void        funcdata::clear_blocks_mark()
{
    int i;

    for (i = 0; i < bblocks.blist.size(); i++)
        bblocks.blist[i]->flags.f_mark = 0;
}

void        funcdata::follow_flow(void)
{
    char buf[128];
    sprintf(buf, "%s/%s", d->filename.c_str(), name.c_str());
    mdir_make(buf);

    generate_ops_start();
    generate_blocks();
}

void        funcdata::start_processing(void)
{
    flags.processing_started = 1;
}

void        funcdata::visit_incr(flowblock *qnode, flowblock *vnode)
{
    int i, j, k;
    flowblock *v, *child;
    vector<flowblock *>::iterator   iter, enditer;

    i = vnode->index;
    j = qnode->index;
    iter = augment[i].begin();
    enditer = augment[i].end();

    for (; iter != enditer; ++iter) {
        v = *iter;
        if (v->immed_dom->index < j) {
            k = v->index;
            if ((phiflags[k] & merged_node) == 0) {
                merge.push_back(v);
                phiflags[k] |= merged_node;
            }

            if ((phiflags[k] & mark_node) == 0) {
                phiflags[k] |= mark_node;
                pq.insert(v, domdepth[k]);
            }
        }
        else
            break;
    }

    if ((phiflags[i] && boundary_node) == 0) {
        for (j = 0; j < domchild[i].size(); ++j) {
            child = domchild[i][j];
            if ((phiflags[child->index] & mark_node) == 0)
                visit_incr(qnode, child);
        }
    }
}

int funcdata::collect(Address addr, int size, vector<varnode *> &read, vector<varnode *> &write, vector<varnode *> &input)
{
    varnode *vn;
    varnode_loc_set::const_iterator     viter = begin_loc(addr);
    varnode_loc_set::const_iterator     enditer;
    uintb start = addr.getOffset();
    addr = addr + size;

    if (addr.getOffset() < start) {
        assert(0);
    }
    else
        enditer = begin_loc(addr);

    int maxsize = 0;

    for (; viter != enditer; viter++) {
        vn = *viter;

        if (vn->flags.written) {
            if (vn->size > maxsize)     maxsize = vn->size;
            write.push_back(vn);
        }
        else if (!vn->is_heritage_known() && vn->uses.size())
            read.push_back(vn);
        else if (vn->flags.input)
            input.push_back(vn);
    }

    return maxsize;
}

void        funcdata::heritage(void)
{
    varnode_loc_set::const_iterator iter, enditer;
    varnode *vn;
    int i;

    if (maxdepth == -1)
        build_adt();

    printf("%sheritage scan node.... \n", print_indent());
    for (i = 0; i < d->trans->numSpaces(); i++) {
        AddrSpace *space = d->trans->getSpace(i);

        iter = begin_loc(space);
        enditer = end_loc(space);

        while (iter != enditer) {
            vn = *iter++;

            if (!vn->flags.written && vn->has_no_use() && !vn->flags.input)
                continue;

#if 0
            char buf[128];
            print_varnode(d->trans, buf, vn);
            if (vn->def)
                printf("%s - %d\n", buf, vn->def->start.getTime());
            else
                printf("%s\n", buf);
#endif

            int prev = 0;
            LocationMap::iterator iter = globaldisjoint.add(vn->get_addr(), vn->size, pass, prev);
            if (prev == 0)
                disjoint.add((*iter).first, (*iter).second.size, pass, prev);
            else {
                assert(0);
            }
        }
    }
    place_multiequal();
    rename();

    constant_propagation(0);

    if (callop) {
        char buf[128];
        sprintf(buf, "cond_%d_orig", callop->start.getTime());
        dump_cfg(name, buf, 1);
    }

    alias_analysis();
    constant_propagation(1);
    printf("%sheritage scan node end. \n", print_indent());
}

void    funcdata::heritage_clear()
{
    topname.clear();
    disjoint.clear();
    globaldisjoint.clear();
    domchild.clear();
    augment.clear();
    phiflags.clear();
    domdepth.clear();
    merge.clear();
    alias_clear();

    maxdepth = -1;
    pass = 0;
}

int     funcdata::constant_propagation(int listype)
{
    list<pcodeop *>::const_iterator iter;
    list<pcodeop *> w = listype ? safe_aliaslist:deadlist;
    list<pcodeop *>::const_iterator iter1;
    int ret = 0;
    flowblock *b;

    while (!w.empty()) {
        iter = w.begin();
        pcodeop *op = *iter;
        w.erase(iter);

        if (op->flags.dead) continue;

        ret |= op->compute(-1, &b, &w);

        varnode *out = op->output;
        if (!out) continue;

        if (out->is_constant() || out->is_rel_constant()) {
            for (iter1 = out->uses.begin(); iter1 != out->uses.end(); ++iter1) {
                pcodeop *use = *iter1;
                w.push_back(use);
            }
        }
    }

    return ret;
}

int         funcdata::cond_constant_propagation()
{
    flowblock *parent, *to;
    blockedge *del_edge;
    pcodeop *op;
    varnode *in;
    int i;

    for (i = 0; i < cbrlist.size(); i++) {
        op = cbrlist[i];
        parent = op->parent;

        //printf("%sfind cbr block:%llx:%d\n", print_indent(), op->get_addr().getOffset(), parent->dfnum);

        in = op->get_in(1);
        assert(in->is_constant());

        /* 获取要删除的边，所以要取相反条件的边 */
        del_edge = in->get_val() ? parent->get_false_edge() : parent->get_true_edge();
        to = del_edge->point;

        /* 删除cbranch上的条件判断，已经不需要了, 前面定义这个条件的语句也会在后面的死代码删除中去掉*/
        branch_remove_internal(parent, parent->get_out_index(to));

        /* 清楚标记的时候，我们并不在乎删除的是哪条边，反正把a_true_edge的标记清除就可以了 */
        parent->clear_out_edge_flag(0, a_true_edge);
    }

    cbrlist.clear();
    remove_unreachable_blocks(true, true);

    //printf("%safter cbr remove, now blocks size = %d, dead is = %d\n", print_indent(), bblocks.get_size(), bblocks.deadlist.size());

    redundbranch_appy();


    return 0;
}

void    funcdata::compute_sp(void)
{
    if (!flags.blocks_generated)
        throw LowlevelError("compute sp need block generated");
}

/* FIXME:set_safezone 函数和 in_safezone 都使用codespace作为空间，我是乱写的 
因为实际上的AddrSpace基本不参与太具体的运算所以填啥基本都ok，只要不写NULL, 
我这里实际上想写的是 StackSpace，但是我没找到这么一个StackSpace;
*/
void        funcdata::set_safezone(intb addr, int size)
{
    addr += safezone_base;
    safezone.insertRange(d->get_code_space(), addr, addr + size);
}

bool        funcdata::in_safezone(intb a, int size)
{
    a += safezone_base;

    Address addr(d->get_code_space(), a);
    return safezone.inRange(addr, size);
}

void        funcdata::enable_safezone(void)
{
    flags.safezone = 1;
}

void        funcdata::disable_safezone(void)
{
    flags.safezone = 0;
}

intb        funcdata::get_stack_value(intb offset, int size)
{
    u1 *p = memstack.bottom + offset;

    if ((offset > 0) || (offset + size - 1) > 0)
        throw LowlevelError(name + " memstack downflow");

    if (size & (size - 1))
        throw LowlevelError("get_stack_value not support size:" + size);

    if (size == 8)
        return *(intb *)p;
    else if (size == 4)
        return *(int *)p;
    else if (size == 2)
        return *(short *)p;
    else
        return p[0];
}

void        funcdata::set_stack_value(intb offset, int size, intb val)
{
    u1 *p = memstack.bottom + offset;

    if ((offset > 0) || (offset + size - 1) > 0)
        throw LowlevelError(name + " memstack downflow");

    memcpy(p, (char *)&val, size);
}

void        funcdata::add_to_codelist(pcodeop *op)
{
    switch (op->opcode) {
    case CPUI_STORE:
        op->codeiter = storelist.insert(storelist.end(), op);
        break;

    case CPUI_LOAD:
        op->codeiter = loadlist.insert(loadlist.end(), op);
        break;

    default:break;
    }
}

void        funcdata::remove_from_codelist(pcodeop *op)
{
    switch (op->opcode) {
    case CPUI_LOAD:
        loadlist.erase(op->codeiter);
        break;

    case CPUI_STORE:
        storelist.erase(op->codeiter);
    }
}

void        funcdata::calc_load_store_info()
{
}

bool        funcdata::test_hard_inline_restrictions(funcdata *inlinefd, pcodeop *op, Address &retaddr)
{
    list<pcodeop *>::iterator iter = op->insertiter;
    ++iter;

    /* 理论上即使指向end也应该可以inline的，但是这里为了处理方便，不关心末尾的特殊情况 */
    if (iter == deadlist.end())
        throw LowlevelError("No fallthrough prevents inline here");
 
    pcodeop *nextop = *iter;
    retaddr = nextop->get_addr();

    nextop->flags.startblock = 1;

    return true;
}

bool        funcdata::is_first_op(pcodeop *op)
{
    if (!flags.blocks_generated)
        return op == (*deadlist.begin());

    flowblock *b = bblocks.get_block(0);
    list<pcodeop *>::iterator it = b->ops.begin();

    return *it == op;
}

pcodeop*    funcdata::loop_pre_get(flowblock *h, int index)
{
    flowblock *pre = NULL;
    int i, sizein = h->in.size();

    /* 查找入口节点的非循环前驱节点，也就是哪个节点可以进来 */
    for (i = 0; i < sizein; i++) {
        pre = h->get_in(i);
        
        if (pre->dfnum < h->dfnum)
            return pre->last_op();
    }

    return NULL;
}

bool        funcdata::trace_push(pcodeop *op)
{
    flowblock *bb;
    list<pcodeop *>::const_iterator it;

    if (trace.empty()) {
        bb = op->parent;

        for (it = bb->ops.begin(); it != bb->ops.end(); it++) {
            trace_push_op(*it);
        }
    }
    else
        trace_push_op(op);

    return true;
}

void        funcdata::trace_push_op(pcodeop *op)
{
    trace.push_back(op);

    switch (op->opcode) {
    case CPUI_STORE:
        aliaslist.push_back(op);
        break;

    case CPUI_LOAD:
        aliaslist.push_back(op);
        break;

    default:break;
    }
}

void        funcdata::trace_clear()
{
    trace.clear();
    aliaslist.clear();
}

pcodeop*    funcdata::trace_load_query(varnode *vn)
{
    list<pcodeop *>::reverse_iterator it;
    pcodeop *p;
    varnode *in;

    for (it = aliaslist.rbegin(); it != aliaslist.rend(); it++) {
        p = *it;
        if (p->opcode == CPUI_LOAD) {
            in = p->get_in(1);
            if (in->type == vn->type)
                return p;
        }
    }

    return NULL;
}

pcodeop*    funcdata::trace_store_query(varnode *vn)
{
    list<pcodeop *>::reverse_iterator it;
    pcodeop *p;
    varnode *in;

    for (it = aliaslist.rbegin(); it != aliaslist.rend(); it++) {
        p = *it;
        if (p->opcode == CPUI_STORE) {
            in = p->get_in(1);
            if (in->type == vn->type)
                return p;
        }
    }

    return NULL;
}

pcodeop*    funcdata::store_query(pcodeop *load)
{
    list<pcodeop *>::reverse_iterator it = load->parent->get_rev_iterator(load);
    flowblock *b = load->parent, *bb;
    varnode *pos = load->get_in(1);
    pcodeop *p;

    while (1) {
        for (; it != b->ops.rend(); it++) {
            p = *it;

            if (p->callfd && p->callfd->have_side_effect())
                return NULL;
            if (p->opcode != CPUI_STORE) continue;

            varnode *a = p->get_in(1);

            if (a->type == pos->type)
                return p;
        }

        if (b->is_entry_point()) {
            if (b->fd->caller) {
                pcodeop *p1 = b->fd->callop;
                b =  b->fd->callop->parent;
                it = b->get_rev_iterator(p1);
                // skip , FIXME:没有处理当这个op已经为第一个的情况
                ++it;
            }
            else
                break;
        }
        else if (b->in.size() > 1){
            flowblock *dom = b->immed_dom;
            vector<flowblock *> stack;
            vector<int> visited;

            visited.clear();
            visited.resize(b->fd->bblocks.get_size());

            for (int i = 0; i < b->in.size(); i++) {
                stack.push_back(b->get_in(i));
            }

            while (stack.empty()) {
                b = stack.back();
                stack.pop_back();

                if (visited[b->dfnum])
                    continue;

                visited[b->dfnum] = 1;

                for (it = b->ops.rbegin(); it != b->ops.rend(); it++) {
                    p = *it;

                    if (p->callfd && p->callfd->have_side_effect())
                        return NULL;

                    if (p->opcode == CPUI_STORE) {
                        varnode *a = p->get_in(1);

                        /* 在分支中找到了store节点，直接抛弃 */
                        if (a->type == pos->type)
                            return NULL;
                    }
                }

                for (int i = 0; i < b->in.size(); i++) {
                    bb = b->get_in(i);
                    if (bb == dom)
                        continue;

                    stack.push_back(bb);
                }
            }

            b = dom;
            it = b->ops.rbegin();
       }
        else {
            b = b->get_in(0);
            it = b->ops.rbegin();
        }
    }

    return NULL;
}

bool       funcdata::loop_unrolling(flowblock *h, int times)
{
    int unroll_times = 0;
    int i, inslot, ret;
    flowblock *start,  *cur, *prev, *br, *web;
    list<pcodeop *>::const_iterator it;
    const SeqNum sq;
    pcodeop *p, *op;
    int unrolling;

    printf("\n\nloop_unrolling sub_%llx \n", h->get_start().getOffset());

    /* loop execute */
    while (times--) {
        /* 取loop的进入节点*/
        prev = start = loop_pre_get(h, 0)->parent;
        /* FIXME:压入trace堆栈 
        这种压入方式有问题，无法识别 undefined bcond
        */
        trace_push(start->last_op());
        cur = h;

        unrolling = 0;
        do {
            printf("\tprocess flowblock sub_%llx\n", cur->get_start().getOffset());

            it = cur->ops.begin();
            inslot = cur->get_inslot(prev);
            assert(inslot >= 0);

            for (; it != cur->ops.end(); it++) {
                p = *it;

                br = NULL;
                ret = p->compute(inslot, &br, NULL);

#if 0
                char buf[256];
                p->dump(buf, PCODE_DUMP_SIMPLE & ~PCODE_HTML_COLOR);
                printf("%s\n", buf);
#endif

                if (p->opcode == CPUI_CALL) {
                    unrolling = 1;
                    break;
                }

                trace_push(p);
            }

            if (unrolling)
                break;

            if ((cur->out.size() > 1) && (ret != ERR_MEET_CALC_BRANCH)) {
                throw LowlevelError("loop_unrolling meet unsupport branch");
            }

            prev = cur;
            cur = br;
        } while (cur != h);
    }

    cur = trace.back()->parent;
    while (trace.back()->parent == cur) {
        trace.pop_back();
    }

    /* 把刚才走过的路径复制出来，剔除jmp节点，最后一个节点的jmp保留 */
    br = trace.back()->parent;
    cur = bblocks.new_block_basic(this);
    user_offset += user_step;
    Address addr(d->get_code_space(), user_offset);
    /* 进入节点抛弃 */
    for (i = 0; trace[i]->parent == start; i++);
    /* 从主循环开始 */
    for (; i < trace.size(); i++) {
        p = trace[i];
        /* 最后一个节点的jmp指令不删除 */
        if ((i != trace.size() - 1) 
            && (p->opcode == CPUI_BRANCH) || (p->opcode == CPUI_CBRANCH) || (p->opcode == CPUI_INDIRECT) || (p->opcode == CPUI_MULTIEQUAL) || (p->opcode == CPUI_BRANCHIND))
            continue;

        Address addr2(d->get_code_space(), user_offset += p->get_addr().getOffset());
        const SeqNum sq(addr2, op_uniqid++);
        op = cloneop(p, sq);
        op->disaddr = new Address(p->get_dis_addr());
        op_insert(op, cur, cur->ops.end());
    }
    cur->set_initial_range(addr, addr);

    vector<flowblock *> cloneblks;
    web = clone_web(br->get_out(0), h, cloneblks);
    cloneblks.push_back(cur);

    bblocks.add_edge(cur, web);

    /* 删除start节点和loop 头节点的边，连接 start->cur->loop_header */
    bblocks.remove_edge(start, h);
    bblocks.add_edge(start, cur);

    structure_reset();

    clear_block_phi(h);

    heritage_clear();
    heritage();

    dead_code_elimination(cloneblks);

    p = bblocks.first_callop_vmp(h);

    if (p) {
        /* FIXME:我们暂时只处理简单的dowhile情形，实际上要做condinline的block，当前不能允许在任何dowhile块内，
        你需要判断自己是不是在循环内，假如是的话，就要一直展开*/
        if (bblocks.is_dowhile(p->parent)) {
            vector<flowblock *> v;
            v.push_back(p->parent);
            p = dowhile2ifwhile(v)->first_callop();
            heritage_clear();
            heritage();
        }

        cond_inline(p->callfd, p);
    }

    return true;
}

void        funcdata::dead_code_elimination(vector<flowblock *> blks)
{
    flowblock *b;
    list<pcodeop *>::iterator it;
    list<pcodeop *> worklist;
    vector<flowblock *> marks;
    pcodeop *op;
    int i;

    marks.clear();
    marks.resize(bblocks.get_size());

    for (i = blks.size() - 1; i >= 0; i--) {
        b = blks[i];

        marks[b->dfnum] = b;

        for (it = b->ops.begin(); it != b->ops.end(); it++) {
            op = *it;
            if (op->output && op->output->has_no_use()) {
                worklist.push_back(op);
            }
        }
    }

    while (!worklist.empty()) {
        it = worklist.begin();
        op = *it;
        worklist.erase(it);

        if (op->flags.dead) continue;
        //printf("delete pcode = %d\n", op->start.getTime());

        if (!op->output->has_no_use()) continue;
        /*
        FIXME:暂时不允许删除store命令

        store在某些情况下被赋予了 virtual节点以后，会和load关联起来
        并生成了一个虚拟out节点，当这个节点没人使用时，也不允许删除
        store只有一种情况下可以删除，就是

        1. mem[x] = 1;
        2. mem[x] = 2;

        对同一地址的写入会导致上一个写入失效，这中间不能有上一个地址的访问
        */
        if (op->opcode == CPUI_STORE) continue;

        pcodeop_def_set::iterator it1 = topname.find(op);
        if ((it1 != topname.end()) && (*it1 == op))
            continue;

        for (int i = 0; i < op->num_input(); i++) {
            varnode *in = op->get_in(i);
            /* 有些varnode节点是常量传播出来得常量，有些天然常量 */
            if (in->get_addr().isConstant()) continue;
            /* 输入节点是没有def的 */
            if (in->is_input()) continue;
            worklist.push_back(in->def);
        }

        if (marks[op->parent->dfnum])
            op_destroy(op);
    }
}

bool        funcdata::is_code(varnode *v) 
{ 
    return (v->loc.getSpace()->getType() == IPTR_CONSTANT) && (v->loc.getOffset() == (uintb)(d->trans->getDefaultCodeSpace())); 
}

bool        funcdata::is_sp_rel_constant(varnode *v)
{
    return v->is_rel_constant() && (v->get_rel() == d->sp_addr);
}

void        funcdata::place_multiequal(void)
{
    LocationMap::iterator iter;
    vector<varnode *> readvars;
    vector<varnode *> writevars;
    vector<varnode *> inputvars;
    pcodeop *multiop;
    varnode *vnin;
    blockbasic *bl;
    int max, i, j;

    for (iter = disjoint.begin(); iter != disjoint.end(); ++iter) {
        Address addr = (*iter).first;
        int size = (*iter).second.size;

        readvars.clear();
        writevars.clear();
        inputvars.clear();
        max = collect(addr, size, readvars, writevars, inputvars);
        if ((size > 4) && (max < size)) {
        }

        /* FIXME:后面那个判断我没看懂，抄Ghidra的 */
        if (readvars.empty() && (addr.getSpace()->getType() == IPTR_INTERNAL))
            continue;

        if (readvars.empty() && writevars.empty())
            continue;

        calc_phi_placement(writevars);
        for (i = 0; i < merge.size(); ++i) {
            bl = merge[i];
            multiop = newop(bl->in.size(), bl->get_start());
            varnode *vnout = new_varnode_out(size, addr, multiop);

            op_set_opcode(multiop, CPUI_MULTIEQUAL);
            for (j = 0; j < bl->in.size(); j++) {
                vnin = new_varnode(size, addr);
                op_set_input(multiop, vnin, j);
            }

            op_insert_begin(multiop, bl);
        }
    }

    merge.clear();
}

void        funcdata::rename()
{
    variable_stack varstack;

    rename_recurse(bblocks.get_block(0), varstack);

    disjoint.clear();
}

void        funcdata::rename_recurse(blockbasic *bl, variable_stack &varstack)
{
    /* 当前block内，被def过得varnode集合 */
    vector<varnode *> writelist;
    blockbasic *subbl;
    list<pcodeop *>::iterator oiter, suboiter;
    pcodeop *op, *multiop;
    varnode *vnout, *vnin, *vnnew;
    int i, slot, j = 0;

    for (oiter = bl->ops.begin(); oiter != bl->ops.end(); oiter++) {
        op = *oiter ;
        op->start.setOrder(j++);

        if (op->opcode != CPUI_MULTIEQUAL) {
            if ((op->opcode == CPUI_COPY) && (op->output->get_addr() == op->get_in(0)->get_addr())) {
                oiter--;
                op_destroy_ssa(op);
                continue;
            }

            for (slot = 0; slot < op->inrefs.size(); ++slot) {
                vnin = op->get_in(slot);

                if (vnin->flags.annotation || vnin->is_constant())
                    continue;

                vector<varnode *> &stack(varstack[vnin->get_addr()]);
                if (stack.empty()) {
                    vnnew = new_varnode(vnin->size, vnin->get_addr());
                    vnnew = set_input_varnode(vnnew);
                    vnnew->version = stack.size();
                    stack.push_back(vnnew);
                }
                else 
                    vnnew = stack.back();

                if (vnnew->flags.written && (vnnew->def->opcode == CPUI_INDIRECT)) {
                }

                op_set_input(op, vnnew, slot);
                if (vnin->has_no_use()) {
                    delete_varnode(vnin);
                }
            }
        }

        if (op->is_call()) {
            if (op->callctx) delete op->callctx;

            cpuctx *ctx = new cpuctx();
            
            vector<varnode *> *stack = &(varstack[d->r0_addr]);
            if (!stack->empty()) ctx->r0 = stack->back();

            stack = &(varstack[d->r1_addr]);
            if (!stack->empty()) ctx->r1 = stack->back();

            stack = &(varstack[d->r2_addr]);
            if (!stack->empty()) ctx->r2 = stack->back();

            stack = &(varstack[d->r3_addr]);
            if (!stack->empty()) ctx->r3 = stack->back();

            stack = &(varstack[d->lr_addr]);
            if (!stack->empty()) ctx->lr = stack->back();

            stack = &(varstack[d->sp_addr]);
            if (!stack->empty()) ctx->sp = stack->back();

            op->callctx = ctx;
        }

        vnout = op->output;
        if (vnout == NULL) continue;
        vnout->version = varstack[vnout->get_addr()].size();
        varstack[vnout->get_addr()].push_back(vnout);
        writelist.push_back(vnout);
    }

    for (i = 0; i < bl->out.size(); ++i) {
        subbl = bl->get_out(i);
        slot = bl->get_out_rev_index(i);
        for (suboiter = subbl->ops.begin(); suboiter != subbl->ops.end(); suboiter++) {
            multiop = *suboiter;
            if (multiop->opcode != CPUI_MULTIEQUAL)
                break;

            vnin = multiop->get_in(slot);
            if (vnin->is_heritage_known()) continue;

            vector<varnode *> &stack(varstack[vnin->get_addr()]);
            if (stack.empty()) {
                vnnew = new_varnode(vnin->size, vnin->get_addr());
                vnnew = set_input_varnode(vnnew);
                vnnew->version = stack.size();
                stack.push_back(vnnew);
            }
            else
                vnnew = stack.back();

            op_set_input(multiop, vnnew, slot);
            if (!vnin->uses.size()) {
                delete_varnode(vnin);
            }
        }
    }

    i = bl->index;
    for (slot = 0; slot < domchild[i].size(); ++slot)
        rename_recurse(domchild[i][slot], varstack);

    for (i = 0; i < writelist.size(); ++i) {
        vnout = writelist[i];

        vector<varnode *> &stack(varstack[vnout->get_addr()]);
        varnode *v = stack.back();

        if (v->get_addr().getSpace()->getType() == IPTR_PROCESSOR) {
            pair<pcodeop_def_set::iterator, bool> check;
            topname.insert(v->def);
        }

        stack.pop_back();
    }
}

/* calc_multiequal 
Algorithms for Computing the Static Single Assignment Form. P39 
*/
void        funcdata::calc_phi_placement(const vector<varnode *> &write)
{
    pq.reset(maxdepth);
    merge.clear();

    int i, j;
    flowblock *bl;

    for (i = 0; i < write.size(); ++i) {
        bl = write[i]->def->parent;
        j = bl->index;
        if (phiflags[j] & mark_node)
            continue;

        pq.insert(bl, domdepth[j]);
        phiflags[j] |= mark_node;
    }

    if ((phiflags[0] & mark_node) == 0) {
        pq.insert(bblocks.get_block(0), domdepth[0]);
        phiflags[0] |= mark_node;
    }

    while (!pq.empty()) {
        bl = pq.extract();
        visit_incr(bl, bl);
    }

    for (i = 0; i < phiflags.size(); ++i)
        phiflags[i] &= ~(mark_node | merged_node );
}

void        funcdata::calc_phi_placement2(const vector<varnode *> &write)
{
    int i, j;
    flowblock *bl;
    mergedj.clear();

    for (i = 0; i < write.size(); ++i) {
        bl = write[i]->def->parent;
        j = bl->index;

        pq.insert(bl, domdepth[j]);
        bl->flags.f_mark = 0;
    }

    while (!pq.empty()) {
        bl = pq.extract();
        bl->set_mark();
        visit_dj(write, bl);
    }

    for (i = 0; i < bblocks.get_size(); i++) {
        bblocks.blist[i]->flags.f_mark = 0;
    }
}

bool        funcdata::in_mergedj(flowblock *v) 
{
    int i;
    for (i = 0; i < mergedj.size(); i++) {
        if (mergedj[i] == v)
            return true;
    }

    return false;
}

void        funcdata::visit_dj(const vector<varnode *> &s, flowblock *v)
{
    int i, j;

    for (i = 0; i < v->out.size(); i++) {
        flowblock *out = v->get_out(i);

        if (out->immed_dom == v) continue;

        if (domdepth[out->index] >= domdepth[v->index]) {
            if (!in_mergedj(out)) {
                mergedj.push_back(out);

                for (j = 0; j < s.size(); j++) {
                    if (s[j]->def->parent == out)
                        break;
                }

                if (j == s.size())
                    pq.insert(out, domdepth[j]);
            }
        }
    }

    for (i = 0; i < v->out.size(); i++) {
        flowblock *out = v->get_out(i);

        if (out->immed_dom == v) continue;

        if (!out->flags.f_mark) {
            out->set_mark();
            visit_dj(s, out);
        }
    }
}

flowblock*  funcdata::get_vm_loop_header(void)
{
    int i, max_count = -1, t;
    flowblock *max = NULL;

    for (i = 0; i < bblocks.blist.size(); i++) {
        t = bblocks.blist[i]->get_back_edge_count();
        if (t  > max_count) {
            max_count = t;
            max = bblocks.blist[i];
        }
    }

    return max;
}

bool        funcdata::use_outside(varnode *vn)
{
    return false;
}

/* 这里我调整了Ghidra的做法，原始的Ghidra为了更好的兼容性做了很多的考虑 */
void        funcdata::use2undef(varnode *vn)
{
    pcodeop *op;
    list<pcodeop *>::const_iterator iter;
    int i, size;
    bool res;

    res = false;
    size = vn->size;
    iter = vn->uses.begin();
    while (iter != vn->uses.end()) {
        op = *iter++;
        if (op->parent->is_dead()) continue;
        assert(op->parent->in.size());
        i = op->get_slot(vn);

        if (op->opcode != CPUI_MULTIEQUAL)
            throw LowlevelError("use2undef only support CPUI_MULTIEQUAL");

        op_remove_input(op, i);

        /* phi节点的in不能为0 */
        assert(op->inrefs.size());
    }
}

void        funcdata::branch_remove(blockbasic *bb, int num)
{
    branch_remove_internal(bb, num);
    structure_reset();
}

void        funcdata::branch_remove_internal(blockbasic *bb, int num)
{
    blockbasic *bbout;
    list<pcodeop *>::iterator iter;
    int blocknum;

    if (bb->out.size() == 2)
        op_destroy(bb->last_op());

    bbout = (blockbasic *)bb->get_out(num);
    blocknum = bbout->get_in_index(bb);
    bblocks.remove_edge(bb, bbout);
    clear_block_phi(bbout);
#if 0
    for (iter = bbout->ops.begin(); iter != bbout->ops.end(); iter++) {
        op = *iter;
        if (op->opcode != CPUI_MULTIEQUAL) continue;

        /* 当删除一个branch的分支节点时，他的分支里面的phi节点需要清理掉对应的in节点*/
        op_remove_input(op, blocknum);
        op_zero_multi(op);
    }
#endif
}

void        funcdata::block_remove_internal(blockbasic *bb, bool unreachable)
{
    list<pcodeop *>::iterator iter;
    pcodeop *op;
    varnode *deadvn;

    op = bb->last_op();
    if (op) {
        assert(op->opcode != CPUI_BRANCHIND);
    }

    bblocks.remove_from_flow(bb);

    iter = bb->ops.begin();
    while (iter != bb->ops.end()) {
        op = *iter;
        if (op->output) {
            deadvn = op->output;
            if (unreachable) {
                use2undef(deadvn);
            }

            if (use_outside(deadvn))
                throw LowlevelError("deleting op with use");
        }

        iter++;
        op_destroy(op);
    }
    bblocks.remove_block(bb);
}

bool        funcdata::remove_unreachable_blocks(bool issuewarnning, bool checkexistence)
{
    vector<flowblock *> list;
    int i;

#if 0
    for (i = 0; i < bblocks.get_size(); i++) {
        flowblock *blk = bblocks.get_block(i);

        if (blk->is_entry_point())
            continue;

        if ((0 == blk->in.size()) || (NULL == blk->immed_dom))
            break;
    }

    if (i == bblocks.get_size())
        return false;
#endif

    bblocks.collect_reachable(list, bblocks.get_entry_point(), true);
    if (list.size() == 0) return false;

    for (i = 0; i < list.size(); i++) {
        list[i]->set_dead();
    }

    for (i = 0; i < list.size(); i++) {
        blockbasic *bb = list[i];
        while (bb->out.size() > 0)
            branch_remove_internal(bb, 0);
    }

    for (i = 0; i < list.size(); ++i) {
        blockbasic *bb = list[i];
        block_remove_internal(bb, true);
    }
    structure_reset();
    return true;
}

void        funcdata::splice_block_basic(blockbasic *bl)
{
    blockbasic *outbl = NULL;
    if (bl->out.size() == 1) {
        outbl = bl->get_out(0);
        if (outbl->in.size() != 1)
            outbl = NULL;
    }

    if (!outbl)
        throw LowlevelError("cannot splice basic blocks");

    if (!bl->ops.empty()) {
        pcodeop *jumpop = bl->last_op();
        if ((jumpop->opcode == CPUI_BRANCH) || (jumpop->opcode == CPUI_CBRANCH))
            op_destroy(jumpop);
    }

    if (!outbl->ops.empty()) {
        pcodeop *firstop = outbl->ops.front();
        if (firstop->opcode == CPUI_MULTIEQUAL)
            throw LowlevelError("splicing block with MULTIEQUAL");
        firstop->flags.startblock = 0;
        list<pcodeop *>::iterator iter;

        for (iter = outbl->ops.begin(); iter != outbl->ops.end(); iter++) {
            pcodeop *op = *iter;
            op->parent = bl;
        }

        bl->ops.splice(bl->ops.end(), outbl->ops, outbl->ops.begin(), outbl->ops.end());
        /* Ghidra中有个set_order的操作，但是我们因为用不到order这个域，所以不用调这个函数 */
        // bl->set_order();
    }
    bblocks.splice_block(bl);
    structure_reset();
}

void        funcdata::redundbranch_appy()
{
    int i;
    flowblock *bb, *bl;

    for (i = 0; i < bblocks.get_size(); i++) {
        bb = bblocks.get_block(i);
        if (bb->out.size() == 0)
            continue;

        bl = bb->get_out(0);
        if (bb->out.size() == 1) {
            if ((bl->in.size() == 1) && !bl->is_entry_point() && !bb->is_switch_out()) {
                //printf("%sfound a block can splice, [%llx, %d]\n", print_indent(), bl->get_start().getOffset(), bl->dfnum);

                splice_block_basic(bb);
                i -= 1;
            }
        }
    }
}

void        funcdata::dump_store_info(const char *postfix)
{
    char obuf[256];
    list<pcodeop *>::const_iterator it;
    pcodeop *op;
    varnode *vn;
    FILE *fp;

    sprintf(obuf, "%s/%s/store_%s.txt", d->filename.c_str(), name.c_str(), postfix);

    fp = fopen(obuf, "w");

    for (it = storelist.begin(); it != storelist.end(); it++) {
        op = *it;
        if (op->is_dead()) {
            printf("op store[%d] is dead\n", op->start.getTime());
            continue;
        }
        vn = op->get_in(1);

        print_vartype(d->trans, obuf, vn);
        fprintf(fp, "(%s)\n", obuf);

        op->dump(obuf, PCODE_DUMP_SIMPLE & ~PCODE_HTML_COLOR);
        fprintf(fp, "%s\n", obuf);
    }

    fclose(fp);
}

void        funcdata::dump_load_info(const char *postfix)
{
}

flowblock*  funcdata::clone_block(flowblock *f)
{
    list<pcodeop *>::iterator it;
    flowblock *b;
    pcodeop *op, *p;

    b = bblocks.new_block_basic(this);
    user_offset += user_step;

    Address addr(d->trans->getDefaultCodeSpace(), user_offset);

    for (it = f->ops.begin(); it != f->ops.end(); it++) {
        op = *it;

        if (op->opcode == CPUI_MULTIEQUAL) continue;

        Address addr2(d->get_code_space(), user_offset + op->get_addr().getOffset());
        SeqNum seq(addr2, op_uniqid++);
        p = cloneop(op, seq);
        p->disaddr = new Address(op->get_dis_addr());

        op_insert(p, b, b->ops.end());
    }

    b->set_initial_range(addr, addr);

    return b;
}

char*       funcdata::get_dir(char *buf)
{
    funcdata *p = caller;

    while (p && p->caller) p = p->caller;

    if (p)
        sprintf(buf, "%s/%s", d->filename.c_str(), p->name.c_str());
    else 
        sprintf(buf, "%s/%s", d->filename.c_str(), name.c_str());

    return buf;
}

int         funcdata::get_input_sp_val()
{
    if (callop) {
        return callop->callctx->sp->type.v;
    }
    else
        return 0;
}

void        funcdata::alias_collect(void)
{
    list<pcodeop *>::iterator it;
    pcodeop *p;
    varnode *in;

    varnode_gvn_map   gvn;
    varnode_gvn_map::iterator it_gvn;

    for (it = storelist.begin(); it != storelist.end(); it++) {
        p = *it;
        if (p->is_dead()) continue;

        in = p->get_in(1);

        vector<pcodeop *> &v(gvn[in]);
        v.push_back(p);

        if (p->output)
            destroy_varnode(p->output);
    }

    for (it = loadlist.begin(); it != loadlist.end(); it++) {
        p = *it;
        if (p->is_dead()) continue;

        vector<pcodeop *> &v(gvn[in]);
        v.push_back(p);

        if (p->inrefs.size() == 3)
            op_remove_input(p, 2);
    }

    it_gvn = gvn.begin();
    for (it_gvn = gvn.begin(); it_gvn != gvn.end(); it_gvn++) {
        vector<pcodeop *> &v(it_gvn->second);
        vector<pcodeop *>::iterator itp;
        varnode *vn;

        int size = p->get_in(1)->size;
        Address addr(d->get_uniq_space(), virtualbase += size);

        for (itp = v.begin(); itp != v.end(); itp++) {
            pcodeop *p = *itp;

            if (p->opcode == CPUI_LOAD) {
                vn = new_varnode(p->get_in(1)->size, addr);

                vn->flags.virtualnode = 1;

                op_resize(p, 3);
                op_set_input(p, vn, 2);
            }
            else {
                vn = new_varnode_out(p->get_in(1)->size, addr, p);
            }
        }
    }
}

void        funcdata::alias_propagation(void)
{
}

void        funcdata::alias_clear(void)
{
    list<pcodeop *>::iterator it = safe_aliaslist.begin();
    varnode *vn;
    /* 清除别名信息 */
    for (; it != safe_aliaslist.end(); it++) {
        pcodeop *op = *it;

        if (op->opcode != CPUI_LOAD) continue;
        if (vn = op->get_virtualnode())
            destroy_varnode(vn);
    }

    safe_aliaslist.clear();
}

flowblock*  funcdata::dowhile2ifwhile(vector<flowblock *> &dowhile)
{
    flowblock *b, *dw, *before, *after;
    list<pcodeop *>::iterator  it;

    assert(dowhile.size() == 1);
    
    dw = dowhile[0];

    before = (dw->get_in(0) == dw) ? dw->get_in(1):dw->get_in(0);
    after = (dw->get_out(0) == dw) ? dw->get_out(1) : dw->get_out(0);

    b = clone_block(dowhile[0]);

    bblocks.remove_edge(before, dw);
    bblocks.add_edge(before, b);
    bblocks.add_block_if(b, dw, after);

    clear_block_phi(after);
    clear_block_phi(dw);
    structure_reset();

    return b;
}

char*       funcdata::print_indent(void)
{
    static char buf[128];
    int i = 0;

    pcodeop *c = callop;

    while (c) {
        buf[i++] = ' ';
        buf[i++] = ' ';
        buf[i++] = ' ';
        buf[i++] = ' ';

        c = c->parent->fd->callop;
    }
    buf[i] = 0;

    return buf;
}

bool        funcdata::test_strict_alias(pcodeop *load, pcodeop *store)
{
    if (load->parent != store->parent) {
        return false;
    }

    list<pcodeop *>::iterator it = store->basiciter;

    for (; it != load->basiciter; it++) {
        pcodeop *p = *it;

        if (p->opcode != CPUI_STORE) continue;

        /* 发现store - load链上有一个的地址无法识别，直接停止 */
        if (p->get_in(1)->type.height == a_top)
            return false;
    }

    return true;
}


void        funcdata::alias_analysis(void)
{
    list<pcodeop *>::iterator it = loadlist.begin();

    for (; it != loadlist.end(); it++) {
        pcodeop *load = *it;
        if (load->is_dead()) continue;

        pcodeop *store = store_query(load);

        if (!store) continue;

        /* 来自于caller，假如来自caller就无法建立use-def链了，直接把值复制过来 */
        if (store->parent->fd != this) {
            load->output->type = store->get_in(2)->type;
            load->flags.input = 1;
            safe_aliaslist.push_back(load);
            continue;
        }

        //printf("pcode load[%d] = store[%d]\n", load->start.getTime(), store->start.getTime());
        varnode *in = store->get_in(1);
        safe_aliaslist.push_back(store);

        /* 假如这个store已经被分析过，直接把store的版本设置过来 */
        if (store->output) {
            op_set_input(load, store->output, 2);
        }
        else {
            Address oaddr(d->get_uniq_space(), virtualbase += in->size);
            varnode *out;
            out = new_varnode_out(in->size, oaddr, store);

            op_resize(load, 3);
            op_set_input(load, out, 2);
        }
    }
}

flowblock*  funcdata::clone_web(flowblock *start, flowblock *end, vector<flowblock *> &cloneblks)
{
    int i, j;
    blockbasic *b, *out;
    vector<flowblock *> stack;
    vector<flowblock *> webs;

    stack.push_back(start);

    while (!stack.empty()) {
        b = stack.back();
        stack.pop_back();
        b->set_mark();
        webs.push_back(b);

        for (i = 0; i < b->out.size(); i++) {
            out = b->get_out(i);
            if (out == end) continue;

            if (!out->is_mark()) {
                stack.push_back(out);
            }
        }
    }

    for (i = 0; i < webs.size(); i++) {
        b = clone_block(webs[i]);
        webs[i]->copymap = b;

        cloneblks.push_back(b);
    }

    for (i = 0; i < webs.size(); i++) {
        for (j = 0; j < webs[i]->out.size(); j++) {
            out = webs[i]->get_out(j);

            bblocks.add_edge(webs[i]->copymap, out->copymap);
        }
    }

    b = webs[0]->copymap;

    for (i = 0; i < webs.size(); i++) {
        webs[i]->copymap = NULL;
        webs[i]->clear_mark();
    }

    return b;
}

func_call_specs::func_call_specs(pcodeop *o, funcdata *f)
{
    op = o;
    fd = f;
}

func_call_specs::~func_call_specs()
{
}

void priority_queue::reset(int maxdepth)
{
    if ((curdepth == -1) && (maxdepth == queue.size() - 1))
        return;

    queue.clear();
    queue.resize(maxdepth + 1);
    curdepth = -1;
}

void priority_queue::insert(flowblock *b, int depth)
{
    queue[depth].push_back(b);
    if (depth > curdepth)
        curdepth = depth;
}

flowblock *priority_queue::extract()
{
    flowblock *res = queue[curdepth].back();
    queue[curdepth].pop_back();
    while (queue[curdepth].empty()) {
        curdepth -= 1;
        if (curdepth < 0)
            break;
    }

    return res;
}
