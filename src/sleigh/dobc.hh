
#include "mcore/mcore.h"
#include "elfloadimage.hh"

typedef struct funcdata     funcdata;
typedef struct pcodeop      pcodeop;
typedef struct varnode      varnode;
typedef struct flowblock    flowblock, blockbasic, blockgraph;
typedef struct dobc         dobc;
typedef struct jmptable     jmptable;
typedef struct func_call_specs  func_call_specs;

class pcodeemit2 : public PcodeEmit {
public:
    funcdata *fd;
    virtual void dump(const Address &address, OpCode opc, VarnodeData *outvar, VarnodeData *vars, int size);
};

struct varnode {
    struct {
        unsigned    mark : 1;
        unsigned    constant : 1;
        unsigned    annotation : 1;
        unsigned    input : 1;          // 没有祖先
        unsigned    writtern : 1;       // 是def
        unsigned    insert : 1;
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

    list<pcodeop *> descend;

    varnode(int s, const Address &m);
    ~varnode();

    const Address &get_addr(void) { return loc; }
};

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
        unsigned startmark : 1;     // instruction的第一个pcode
        unsigned mark : 1;          // 临时性标记，被某些算法拿过来做临时性处理，处理完都要重新清空

        unsigned branch_call : 1;   // 一般的跳转都是在函数内进行的，但是有些壳的函数，会直接branch到另外一个函数里面去
        unsigned exit : 1;          // 这个指令起结束作用
    } flags;

    OpCode opcode;
    /* 一个指令对应于多个pcode，这个是用来表示inst的 */
    SeqNum start;               
    flowblock *parent;

    varnode *output;
    vector<varnode *> inrefs;

    list<pcodeop *>::iterator basiciter;
    list<pcodeop *>::iterator insertiter;
    list<pcodeop *>::iterator codeiter;

    pcodeop(int s, const SeqNum &sq);
    ~pcodeop();

    void            set_opcode(OpCode op);
    varnode*        get_in(int slot) { return inrefs[slot];  }
    const Address&  get_addr() { return start.getAddr();  }
};

typedef struct blockedge            blockedge;

#define a_tree_edge             0x1
#define a_forward_edge          0x2
#define a_cross_edge            0x4
#define a_back_edge             0x8
#define a_loop_edge             0x10

struct blockedge {
    int label;
    flowblock *point;
    int reverse_index;

    blockedge(flowblock *pt, int lab, int rev) { point = pt, label = lab; reverse_index = rev; }
    blockedge() {};
};

enum block_type{
    a_condition,
    a_if,
    a_whiledo,
    a_dowhile,
    a_switch,
};

struct flowblock {
    enum block_type     type;

    struct {
        unsigned f_goto_goto : 1;
        unsigned f_break_goto : 1;
        unsigned f_continue_goto : 1;
        unsigned f_entry_point : 1;
        unsigned f_dead : 1;

        unsigned f_switch_case : 1;
        unsigned f_switch_default : 1;

        /* 在某些算法中，做临时性标记用 */
        unsigned f_mark : 1;

        unsigned f_return : 1;
    } flags = { 0 };

    struct {
        Address     start;
        Address     end;
    } cover;

    list<pcodeop*>      ops;

    flowblock *parent = NULL;
    flowblock *immed_dom = NULL;

    int index = 0;
    int visitcount = 0;
    int numdesc = 0;        // 在 spaning tree中的后代数量

    vector<blockedge>   in;
    vector<blockedge>   out;
    vector<flowblock *> blist;

    jmptable *jmptable = NULL;

    funcdata *fd;

    flowblock(funcdata *fd);
    ~flowblock();

    void        add_block(flowblock *b);
    blockbasic* new_block_basic(funcdata *f);
    flowblock*  get_out(int i) { return out[i].point;  }
    flowblock*  get_in(int i) { return in[i].point;  }

    void        set_start_block(flowblock *bl);
    void        set_initial_range(const Address &begin, const Address &end);
    void        add_edge(flowblock *begin, flowblock *end);
    void        add_inedge(flowblock *b, int lab);
    void        add_op(pcodeop *);
    void        insert(list<pcodeop *>::iterator iter, pcodeop *inst);

    int         sub_id() { return (int)cover.start.getOffset();  }
    void        structure_loops(vector<flowblock *> &rootlist);
    void        find_spanning_tree(vector<flowblock *> &preorder, vector<flowblock *> &rootlist);
    void        calc_forward_dominator(const vector<flowblock *> &rootlist);
};

typedef map<SeqNum, pcodeop *>  pcodeop_tree;
typedef struct op_edge      op_edge;
typedef struct jmptable     jmptable;

struct op_edge {
    pcodeop *from;
    pcodeop *to;

    op_edge(pcodeop *from, pcodeop *to);
    ~op_edge();
} ;

struct jmptable {
    pcodeop *op;
    Address opaddr;
    int defaultblock;
    int lastblock;
    int size;

    vector<Address>     addresstable;

    jmptable(pcodeop *op);
    ~jmptable();
};

struct func_call_specs {
    pcodeop *op;
    funcdata *fd;

