#include "render_device.h"

#include <core/log.h>
#include <core/assert.h>
#include <core/memory.h>

#include <sys/sys_rgfw.h>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <ext/glad.h>
#endif // __EMSCRIPTEN__

#ifdef __EMSCRIPTEN
static const char* RENDER_DEVICE_GLSL_VERSION_STRING = "#version 300 es\n#\n";
static const char* RENDER_DEVICE_GLSL_FLOAT_PRECISION_STRING = "precision mediump float\n";
#else
static const char* RENDER_DEVICE_GLSL_VERSION_STRING = "#version 330 core\n";
static const char* RENDER_DEVICE_GLSL_FLOAT_PRECISION_STRING = "";
#endif

typedef struct
{
    GLuint id;
    GLuint vbo;
    GLuint ebo;

    u32 vertexAttributes;

    u32 vertexBufferSize;
    renderVertexDataUsage_t vertexBufferUsage;
    u32 elementBufferSize;
    renderVertexDataUsage_t elementBufferUsage;
} renderMeshGL_t;

typedef struct
{
    GLuint id;
    GLsizei width;
    GLsizei height;
    GLenum format;
    GLenum internalFormat;
    GLenum type;
} renderTextureGL_t;

typedef struct
{
    GLuint id;
    GLuint texture;
    renderTextureSampler_t sampler;
    GLint activeUniforms;
} renderMaterialGL_t;

typedef struct
{
    GLint location;
    GLint size;
    GLenum type;
} renderUniformGL_t;

static struct
{
    bool initialized;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint fbo;
    GLuint texture;
    GLuint program;
} s_renderStateGL;

static void RenderDeviceGL_Prepare(void);
static bool RenderDeviceGL_Initialize(void);
static renderTexture_t* RenderDeviceGL_TextureCreate(u32 width, u32 height, renderTextureFormat_t format);
static void RenderDeviceGL_TextureUpload(renderTexture_t* texture, void* data);
static void RenderDeviceGL_TextureRelease(renderTexture_t* texture);
static renderMesh_t* RenderDeviceGL_MeshCreate(void);
static void RenderDeviceGL_MeshSetVertexAttributes(renderMesh_t* mesh, renderVertexAttribute_t* attributes, u32 count);
static void RenderDeviceGL_MeshUploadVertices(renderMesh_t* mesh, void* data, u32 size, u32 dest, renderVertexDataUsage_t usage);
static void RenderDeviceGL_MeshUploadElements(renderMesh_t* mesh, void* data, u32 size, u32 dest, renderVertexDataUsage_t usage);
static void RenderDeviceGL_MeshRelease(renderMesh_t* mesh);
static renderMaterial_t* RenderDeviceGL_MaterialCreate(const char* vsh, const char* fsh);
static void RenderDeviceGL_Clear(renderClearDescriptor_t desc);
static void RenderDeviceGL_Swap(void);

INLINE static void RenderDeviceGL_TextureBind(GLuint id);
INLINE static void RenderDeviceGL_VaoBind(GLuint id);
INLINE static void RenderDeviceGL_VboBind(GLuint id);
INLINE static void RenderDeviceGL_EboBind(GLuint id);

void RenderDevice_CreateGL(renderDevice_t *device)
{
    device->prepare = RenderDeviceGL_Prepare;
    device->initialize = RenderDeviceGL_Initialize;
    device->textureCreate = RenderDeviceGL_TextureCreate;
    device->textureUpload = RenderDeviceGL_TextureUpload;
    device->textureRelease = RenderDeviceGL_TextureRelease;
    device->meshCreate = RenderDeviceGL_MeshCreate;
    device->meshSetVertexAttributes = RenderDeviceGL_MeshSetVertexAttributes;
    device->meshUploadVertices = RenderDeviceGL_MeshUploadVertices;
    device->meshUploadElements = RenderDeviceGL_MeshUploadElements;
    device->meshRelease = RenderDeviceGL_MeshRelease;
    device->materialCreate = RenderDeviceGL_MaterialCreate;
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
    ASSERT_MESSAGE(!s_renderStateGL.initialized, "Attempted to initialize OpenGL when it was already initialized");

    Sys_ActivateContextGL();

    #ifndef __EMSCRIPTEN__
    if (!gladLoadGLLoader((GLADloadproc)Sys_GetProcAddressGL))
    {
        LOG_FATAL("Failed to initialize OpenGL render device");
        return FALSE;
    }
    #endif // __EMSCRIPTEN__

    LOG_SUCCESS("Initialized OpenGL: %s, %s", glGetString(GL_VERSION), glGetString(GL_RENDERER));
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

    renderTextureGL_t* result = (renderTextureGL_t*)Memory_BumpAlloc(sizeof(renderTextureGL_t));
    *result = texture;
    return (renderTexture_t*)result;
}

