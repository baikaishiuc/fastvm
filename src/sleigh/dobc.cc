

#include "sleigh_arch.hh"
#include "ElfLoadImage.hh"
#include "sleigh.hh"
#include "dobc.hh"
#include <iostream>

static char help[] = {
    "dobc [.sla filename] [filename]"
};

class PcodeRawOut1 : public PcodeEmit {

public:
    virtual void dump(const Address &address, OpCode opc, VarnodeData *outvar, VarnodeData *vars, int size);
};

class AssemblyRaw : public AssemblyEmit {
public:
  virtual void dump(const Address &addr,const string &mnem,const string &body) {
    addr.printRaw(cout);
    cout << ": " << mnem << ' ' << body << endl;
  }
};

static void print_vardata(ostream &s, VarnodeData &data)

{
    s << '(' << data.space->getName() << ',';
    data.space->printOffset(s, data.offset);
    s << ',' << dec << data.size << ')';
}

static dobc *g_dobc = NULL;

void PcodeRawOut1::dump(const Address &addr, OpCode opc, VarnodeData *outvar, VarnodeData *vars, int4 isize)
{
    int i;
    if (outvar != (VarnodeData *)0) {
        print_vardata(cout, *outvar);
        cout << " = ";
    }
    cout << get_opname(opc);
    // Possibly check for a code reference or a space reference
    for (i = 0; i < isize; ++i) {
        cout << ' ';
        print_vardata(cout, vars[i]);
    }
    cout << endl;

#if 0
    if (opc == CPUI_LOAD) {
        if ((vars[0].space->getType() == IPTR_CONSTANT)
            && (vars[0].offset == (uintb)g_dobc->get_code_space())
            && (vars[1].space->getType() == IPTR_CONSTANT)) {
            g_dobc->loader->markData((int)vars[1].offset);
        }
    }
#endif
}

// FIXME:loader的类型本来应该是LoadImageB的，但是不知道为什么loader的getNextSymbol访问的是
// LoadImageB的，而不是ElfLoadImage的···
void dobc::dump_pcode()
{
    LoadImageFunc sym;

#if 0
    while (loader->getNextSymbol(sym)) {
        if (!sym.size) continue;

        dump_function(sym, 1);
    }
#else
    //loader->getSymbol("strcmp", sym);
    //dump_function(sym, 1);
#endif
}

void dobc::dump_function(LoadImageFunc &sym, int pcode)
{
#if 0
    PcodeRawOut1 emit;
    AssemblyRaw assememit;
    int length;

    Address addr(sym.address);
    Address lastaddr(trans->getDefaultCodeSpace(), sym.address.getOffset() + sym.size);

    printf("function:%s\n", sym.name.c_str());

    while (addr < lastaddr) {
        if (this->loader->isData(addr)) break;

        trans->printAssembly(assememit, addr);
        length = trans->oneInstruction(emit, addr);
        addr = addr + length;
    }
#endif
    printf("\n");
}

dobc::dobc(const char *sla, const char *bin)
    : SleighArchitecture(NULL, NULL, NULL)
{
#if 0
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
#endif
}

dobc::~dobc()
{
}

#if 1 //defined(DOBC)
int main(int argc, char **argv)
{
    if (argc != 3) {
        puts(help);
        return 0;
    }

    dobc d(argv[1], argv[2]);

    g_dobc = &d;

    d.dump_pcode();

    return 0;
}

#endif
