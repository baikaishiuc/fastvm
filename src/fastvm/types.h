
#ifndef __types_h__
#define __types_h__

#if defined(_WINDOWS)

#if defined(_WIN64)

#define HOST_ENDIAN 0
typedef unsigned int uintm;
typedef int intm;
typedef unsigned long long uint8;
typedef long long int8;
typedef unsigned int uint4;
typedef int int4;
typedef unsigned short uint2;
typedef short int2;
typedef unsigned char uint1;
typedef char int1;
typedef uint8 uintp;

#else // _WIN32 assumed

#define HOST_ENDIAN 0
typedef unsigned long uintm;
typedef long intm;
typedef unsigned long long uint8;
typedef long long int8;
typedef unsigned int uint4;
typedef int int4;
typedef unsigned short uint2;
typedef short int2;
typedef unsigned char uint1;
typedef char int1;
typedef uint4 uintp;
#endif // _WIN32

#endif // _WINDOWS

#define UINTB8

typedef int8 intb;		/* This is a signed big integer */
//#include "integer.hh"
#ifdef UINTB8
typedef uint8 uintb;		/* This is an unsigned big integer */
#else
typedef uint4 uintb;
#endif

#define HOST_ENDIAN     0

#endif