void RenderDeviceGL_TextureUpload(renderTexture_t* texture, void* data)
{
    ASSERT_MESSAGE(texture != NULL, "Passed texture is null");
    ASSERT_MESSAGE(data != NULL, "Passed data is null");
    renderTextureGL_t* tex = (renderTextureGL_t*)texture;

    RenderDeviceGL_TextureBind(tex->id);
    glTexImage2D(GL_TEXTURE_2D, 0, tex->internalFormat, tex->width, tex->height, 0, tex->format, tex->type, data);
}

void RenderDeviceGL_TextureRelease(renderTexture_t* texture)
{
    ASSERT_MESSAGE(texture != NULL, "Passed texture is null");
    renderTextureGL_t* tex = (renderTextureGL_t*)texture;

    glDeleteTextures(1, &tex->id);
    Memory_BumpFree(tex);
}

renderMesh_t* RenderDeviceGL_MeshCreate(void)
{
    renderMeshGL_t result = { 0 };
    glGenVertexArrays(1, &result.id);
    if (result.id == 0) return NULL;

    renderMeshGL_t* mesh = (renderMeshGL_t*)Memory_BumpAlloc(sizeof(renderMeshGL_t));
    *mesh = result;
    return (renderMesh_t*)mesh;
}

void RenderDeviceGL_MeshSetVertexAttributes(renderMesh_t* mesh, renderVertexAttribute_t* attributes, u32 count)
{
    ASSERT_MESSAGE(mesh != NULL, "Passed mesh is null");
    ASSERT_MESSAGE(attributes != NULL, "Passed attribute data is null");
    renderMeshGL_t* m = (renderMeshGL_t*)mesh;
    RenderDeviceGL_VaoBind(m->id);

    if (m->vbo == 0)
    {
        glGenBuffers(1, &m->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
        s_renderStateGL.vbo = m->vbo;
    }
    else if (s_renderStateGL.vbo != m->vbo)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
        s_renderStateGL.vbo = m->vbo;
    }

    u32 stride = 0;
    for (u32 i = 0; i < count; ++i)
    {
        switch (attributes[i].type)
        {
            case RENDER_VERTEX_ATTRIBUTE_TYPE_UBYTE4:
                stride += sizeof(GLubyte) * 4;
                break;
            case RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT:
                stride += sizeof(GLfloat);
                break;
            case RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT2:
                stride += sizeof(GLfloat) * 2;
                break;
            case RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT3:
                stride += sizeof(GLfloat) * 3;
                break;
            case RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT4:
                stride += sizeof(GLfloat) * 4;
                break;
        }
    }

    u32 j = m->vertexAttributes > count ? m->vertexAttributes : count;
    GLsizeiptr ptr = 0;
    for (u32 i = 0; i < j; ++i)
    {
        if (i >= count)
        {
            glDisableVertexAttribArray(i);
            continue;
        }

        GLenum type = GL_FLOAT;
        GLsizei size = sizeof(GLfloat);
        GLsizei components = 1;

        switch (attributes[i].type)
        {
            case RENDER_VERTEX_ATTRIBUTE_TYPE_UBYTE4:
                type = GL_UNSIGNED_BYTE;
                size = sizeof(GLubyte);
                components = 4;
                break;
            case RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT:
                type = GL_FLOAT;
                size = sizeof(GLfloat);
                break;
            case RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT2:
                type = GL_FLOAT;
                size = sizeof(GLfloat);
                components = 2;
                break;
            case RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT3:
                type = GL_FLOAT;
                size = sizeof(GLfloat);
                components = 3;
                break;
            case RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT4:
                type = GL_FLOAT;
                size = sizeof(GLfloat);
                components = 4;
                break;
        }

        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, components, type, attributes[i].normalized, stride, (void*)ptr);

        ptr += components * size;
    }

    m->vertexAttributes = count;
}

