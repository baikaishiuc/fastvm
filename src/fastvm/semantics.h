
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
#define MACROBUILD  CPUI_CAST
#define LABELBUILD  CPUI_PTRADD

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

typedef struct ConstTpl { 
  const_type type;
  union {
    //    uintb real;			// an actual constant
    AddrSpace *spaceid;	// Id (pointer) for registered space
    int4 handle_index;		// Place holder for run-time determined value
  } value;
  uintb value_real;
  v_field select;		// Which part of handle to use as constant
} ConstTpl;

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

typedef struct VarnodeTpl {
  ConstTpl space,offset,size;
  bool unnamed_flag;
} VarnodeTpl;

ConstTpl *VarnodeTpl_getOffset(VarnodeTpl *v);

typedef struct HandleTpl {
  ConstTpl space;
  ConstTpl size;
  ConstTpl ptrspace;
  ConstTpl ptroffset;
  ConstTpl ptrsize;
  ConstTpl temp_space;
  ConstTpl temp_offset;
} HandleTpl;

typedef struct OpTpl {
  VarnodeTpl *output;
  OpCode opc;
  struct dynarray input;    // VarnodeTpl *
} OpTpl;

typedef struct ConstructTpl {
  uint4 delayslot;
  uint4 numlabels;		// Number of label templates
  struct dynarray *vec;
  HandleTpl *result;
} ConstructTpl;

#define ConstructTpl_setOpvec(c, v)     (c)->vec = v
#define ConstructTpl_setNumLabels(c, v) (c)->numlabels = val
#define ConstructTpl_getOpvec(c)        ((c)->vec)
#define ConstructTpl_getResult(c)       (c)->result

ConstructTpl*   ConstructTpl_new();
void            ConstructTpl_delete(ConstructTpl *c);

bool            ConstructTpl_addOpList(ConstructTpl *c, struct dynarray *oplist);

typedef struct PcodeBuilder { // SLEIGH specific pcode generator
  uint4 labelbase;
  uint4 labelcount;
} PcodeBuilder;

#ifdef __cplusplus
}
#endif

#endif