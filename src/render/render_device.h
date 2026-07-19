#ifndef __RENDER_DEVICE_H__
#define __RENDER_DEVICE_H__

#include <core/defines.h>

typedef void (*renderDevicePrepareProc)(void);
typedef bool (*renderDeviceInitializeProc)(void);
typedef void (*renderDeviceClearProc)(u32 color);
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
