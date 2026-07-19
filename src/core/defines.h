#ifndef __DEFINES_H__
#define __DEFINES_H__

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;

#ifdef __EMSCRIPTEN__
#include <stdbool.h>
#undef bool
#endif
typedef u8 bool;
#define TRUE 1U
#define FALSE 0U

#define NULL ((void*)0)

#endif // __DEFINES_H__
