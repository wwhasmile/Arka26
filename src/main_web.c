#ifdef __EMSCRIPTEN__

#include <sys/sys.h>
#include <render/render_device.h>

#include <emscripten/emscripten.h>

static renderDevice device;

static void Main_Loop(void)
{
    sysEvent ev;
    while (Sys_TryPollEvent(&ev))
    {
        if (ev.type == SYS_EVENT_WINDOW_CLOSED)
        {
            Sys_Quit();
            break;
        }

        device.clear((color32) { 0x6496EDFF });
        device.swap();
    }
}

int main(void)
{
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
