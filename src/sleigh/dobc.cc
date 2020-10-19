

#include "sleigh_arch.hh"
#include "sleigh.hh"
#include "funcdata.hh"
#include "flow.hh"
#include "dobc.hh"
#include <iostream>
#include <assert.h>

#define strdup _strdup

static char help[] = {
    "dobc [.sla filename] [filename]"
};

class AssemblyRaw : public AssemblyEmit {

public:
    char *buf = NULL;

    virtual void dump(const Address &addr, const string &mnem, const string &body) {
        if (buf) {
            sprintf(buf, "0x%08x: %s %s", (int)addr.getOffset(), mnem.c_str(), body.c_str());
        }
        else {
            addr.printRaw(cout);
            cout << ": " << mnem << ' ' << body << endl;
        }
    }

    void set_buf(char *b) { buf = b; }
};

static void print_vardata(Translate *trans, ostream &s, VarnodeData &data)

{
    string name = trans->getRegisterName(data.space, data.offset, data.size);
    s << "(" << data.space->getName() << ',';
    if (name == "")
        data.space->printOffset(s, data.offset);
    else
        s << name;

    s << ',' << dec << data.size << ')';
}

static dobc *g_dobc = NULL;

void pcodeemit2::dump(const Address &addr, OpCode opc, VarnodeData *outvar, VarnodeData *vars, int4 isize)
{
    int i;
    pcodeop *op;
    varnode *vn;

    cout << "    "; 
    if (outvar != (VarnodeData *)0) {
        print_vardata(fd->d->trans, cout, *outvar); cout << " = ";
        Address oaddr(outvar->space, outvar->offset);
        op = fd->newop(isize, addr);
        fd->new_varnode_out(outvar->size, oaddr, op);
    }
    else
        op = fd->newop(isize, addr);

    fd->op_set_opcode(op, opc);

    cout << get_opname(opc);
    // Possibly check for a code reference or a space reference
    for (i = 0; i < isize; ++i) {
        cout << ' '; print_vardata(fd->d->trans, cout, vars[i]);
    }
    cout << endl;

    i = 0;
    /* coderef */

    for (; i < isize; i++) {
        vn = fd->new_varnode(vars[i].size, vars[i].space, vars[i].offset);
        fd->op_set_input(op, vn, i);
    }
}

// FIXME:loader的类型本来应该是LoadImageB的，但是不知道为什么loader的getNextSymbol访问的是
// LoadImageB的，而不是ElfLoadImage的···
void dobc::run()
{
    LoadImageFunc sym;

#if 0
    while (loader->getNextSymbol(sym)) {
        if (!sym.size) continue;

        dump_function(sym, 1);
    }
#else
    //loader->getSymbol("strcmp", sym);
    if (loader->getSymbol("_Z10__fun_a_18Pcj", sym)) {
        printf("not found symbol");
        exit(-1);
    }
    dump_function(sym);
#endif
}

void dobc::dump_function(LoadImageFunc &sym)
{
    Address addr(sym.address);
    Address lastaddr(trans->getDefaultCodeSpace(), sym.address.getOffset() + sym.size);

    funcdata *func;

    func = new funcdata(sym.name.c_str(), addr, sym.size, this);

    mlist_add(funcs, func, node);

    printf("function:%s\n", sym.name.c_str());

    func->generate_ops();

    func->dump_dot("1");

    printf("\n");
}

