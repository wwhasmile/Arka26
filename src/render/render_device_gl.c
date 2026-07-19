#include "core/color.h"
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
    GLuint vao;

    GLuint vbo;
    GLuint ebo;
    GLuint fbo;
    GLuint texture;
    GLuint shader;
} s_renderStateGL;

static void RenderDeviceGL_Prepare(void);
static bool RenderDeviceGL_Initialize(void);
static void RenderDeviceGL_Clear(color32 color);
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
    sysState_RGFW* state = Sys_GetStateRGFW();
    RGFW_window_makeCurrentContext_OpenGL(state->window);

    #ifndef __EMSCRIPTEN__
    if (!gladLoadGLLoader((GLADloadproc)RGFW_getProcAddress_OpenGL))
        return FALSE;
    #endif // __EMSCRIPTEN__

    glGenVertexArrays(1, &s_renderStateGL.vao);
    if (s_renderStateGL.vao == 0)
        return FALSE;
    glBindVertexArray(s_renderStateGL.vao);
    return TRUE;
}

void RenderDeviceGL_Clear(color32 color)
{
    color128 color128 = Color_32to128(&color);
    glClearColor(color128.r, color128.g, color128.b, color128.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderDeviceGL_Swap(void)
{
    sysState_RGFW* state = Sys_GetStateRGFW();
    RGFW_window_swapBuffers_OpenGL(state->window);
}
