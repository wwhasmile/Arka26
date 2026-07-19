#ifndef __RENDER_H__
#define __RENDER_H__

#include <core/defines.h>

typedef void renderTexture_t;
typedef void renderMesh_t;
typedef void renderMaterial_t;
typedef void renderSurface_t;

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

typedef enum
{
    RENDER_TEXTURE_FORMAT_RGBA,
    RENDER_TEXTURE_FORMAT_RGB,
    RENDER_TEXTURE_FORMAT_RED,
    RENDER_TEXTURE_FORMAT_DEPTH_STENCIL,
} renderTextureFormat_t;

typedef enum
{
    RENDER_TEXTURE_FILTER_NEAREST,
    RENDER_TEXTURE_FILTER_LINEAR,
} renderTextureFilter_t;

typedef enum
{
    RENDER_TEXTURE_WRAP_CLAMP,
    RENDER_TEXTURE_WRAP_REPEAT,
    RENDER_TEXTURE_WRAP_MIRROR
} renderTextureWrap_t;

typedef struct
{
    u8 filter;
    u8 horizontalWrap;
    u8 verticalWrap;
    u8 _padding0;
} renderTextureSampler_t;

typedef enum
{
    RENDER_TESTS_BLEND = 1 << 0,
    RENDER_TESTS_SCISSOR = 1 << 1,
    RENDER_TESTS_VIEWPORT = 1 << 2,
} renderTests_t;

typedef struct
{
    u32 x;
    u32 y;
    u32 width;
    u32 height;
} renderRect_t;

typedef struct
{
    renderSurface_t* surface;
    renderMesh_t* mesh;
    renderMaterial_t* material;
    renderTests_t tests;
    renderRect_t scissorsRect;
    renderRect_t viewportRect;
    u32 start;
    u32 count;
} renderDrawDescriptor_t;

typedef struct
{
    renderSurface_t* surface;
    renderColor32_t color;
    renderTests_t tests;
    renderRect_t scissorsRect;
    renderRect_t viewportRect;
} renderClearDescriptor_t;

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