void RenderDeviceGL_MeshUploadVertices(renderMesh_t* mesh, void* data, u32 size, u32 dest, renderVertexDataUsage_t usage)
{
    ASSERT_MESSAGE(mesh != NULL, "Passed mesh is null");
    ASSERT_MESSAGE(data != NULL, "Passed vertex data is null");
    renderMeshGL_t* m = (renderMeshGL_t*)mesh;
    RenderDeviceGL_VaoBind(m->id);

    if (m->vbo == 0)
    {
        glGenBuffers(1, &m->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
        s_renderStateGL.vbo = m->vbo;
    }
    else if (s_renderStateGL.vbo != m->vbo)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
        s_renderStateGL.vbo = m->vbo;
    }

    u32 total = dest + size;
    if (total > m->vertexBufferSize || usage != m->vertexBufferUsage)
    {
        GLenum glUsage = GL_DYNAMIC_DRAW;
        switch (usage)
        {
            case RENDER_VERTEX_DATA_USAGE_DYNAMIC:
                glUsage = GL_DYNAMIC_DRAW;
                break;
            case RENDER_VERTEX_DATA_USAGE_STATIC:
                glUsage = GL_STATIC_DRAW;
                break;
        }

        glBufferData(GL_ARRAY_BUFFER, total, NULL, glUsage);
        m->vertexBufferUsage = usage;
        m->vertexBufferSize = total;
    }

    glBufferSubData(GL_ARRAY_BUFFER, dest, size, data);
}

void RenderDeviceGL_MeshUploadElements(renderMesh_t* mesh, void* data, u32 size, u32 dest, renderVertexDataUsage_t usage)
{
    ASSERT_MESSAGE(mesh != NULL, "Passed mesh is null");
    ASSERT_MESSAGE(data != NULL, "Passed element data is null");
    renderMeshGL_t* m = (renderMeshGL_t*)mesh;
    RenderDeviceGL_VaoBind(m->id);

    if (m->ebo == 0)
    {
        glGenBuffers(1, &m->ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
        s_renderStateGL.ebo = m->ebo;
    }
    else if (s_renderStateGL.ebo != m->ebo)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
        s_renderStateGL.ebo = m->ebo;
    }

    u32 total = dest + size;
    if (total > m->elementBufferSize || usage != m->elementBufferUsage)
    {
        GLenum glUsage = GL_DYNAMIC_DRAW;
        switch (usage)
        {
            case RENDER_VERTEX_DATA_USAGE_DYNAMIC:
                glUsage = GL_DYNAMIC_DRAW;
                break;
            case RENDER_VERTEX_DATA_USAGE_STATIC:
                glUsage = GL_STATIC_DRAW;
                break;
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, total, NULL, glUsage);
        m->elementBufferUsage = usage;
        m->elementBufferSize = total;
    }

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, dest, size, data);
}

void RenderDeviceGL_MeshRelease(renderMesh_t* mesh)
{
    ASSERT_MESSAGE(mesh != NULL, "Passed mesh is null");
    renderMeshGL_t* m = (renderMeshGL_t*)mesh;

    if (m->vbo != 0)
        glDeleteBuffers(1, &m->vbo);
    if (m->ebo != 0)
        glDeleteBuffers(1, &m->ebo);
    if (m->id != 0)
        glDeleteVertexArrays(1, &m->id);

    Memory_Free(m);
}

