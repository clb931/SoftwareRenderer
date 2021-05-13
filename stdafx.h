// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#undef CreateWindow

// C RunTime Header Files
#include <memory.h>
#include <stdint.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <intrin.h>
#include <algorithm>

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

#define INTERN	static
#define PERSIST	static

#define PI 3.14159265f
#define ToRadians(a) ((a) * (PI / 180.0f))
//#define min(a, b) (((a) < (b)) ? (a) : (b))
//#define max(a, b) (((a) > (b)) ? (a) : (b))
//#define clamp(a, b, c) (clip((a) / (c) + (b), b, c))
#define clip(a, b, c) (min(max((a), (b)), (c)))
#define lerp(a, b, c) ((a) * (1 - (c)) + (b) * (c))


// TODO: reference additional headers your program requires here
