
#ifndef __PARTMAP_H__
#define __PARTMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mcore/mcore.h"

typedef int(*partmap_cmp)(void *key1, void *key2);

typedef void*       linetype;
typedef void*       valuetype;

typedef struct partmap{
    valuetype       defaultvalue;
    partmap_cmp     cmp;
} partmap;

partmap*    partmap_new(partmap_cmp cmp);
void        partmap_delete(partmap *);

valuetype   partmap_getValue(partmap *, linetype key);
valuetype   partmap_bounds(partmap *, linetype pnt, linetype before, linetype after, linetype invalid);
valuetype   partmap_split(partmap *, linetype pnt);
#define partmap_defaultValue(p)         p->defaultvalue

#ifdef __cplusplus
}
#endif

#endif