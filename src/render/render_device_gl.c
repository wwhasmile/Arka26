#include "render_device.h"

#include <sys/sys_rgfw.h>

#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <ext/glad.h>
#endif // __EMSCRIPTEN__

typedef struct
{
    GLuint id;
    GLsizei width;
    GLsizei height;
    GLenum format;
    GLenum internalFormat;
    GLenum type;
} renderTextureGL_t;

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
static renderTexture_t* RenderDeviceGL_TextureCreate(u32 width, u32 height, renderTextureFormat_t format);
static void RenderDeviceGL_TextureUpload(renderTexture_t* texture, u8* data);
static void RenderDeviceGL_TextureRelease(renderTexture_t* texture);
static void RenderDeviceGL_Clear(renderClearDescriptor_t desc);
static void RenderDeviceGL_Swap(void);

INLINE static void RenderDeviceGL_TextureBind(GLuint id);

void RenderDevice_CreateGL(renderDevice_t *device)
{
    device->prepare = RenderDeviceGL_Prepare;
    device->initialize = RenderDeviceGL_Initialize;
    device->textureCreate = RenderDeviceGL_TextureCreate;
    device->textureUpload = RenderDeviceGL_TextureUpload;
    device->textureRelease = RenderDeviceGL_TextureRelease;
    device->clear = RenderDeviceGL_Clear;
    device->swap = RenderDeviceGL_Swap;
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

renderTexture_t* RenderDeviceGL_TextureCreate(u32 width, u32 height, renderTextureFormat_t format)
{
    renderTextureGL_t texture = {
        .width = width,
        .height = height,
    };

    glGenTextures(1, &texture.id);
    if (texture.id == 0) return NULL;

    switch (format)
    {
        case RENDER_TEXTURE_FORMAT_RGBA:
            texture.format = GL_RGBA;
            texture.internalFormat = GL_RGBA8;
            texture.type = GL_UNSIGNED_BYTE;
            break;
        case RENDER_TEXTURE_FORMAT_RGB:
            texture.format = GL_RGB;
            texture.internalFormat = GL_RGB8;
            texture.type = GL_UNSIGNED_BYTE;
            break;
        case RENDER_TEXTURE_FORMAT_RED:
            texture.format = GL_RED;
            texture.internalFormat = GL_R8;
            texture.type = GL_UNSIGNED_BYTE;
            break;
        case RENDER_TEXTURE_FORMAT_DEPTH_STENCIL:
            texture.format = GL_DEPTH_STENCIL;
            texture.internalFormat = GL_DEPTH24_STENCIL8;
            texture.type = GL_UNSIGNED_INT_24_8;
            break;
    }

    renderTextureGL_t* result = (renderTextureGL_t*)malloc(sizeof(renderTextureGL_t));
    *result = texture;
    return (renderTexture_t*)result;
}

void RenderDeviceGL_TextureUpload(renderTexture_t* texture, u8* data)
{
    renderTextureGL_t* tex = (renderTextureGL_t*)texture;

    RenderDeviceGL_TextureBind(tex->id);
    glTexImage2D(GL_TEXTURE_2D, 0, tex->internalFormat, tex->width, tex->height, 0, tex->format, tex->type, data);
}

void RenderDeviceGL_TextureRelease(renderTexture_t* texture)
{
    renderTextureGL_t* tex = (renderTextureGL_t*)texture;

    glDeleteTextures(1, &tex->id);
    free(tex);
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

void RenderDeviceGL_TextureBind(GLuint id)
{
    if (s_renderStateGL.texture == id) return;
    glBindTexture(GL_TEXTURE_2D, id);
}
