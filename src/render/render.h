#ifndef __RENDER_H__
#define __RENDER_H__

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
} renderColor32_t;

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
} renderColor_t;

FORCE_INLINE renderColor_t Render_ColorFrom32(renderColor32_t source)
{
    f32 div = 1.0f / 255.0f;
    return (renderColor_t) {
        .r = source.r * div,
        .g = source.g * div,
        .b = source.b * div,
        .a = source.a * div,
    };
}

FORCE_INLINE renderColor32_t Render_ColorTo32(const renderColor_t* source)
{
    return (renderColor32_t) {
        .r = (u8)(source->r * 255),
        .g = (u8)(source->g * 255),
        .b = (u8)(source->b * 255),
        .a = (u8)(source->a * 255),
    };
}

#endif // __RENDER_H__
