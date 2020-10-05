
class dobc : public SleighArchitecture {
    string filename;
    string slafilename;

    ContextInternal *context;
    Sleigh *trans;

public:
    dobc(const char *slafilename, const char *filename);
    ~dobc();

    int run();

    /* 在一个函数内inline另外一个函数 */
    int inline_func(LoadImageFunc &func1, LoadImageFunc &func2);
    int loop_unrolling(LoadImageFunc &func1, Address &pos);
    /* 设置安全区，安全区内的代码是可以做别名分析的 */
    int set_safe_room(Address &pos, int size);

    void analysis();
    void dump_pcode();
    void dump_function(LoadImageFunc &func1, int pcode);
    AddrSpace *get_code_space() { return trans->getDefaultCodeSpace();  }

    void plugin_dvmp360();
    void plugin_dvmp();

    virtual void buildLoader(DocumentStorage &storage) {
    }
};