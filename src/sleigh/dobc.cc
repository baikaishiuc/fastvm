

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

static char help[] = {
    "dobc [.sla filename] [filename]"
};

static dobc *g_dobc = NULL;

class AssemblyRaw : public AssemblyEmit {

public:
    char *buf = NULL;
    FILE *fp = NULL;

    virtual void dump(const Address &addr, const string &mnem, const string &body) {
        if (!fp) fp = stdout;

        if (buf) {
            sprintf(buf, "<tr><td>0x%08x:</td><td align=\"left\">%s </td><td align=\"left\">%s</td></tr>", (int)addr.getOffset(), mnem.c_str(), body.c_str());
            //sprintf(buf, "0x%08x:%10s %s", (int)addr.getOffset(), mnem.c_str(), body.c_str());
        }
        else {
            fprintf(fp, "0x%08x: %s %s\n", (int)addr.getOffset(), mnem.c_str(), body.c_str());
        }
    }

    void set_buf(char *b) { buf = b; }
    void set_fp(FILE *f) { fp = f; }
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
    if (!data)
        return 0;

    if (data->type.height == a_constant)
        return sprintf(buf, "[%llx]", data->type.v);
    else if (data->type.height == a_rel_constant) {
        Address &addr = data->get_rel();
        string name = trans->getRegisterName(addr.getSpace(), addr.getOffset(), data->size);
        name = g_dobc->get_abbrev(name);

        return sprintf(buf, "[%c%s%c%llx]", addr.getShortcut(), name.c_str(), data->type.v > 0 ? '+':'-', abs(data->type.v));
    }
    else 
        return sprintf(buf, "[]");
}

static int print_udchain(char *buf, pcodeop *op)
{
    varnode *out = op->output;
    int i = 0, j;

    if (out && out->uses.size()) {
        list<pcodeop *>::iterator iter = out->uses.begin();
        i += sprintf(buf + i, " [u:");
        for (; iter != out->uses.end(); iter++) {
            i += sprintf(buf + i, "%d ", (*iter)->start.getTime());
        }
        i += sprintf(buf + i, "]");
    }

    i += sprintf(buf + i, " [d:");
    for (j = 0; j < op->inrefs.size(); j++) {
        if (op->inrefs[j]->def)
            i += sprintf(buf + i, "%d ", op->inrefs[j]->def->start.getTime());
    }
    i += sprintf(buf + i, "]");

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
            return sprintf(buf, "(%c%llx)", addr.getSpace()->getShortcut(), addr.getOffset());
        else
            return sprintf(buf, "(%c%llx:%d)", addr.getSpace()->getShortcut(), addr.getOffset(), data->size);
    }
    else
        return sprintf(buf, "(%c%s:%d)", addr.getSpace()->getShortcut(), name.c_str(), data->size);
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
    /* coderef */

    for (; i < isize; i++) {
        vn = fd->new_varnode(vars[i].size, vars[i].space, vars[i].offset);
        fd->op_set_input(op, vn, i);
    }

#if 1
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

