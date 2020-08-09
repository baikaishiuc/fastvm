
#ifndef __PARTMAP_H__
#define __PARTMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mcore/mcore.h"

typedef int(*partmap_cmp)(void *key1, void *key2);

typedef void*       valuetype;
typedef void*       linetype;

typedef struct partmap{
    valuetype       defaultvalue;
    partmap_cmp     cmp;
    struct rb_root  tree;
    int count;
} partmap;


partmap*    partmap_new(partmap_cmp cmp, valuetype defaultvalue);
void        partmap_delete(partmap *);

valuetype   partmap_getValue(partmap *, linetype key);
valuetype   partmap_bounds(partmap *p, linetype pnt, linetype *before, linetype *after, int *valid);
valuetype   partmap_split(partmap *, linetype pnt);
int         partmap_insert(partmap *p, linetype pnt, valuetype v);
#define partmap_defaultValue(p)         p->defaultvalue

#ifdef __cplusplus
}
#endif

#endif