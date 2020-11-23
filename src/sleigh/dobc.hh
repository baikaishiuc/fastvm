
#include "mcore/mcore.h"
#include "elfloadimage.hh"

typedef struct funcdata     funcdata;
typedef struct pcodeop      pcodeop;
typedef struct varnode      varnode;
typedef struct flowblock    flowblock, blockbasic, blockgraph;
typedef struct dobc         dobc;
typedef struct jmptable     jmptable;
typedef struct func_call_specs  func_call_specs;
typedef map<Address, vector<varnode *>> variable_stack;
typedef struct valuetype    valuetype;

class pcodeemit2 : public PcodeEmit {
public:
    funcdata *fd = NULL;
    FILE *fp = stdout;
    virtual void dump(const Address &address, OpCode opc, VarnodeData *outvar, VarnodeData *vars, int size);

    void set_fp(FILE *f) { fp = f;  }
};

enum ARMInstType {
    a_null,
    a_stmdb,
    a_sub,
    a_add,
};

struct VisitStat {
    SeqNum seqnum;
    int size;
    struct {
        unsigned condinst: 1;
    } flags;

    enum ARMInstType    inst_type;
};

enum height {
    a_top,
    /* 
    普通常量:
    mov r0, 1
    这样的就是普通的常量, 1是常量, 赋给r0以后，当前的r0也成了常量 */

    a_constant,
    /*
    相对常量

    0x0001. add esp, 4
    0x0002. ...
    0x0003. ...
    0x0004. sub esp, 16
    
    esp就是相对常量，我们在做一些分析的时候，会使用模拟的方式来计算esp，比如给esp一个初始值，这种方式在分析领域不能算错，
    因为比如像IDA或者Ghidra，都有对错误的容忍能力，最终的结果还是需要开发人员自己判断，但是在编译还原领域，不能有这种模糊语义
    的行为。

    假设壳的开发人员，在某个代码中采取了判断esp值的方式来实现了部分功能，比如
    if (esp > xxxxx)  {} else {}
    很可能其中一个 condition_block 就会被判断成 unreachable code。这个是给esp强行赋值带来的结果，但是不赋值可能很多计算进行不下去

    所以我们把 inst.0x0001 中的 esp设置为相对常量(rel_constant)，他的值为 esp.4, 0x0004中的esp 为  esp.16，在做常量传播时，有以下几个规则

    1. const op const = const，      (常量和常量可以互相操作，比如 加减乘除之类)
    2. sp.rel_const op const = sp.rel_const;   (非常危险，要判断这个操作不是在循环内)
    3. sp.rel_const op sp.rel_const = sp.rel_const      (这种情况起始非常少见，)
    4. sp.rel_const op rN.rel_const (top)   (不同地址的相对常量不能参与互相运算)
    */ 
    a_rel_constant,
    a_bottom,

    /* */
};

struct valuetype {
    enum height height;
    intb v;
    Address rel;

    int cmp(const valuetype &b);
    bool operator==(const valuetype &b) { return cmp(b) == 0;  }
    bool operator!=(const valuetype &b) { return !operator==(b); }
};

struct varnode_cmp_loc_def {
    bool operator()(const varnode *a, const varnode *b) const;
};

struct varnode_cmp_def_loc {
    bool operator()(const varnode *a, const varnode *b) const;
};

typedef set<varnode *, varnode_cmp_loc_def> varnode_loc_set;
typedef set<varnode *, varnode_cmp_def_loc> varnode_def_set;

struct varnode {
    /* varnode的值类型和值，在编译分析过后就不会被改*/
    valuetype   type;

    struct {
        unsigned    mark : 1;
        unsigned    annotation : 1;
        unsigned    input : 1;          // 没有祖先
        unsigned    written : 1;       // 是def
        unsigned    insert : 1;         // 这个
        unsigned    implied : 1;        // 是一个临时变量
        unsigned    exlicit : 1;        // 不是临时变量

        unsigned    readonly : 1;

        unsigned    covertdirty : 1;    // cover没跟新
    } flags = { 0 };

    int size = 0;
    int create_index = 0;
    Address loc;

    pcodeop     *def = NULL;
    uintb       nzm;

    varnode_loc_set::iterator lociter;  // sort by location
    varnode_def_set::iterator defiter;  // sort by definition

