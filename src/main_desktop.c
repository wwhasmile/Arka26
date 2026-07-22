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
    "uniform vec4 colors[5];\n"
    "void main()\n"
    "{\n"
    "FragColor = colors[0];\n"
    "}\n";

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

        device.clear(clear);
        device.swap();
    }
}

#endif // __EMSCRIPTEN__