dobc::dobc(const char *sla, const char *bin) 
{
    slafilename.assign(sla);
    filename.assign(bin);

    loader = new ElfLoadImage(bin);
    context = new ContextInternal();
    trans = new Sleigh(loader, context);

    DocumentStorage docstorage;
    Element *sleighroot = docstorage.openDocument(slafilename)->getRoot();
    docstorage.registerTag(sleighroot);
    trans->initialize(docstorage); // Initialize the translator

    loader->setCodeSpace(trans->getDefaultCodeSpace());

    mdir_make(bin);
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

    spacetype tp = m.getSpace()->getType();

    if (tp == IPTR_CONSTANT) {
        flags.constant = 1;
        nzm = m.getOffset();
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

    if (list.empty())
        index = min;
    else {
        if (min < index) index = min;
    }

    b->parent = this;
    list.push_back(b);
}

void blockgraph::find_spanning_tree(vector<flowblock *> &preorder, vector<flowblock *> &rootlist)
{
    if (list.size() == 0) return;

    int i;
    flowblock *tmpbl;

    preorder.reserve(list.size());

    for (i = 0; i < list.size(); i++) {
        tmpbl = list[i];
        tmpbl->index = -1;
        tmpbl->visitcount = -1;
    }
}

/*
1. 找到不可规约边
2. 找到 spanning tree(计算df需要)
3. 设置flowblock的索引为反向支配顺序
4. 标记 tree-edge, forward-edges, cross-edges, 和 back-edge
    初步怀疑: tree-edge 是spanning tree
              forward-edge 是
*/
void blockgraph::structure_loops(vector<flowblock *> &rootlist)
{
    vector<flowblock *> preorder;
    bool needrebuild;
    int irreduciblecount = 0;

    do {
        needrebuild = false;
    } while (needrebuild);
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
    if (list[0]->flags.f_entry_point) {
        if (bl == list[0]) return;
    }

    for (i = 0; i < list.size(); i++)
        if (list[i] == bl) break;

    for (; i > 0; --i)
        list[i] = list[i - 1];

    list[0] = bl;
    bl->flags.f_entry_point = 1;
}

void        blockgraph::set_initial_range(const Address &b, const Address &e)
{
    cover.start = b;
    cover.end = e;
}

void        flowblock::add_inedge(flowblock *b, int lab)
{
    int osize = b->outofthis.size();
    int isize = intothis.size();

    intothis.push_back(blockedge(b, lab, osize));
    b->outofthis.push_back(blockedge(blockedge(this, lab, isize)));
}

void        flowblock::add_edge(flowblock *begin, flowblock *end)
{
    end->add_inedge(begin, 0);
}

funcdata::funcdata(const char *nm, const Address &a, int size, dobc *d1)
    : addr(a),
    bblocks(this)
{
    char buf[256];
    name = strdup(nm);
    d = d1;

    emitter.fd = this;

    sprintf(buf, "%s/%s", d->filename.c_str(), nm);
    mdir_make(buf);
}

funcdata::~funcdata(void)
{
    free(name);
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
    vn->def = op;

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
}

void        funcdata::op_set_input(pcodeop *op, varnode *vn, int slot)
{
    if (vn == op->get_in(slot))
        return; 

    if (vn->flags.constant) {
    }

    op->inrefs[slot] = vn;
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

pcodeop*    funcdata::find_op(const SeqNum &num) const
{
    pcodeop_tree::const_iterator iter = optree.find(num);
    if (iter == optree.end()) return NULL;
    return iter->second;
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

pcodeop*    funcdata::xref_control_flow(list<pcodeop *>::const_iterator oiter, bool &startbasic, bool &isfallthru)
{
    pcodeop *op = NULL;
    isfallthru = false;
    uintm maxtime = 0;

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
            else
                new_address(op, destaddr);

            startbasic = true;
        }
            break;

        case CPUI_BRANCHIND:
            tablelist.push_back(op);
        case CPUI_RETURN:

            if (op->start.getTime() >= maxtime) {
                del_remaining_ops(oiter);
                oiter = deadlist.end();
            }
            startbasic = true;
            break;
        }

        if ((op->opcode == CPUI_BRANCH
            || op->opcode == CPUI_BRANCHIND
            || op->opcode == CPUI_RETURN) && (op->start.getTime() >= maxtime)) {
            del_remaining_ops(oiter);
            oiter = deadlist.end();
        }
    }

    if (isfallthru)
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
    }
    jmpvec.push_back(newjt);
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
            || (data1 & ARM_CMP_MASK) == ARM_CMP_MASK) {
            reg = data & 0xf;
            reg1 = (data1 >> 16) & 0xf;
            if (reg == reg1) {
                recover_jmptable(op, data1 & 0xfff);
                return;
            }
        }
    }

    assert(0);
}

void        funcdata::generate_ops()
{
    vector<pcodeop *> notreached;       // 间接跳转是不可达的?

    addrlist.push_back(addr);
    while (!addrlist.empty())
        fallthru();

    while (!tablelist.empty()) {
        pcodeop *op = tablelist.back();
        tablelist.pop_back();

        analysis_jmptable(op);

        while (!addrlist.empty())
            fallthru();
    }

    dump_inst();
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
    pcodeop *op, *target_op;
    bool nextstart;
    int i;

    if (bblocks.list.size())
        throw RecovError("Basic blocks already calculated");

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
            target_op = branch_target(op);
            block_edge.push_back(new op_edge(op, target_op));
            break;

        case CPUI_BRANCHIND:
            jt = find_jmptable(op);
            if (jt == NULL) break;

            for (i = 0; i < jt->size; i++) {
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

            target_op = branch_target(op);
            block_edge.push_back(new op_edge(op, target_op));
            break;

        default:
            if (nextstart) {
                target_op = fallthru_op(op);
                block_edge.push_back(new op_edge(op, target_op));
            }
            break;
        }
    }
}

