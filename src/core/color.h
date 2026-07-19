#ifndef __COLOR_H__
#define __COLOR_H__

#include <core/defines.h>

typedef union
{
    u32 bgra;
    u8 components[4];
    struct
    {
        u8 a;
        u8 b;
        u8 g;
        u8 r;
    };
} color32;

typedef struct
{
    f32 components[4];
    struct
    {
        f32 a;
        f32 b;
        f32 g;
        f32 r;
    };
} color128;

FORCE_INLINE color128 Color_32to128(const color32* source)
{
    f32 div = 1.0f / 255.0f;
    return (color128) {
        .r = source->r * div,
        .g = source->g * div,
        .b = source->b * div,
        .a = source->a * div,
    };
}

FORCE_INLINE color32 Color_128to32(const color128* source)
{
    return (color32) {
        .r = (u8)(source->r * 255),
        .g = (u8)(source->g * 255),
        .b = (u8)(source->b * 255),
        .a = (u8)(source->a * 255),
    };
}

#endif // __COLOR_H__
