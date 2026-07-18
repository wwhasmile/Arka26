#define RGFW_IMPLEMENTATION
#include "sys_rgfw.h"

#include <stdlib.h>

static sysState_RGFW s_sysState;

bool Sys_Initialize(const char *title, const char *appName, i32 width, i32 height, sysFlags flags)
{
    if (RGFW_init(appName, 0) != 0)
        return FALSE;

    RGFW_windowFlags rgfwFlags = RGFW_windowCenter;
    rgfwFlags = RGFW_windowCenter;
    if (flags & SYS_FLAGS_FULLSCREEN)
        rgfwFlags |= RGFW_windowFullscreen;
    if (!(flags & SYS_FLAGS_RESIZABLE))
        rgfwFlags |= RGFW_windowNoResize;
    s_sysState.flags = flags;
    s_sysState.window = RGFW_createWindow(title, 0, 0, width, height, rgfwFlags);
    RGFW_window_showMouse(s_sysState.window, FALSE);
    return TRUE;
}

bool Sys_TryPollEvent(sysEvent* event)
{
    sysEvent resultEvent = { 0 };

    RGFW_event ev;
    if (!RGFW_window_checkEvent(s_sysState.window, &ev))
        return FALSE;

    if (ev.type == RGFW_keyPressed && ev.key.value == RGFW_keyEnter && (ev.key.mod & RGFW_modAlt))
    {
        bool isFullscreen = s_sysState.flags & SYS_FLAGS_FULLSCREEN;
        RGFW_window_setFullscreen(s_sysState.window, !isFullscreen);
        if (isFullscreen)
            s_sysState.flags &= ~SYS_FLAGS_FULLSCREEN;
        else
            s_sysState.flags |= SYS_FLAGS_FULLSCREEN;

        goto success;
    }

    switch (ev.type)
    {
        case RGFW_keyPressed:
            resultEvent.type = SYS_EVENT_KEY_PRESSED;
            resultEvent.key.code = ev.key.value;
            resultEvent.key.mod = ev.key.mod;
            break;
        case RGFW_keyReleased:
            resultEvent.type = SYS_EVENT_KEY_RELEASED;
            resultEvent.key.code = ev.key.value;
            resultEvent.key.mod = ev.key.mod;
            break;
        case RGFW_keyChar:
            resultEvent.type = SYS_EVENT_KEY_CHAR;
            resultEvent.keyChar.value = ev.keyChar.value;
            break;
        case RGFW_windowResized:
            resultEvent.type = SYS_EVENT_WINDOW_RESIZED;
            resultEvent.windowResized.width = ev.update.w;
            resultEvent.windowResized.height = ev.update.h;
            break;
        case RGFW_windowFocusIn:
            resultEvent.type = SYS_EVENT_WINDOW_FOCUSED;
            break;
        case RGFW_windowFocusOut:
            resultEvent.type = SYS_EVENT_WINDOW_UNFOCUSED;
            break;
        case RGFW_windowClose:
            resultEvent.type = SYS_EVENT_WINDOW_CLOSED;
            break;
        default:
            break;
    }

success:
    *event = resultEvent;
    return TRUE;
}

void Sys_Quit(void)
{
    exit(0);
}

sysState_RGFW* Sys_GetStateRGFW(void)
{
    return &s_sysState;
}
