#ifndef __EMSCRIPTEN__

#include <sys/sys.h>
#include <render/render_device.h>

int main(int argc, char** argv)
{
    renderDevice_t device;
    RenderDevice_CreateGL(&device);
    device.prepare();

    if (!Sys_Initialize("Arkanoid 26", "ARKA26", 1280, 1024, 0))
        return 1;

    if (!device.initialize())
        return 1;

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

        renderColor32_t background = { 0x6496EDFF };
        device.clear(background);
        device.swap();
    }
}

#endif // __EMSCRIPTEN__
