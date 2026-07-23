#ifndef __EMSCRIPTEN__

#include <sys/sys.h>
#include <core/log.h>
#include <core/memory.h>
#include <core/assert.h>
#include <render/render_device.h>

static const char* TEST_VSH = ""
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "layout (location = 2) in vec4 aColor;\n"
    "out vec2 oTexCoord;\n"
    "out vec4 oColor;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(aPos, 1.0);\n"
    "oTexCoord = aTexCoord;\n"
    "oColor = aColor;\n"
    "}\n";

static const char* TEST_FSH = ""
    "out vec4 FragColor;\n"
    "in vec2 oTexCoord;\n"
    "in vec4 oColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = oColor;\n"
    "}\n";

typedef struct
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    } position;
    struct
    {
        f32 u;
        f32 v;
    } texCoord;
    renderColor32_t color;
} testVertex_t;

static testVertex_t TEST_VERTICES[] = {
    { { -0.5f, 0.5f, 0.0f, }, { 0.0f, 0.0f, }, { 0xFFFFFFFF }, },
    { { 0.5f, 0.5f, 0.0f, }, { 1.0f, 0.0f, }, { 0xFF0000FF }, },
    { { 0.5f, -0.5f, 0.0f, }, { 1.0f, 1.0f, }, { 0xFF00FF00 }, },
    { { -0.5f, -0.5f, 0.0f, }, { 0.0f, 1.0f, }, { 0xFFFF0000 }, },
};

static u32 TEST_ELEMENTS[] = {
    0, 1, 2, 2, 3, 0
};

int main(int argc, char** argv)
{
    Memory_Initialize();

    renderDevice_t device;
    RenderDevice_CreateGL(&device);
    device.prepare();

    if (!Sys_Initialize("Arkanoid 26", "ARKA26", 1280, 1024, 0))
        return 1;

    if (!device.initialize())
        return 1;

    renderMaterial_t* material = device.materialCreate(TEST_VSH, TEST_FSH);
    renderMesh_t* mesh = device.meshCreate();
    renderVertexAttribute_t attributes[] = {
        { RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT3, FALSE },
        { RENDER_VERTEX_ATTRIBUTE_TYPE_FLOAT2, FALSE },
        { RENDER_VERTEX_ATTRIBUTE_TYPE_UBYTE4, TRUE },
    };
    device.meshSetVertexAttributes(mesh, attributes, sizeof(attributes) / sizeof(renderVertexAttribute_t));
    device.meshUploadVertices(mesh, TEST_VERTICES, sizeof(TEST_VERTICES), 0, RENDER_VERTEX_DATA_USAGE_STATIC);
    device.meshUploadElements(mesh, TEST_ELEMENTS, sizeof(TEST_ELEMENTS), 0, RENDER_VERTEX_DATA_USAGE_STATIC);

    renderDrawDescriptor_t draw = {
        .surface = NULL,
        .mesh = mesh,
        .material = material,
        // .tests = RENDER_TEST_SCISSOR | RENDER_TEST_VIEWPORT,
        0,
        .scissorsRect = { 0, 0, 640, 480 },
        .viewportRect = { 0, 0, 640, 480 },
        .count = 6,
    };

    renderClearDescriptor_t clear = {
        .surface = NULL,
        .color = { 0x6496EDFF },
        .tests = RENDER_TEST_SCISSOR | RENDER_TEST_VIEWPORT,
        .scissorsRect = { 0, 0, 640, 480 },
        .viewportRect = { 0, 0, 640, 480 },
    };

    while (1)
    {
        sysEvent_t ev;
        while (Sys_TryPollEvent(&ev))
        {
            if (ev.type == SYS_EVENT_WINDOW_CLOSED)
            {
                Sys_Quit();
                break;
            }
        }

        device.clear(&clear);
        device.draw(&draw);
        device.swap();
    }
}

#endif // __EMSCRIPTEN__
