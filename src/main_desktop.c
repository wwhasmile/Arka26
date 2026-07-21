#ifndef __EMSCRIPTEN__

#include <sys/sys.h>
#include <core/memory.h>
#include <stdio.h>
#include <render/render_device.h>

int main(int argc, char** argv)
{
    Memory_Initialize();

    u32* a1 = (u32*)Memory_Malloc(sizeof(u32) * 16);
    u32* a2 = (u32*)Memory_Malloc(sizeof(u32) * 48);
    u32* a3 = (u32*)Memory_Malloc(sizeof(u32) * 24);
    Memory_Free(a2);
    a2 = (u32*)Memory_Malloc(sizeof(u32) * 32);
    u32* a4 = (u32*)Memory_Malloc(sizeof(u32) * 16);
    u32* a5 = (u32*)Memory_Malloc(sizeof(u32) * 24);
    Memory_Free(a4);
    Memory_Free(a5);
    Memory_Free(a1);
    Memory_Free(a3);
    Memory_Free(a2);

    /* renderDevice_t device;
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
        } */
}

#endif // __EMSCRIPTEN__