    list<pcodeop *>     uses;    // descend, Ghidra把这个取名为descend，搞的我头晕，改成use

    varnode(int s, const Address &m);
    ~varnode();

    const Address &get_addr(void) const { return (const Address &)loc; }
    bool            is_heritage_known(void) const { return (flags.insert | flags.annotation) || is_constant(); }
    bool            has_no_use(void) { return uses.empty();  }

    void            set_def(pcodeop *op);
    pcodeop*        get_def() { return def;  }
    bool            is_constant(void) const { return type.height == a_constant;  }
    void            set_val(intb v) { type.height = a_constant;  type.v = v; }
    bool            is_rel_constant(void) { return type.height == a_rel_constant;  }
    void            set_rel_constant(Address &r, int v) { type.height = a_rel_constant; type.v = v;  type.rel = r;  }
    intb            get_val(void);
    Address         &get_rel(void) { return type.rel; }

    void            add_use(pcodeop *op);
    void            del_use(pcodeop *op);
    bool            is_free() { return !flags.written && !flags.input;  }
};

#define PCODE_DUMP_VAL              0x01
#define PCODE_DUMP_UD               0x02
#define PCODE_DUMP_DEAD             0x04
/* 有些def的值被use的太多了，可能有几百个，导致整个cfg图非常的不美观，可以开启这个标记，打印cfg时，只打印部分use，
具体多少，可以参考print_udchain的值
*/

#define PCODE_OMIT_MORE_USE         0x08            
#define PCODE_OMIT_MORE_DEF         0x10            
#define PCODE_OMIT_MORE_BUILD       0x20            
#define PCODE_OMIT_MORE_IN          0x40

#define PCODE_DUMP_ALL              ~(PCODE_OMIT_MORE_USE | PCODE_OMIT_MORE_DEF | PCODE_OMIT_MORE_BUILD | PCODE_OMIT_MORE_IN)
#define PCODE_DUMP_SIMPLE           0xffffffff

struct pcodeop {
    struct {
        unsigned startblock : 1;
        unsigned branch : 1;
        unsigned call : 1;
        unsigned returns: 1;
        unsigned nocollapse : 1;
        unsigned dead : 1;
        unsigned marker : 1;        // 特殊的站位符， (phi 符号 或者 间接引用 或 CPUI_COPY 对同一个变量的操作)，
        unsigned boolouput : 1;     // 布尔操作

        unsigned coderef : 1;
        unsigned startinst : 1;     // instruction的第一个pcode
        /* 临时算法有用:
        1. compute_sp
        */
        unsigned mark : 1;          // 临时性标记，被某些算法拿过来做临时性处理，处理完都要重新清空

        unsigned branch_call : 1;   // 一般的跳转都是在函数内进行的，但是有些壳的函数，会直接branch到另外一个函数里面去
        unsigned exit : 1;          // 这个指令起结束作用
        unsigned inlined : 1;       // 这个opcode已经被inline过了
        unsigned changed : 1;       // 这个opcode曾经被修改过
    } flags;

    OpCode opcode;
    /* 一个指令对应于多个pcode，这个是用来表示inst的 */
    SeqNum start;               
    flowblock *parent;
    /* 我们认为程序在分析的时候，sp的值是可以静态分析的，他表示的并不是sp寄存器，而是系统当前堆栈的深度 */
    int     sp = 0;

    varnode *output = NULL;
    vector<varnode *> inrefs;

    list<pcodeop *>::iterator basiciter;
    list<pcodeop *>::iterator insertiter;
    list<pcodeop *>::iterator codeiter;

    pcodeop(int s, const SeqNum &sq);
    ~pcodeop();

    void            set_opcode(OpCode op);
    varnode*        get_in(int slot) { return inrefs[slot];  }
    varnode*        get_out() { return output;  }
    const Address&  get_addr() { return start.getAddr();  }

    int             num_input() { return inrefs.size();  }
    void            clear_input(int slot) { inrefs[slot] = NULL; }
    void            remove_input(int slot);
    void            insert_input(int slot);

