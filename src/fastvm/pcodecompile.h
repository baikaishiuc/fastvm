
#ifndef __pcodecompile_h__
#define __pcodecompile_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "slghsymbol.h"

typedef struct Location Location;
typedef struct StarQuality StarQuality;

struct Location {
    char *filename;
    int fileno;
};

Location*   Location_new();
Location*   Location_new2(const char *name, int line);
void        Location_delete(Location *l);

typedef struct StarQuality {
    int reserved;
} StarQuality;

#ifdef __cplusplus
}
#endif

#endif