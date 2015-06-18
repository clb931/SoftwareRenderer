#ifndef ATLAS_STDAFX_H

#include <stdint.h>
#include <string>
#include <stdio.h>
#include <vector>

using std::string;
using std::wstring;

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
#define ToRadians(a) ((a) * (PI / 180.0f))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
//#define clamp(a, b, c) (clip((a) / (c) + (b), b, c))
#define clip(a, b, c) (min(max((a), (b)), (c)))
#define lerp(a, b, c) ((a) * (1 - (c)) + (b) * (c))

#include "ATLAS_Vector.h"
#include "ATLAS_Matrix.h"
#include "ATLAS_Color.h"

#define ATLAS_STDAFX_H
#endif