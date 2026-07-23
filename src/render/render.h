#ifndef __RENDER_H__
#define __RENDER_H__

#include <core/defines.h>

#define RENDER_MAX_TEXTURES 16

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

ENUM(renderTextureFormat_t, u8)
{
    RENDER_TEXTURE_FORMAT_RGBA,
    RENDER_TEXTURE_FORMAT_RGB,
    RENDER_TEXTURE_FORMAT_RED,
    RENDER_TEXTURE_FORMAT_DEPTH_STENCIL,
    RENDER_TEXTURE_FORMAT_ENUM_MAX,
};

ENUM(renderTextureFilter_t, u8)
{
    RENDER_TEXTURE_FILTER_NEAREST,
    RENDER_TEXTURE_FILTER_LINEAR,
    RENDER_TEXTURE_FILTER_ENUM_MAX,
};

ENUM(renderTextureWrap_t, u8)
{
    RENDER_TEXTURE_WRAP_CLAMP,
    RENDER_TEXTURE_WRAP_REPEAT,
    RENDER_TEXTURE_WRAP_MIRROR,
    RENDER_TEXTURE_WRAP_ENUM_MAX
};

typedef struct
{
    renderTextureFilter_t filter;
    renderTextureWrap_t horizontalWrap;
    renderTextureWrap_t verticalWrap;
    u8 _padding0;
} renderTextureSampler_t;

ENUM(renderVertexAttributeType_t, u8)
{
    RENDER_VERTEX_ATTRIBUTE_TYPE_NONE,
    RENDER_VERTEX_ATTRIBUTE_TYPE_UBYTE4,
    RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT,
    RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT2,
    RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT3,
    RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT4,
    RENDER_VERTEX_ATTRIBUTE_TYPE_ENUM_MAX,
};

typedef struct
{
    renderVertexAttributeType_t type : 7;
    bool normalized : 1;
} renderVertexAttribute_t;

ENUM(renderVertexDataUsage_t, u8)
{
    RENDER_VERTEX_DATA_USAGE_DYNAMIC,
    RENDER_VERTEX_DATA_USAGE_STATIC,
    RENDER_VERTEX_DATA_USAGE_ENUM_MAX,
};

ENUM(renderTests_t, u8)
{
    RENDER_TEST_BLEND = 1 << 0,
    RENDER_TEST_DEPTH = 1 << 1,
    RENDER_TEST_SCISSOR = 1 << 2,
    RENDER_TEST_VIEWPORT = 1 << 3,
};

ENUM(renderDepthCompare_t, u8)
{
    RENDER_DEPTH_COMPARE_L,
    RENDER_DEPTH_COMPARE_LE,
    RENDER_DEPTH_COMPARE_E,
    RENDER_DEPTH_COMPARE_GE,
    RENDER_DEPTH_COMPARE_G,
    RENDER_DEPTH_COMPARE_NEQ,
    RENDER_DEPTH_COMPARE_ENUM_MAX,
};

ENUM(renderBlendOp_t, u8)
{
    RENDER_BLEND_OP_ADD,
    RENDER_BLEND_OP_SUBTRACT,
    RENDER_BLEND_OP_REVERSE_SUBTRACT,
    RENDER_BLEND_OP_MIN,
    RENDER_BLEND_OP_MAX,
    RENDER_BLEND_OP_ENUM_MAX,
};

ENUM(renderBlendFunc_t, u8)
{
    RENDER_BLEND_FUNC_ZERO,
    RENDER_BLEND_FUNC_ONE,
    RENDER_BLEND_FUNC_SRC_COLOR,
    RENDER_BLEND_FUNC_INV_SRC_COLOR,
    RENDER_BLEND_FUNC_DST_COLOR,
    RENDER_BLEND_FUNC_INV_DST_COLOR,
    RENDER_BLEND_FUNC_SRC_ALPHA,
    RENDER_BLEND_FUNC_INV_SRC_ALPHA,
    RENDER_BLEND_FUNC_DST_ALPHA,
    RENDER_BLEND_FUNC_INV_DST_ALPHA,
    RENDER_BLEND_FUNC_ENUM_MAX,
};

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
    renderBlendOp_t blendOpColor;
    renderBlendOp_t blendOpAlpha;
    renderBlendFunc_t blendFuncSrcColor;
    renderBlendFunc_t blendFuncDstColor;
    renderBlendFunc_t blendFuncSrcAlpha;
    renderBlendFunc_t blendFuncDstAlpha;
    renderDepthCompare_t depthCompare;
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
