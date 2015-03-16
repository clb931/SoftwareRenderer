#ifndef ATLAS_STDAFX_H

#include <stdint.h>
#include <string>
#include <stdio.h>

using std::string;

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;
typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;
typedef int32_t		bool32;
typedef float		real32;
typedef double		real64; 

#define GLOBAL	static
#define INTERN	static
#define PERSIST	static

#define PI 3.14159265f

#define ATLAS_STDAFX_H
#endif