    int             get_slot(const varnode *vn) { 
        int i, n; n = inrefs.size(); 
        for (i = 0; i < n; i++)
            if (inrefs[i] == vn) return i;
        return -1;
    }
    int             dump(char *buf, uint32_t flags);
    /* trace compute 
    这个compute_t 和传统的compute不一样，普通的comupte只能用来做常量传播，这个compute_t
    是在循环展开时，沿某条路径开始计算

    compute_t 和 模拟执行是完全不一样的， 模拟执行会给所有系统寄存器赋值，然后走入函数，
    compute_t 不会做这样，compute_t是在某条路径上执行，假如可以计算，就计算，假如不能计算
    就跳过

@inslot         常量传播时，填-1，假如在做trace分析时，填从哪个快进入
@return     
            1       unknown bcond
    */

    /* 碰见了可以计算出的跳转地址 */
#define         ERR_MEET_CALC_BRANCH            1
#define         ERR_UNPROCESSED_ADDR            2
#define         ERR_CONST_CBRANCH               4
    int             compute(int inslot, flowblock **branch);
    /* FIXME:判断哪些指令是别名安全的 */
    bool            is_safe_inst();
    void            set_output(varnode *vn) { output = vn;  }
};

typedef struct blockedge            blockedge;

#define a_tree_edge             0x1
#define a_forward_edge          0x2
#define a_cross_edge            0x4
#define a_back_edge             0x8
#define a_loop_edge             0x10
#define a_true_edge             0x20

struct blockedge {
    int label;
    flowblock *point;
    int reverse_index;

    blockedge(flowblock *pt, int lab, int rev) { point = pt, label = lab; reverse_index = rev; }
    blockedge() {};
    bool is_true_edge() { return label & a_true_edge;  }
};


enum block_type{
    a_condition,
    a_if,
    a_whiledo,
    a_dowhile,
    a_switch,
};

/* 模拟stack行为，*/
struct mem_stack {

    int size;
    char *data;
    
    mem_stack();
    ~mem_stack();

    void    push(char *byte, int size);
    int     top(int size);
    int     pop(int size);
};

struct flowblock {
    enum block_type     type;

    struct {
        unsigned f_goto_goto : 1;
        unsigned f_break_goto : 1;
        unsigned f_continue_goto : 1;
        unsigned f_entry_point : 1;
        /* 
        1. 在cbranch中被分析为不可达，确认为死 */
        unsigned f_dead : 1;

        unsigned f_switch_case : 1;
        unsigned f_switch_default : 1;

        /* 在某些算法中，做临时性标记用 

        1. reachable测试
        2. reducible测试
        */
        unsigned f_mark : 1;

        unsigned f_return : 1;

    } flags = { 0 };

    RangeList cover;

    list<pcodeop*>      ops;

    flowblock *parent = NULL;
    flowblock *immed_dom = NULL;
    flowblock *copymap = NULL;

    /* 这个index是 反后序遍历的索引，用来计算支配节点数的时候需要用到 */
    int index = 0;
    int dfnum = 0;
    int numdesc = 1;        // 在 spaning tree中的后代数量，自己也是自己的后代，所以假如正式计算后代数量，自己起始为1

    vector<blockedge>   in;
    vector<blockedge>   out;
    vector<flowblock *> blist;
    /* 有些block是不可到达的，都放到这个列表内 */
    vector<flowblock *> deadlist;

    jmptable *jmptable = NULL;

    funcdata *fd;

    flowblock(funcdata *fd);
    ~flowblock();

    void        add_block(flowblock *b);
    blockbasic* new_block_basic(funcdata *f);
    flowblock*  get_out(int i) { return out[i].point;  }
    flowblock*  get_in(int i) { return in[i].point;  }
    flowblock*  get_block(int i) { return blist[i]; }
    pcodeop*    first_op(void) { return *ops.begin();  }
    pcodeop*    last_op(void) { return *--ops.end();  }
    int         get_out_rev_index(int i) { return out[i].reverse_index;  }

    void        set_start_block(flowblock *bl);
    void        set_initial_range(const Address &begin, const Address &end);
    void        add_op(pcodeop *);
    void        insert(list<pcodeop *>::iterator iter, pcodeop *inst);

