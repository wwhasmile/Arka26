#ifndef __RENDER_DEVICE_H__
#define __RENDER_DEVICE_H__

#include <core/defines.h>
#include <core/color.h>

typedef void* mesh_t;
typedef void* texture_t;
typedef void* surface_t;
typedef void* shader_t;

typedef void (*renderDevicePrepareProc_t)(void);
typedef bool (*renderDeviceInitializeProc_t)(void);
typedef void (*renderDeviceClearProc_t)(color32_t color);
typedef void (*renderDeviceSwapProc_t)(void);

typedef struct
{
    renderDevicePrepareProc_t prepare;
    renderDeviceInitializeProc_t initialize;

    renderDeviceClearProc_t clear;

    renderDeviceSwapProc_t swap;
} renderDevice_t;

void RenderDevice_CreateGL(renderDevice_t* device);

#endif // __RENDER_DEVICE_H__
