#ifndef __RENDER_DEVICE_H__
#define __RENDER_DEVICE_H__

#include "render.h"

typedef struct
{
    void (*prepare)(void);
    bool (*initialize)(void);

    renderMesh_t* (*meshCreate)(void);
    void (*meshUploadVertices)(renderMesh_t*, void*, u32, u32, renderVertexDataUsage_t);
    void (*meshUploadElements)(renderMesh_t*, void*, u32, u32, renderVertexDataUsage_t);
    void (*meshRelease)(renderMesh_t*);

    renderTexture_t* (*textureCreate)(u32, u32, renderTextureFormat_t);
    void (*textureUpload)(renderTexture_t*, void*);
    void (*textureRelease)(renderTexture_t*);

    void (*clear)(renderClearDescriptor_t);

    void (*swap)(void);
} renderDevice_t;

void RenderDevice_CreateGL(renderDevice_t* device);

#endif // __RENDER_DEVICE_H__