    func_call_specs(pcodeop *op);
    ~func_call_specs();

    const string &get_name(void) { return fd->name;  }
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
    } flags;

    struct VisitStat {
        SeqNum seqnum;
        int size;
        struct {
            unsigned condinst: 1;
        } flags;
    };

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

    list<pcodeop *>     deadlist;
    list<pcodeop *>     alivelist;
    list<pcodeop *>     storelist;
    list<pcodeop *>     loadlist;
    list<pcodeop *>     useroplist;
    list<pcodeop *>     deadandgone;
    int op_uniqid = 0;

    map<Address,VisitStat> visited;
    dobc *d = NULL;

    struct {
        long uniqbase = 0;
        int uniqid = 0;
        int create_index = 0;
        struct dynarray all = { 0 };
    } vbank;

    /* control-flow graph */
    blockgraph bblocks;

    list<op_edge *>       block_edge;

    int     intput;         // 这个函数有几个输入参数
    int     output;         // 有几个输出参数
    vector<func_call_specs *>   qlst;

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

    vector<Address>     addrlist;
    pcodeemit2 emitter;

    funcdata(const char *name, const Address &a, int size, dobc *d);
    ~funcdata(void);

    const Address&  get_addr(void) { return startaddr;  }
    string&      get_name() { return name;  }
    void        set_range(Address &b, Address &e) { baddr = b; eaddr = e; }

    pcodeop*    newop(int inputs, const SeqNum &sq);
    pcodeop*    newop(int inputs, const Address &pc);
    pcodeop*    cloneop(pcodeop *op, const SeqNum &seq);

    varnode*    new_varnode_out(int s, const Address &m, pcodeop *op);
    varnode*    new_varnode(int s, AddrSpace *base, uintb off);
    varnode*    new_varnode(int s, const Address &m);
    varnode*    new_coderef(const Address &m);
    varnode*    clone_varnode(const varnode *vn);

    varnode*    create_vn(int s, const Address &m);
    varnode*    create_def(int s, const Address &m, pcodeop *op);
    varnode*    create_def_unique(int s, pcodeop *op);

    void        op_set_opcode(pcodeop *op, OpCode opc);
    void        op_set_input(pcodeop *op, varnode *vn, int slot);
    pcodeop*    find_op(const Address &addr);
    pcodeop*    find_op(const SeqNum &num) const;
    void        del_op(pcodeop *op);
    void        del_varnode(varnode *vn);

    void        del_remaining_ops(list<pcodeop *>::const_iterator oiter);
    void        new_address(pcodeop *from, const Address &to);
    pcodeop*    find_rel_target(pcodeop *op, Address &res) const;
    pcodeop*    target(const Address &addr) const;
    pcodeop*    branch_target(pcodeop *op);
    pcodeop*    fallthru_op(pcodeop *op);

    bool        set_fallthru_bound(Address &bound);
    void        fallthru();
    pcodeop*    xref_control_flow(list<pcodeop *>::const_iterator oiter, bool &startbasic, bool &isfallthru);
    void        generate_ops();
    bool        process_instruction(const Address &curaddr, bool &startbasic);
    void        recover_jmptable(pcodeop *op, int indexsize);
    void        analysis_jmptable(pcodeop *op);
    jmptable*   find_jmptable(pcodeop *op);

    void        collect_edges();
    void        generate_blocks();
    void        split_basic();
    void        connect_basic();

    void        dump_inst();
    void        dump_dot(const char *filename);

    void        remove_from_codelist(pcodeop *op);
    void        op_insert_before(pcodeop *op, pcodeop *follow);
    void        op_insert_after(pcodeop *op, pcodeop *prev);
    void        op_insert(pcodeop *op, blockbasic *bl, list<pcodeop *>::iterator iter);
    void        inline_flow(funcdata *inlinefd, pcodeop *fd);
    void        inline_clone(funcdata *inelinefd, const Address &retaddr);
    void        inline_ezclone(funcdata *fd, const Address &calladdr);
    bool        check_ezmodel(void);
    void        structure_reset();

    void        mark_dead(pcodeop *op);
    void        mark_alive(pcodeop *op);
    void        fix_jmptable();
    char*       block_color(flowblock *b);
    void        build_dom_tree();
    void        start_processing(void);
    void        follow_flow(void);
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

    vector<TypeOp *> inst;

    dobc(const char *slafilename, const char *filename);
    ~dobc();

    void init();

    /* 在一个函数内inline另外一个函数 */
    int inline_func(LoadImageFunc &func1, LoadImageFunc &func2);
    int loop_unrolling(LoadImageFunc &func1, Address &pos);
    /* 设置安全区，安全区内的代码是可以做别名分析的 */
    int set_safe_room(Address &pos, int size);

    void analysis();
    void run();
    void dump_function(char *name);
    funcdata* find_func(const Address &addr);
    funcdata* find_func(const char *name);
    AddrSpace *get_code_space() { return trans->getDefaultCodeSpace();  }

    void plugin_dvmp360();
    void plugin_dvmp();

    void gen_sh(void);
};
