#include "render/render.h"
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
static void RenderDeviceGL_Clear(renderClearDescriptor_t desc);
static void RenderDeviceGL_Swap(void);

void RenderDevice_CreateGL(renderDevice_t *device)
{
    device->prepare = (renderDevicePrepareProc_t)RenderDeviceGL_Prepare;
    device->initialize = (renderDeviceInitializeProc_t)RenderDeviceGL_Initialize;
    device->clear = (renderDeviceClearProc_t)RenderDeviceGL_Clear;
    device->swap = (renderDeviceSwapProc_t)RenderDeviceGL_Swap;
}

void RenderDeviceGL_Prepare(void)
{
    sysStateRGFW_t* state = Sys_GetStateRGFW();
    state->rgfwInitFlags |= RGFW_initOpenGL;
    state->rgfwWindowFlags |= RGFW_windowOpenGL;
}

bool RenderDeviceGL_Initialize(void)
{
    sysStateRGFW_t* state = Sys_GetStateRGFW();
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

void RenderDeviceGL_Clear(renderClearDescriptor_t desc)
{
    if (desc.tests & RENDER_TESTS_SCISSOR)
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(desc.scissorsRect.x, desc.scissorsRect.y, desc.scissorsRect.width, desc.scissorsRect.height);
    }
    if (desc.tests & RENDER_TESTS_VIEWPORT)
        glViewport(desc.viewportRect.x, desc.viewportRect.y, desc.viewportRect.width, desc.viewportRect.height);
    renderColor_t converted = Render_ColorFrom32(desc.color);
    glClearColor(converted.r, converted.g, converted.b, converted.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderDeviceGL_Swap(void)
{
    sysStateRGFW_t* state = Sys_GetStateRGFW();
    RGFW_window_swapBuffers_OpenGL(state->window);
}