    int         sub_id();
    void        structure_loops(vector<flowblock *> &rootlist);
    void        find_spanning_tree(vector<flowblock *> &preorder, vector<flowblock *> &rootlist);
    void        dump_spanning_tree(const char *filename, vector<flowblock *> &rootlist);
    void        calc_forward_dominator(const vector<flowblock *> &rootlist);
    void        build_dom_tree(vector<vector<flowblock *>> &child);
    int         build_dom_depth(vector<int> &depth);
    bool        find_irrereducible(const vector<flowblock *> &preorder, int &irreduciblecount);
    void        calc_loop();

    int         get_size(void) { return blist.size();  }
    Address     get_start(void);

    bool        is_back_edge_in(int i) { return in[i].label & a_back_edge; }
    void        set_mark() { flags.f_mark = 1;  }
    void        clear_mark() { flags.f_mark = 0;  }
    void        clear_marks(void);
    bool        is_mark() { return flags.f_mark;  }
    bool        is_entry_point() { return flags.f_entry_point;  }
    flowblock*  get_entry_point(void);
    int         get_in_index(const flowblock *bl);
    int         get_out_index(const flowblock *bl);

    void        clear(void);
    void        remove_edge(flowblock *begin, flowblock *end);
    void        add_edge(flowblock *begin, flowblock *end);
    void        add_in_edge(flowblock *b, int lab);
    void        remove_in_edge(int slot);
    void        remove_out_edge(int slot);
    void        half_delete_out_edge(int slot);
    void        half_delete_in_edge(int slot);
    int         get_back_edge_count(void);
    /* 当这个block的末尾节点为cbranch节点时，返回条件为真或假的跳转地址 */
    blockedge*  get_true_edge(void);
    blockedge*  get_false_edge(void);

    void        set_out_edge_flag(int i, uint4 lab);
    void        clear_out_edge_flag(int i, uint4 lab);

    int         get_inslot(flowblock *inblock) {
        for (int i = 0; i < in.size(); i++) {
            if (in[i].point == inblock)
                return i;
        }

        return -1;
    }

    void        set_dead(void) { flags.f_dead = 1;  }
    int         is_dead(void) { return flags.f_dead;  }
    void        remove_from_flow(flowblock *bl);
    void        remove_op(pcodeop *inst);
    void        remove_block(flowblock *bl);
    void        collect_reachable(vector<flowblock *> &res, flowblock *bl, bool un) const;
    void        splice_block(flowblock *bl);
    void        move_out_edge(flowblock *blold, int slot, flowblock *blnew);
    void        replace_in_edge(int num, flowblock *b);
};

typedef struct priority_queue   priority_queue;

struct priority_queue {
    vector<vector<flowblock *>> queue;
    int curdepth;

    priority_queue(void) { curdepth = -2;  }
    void reset(int maxdepth);
    void insert(flowblock *b, int depth);
    flowblock *extract();
    bool empty(void) const { return (curdepth == -1);  }
};

typedef map<SeqNum, pcodeop *>  pcodeop_tree;
typedef struct op_edge      op_edge;
typedef struct jmptable     jmptable;

struct op_edge {
    pcodeop *from;
    pcodeop *to;
    int t = 0;

    op_edge(pcodeop *from, pcodeop *to);
    ~op_edge();
} ;

struct funcproto {
    struct {
        unsigned vararg : 1;        // variable argument
        unsigned exit : 1;          // 调用了这个函数会导致整个流程直接结束，比如 exit, stack_check_fail
    } flags;
    int     inputs;
    int     output;
    string  name;
    Address addr;
};

struct jmptable {
    pcodeop *op;
    Address opaddr;
    int defaultblock;
    int lastblock;
    int size;

    vector<Address>     addresstable;

    jmptable(pcodeop *op);
    jmptable(const jmptable *op2);
    ~jmptable();

    void    update(funcdata *fd);
};

struct funcdata {
    struct {
        unsigned blocks_generated : 1;
        unsigned blocks_unreachable : 1;    // 有block无法到达
        unsigned processing_started : 1;
        unsigned processing_complete : 1;
        unsigned no_code : 1;
        unsigned unimplemented_present : 1;
        unsigned baddata_present : 1;

        unsigned safezone : 1;
        unsigned plt : 1;               // 是否是外部导入符号
        unsigned exit : 1;              // 有些函数有直接结束整个程序的作用，比如stack_check_fail, exit, abort
    } flags = { 0 };

    int op_generated = 0;

    pcodeop_tree     optree;
    AddrSpace   *uniq_space = NULL;