renderMaterial_t* RenderDeviceGL_MaterialCreate(const char* vsh, const char* fsh)
{
    ASSERT_MESSAGE(vsh != NULL, "Vertex shader source is null");
    ASSERT_MESSAGE(fsh != NULL, "Fragment shader source is null");

    GLchar charBuffer[512] = { 0 };
    GLsizei charBufferStringLength;

    GLuint vshId = glCreateShader(GL_VERTEX_SHADER);
    {
        GLint success;
        const char* source[] = { RENDER_DEVICE_GLSL_VERSION_STRING, vsh };
        glShaderSource(vshId, 2, source, NULL);
        glCompileShader(vshId);
        glGetShaderiv(vshId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vshId, 512, &charBufferStringLength, charBuffer);
            glDeleteShader(vshId);
            vshId = 0;
            if (charBufferStringLength > 0)
                LOG_ERROR("Failed to compile vertex shader: %s", charBuffer);
            else
                LOG_ERROR("Failed to compile vertex shader");
        }
    }
    GLuint fshId = glCreateShader(GL_FRAGMENT_SHADER);
    {
        GLint success;
        const char* source[] = { RENDER_DEVICE_GLSL_VERSION_STRING, RENDER_DEVICE_GLSL_FLOAT_PRECISION_STRING, fsh };
        glShaderSource(fshId, 3, source, NULL);
        glCompileShader(fshId);
        glGetShaderiv(fshId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fshId, 512, &charBufferStringLength, charBuffer);
            glDeleteShader(fshId);
            fshId = 0;
            if (charBufferStringLength> 0)
                LOG_ERROR("Failed to compile fragment shader: %s", charBuffer);
            else
                LOG_ERROR("Failed to compile fragment shader");
        }
    }
    if (vshId == 0 || fshId == 0)
        return NULL;

    GLint success;
    GLuint id = glCreateProgram();
    glAttachShader(id, vshId);
    glAttachShader(id, fshId);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, 512, &charBufferStringLength, charBuffer);
        glDeleteProgram(id);
        id = 0;
        if (charBufferStringLength > 0)
            LOG_ERROR("Failed to link shader program: %s", charBuffer);
        else
            LOG_ERROR("Failed to link shader program");
    }
    glDeleteShader(vshId);
    glDeleteShader(fshId);
    if (id == 0)
        return NULL;

    renderMaterialGL_t result = {
        .id = id,
        .texture = 0,
        .sampler = { 0 },
    };
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &result.activeUniforms);

    renderMaterialGL_t* mat = (renderMaterialGL_t*)Memory_BumpAlloc(sizeof(renderMaterialGL_t) +
        result.activeUniforms * sizeof(renderUniformGL_t));
    *mat = result;

    for (GLint i = 0; i < mat->activeUniforms; ++i)
    {
        renderUniformGL_t uniformResult = { 0 };
        glGetActiveUniform(id, i, 512, &charBufferStringLength, &uniformResult.size, &uniformResult.type, charBuffer);
        if (charBuffer[charBufferStringLength - 1] == ']')
            charBuffer[charBufferStringLength - 3] = '\0';
        uniformResult.location = glGetUniformLocation(id, charBuffer);
        renderUniformGL_t* uniform = (renderUniformGL_t*)(mat + 1) + i;
        *uniform = uniformResult;
    }

    return (renderMaterial_t*)mat;
}

void RenderDeviceGL_Clear(renderClearDescriptor_t desc)
{
    if (desc.tests & RENDER_TEST_SCISSOR)
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(desc.scissorsRect.x, desc.scissorsRect.y, desc.scissorsRect.width, desc.scissorsRect.height);
    }
    if (desc.tests & RENDER_TEST_VIEWPORT)
        glViewport(desc.viewportRect.x, desc.viewportRect.y, desc.viewportRect.width, desc.viewportRect.height);
    renderColor_t converted = Render_ColorFrom32(desc.color);
    glClearColor(converted.r, converted.g, converted.b, converted.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderDeviceGL_Swap(void)
{
    Sys_SwapGL();
}

void RenderDeviceGL_TextureBind(GLuint id)
{
    if (s_renderStateGL.texture != id)
    {
        glBindTexture(GL_TEXTURE_2D, id);
        s_renderStateGL.texture = id;
    }
}

void RenderDeviceGL_VaoBind(GLuint id)
{
    if (s_renderStateGL.vao != id)
    {
        glBindVertexArray(id);
        s_renderStateGL.vao = id;
    }
}

void RenderDeviceGL_VboBind(GLuint id)
{
    if (s_renderStateGL.vbo != id)
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
        s_renderStateGL.vbo = id;
    }
}

void RenderDeviceGL_EboBind(GLuint id)
{
    if (s_renderStateGL.ebo != id)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        s_renderStateGL.ebo = id;
    }
}
