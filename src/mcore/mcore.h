
#ifndef __mcore_h__
#define __mcore_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_MSC_VER)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "mcore/process.h"
#endif

#include "mcore/basic_types.h"
#include "mcore/merror.h"
#include "mcore/macro_list.h"
#include "mcore/mbytes.h"
#include "mcore/mtime_ex.h"
#include "mcore/print_util.h"
#include "mcore/mlock.h"
#include "mcore/netx.h"
#include "mcore/string_ex.h"
#include "mcore/pgm.h"
#include "mcore/rbtree.h"
#include "mcore/dynarray.h"
//#include "mcore/bitset.h"
#include "mcore/queue.h"
#include "mcore/graph.h"
#include "mcore/image.h"
#include "mcore/minput.h"
#include "mcore/file.h"
#include "mcore/mheap.h"
#include "mcore/mthread.h"
#include "mcore/mrand.h"
#include "mcore/mdir.h"
#include "mcore/mutil.h"
#include "mcore/mcrc.h"

#pragma comment(lib, "mcore.lib")

#ifdef __cplusplus
}
#endif

#endif