    struct {
        funcdata *next = NULL;
        funcdata *prev = NULL;
    } node;

    list<op_edge *>    edgelist;

    /* jmp table */
    vector<pcodeop *>   tablelist;
    vector<jmptable *>  jmpvec;

    /* op_gen_iter 用来分析ops时用到的，它指向上一次分析到的pcode终点 */
    list<pcodeop *>::iterator op_gen_iter;
    /* deadlist用来存放所有pcode */
    list<pcodeop *>     deadlist;
    list<pcodeop *>     storelist;
    list<pcodeop *>     loadlist;
    list<pcodeop *>     useroplist;
    list<pcodeop *>     deadandgone;
    /* 可以做别名分析的 storelist */
    list<pcodeop *>     safe_storelist;
    int op_uniqid = 0;

    map<Address,VisitStat> visited;
    dobc *d = NULL;

    /* vbank------------------------- */
    struct {
        long uniqbase = 0;
        int uniqid = 0;
        int create_index = 0;
        struct dynarray all = { 0 };
    } vbank;

    varnode_loc_set     loc_tree;
    varnode_def_set     def_tree;
    varnode             searchvn;
    /* vbank------------------------- */

    /* control-flow graph */
    blockgraph bblocks;

    list<op_edge *>       block_edge;

    int     intput;         // 这个函数有几个输入参数
    int     output;         // 有几个输出参数
    list<func_call_specs *>     qlst;

    /* heritage start ................. */
    vector<vector<flowblock *>> domchild;
    vector<vector<flowblock *>> augment;
#define boundary_node       1
#define mark_node           2
#define merged_node          3
    vector<uint4>   phiflags;   
    vector<int>     domdepth;
    /* dominate frontier */
    vector<flowblock *>     merge;      // 哪些block包含phi节点
    priority_queue pq;

    int maxdepth = -1;

    LocationMap     disjoint;
    LocationMap     globaldisjoint;
    /* FIXME:我不是很理解这个字段的意思，所以我没用他，一直恒为0 */
    int pass = 0;

    /* heritage end  ============================================= */

    Address startaddr;

    Address baddr;
    Address eaddr;
    string fullpath;
    string name;
    int size = 0;

    /* 扫描到的最小和最大指令地址 */
    Address minaddr;
    Address maxaddr;
    int inst_count = 0;
    int inst_max = 1000000;

    /* 这个区域内的所有可以安全做别名分析的点 */
    RangeList   safezone;
    intb        safezone_base;

    vector<Address>     addrlist;
    /* 常量cbranch列表 */
    vector<pcodeop *>    cbrlist;
    pcodeemit2 emitter;

    struct {
        int     size;
        u1      *bottom;
        u1      *top;
    } memstack;

    funcdata(const char *name, const Address &a, int size, dobc *d);
    ~funcdata(void);

    const Address&  get_addr(void) { return startaddr;  }
    string&      get_name() { return name;  }
    void        set_range(Address &b, Address &e) { baddr = b; eaddr = e; }
    void        set_op_uniqid(int val) { op_uniqid = val;  }
    int         get_op_uniqid() { return op_uniqid; }

    pcodeop*    newop(int inputs, const SeqNum &sq);
    pcodeop*    newop(int inputs, const Address &pc);
    pcodeop*    cloneop(pcodeop *op, const SeqNum &seq);
    void        op_destroy_raw(pcodeop *op);
    void        op_destroy(pcodeop *op);

    varnode*    new_varnode_out(int s, const Address &m, pcodeop *op);
    varnode*    new_varnode(int s, AddrSpace *base, uintb off);
    varnode*    new_varnode(int s, const Address &m);
    /* new_coderef是用来创建一些程序位置的引用点的，但是这个函数严格来说是错的····，因为标识函数在pcode
    的体系中，多个位置他们的address可能是一样的
    */
    varnode*    new_coderef(const Address &m);
    varnode*    new_unique(int s);
    varnode*    new_unique_out(int s, pcodeop *op);

    varnode*    clone_varnode(const varnode *vn);
    void        destroy_varnode(varnode *vn);
    void        delete_varnode(varnode *vn);
    /* 设置输入参数 */
    varnode*    set_input_varnode(varnode *vn);

