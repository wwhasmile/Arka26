#ifdef __EMSCRIPTEN__

#include <sys/sys.h>

#include <emscripten/emscripten.h>

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
    }
}

int main(void)
{
    if (!Sys_Initialize("Arkanoid 26", "ARKA26", 1280, 1024, 0))
        return 1;

    emscripten_set_main_loop((em_callback_func)Main_Loop, 0, FALSE);

    return 0;
}

#endif // __EMSCRIPTEN__
