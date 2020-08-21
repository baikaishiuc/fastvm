#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

	struct vertex3i
	{
		int x;
		int y;
		int z;
	};

	struct vertex2i
	{
		int x;
		int y;
	};

	struct vertex3f
	{
		float x;
		float y;
		float z;
	};

typedef uint8_t 	u1;
typedef uint16_t  	u2;
typedef uint32_t 	u4;
typedef uint64_t 	u8;
typedef int8_t 		s1;
typedef int16_t 	s2;
typedef int32_t 	s4;
typedef int64_t 	s8;
typedef int8_t 		i1;
typedef int16_t 	i2;
typedef int32_t 	i4;
typedef int64_t 	i8;


#ifdef __cplusplus
}
#endif