    varnode*    create_vn(int s, const Address &m);
    varnode*    create_def(int s, const Address &m, pcodeop *op);
    varnode*    create_def_unique(int s, pcodeop *op);
    varnode*    xref(varnode *vn);
    varnode*    set_def(varnode *vn, pcodeop *op);

    void        op_resize(pcodeop *op, int size);
    void        op_set_opcode(pcodeop *op, OpCode opc);
    void        op_set_input(pcodeop *op, varnode *vn, int slot);
    void        op_set_output(pcodeop *op, varnode *vn);
    void        op_unset_input(pcodeop *op, int slot);
    void        op_unset_output(pcodeop *op);
    void        op_remove_input(pcodeop *op, int slot);
    void        op_insert_input(pcodeop *op, varnode *vn, int slot);
    void        op_zero_multi(pcodeop *op);

    pcodeop*    find_op(const Address &addr);
    pcodeop*    find_op(const SeqNum &num) const;
    void        del_op(pcodeop *op);
    void        del_varnode(varnode *vn);

    varnode_loc_set::const_iterator     begin_loc(const Address &addr);
    varnode_loc_set::const_iterator     end_loc(const Address &addr);
    varnode_loc_set::const_iterator     begin_loc(AddrSpace *spaceid);
    varnode_loc_set::const_iterator     end_loc(AddrSpace *spaceid);

    void        del_remaining_ops(list<pcodeop *>::const_iterator oiter);
    void        new_address(pcodeop *from, const Address &to);
    pcodeop*    find_rel_target(pcodeop *op, Address &res) const;
    pcodeop*    target(const Address &addr) const;
    pcodeop*    branch_target(pcodeop *op);
    pcodeop*    fallthru_op(pcodeop *op);

    bool        set_fallthru_bound(Address &bound);
    void        fallthru();
    pcodeop*    xref_control_flow(list<pcodeop *>::const_iterator oiter, bool &startbasic, bool &isfallthru);
    void        generate_ops_start(void);
    void        generate_ops(void);
    list<pcodeop *>::const_iterator get_gen_op_iter(void);
    bool        process_instruction(const Address &curaddr, bool &startbasic);
    void        recover_jmptable(pcodeop *op, int indexsize);
    void        analysis_jmptable(pcodeop *op);
    jmptable*   find_jmptable(pcodeop *op);

    void        collect_edges();
    void        generate_blocks();
    void        split_basic();
    void        connect_basic();

    void        dump_inst();
    void        dump_block(FILE *fp, blockbasic *b, int pcode);
    /* flag: 1: enable pcode */
    void        dump_cfg(const char *postfix, int flag);
    void        dump_pcode(const char *postfix);
    /* dump dom-joint graph */
    void        funcdata::dump_djgraph(const char *postfix, int flag);

    void        remove_from_codelist(pcodeop *op);
    void        op_insert_before(pcodeop *op, pcodeop *follow);
    void        op_insert_after(pcodeop *op, pcodeop *prev);
    void        op_insert(pcodeop *op, blockbasic *bl, list<pcodeop *>::iterator iter);
    void        op_insert_begin(pcodeop *op, blockbasic *bl);
    void        op_insert_end(pcodeop *op, blockbasic *bl);
    void        inline_flow(funcdata *inlinefd, pcodeop *fd);
    void        inline_clone(funcdata *inelinefd, const Address &retaddr);
    void        inline_call(string name, int num);
    void        inline_call(const Address &addr, int num);
    void        inline_ezclone(funcdata *fd, const Address &calladdr);
    bool        check_ezmodel(void);
    void        structure_reset();