void        funcdata::op_insert(pcodeop *op, blockbasic *bl, list<pcodeop *>::iterator iter)
{
    deadlist.erase(op->insertiter);
    op->flags.dead = 0;
}

void        funcdata::connect_basic()
{
    op_edge *edge;
    list<op_edge *>::const_iterator iter;

    iter = block_edge.begin();
    while (iter != block_edge.end()) {
        edge = *iter++;
        bblocks.add_edge(edge->from->parent, edge->to->parent);
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
    if (!op->flags.startblock)
        throw LowlevelError("First op not marked as entry point");

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
    }
    cur->set_initial_range(start, stop);
}

void        funcdata::generate_blocks()
{
    collect_edges();
    split_basic();
    connect_basic();
}

void        funcdata::dump_inst()
{
    map<Address, VisitStat>::iterator it, prev_it;
    AssemblyRaw assememit;

    for (it = visited.begin(); it != visited.end(); it++) {
        if (it != visited.begin()) {
            /* 假如顺序的地址，加上size不等于下一个指令的地址，代表中间有缺漏，多打印一个 空行 */
            if ((prev_it->first + prev_it->second.size) != it->first) {
                cout << endl;
            }
        }

        d->trans->printAssembly(assememit, it->first);

        prev_it = it;
    }
}

void        funcdata::dump_dot(const char *postfix)
{
    char obuf[512];
    Address prev_addr;
    AssemblyRaw assem;
    list<pcodeop *>::iterator iter;

    sprintf(obuf, "%s/%s/cfg_%s.dot", d->filename.c_str(), name, postfix);

    assem.set_buf(obuf);

    FILE *fp = fopen(obuf, "w");
    if (NULL == fp) {
        printf("fopen failure %s", obuf);
        exit(0);
    }

    fprintf(fp, "digraph G {\n");
    fprintf(fp, "node [fontname = \"helvetica\"]\n");

    int i, j;
    for (i = 0; i < bblocks.list.size(); ++i) {
        blockbasic *b = bblocks.list[i];
        fprintf(fp, "sub_%x [label=<<font color='red'></b>sub_%x(%d, %d)</b></font><br/>",
            b->sub_id(), b->sub_id(), b->index, 0);

        iter = b->ops.begin();
        for (;  iter != b->ops.end() ; iter++) {
            pcodeop *p = *iter;
            if ((iter != b->ops.begin()) && (prev_addr == p->start.getAddr())) continue;

            d->trans->printAssembly(assem, p->start.getAddr());
            fprintf(fp, "%s<br/>", obuf);

            prev_addr = p->start.getAddr();
        }
        fprintf(fp, ">]\n");
    }

    for (i = 0; i < bblocks.list.size(); ++i) {
        blockbasic *b = bblocks.list[i];

        for (j = 0; j < bblocks.outofthis.size(); ++j) {
            blockedge *e = &bblocks.outofthis[j];

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

varnode*    funcdata::clone_varnode(const varnode *vn)
{
    varnode *newvn = new_varnode(vn->size, vn->loc);

    newvn->flags.annotation = vn->flags.annotation;
    newvn->flags.readonly = vn->flags.readonly;

    return newvn;
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

        SeqNum seq(calladdr, op->start.getTime());
        cloneop(op, seq);
    }
}

void        funcdata::inline_flow(funcdata *fd, pcodeop *callop)
{
    if (fd->check_ezmodel()) {
        list<pcodeop *>::const_iterator oiter = deadlist.end();
        --oiter;
        inline_ezclone(fd, callop->get_addr());
        ++oiter;    // 指向inline的第一个pcode

        if (oiter != deadlist.end()) {
            pcodeop *firstop = *oiter;
            oiter = deadlist.end();
            --oiter; // 指向inline的最后一个pcode
            pcodeop *lastop = *oiter;
            if (callop->flags.startblock)
                firstop->flags.startblock = 1;
            else
                firstop->flags.startblock = 0;
        }
    }
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
    flags.block_unreachable = 0;
}

/* build dominator tree*/
void funcdata::build_dom_tree()
{
}
