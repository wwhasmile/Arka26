#include <sys/sys.h>

int main(int argc, char** argv)
{
    if (!Sys_Initialize("Arkanoid 26", "ARKA26", 1280, 1024, 0))
        return 1;

    bool exit = FALSE;
    while (!exit)
    {
        sysEvent ev;
        while (Sys_TryPollEvent(&ev))
        {
            if (ev.type == SYS_EVENT_WINDOW_CLOSED)
            {
                exit = TRUE;
                break;
            }
        }
    }
}
