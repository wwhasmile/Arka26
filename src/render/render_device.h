#ifndef __RENDER_DEVICE_H__
#define __RENDER_DEVICE_H__

#include <core/defines.h>
#include <core/color.h>

typedef void* Mesh;
typedef void* Texture;
typedef void* Surface;
typedef void* Shader;

typedef void (*renderDevicePrepareProc)(void);
typedef bool (*renderDeviceInitializeProc)(void);
typedef void (*renderDeviceClearProc)(color32 color);
typedef void (*renderDeviceSwapProc)(void);

typedef struct
{
    renderDevicePrepareProc prepare;
    renderDeviceInitializeProc initialize;

    renderDeviceClearProc clear;

    renderDeviceSwapProc swap;
} renderDevice;

void RenderDevice_CreateGL(renderDevice* device);

#endif // __RENDER_DEVICE_H__