int valuetype::cmp(const valuetype &b)
{
    if (height == b.height) {
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

void dobc::init_plt()
{
    funcdata *fd;
    Address addr(trans->getDefaultCodeSpace(), 0x1a34);
    fd = new funcdata("__stack_chk_fail", addr, 0, this);

    fd->set_exit(1);

    add_func(fd);
}

const string& dobc::get_abbrev(const string &name)
{
    map<string, string>::iterator iter = abbrev.find(name);

    return (iter != abbrev.end()) ? (*iter).second : name;
}

void dobc::plugin_dvmp360()
{
    funcdata *fd_main = find_func("_Z10__arm_a_21v");
    funcdata *fd_sub = find_func("_Z9__arm_a_0v");
    funcdata *fd_vm = find_func("_Z10__fun_a_18Pcj");

    fd_main->follow_flow();

    int i;
    for (i = 0; i < fd_main->qlst.size(); i++) {
        func_call_specs *cs = fd_main->qlst[i];

        if (cs->get_name() == fd_sub->name) 
            fd_main->inline_flow(cs->fd, cs->op);
    }

    fd_main->heritage();
    if (fd_main->constant_propagation(0)) {
        fd_main->heritage_clear();
        fd_main->heritage();
        fd_main->constant_propagation(1);

        if (!fd_main->addrlist.empty()) {
            fd_main->generate_ops();
            fd_main->generate_blocks();
        }
    }


    fd_main->dump_pcode("1");
    fd_main->dump_inst_dot("1");
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

    func->dump_inst_dot("1");

    printf("\n");
}

void dobc::add_func(funcdata *fd)
{
    mlist_add(funcs, fd, node);
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

    init();
}

dobc::~dobc()
{
}

#define SLA_FILE            "../../../Processors/ARM/data/languages/ARM8_le.sla"
#define PSPEC_FILE          "../../../Processors/ARM/data/languages/ARMCortex.pspec"
#define CSPEC_FILE          "../../../Processors/ARM/data/languages/ARM.cspec"
#define TEST_SO             "../../../data/vmp/360_1/libjiagu.so"

#if 1 //defined(DOBC)
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
        flags.writtern = 1;
    }
    else
        flags.writtern = 0;
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

intb            varnode::get_val(void)
{
    return type.v;
}

inline bool varnode_cmp_loc_def::operator()(const varnode *a, const varnode *b) const
{
    uint4 f1, f2;

    if (a->get_addr() != b->get_addr()) return (a->get_addr() < b->get_addr());
    if (a->size != b->size) return (a->size < b->size);

    f1 = ((int)a->flags.input << 1) + a->flags.writtern;
    f2 = ((int)b->flags.input << 1) + b->flags.writtern;

    /* 这样处理过后，假如一个节点是free的(没有written和Input标记)，那么在和一个带标记的
    节点比较时，就会变大，因为 0 - 1 = 0xffffffff, f1是Unsigned类型，导致他被排后 */
    if (f1 != f2) return (f1 - 1) < (f2 - 1);

    if (a->flags.writtern) {
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

    f1 = ((int)a->flags.input << 1) + a->flags.writtern;
    f2 = ((int)b->flags.input << 1) + b->flags.writtern;

    if (f1 != f2) return (f1 - 1) < (f2 - 1);

    if (a->flags.writtern) {
        if (a->def->start != b->def->start)
            return a->def->start < b->def->start;
    }
    if (a->get_addr() != b->get_addr()) return (a->get_addr() < b->get_addr());
    if (a->size != b->size) return (a->size < b->size);

    if (f1 == 0)
        return a->create_index < b->create_index;

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

void pcodeop::set_opcode(OpCode op)
{
    opcode = op;
}

// 扩展outbuf的内容区，用来做对齐用 
#define expand_line(num)        while (i < num) buf[i++] = ' '
int             pcodeop::dump(char *buf, uint32_t flags)
{
    int i = 0, j;
    Translate *trans = parent->fd->d->trans;

    i += sprintf(buf + i, "    %d:", start.getTime());

    if (output) {
        i += print_varnode(trans, buf + i, output);
        i += sprintf(buf + i, " = ");
    }

    i += sprintf(buf + i, "%s", get_opname(opcode));
    // Possibly check for a code reference or a space reference
    for (j = 0; j < inrefs.size(); ++j) {
        i += sprintf(buf + i, " ");
        i += print_varnode(trans, buf + i, inrefs[j]);
    }

    expand_line(48);

    if (flags & PCODE_DUMP_VAL)
        i += print_vartype(trans, buf + i, output);

    if (flags & PCODE_DUMP_UD)
        i += print_udchain(buf + i, this);

    buf[i] = 0;

    return i;
}

int             pcodeop::compute(void)
{
    varnode *in0, *in1, *in2, *out;
    funcdata *fd = parent->fd;
    dobc *d = fd->d;
    uint1 buf[8];
    int i;

    out = output;
    in0 = get_in(0);

    switch (opcode) {
    case CPUI_COPY:
        if (in0->is_constant()) {
            out->set_val(in0->get_val());
        }
        else if (fd->is_sp_rel_constant(in0)) {
            out->set_rel_constant(in0->get_rel(), in0->get_val());
        }
        else
            out->type.height = in0->type.height;
        break;

        //      out                            in0              in1            
        // 66:(register,r1,4) = LOAD (const,0x11ed0f68,8) (const,0x840c,4)
    case CPUI_LOAD:
        in1 = get_in(1);
        in2 = (inrefs.size() == 3) ? get_in(2):NULL;
        if (fd->is_ram(in0) && in1->is_constant()) {
            Address addr(d->trans->getDefaultCodeSpace(), in1->type.v);

            memset(buf, 0, sizeof(buf));
            d->loader->loadFill(buf, out->size, addr);

            out->set_val(*(intb *)buf);
        }
        else if (fd->is_sp_rel_constant(in1) && fd->in_safezone(in1->get_val(), in1->size)) {

        /* 假如有in2的节点切有def，说明已经被成功别名分析过 */
            if (in2 && in2->def) {
                varnode *def = in2->def->output;
                output->type = def->type;
            }
            else if (!in2){
                in2 = fd->new_varnode(in1->size, d->get_uniq_space(), in1->get_val() + 0x10000);

                fd->op_resize(this, 3);
                fd->op_set_input(this, in2, 2);
            }

            return 1;
        }
        else
            out->type.height = a_top;
        break;

        //
    case CPUI_STORE:
        in1 = get_in(1);
        in2 = get_in(2);

        if (!output && 
            fd->is_sp_rel_constant(in1) && fd->in_safezone(in1->get_val(), in1->size)) {
            Address oaddr(d->get_uniq_space(), in1->get_val() + 0x10000);
            output = fd->new_varnode_out(in2->size, oaddr, this);

            output->type = in2->type;

            return 1;
        }
        break;

    case CPUI_BRANCHIND:
        if (in0->is_constant()) {
            Address addr(d->get_code_space(), in0->get_val());

            if (!fd->find_op(addr)) {
                printf("we found a new address[%llx] to analaysis\n", addr.getOffset());
                fd->addrlist.push_back(addr);
            }
        }
        break;

    case CPUI_INT_ADD:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->type.v + in1->type.v);
        }
        else if (fd->is_sp_rel_constant(in0) && in1->is_constant()) {
            out->set_rel_constant(in0->get_rel(), in0->type.v + in1->type.v);
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_INT_SUB:
        in1 = get_in(1);

        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->type.v - in1->type.v);
        }
        /*      out                             0                   1       */
        /* 0:(register,mult_addr,4) = INT_SUB (register,sp,4) (const,0x4,4) */
        else if (fd->is_sp_rel_constant(in0) && in1->is_constant()) {
            out->set_rel_constant(in0->get_rel(), in0->type.v - in1->type.v);

            if (is_safe_inst())
                fd->set_safezone(out->type.v, in1->type.v);
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

    case CPUI_INT_AND:
        in1 = get_in(1);
        if (in0->is_constant() && in1->is_constant()) {
            out->set_val(in0->get_val() & in1->get_val());
        }
        else
            out->type.height = a_top;
        break;

    case CPUI_MULTIEQUAL:
        for (i = 1; i < inrefs.size(); i++) {
            in1 = get_in(i);
            if (in0->type != in1->type)
                break;
        }

        if (i == inrefs.size())
            output->type = in0->type;
        else
            output->type.height = a_top;
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

    return 0;
}

bool            pcodeop::is_safe_inst()
{
    funcdata *fd = parent->fd;

    VisitStat &stat(fd->visited[start.getAddr()]);

    return stat.inst_type == a_stmdb;
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

jmptable::~jmptable()
{
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
    int *istate = NULL;
    int rpostcount = blist.size();
    flowblock *tmpbl, *child;

    istate = (int *)calloc(1, sizeof(int) * blist.size());
    preorder.reserve(blist.size());

    for (i = 0; i < blist.size(); i++) {
        tmpbl = blist[i];
        tmpbl->index = -1;
        tmpbl->visitcount = -1;
        if (tmpbl->in.size() == 0)
            rootlist.push_back(tmpbl);
    }
    assert(rootlist.size() == 1);

    origrootpos = rootlist.size() - 1;

    state.push_back(blist[0]);

    while (!state.empty()) {
        flowblock *bl = state.back();

        int index = istate[bl->visitcount];

        /* 当前节点的子节点都遍历完成 */
        if (index == bl->out.size()) {
            state.pop_back();
            bl->index = --rpostcount;
            rpostorder[rpostcount] = bl;
            if (!state.empty())
                state.back() += bl->numdesc;
        }
        else {
            blockedge &e = bl->out[index];
            child = e.point;
            istate[bl->visitcount] = index++;

            /* */
            if (child->visitcount == -1) {
                e.label |= a_tree_edge;

                child->visitcount = preorder.size();
                preorder.push_back(child);
                child->numdesc = 1;
            }
            else if (child->index == -1) {
                e.label |= a_back_edge;
                e.label |= a_loop_edge;
            }
            else if (bl->visitcount < child->visitcount) {
                e.label |= a_forward_edge;
            }
            else
                e.label |= a_cross_edge;
        }
    }

    free(istate);
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
    bool needrebuild;
    int irreduciblecount = 0;

    do {
        needrebuild = false;
        find_spanning_tree(preorder, rootlist);
    } while (needrebuild);
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
                assert(0);
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


void        flowblock::add_inedge(flowblock *b, int lab)
{
    int osize = b->out.size();
    int isize = in.size();

    in.push_back(blockedge(b, lab, osize));
    b->out.push_back(blockedge(blockedge(this, lab, isize)));
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
    return (int)fd->startaddr.getOffset();  
}

void        flowblock::clear(void)
{
    vector<flowblock *>::iterator iter;

    for (iter = blist.begin(); iter != blist.end(); ++iter)
        delete *iter;

    blist.clear();
}

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

void        flowblock::add_op(pcodeop *op)
{
    insert(ops.end(), op);
}

void        flowblock::add_edge(flowblock *begin, flowblock *end)
{
    end->add_inedge(begin, 0);
}

funcdata::funcdata(const char *nm, const Address &a, int size, dobc *d1)
    : startaddr(a),
    bblocks(this),
    name(nm),
    searchvn(0, Address(Address::m_minimal))
{
    char buf[256];
    d = d1;

    emitter.fd = this;

    sprintf(buf, "%s/%s", d->filename.c_str(), nm);
    mdir_make(buf);

    memstack.size = 256 * 1024;
    memstack.bottom = (u1 *)malloc(sizeof (u1) * memstack.size);
    memstack.top = memstack.bottom + memstack.size;

    op_gen_iter = deadlist.end();

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

    return vn;
}

varnode*    funcdata::create_def(int s, const Address &m, pcodeop *op)
{
    varnode *vn = new varnode(s, m);
    vn->create_index = vbank.create_index++;
    vn->set_def(op);

    return vn;
}

varnode*    funcdata::create_def_unique(int s, pcodeop *op)
{
    Address addr(uniq_space, vbank.uniqid);

    vbank.uniqid += s;

    return create_def(s, addr, op);
}

void        funcdata::op_set_opcode(pcodeop *op, OpCode opc)
{
    op->opcode = opc;

    add_to_codelist(op);
}

void        funcdata::op_resize(pcodeop *op, int size)
{
    op->inrefs.resize(size);
}

void        funcdata::op_set_input(pcodeop *op, varnode *vn, int slot)
{
    if (vn == op->get_in(slot))
        return; 

    if (vn->is_constant()) {
    }

    if (op->get_in(slot))
        op_unset_input(op, slot);

    vn->add_use(op);
    op->inrefs[slot] = vn;
}

void        funcdata::op_unset_input(pcodeop *op, int slot)
{
    varnode *vn = op->get_in(slot);

    vn->del_use(op);
    op->clear_input(slot);
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
    qlst.push_back(new func_call_specs(p, fd));
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
            if ((fd = d->find_func(destaddr)) && (exit = fd->flags.exit)) {
                startbasic = true;
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

    d->trans->printAssembly(assem, curaddr);
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
        (*oiter)->flags.startmark = 1;

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

    for (i = 0; i < jmpvec.size(); i++) {
        jmptable *jt = jmpvec[i];
        for (j = 0; j < jt->addresstable.size(); j++) {
            Address &addr = jt->addresstable[j];
            pcodeop *op = find_op(addr);

            if (!op->flags.startblock)
                throw LowlevelError("indirect jmp not is start block");

            op->parent->jmptable = jt;
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

list<pcodeop *>::const_iterator funcdata::get_gen_op_iter(void)
{
    if (op_gen_iter == deadlist.end())
        return deadlist.begin();

    list<pcodeop *>::const_iterator it = op_gen_iter;

    return ++it;
}

void        funcdata::generate_ops(void)
{
    vector<pcodeop *> notreached;       // 间接跳转是不可达的?

    /* 修改了原有逻辑，可以多遍op_generated*/
    printf("%s generate ops %d times\n", name.c_str(), op_generated+1);

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
        if (!retop->flags.startmark)
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
    op_edge *edge;
    list<op_edge *>::const_iterator iter;

    iter = block_edge.begin();
    while (iter != block_edge.end()) {
        edge = *iter++;
        bblocks.add_edge(edge->from->parent, edge->to->parent);

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
    char buf[128];
    Address prev_addr;
    AssemblyRaw assememit;

    sprintf(buf, "%s/%s/pcode_%s.txt", d->filename.c_str(), name.c_str(), postfix);
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
        if ((*iter)->get_addr() != prev_addr) {
            d->trans->printAssembly(assememit, (*iter)->get_addr());
        }

        (*iter)->dump(buf, PCODE_DUMP_ALL);
        fprintf(fp, "%s\n", buf);

        prev_addr = (*iter)->get_addr();
    }

    fclose(fp);
}

char*       funcdata::block_color(flowblock *b)
{
    list<pcodeop *>::iterator iter = b->ops.end();
    if (b->flags.f_entry_point)     return "red";

    iter--;
    if ((*iter)->opcode == CPUI_RETURN)     return "blue";

    return "white";
}

void        funcdata::dump_inst_dot(const char *postfix)
{
    char obuf[512];
    Address prev_addr, nextaddr;
    AssemblyRaw assem;
    list<pcodeop *>::iterator iter;
    map<Address, VisitStat>::iterator st_iter;
    VisitStat stat;
    pcodeop *p;

    sprintf(obuf, "%s/%s/cfg_%s.dot", d->filename.c_str(), name.c_str(), postfix);

    assem.set_buf(obuf);

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

        // 把指令都以html.table的方式打印，dot直接segment fault了，懒的调dot了
        fprintf(fp, "sub_%x [style=\"filled\" fillcolor=%s label=<<table bgcolor=\"white\" align=\"left\" border=\"0\"><tr><td><font color=\"red\">sub_%x</font></td></tr>",
            b->sub_id(),
            block_color(b),
            b->sub_id());

        iter = b->ops.begin();
        p = *iter;

        for (p = NULL;  iter != b->ops.end() ; iter++) {
            p = *iter;
            /* 一个指令对应多个pcode，跳过同个指令的pcode */
            if ((iter != b->ops.begin()) && (prev_addr == p->start.getAddr())) continue;

            d->trans->printAssembly(assem, p->start.getAddr());
            fprintf(fp, "%s", obuf);

            prev_addr = p->start.getAddr();
        }
#if 0
        if (b->type == a_switch) {
            fprintf(fp, "<tr><td>----------</td></tr>");
            for (j = 0; j < b->jmptable->addresstable.size(); j++) {
                d->trans->printAssembly(assem, b->jmptable->addresstable[j]);
                fprintf(fp, "%s", obuf);
            }
        }
#endif
        fprintf(fp, "</table>>]\n");
    }

    for (i = 0; i < bblocks.blist.size(); ++i) {
        blockbasic *b = bblocks.blist[i];

        for (j = 0; j < b->out.size(); ++j) {
            blockedge *e = &b->out[j];

                fprintf(fp, "sub_%x ->sub_%x [label = \"%s\"]\n",
                    b->sub_id(), e->point->sub_id(), "true");
        }
    }

    fprintf(fp, "}");
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

    return newvn;
}

void        funcdata::destroy_varnode(varnode *vn)
{
    list<pcodeop *>::const_iterator iter;

    for (iter = vn->uses.begin(); iter != vn->uses.end(); ++iter) {
        pcodeop *op = *iter;

        op->inrefs[op->get_slot(vn)] = NULL;
    }

    if (vn->def) {
        vn->def->output = NULL;
        vn->def = NULL;
    }

    vn->uses.clear();
    delete(vn);
}

void        funcdata::delete_varnode(varnode *vn)
{
    delete(vn);
}

varnode*    funcdata::set_input_varnode(varnode *vn)
{
    varnode *invn;

    if (vn->flags.input) return vn;

    if (vn->get_addr() == d->sp_addr) {
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

    newop1->flags.startmark = op->flags.startmark;
    newop1->flags.startblock = op->flags.startblock;
    if (op->output)
        newop1->output = clone_varnode(op->output);
    
    for (i = 0; i < op->inrefs.size(); i++)
        op_set_input(newop1, clone_varnode(op->get_in(i)), i);

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
        // 另外关于 uniqid，也使用了以前的，我也不理解的，这里统统改成了全新的uniqid，否则会
        // 出现重复
        //SeqNum seq(calladdr, op->start.getTime());
        SeqNum seq(op->start.getAddr(), op_uniqid++);
        cloneop(op, seq);
    }
}

void        funcdata::inline_flow(funcdata *fd1, pcodeop *callop)
{
    funcdata fd(fd1->name.c_str(), fd1->get_addr(), fd1->size, fd1->d);
    pcodeop *firstop;

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
        assert(0);
        inline_clone(&fd, retaddr);
    }

    /* 原始的Ghidra中，inline只能inline call，但是因为branch也可以模拟call，所以branch也能inline的 */
    if (callop->opcode == CPUI_CALL) {
    }

    generate_blocks();
}

bool        funcdata::check_ezmodel(void)
{
    list<pcodeop *>::const_iterator iter = deadlist.begin();

    while (iter != deadlist.end()) {
        pcodeop *op = *iter;
        if (op->flags.call || op->flags.branch)
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

void        funcdata::follow_flow(void)
{
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

        if (vn->flags.writtern) {
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

    for (i = 0; i < d->trans->numSpaces(); i++) {
        AddrSpace *space = d->trans->getSpace(i);

        iter = begin_loc(space);
        enditer = end_loc(space);

        while (iter != enditer) {
            vn = *iter++;

            if (!vn->flags.writtern && vn->has_no_use() && !vn->flags.input)
                continue;

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
}

void    funcdata::heritage_clear()
{
    disjoint.clear();
    globaldisjoint.clear();
    domchild.clear();
    augment.clear();
    phiflags.clear();
    domdepth.clear();
    merge.clear();
    safe_storelist.clear();

    maxdepth = -1;
    pass = 0;
}

int     funcdata::constant_propagation(int listype)
{
    list<pcodeop *>::const_iterator iter;
    list<pcodeop *> w = listype ? safe_storelist:deadlist;
    list<pcodeop *>::const_iterator iter1;
    int i, ret = 0;

    while (!w.empty()) {
        iter = w.begin();
        pcodeop *op = *iter;
        w.erase(iter);

        ret |= op->compute();

        varnode *out = op->output;
        if (!out) continue;

        for (iter1 = out->uses.begin(); iter1 != out->uses.end(); ++iter1) {
            pcodeop *use = *iter1;
            w.push_back(use);
        }
    }

    return ret;
}

void        funcdata::set_safezone(intb addr, int size)
{
    safezone.insertRange(NULL, addr, addr + size);
}

bool        funcdata::in_safezone(intb a, int size)
{
    Address addr(NULL, a);
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

void        funcdata::calc_load_store_info()
{
}

bool        funcdata::is_ram(varnode *v) 
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
            assert(0);
        }

        if (readvars.empty() && (addr.getSpace()->getType() == IPTR_INTERNAL))
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
    int i, slot;

    for (oiter = bl->ops.begin(); oiter != bl->ops.end(); oiter++) {
        op = *oiter;
        if (op->opcode != CPUI_MULTIEQUAL) {
            for (slot = 0; slot < op->inrefs.size(); ++slot) {
                vnin = op->get_in(slot);

                if (vnin->flags.annotation || vnin->is_constant())
                    continue;

                vector<varnode *> &stack(varstack[vnin->get_addr()]);
                if (stack.empty()) {
                    vnnew = new_varnode(vnin->size, vnin->get_addr());
                    vnnew = set_input_varnode(vnnew);
                    stack.push_back(vnnew);
                }
                else 
                    vnnew = stack.back();

                if (vnnew->flags.writtern && (vnnew->def->opcode == CPUI_INDIRECT)) {
                }

                op_set_input(op, vnnew, slot);
                if (vnin->has_no_use())
                    delete_varnode(vnin);
            }
        }

        vnout = op->output;
        if (vnout == NULL) continue;
        varstack[vnout->get_addr()].push_back(vnout);
        writelist.push_back(vnout);
        if (op->opcode == CPUI_STORE)
            safe_storelist.push_back(op);
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
                stack.push_back(vnnew);
            }
            else
                vnnew = stack.back();

            op_set_input(multiop, vnnew, slot);
            if (!vnin->uses.size())
                delete_varnode(vnin);
        }
    }

    i = bl->index;
    for (slot = 0; slot < domchild[i].size(); ++slot)
        rename_recurse(domchild[i][slot], varstack);

    for (i = 0; i < writelist.size(); ++i) {
        vnout = writelist[i];
        varstack[vnout->get_addr()].pop_back();
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
