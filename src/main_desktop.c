#ifndef __EMSCRIPTEN__

#include <sys/sys.h>
#include <core/log.h>
#include <core/memory.h>
#include <core/assert.h>
#include <render/render_device.h>

int main(int argc, char** argv)
{
    LOG_INFO("That's info");
    LOG_DEBUG("That's debug");
    LOG_SUCCESS("That's success");
    LOG_WARNING("That's a warning");
    LOG_ERROR("That's an error");
    LOG_FATAL("That's a fatal error");
    Memory_Initialize();

    renderDevice_t device;
    RenderDevice_CreateGL(&device);
    device.prepare();

    if (!Sys_Initialize("Arkanoid 26", "ARKA26", 1280, 1024, 0))
        return 1;

    if (!device.initialize())
        return 1;

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
