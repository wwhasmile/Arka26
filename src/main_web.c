#ifdef __EMSCRIPTEN__

#include <core/memory.h>
#include <sys/sys.h>
#include <render/render_device.h>

#include <emscripten/emscripten.h>

static renderDevice_t device;

static renderClearDescriptor_t clear = {
        .surface = NULL,
        .color = { 0x6496EDFF },
        .tests = RENDER_TEST_SCISSOR | RENDER_TEST_VIEWPORT,
        .scissorsRect = { 0, 0, 640, 480 },
        .viewportRect = { 0, 0, 640, 480 },
    };

static void Main_Loop(void)
{
    sysEvent_t ev;
    while (Sys_TryPollEvent(&ev))
    {
        if (ev.type == SYS_EVENT_WINDOW_CLOSED)
        {
            Sys_Quit();
            break;
        }

        device.clear(clear);
        device.swap();
    }
}

int main(void)
{
    Memory_Initialize();

    RenderDevice_CreateGL(&device);
    device.prepare();
    if (!Sys_Initialize("Arkanoid 26", "ARKA26", 1280, 1024, 0))
        return 1;

    if (!device.initialize())
            return 1;

    emscripten_set_main_loop((em_callback_func)Main_Loop, 0, FALSE);

    return 0;
}

#endif // __EMSCRIPTEN__
