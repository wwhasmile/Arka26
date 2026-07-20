#ifndef __RENDER_DEVICE_H__
#define __RENDER_DEVICE_H__

#include "render.h"

typedef struct
{
    void (*prepare)(void);
    bool (*initialize)(void);

    renderTexture_t* (*textureCreate)(u32, u32, renderTextureFormat_t);
    void (*textureUpload)(renderTexture_t*, u8*);
    void (*textureRelease)(renderTexture_t*);

    void (*clear)(renderClearDescriptor_t);

    void (*swap)(void);
} renderDevice_t;

void RenderDevice_CreateGL(renderDevice_t* device);

#endif // __RENDER_DEVICE_H__
