
#ifndef __semantics_h__
#define __semantics_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "opcodes.h"
#include "context.h"

// we remap these opcodes for internal use during pcode generation

#define BUILD       CPUI_MULTIEQUAL
#define DELAY_SLOT  CPUI_INDIRECT
#define CROSSBUILD  CPUI_PTRSUB
    // zhengxianwei
    // 这个地方虽然叫macrobuild，但是其实他是macro call
    // 我们定义一个macro的时候不会产生pcode，只有在构造constructor或者subtable的时候
    // 在rtlbody内使用macro的时候，这个macro会像c语言里面的宏一样进行展开，展开的过程
    // 被叫做macrobuild
#define MACROBUILD  CPUI_CAST
#define LABELBUILD  CPUI_PTRADD

typedef struct ConstTpl     ConstTpl;
typedef struct VarnodeTpl   VarnodeTpl;
typedef struct HandleTpl    HandleTpl;
typedef struct OpTpl        OpTpl;
typedef struct ConstructTpl ConstructTpl;

typedef enum const_type { 
    real=0,
    handle=1,
    j_start=2,
    j_next=3,
    j_curspace=4,
    j_curspace_size=5, 
    spaceid=6, 
    j_relative=7,
    j_flowref=8, 
    j_flowref_size=9, 
    j_flowdest=10, 
    j_flowdest_size=11
} const_type;

typedef enum v_field {
    v_space=0,
    v_offset=1,
    v_size=2,
    v_offset_plus=3
} v_field;

struct ConstTpl { 
  const_type type;
  union {
    //    uintb real;			// an actual constant
    AddrSpace *spaceid;	// Id (pointer) for registered space
    int4 handle_index;		// Place holder for run-time determined value
  } value;
  uintb value_real;
  v_field select;		// Which part of handle to use as constant
};

ConstTpl*   ConstTpl_clone(ConstTpl *);
ConstTpl*   ConstTpl_newA(AddrSpace *space);
ConstTpl*   ConstTpl_new0(void);
ConstTpl*   ConstTpl_new1(const_type tp);
ConstTpl*   ConstTpl_new2(const_type tp, uintb val);
ConstTpl*   ConstTpl_new3(const_type tp, int4 ht, v_field vf);
ConstTpl*   ConstTpl_new4(const_type tp, int4 ht, v_field vf, uintb plus);

void        ConstTpl_delete(ConstTpl *);
void        ConstTpl_printHandleSelector(FILE *fout, v_field val);
v_field     ConstTpl_readHandleSelector(const char *name);
bool        ConstTpl_isEqual(ConstTpl *lhs, ConstTpl *rhs);
void        ConstTpl_transfer(ConstTpl *c, struct dynarray *params);
bool        ConstTpl_isUniqueSpace(ConstTpl *c);

#define ConstTpl_getSpace(ct)       (ct)->value.spaceid
#define ConstTpl_getReal(ct)        (ct)->value_real
#define ConstTpl_isZero(ct)         (((ct)->type == real) && ((ct)->value_real == 0))


struct VarnodeTpl {
  ConstTpl *space;
  ConstTpl *offset;
  ConstTpl *size;
  bool unnamed_flag;
};

#define VarnodeTpl_getOffset(v)             &v->offset

VarnodeTpl*     VarnodeTpl_new();
VarnodeTpl*     VarnodeTpl_clone(VarnodeTpl *vn);
VarnodeTpl*     VarnodeTpl_new2(int hand, bool zerosize);
VarnodeTpl*     VarnodeTpl_new3(ConstTpl *sp, ConstTpl *off, ConstTpl *sz);
void            VarnodeTpl_delete(VarnodeTpl *vn);
bool            VarnodeTpl_isLocalTemp(VarnodeTpl *vn);
void            VarnodeTpl_setOffset(VarnodeTpl *vn, uintb val);
int             VarnodeTpl_transfer(VarnodeTpl *vn, struct dynarray *params);
#define VarnodeTpl_setSize(c, sz)           c->size = sz
#define VarnodeTpl_isRelative(v)            ((v)->offset->type == j_relative)
#define VarnodeTpl_isZeroSize(v)            ConstTpl_isZero((v)->size)

struct OpTpl {
  VarnodeTpl *output;
  OpCode opc;
  struct dynarray input;    // VarnodeTpl *
};

OpTpl*      OpTpl_new();
OpTpl*      OpTpl_new1(OpCode oc);
void        OpTpl_delete(OpTpl *);

void        OpTpl_clearOutput(OpTpl *o);
void        OpTpl_addInput(OpTpl *o, VarnodeTpl *vt);
bool        OpTpl_isZeroSize(OpTpl *o);
#define OpTpl_getOut(o)                 o->output
#define OpTpl_setOutput(o, out)         o->output = out
#define OpTpl_getIn(o, i)               ((VarnodeTpl *)o->input.ptab[i])


struct ConstructTpl {
  uint4 delayslot;
  uint4 numlabels;		// Number of label templates
  struct dynarray vec;
  HandleTpl *result;
};

#define ConstructTpl_setNumLabels(c, v) (c)->numlabels = val
#define ConstructTpl_getOpvec(c)        (&((c)->vec))
#define ConstructTpl_getResult(c)       (c)->result

ConstructTpl*   ConstructTpl_new();
void            ConstructTpl_delete(ConstructTpl *c);

bool            ConstructTpl_addOpList(ConstructTpl *t, struct dynarray *oplist);
bool            ConstructTpl_addOp(ConstructTpl *t, OpTpl *ot);
int             ConstructTpl_fillinBuild(ConstructTpl *c, struct dynarray *check, AddrSpace *space);

typedef struct PcodeBuilder     PcodeBuilder;

struct PcodeBuilder { // SLEIGH specific pcode generator
  uint4 labelbase;
  uint4 labelcount;
  void *ref;

  void(* dump)(void *ref, OpTpl *bld);
  void(* appendBuild)(void *ref, OpTpl *bld, int secnum);
  void(* delaySlot)(void *ref, OpTpl *op);
  void(* setLabel)(void *ref, OpTpl *op);
  void(* appendCrossBuild)(void *ref, OpTpl *op, int secnum);
};

PcodeBuilder*       PcodeBuilder_new(void *ref);
void                PcodeBuilder_delete(PcodeBuilder *p);
/* 使用这个函数前，必须得注册
dump
appendBuild
delaySlot
setLabel
appendCrossBuild
5个函数
*/
void                PcodeBuilder_build(PcodeBuilder *pb, ConstructTpl *construct, int secnum);

typedef struct HandleTpl        HandleTpl;

/* 函数的参数 */
struct HandleTpl {
    ConstTpl *space;
    ConstTpl *size;
    ConstTpl *ptrspace;
    ConstTpl *ptroffset;
    ConstTpl *ptrsize;
    ConstTpl *temp_space;
    ConstTpl *temp_offset;
};

HandleTpl*          HandleTpl_new();
HandleTpl*          HandleTpl_newV(VarnodeTpl *vn);
HandleTpl*          HandleTpl_new5(ConstTpl *spc, ConstTpl *sz, VarnodeTpl *vn, AddrSpace *sp, uintb offset);
void                HandleTpl_delete(HandleTpl *h);

#ifdef __cplusplus
}
#endif

#endif