    void        mark_dead(pcodeop *op);
    void        mark_alive(pcodeop *op);
    void        mark_free(varnode *vn);
    void        fix_jmptable();
    char*       block_color(flowblock *b);
    char*       edge_color(blockedge *e);
    int         edge_width(blockedge *e);
    void        build_dom_tree();
    void        start_processing(void);
    void        follow_flow(void);
    void        add_callspec(pcodeop *p, funcdata *fd);
    void        clear_blocks();
    void        clear_blocks_mark();
    int         inst_size(const Address &addr);
    void        build_adt(void);
    void        calc_phi_placement(const vector<varnode *> &write);
    void        visit_incr(flowblock *qnode, flowblock *vnode);
    void        place_multiequal(void);
    void        rename();
    void        rename_recurse(blockbasic *bl, variable_stack &varstack);
    int         collect(Address addr, int size, vector<varnode *> &read,
        vector<varnode *> &write, vector<varnode *> &input);
    void        heritage(void);
    void        heritage_clear(void);
    /* 
    listtype:       常量传播分析的列表，0:默认的全oplist，1:新增加的safe_storelist
    return:
    -1: 严重错误
    0: ok
    1: 发现可以被别名分析的load store */
    int         constant_propagation(int listype);
    int         cond_constant_propagation();
    int         in_cbrlist(pcodeop *op) {
        for (int i = 0; i < cbrlist.size(); i++) {
            if (cbrlist[i] == op)
                return 1;
        }

        return 0;
    }
    /* compute sp要计算必须得满足2个要求
    
    1. block 已经被generated
    2. constant_propagation 被执行过
    */
    void        compute_sp(void);
    bool        is_ram(varnode *v);
    bool        is_sp_rel_constant(varnode *v);

    void        set_safezone_base(intb base) { safezone_base = base; }
    void        set_safezone(intb addr, int size);
    bool        in_safezone(intb addr, int size);
    void        enable_safezone(void);
    void        disable_safezone(void);

    intb        get_stack_value(intb offset, int size);
    void        set_stack_value(intb offset, int size, intb val);
    void        add_to_codelist(pcodeop *op);
    void        calc_load_store_info();

    void        set_plt(int v) { flags.plt = v; };
    void        set_exit(int v) { flags.exit = v; }
    bool        test_hard_inline_restrictions(funcdata *inlinefd, pcodeop *op, Address &retaddr);
    bool        is_first_op(pcodeop *op);

    /* 获取loop 的头节点的in 节点，假如有多个，按index顺序取一个 */
    pcodeop*    loop_pre_get(flowblock *h, int index);
    bool        trace_push(vector<pcodeop *> trace, pcodeop *op);
    void        trace_push_block(vector<pcodeop *> trace, flowblock *next);
    bool        loop_unrolling(flowblock *h, int times);
    flowblock*  get_vm_loop_header(void);

    bool        use_outside(varnode *vn);
    void        use2undef(varnode *vn);
    void        branch_remove(blockbasic *bb, int num);
    void        branch_remove_internal(blockbasic *bb, int num);
    void        block_remove_internal(blockbasic *bb, bool unreachable);
    bool        remove_unreachable_blocks(bool issuewarnning, bool checkexistence);
    void        splice_block_basic(blockbasic *bl);
};


struct func_call_specs {
    pcodeop *op;
    funcdata *fd;

    func_call_specs(pcodeop *o, funcdata *f);
    ~func_call_specs();

    const string &get_name(void) { return fd->name;  }
    const Address &get_addr() { return fd->get_addr(); }
};

struct dobc {
    ElfLoadImage *loader;
    string slafilename;

    string fullpath;
    string filename;

    ContextDatabase *context = NULL;
    Translate *trans = NULL;
    TypeFactory *types;

    struct {
        int counts = 0;
        funcdata *list = NULL;
    } funcs;

    int max_basetype_size;
    int min_funcsymbol_size;
    int max_instructions;
    map<string, string>     abbrev;

    Address     sp_addr;
    Address     lr_addr;

    dobc(const char *slafilename, const char *filename);
    ~dobc();

    void init();
    /* 初始化位置位置无关代码，主要时分析原型 */
    void dobc::init_plt(void);

    /* 在一个函数内inline另外一个函数 */
    int inline_func(LoadImageFunc &func1, LoadImageFunc &func2);
    int loop_unrolling(LoadImageFunc &func1, Address &pos);
    /* 设置安全区，安全区内的代码是可以做别名分析的 */

    void analysis();
    void run();
    void dump_function(char *name);
    void add_func(funcdata *fd);
    funcdata* find_func(const Address &addr);
    funcdata* find_func(const char *name);
    AddrSpace *get_code_space() { return trans->getDefaultCodeSpace();  }
    AddrSpace *get_uniq_space() { return trans->getUniqueSpace();  }

    void plugin_dvmp360();
    void plugin_dvmp();

    void gen_sh(void);
    void init_abbrev();
    const string &get_abbrev(const string &name);
};
