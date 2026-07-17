#include "sys_rgfw.h"
#include "sys/sys.h"

#include <string.h>

static sysState_RGFW s_sysState;

bool Sys_Initialize(const char *title, const char *appName, i32 width, i32 height, sysFlags flags)
{
    if (RGFW_init(appName, 0) != 0)
        return FALSE;

    s_sysState.rgfwFlags = RGFW_windowCenter;
    if (flags & SYS_FLAGS_FULLSCREEN)
        s_sysState.rgfwFlags |= RGFW_windowFullscreen;
    if (!(flags & SYS_FLAGS_RESIZABLE))
        s_sysState.rgfwFlags |= RGFW_windowNoResize;
    s_sysState.sysFlags = flags;
    s_sysState.window = RGFW_createWindow(title, 0, 0, width, height, s_sysState.rgfwFlags);
    return TRUE;
}

sysState_RGFW* Sys_GetStateRGFW(void)
{
    return &s_sysState;
}
