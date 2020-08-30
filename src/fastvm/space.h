
#ifndef __SPACE_H__
#define __SPACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mcore/mcore.h"
#include "types.h"

/* copy from Ghidra space.hh */

    typedef enum spacetype spacetype;

enum spacetype {
    IPTR_CONSTANT = 0,
    IPTR_PROCESSOR = 1,
    IPTR_SPACEBASE = 2,
    IPTR_INTERNAL = 3,
    IPTR_FSPEC = 4,
    IPTR_IOP = 5,
    IPTR_JOIN = 6,
};

typedef struct AddrSpace AddrSpace, ConstantSpace, OtherSpace, UniqueSpace, JoinSpace, OverlaySpace;

#define big_endian              0x0001
#define heritaged               0x0002
#define does_deadcode           0x0004
#define programspecific         0x008
#define reverse_justification   0x0010
#define overlay                 0x0020
#define overlaybase             0x0040
#define truncated               0x0080
#define hasphysical             0x0100
#define is_otherspace           0x0200
#define has_nearpointers        0x0400

#define constant_space_index    0
#define other_space_index       1


struct AddrSpace {
    u4 flags;
    /* privated */
    spacetype type;
    void *manage;
    void *trans;
    int refcount;
    uintb highest;
    unsigned pointerLowerBound;
    int shortcut;

    union {
        AddrSpace*  baseSpace;
    };

    /* protected */
    int addrsize;
    int wordsize;
    int minimumPointerSize;
    int index;
    int delay;   // delay in heritage for the space
    int deadcodedelay;      // delay before deadcode removal is allowed on this space
    char name[1];
};

#define AddrSpace_getTrans(a)               (a)->trans
#define AddrSpace_getType(a)                (a)->type
#define AddrSpace_getDelay(a)               (a)->delay
#define AddrSpace_getDeadCodeDelay(a)       (a)->deadcodedelay
#define AddrSpace_getIndex(a)               (a)->index
#define AddrSpace_getWordSize(a)            (a)->wordsize
#define AddrSpace_getAddrSize(a)            (a)->addrsize
#define AddrSpace_getHighest(a)             (a)->highest
#define AddrSpace_getPointerLowerBound(a)   (a)->pointerLowerBound
#define AddrSpace_getMinimumPtrSize(a)      (a)->minimumPointerSize
#define AddrSpace_getShortcut(a)            (a)->shortcut
#define AddrSpace_isHeritaged(a)            ((a)->flags & heritaged)
#define AddrSpace_doesDeadCode(a)           ((a)->flags & does_deadcode)
#define AddrSpace_hasPhysical(a)            ((a)->flags & hashpysical)
#define AddrSpace_isBigEndian(a)            ((a)->flags & big_endian)
#define AddrSpace_isReverseJustified(a)     ((a)->flags & reverse_justification)
#define AddrSpace_isOverlay(a)              ((a)->flags & overlay)

uintb  AddrSpace_wrapOffset(AddrSpace *s, uintb off);

AddrSpace*          AddrSpace_new2(void *m, spacetype tp);
AddrSpace*          AddrSpace_new8(void *m, spacetype tp, const char *name, u4 size, u4 ws, int ind, u4 fl, int dl);
void                AddrSpace_delete(AddrSpace *a);
ConstantSpace*      ConstantSpace_new(void *m, const char *name, int ind);
OtherSpace*         OtherSpace_new(void *m, const char *name, int ind);
UniqueSpace*        UniqueSpace_new(void *m, const char *name, int ind, u4 fl);

typedef struct VarnodeData {
    AddrSpace *space;
    u8 offset;
    uint32_t size;
} VarnodeData;

bool VarnodeData_less(const VarnodeData *op1, const VarnodeData *op2);

#ifdef __cplusplus
}
#endif

#endif