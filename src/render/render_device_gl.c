#include "render_device.h"

#include <sys/sys_rgfw.h>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <ext/glad.h>
#endif // __EMSCRIPTEN__

static struct
{
    RGFW_glContext* context;
} s_renderStateGL;

static void RenderDeviceGL_Prepare(void);
static bool RenderDeviceGL_Initialize(void);
static void RenderDeviceGL_Clear(u32 color);
static void RenderDeviceGL_Swap(void);

void RenderDevice_CreateGL(renderDevice *device)
{
    device->prepare = (renderDevicePrepareProc)RenderDeviceGL_Prepare;
    device->initialize = (renderDeviceInitializeProc)RenderDeviceGL_Initialize;
    device->clear = (renderDeviceClearProc)RenderDeviceGL_Clear;
    device->swap = (renderDeviceSwapProc)RenderDeviceGL_Swap;
}

void RenderDeviceGL_Prepare(void)
{
    sysState_RGFW* state = Sys_GetStateRGFW();
    state->rgfwInitFlags |= RGFW_initOpenGL;
    state->rgfwWindowFlags |= RGFW_windowOpenGL;
}

bool RenderDeviceGL_Initialize(void)
{
    #ifndef __EMSCRIPTEN__
    sysState_RGFW* state = Sys_GetStateRGFW();
    RGFW_window_makeCurrentContext_OpenGL(state->window);

    if (!gladLoadGLLoader((GLADloadproc)RGFW_getProcAddress_OpenGL))
        return FALSE;
    #endif // __EMSCRIPTEN__
    return TRUE;
}

void RenderDeviceGL_Clear(u32 color)
{
    f32 a = (color & 0xFF) / 255.0f;
    f32 b = ((color >> 8) & 0xFF) / 255.0f;
    f32 g = ((color >> 16) & 0xFF) / 255.0f;
    f32 r = ((color >> 24) & 0xFF) / 255.0f;
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderDeviceGL_Swap(void)
{
    sysState_RGFW* state = Sys_GetStateRGFW();
    RGFW_window_swapBuffers_OpenGL(state->window);
}
