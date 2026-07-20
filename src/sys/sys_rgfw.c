#include "sys/sys.h"
#define RGFW_IMPLEMENTATION
#include "sys_rgfw.h"

#include <stdlib.h>

static sysStateRGFW_t s_sysState;

bool Sys_Initialize(const char *title, const char *appName, i32 width, i32 height, sysFlags_t flags)
{
    RGFW_globalHints_OpenGL_SRC.major = 3;
    RGFW_globalHints_OpenGL->minor = 3;

    if (RGFW_init(appName, s_sysState.rgfwInitFlags) != 0)
        return FALSE;

    RGFW_windowFlags rgfwFlags = RGFW_windowCenter | RGFW_windowHide | RGFW_windowHideMouse;
    rgfwFlags = RGFW_windowCenter;

    if (!(flags & SYS_FLAGS_RESIZABLE))
        rgfwFlags |= RGFW_windowNoResize;

    s_sysState.window = RGFW_createWindow(title, 0, 0, width, height, rgfwFlags | s_sysState.rgfwWindowFlags);
    if (flags & SYS_FLAGS_FULLSCREEN)
        Sys_SetFullscreen(TRUE);
    s_sysState.flags = flags;
    s_sysState.width = width;
    s_sysState.height = height;
    s_sysState.title = title;
    s_sysState.appName = appName;

    return TRUE;
}

bool Sys_TryPollEvent(sysEvent_t* event)
{
    sysEvent_t resultEvent = { 0 };

    RGFW_event ev;
    if (!RGFW_window_checkEvent(s_sysState.window, &ev))
        return FALSE;

    if (ev.type == RGFW_keyPressed && ev.key.value == RGFW_keyEnter && (ev.key.mod & RGFW_modAlt))
    {
        bool isFullscreen = s_sysState.flags & SYS_FLAGS_FULLSCREEN;
        Sys_SetFullscreen(!isFullscreen);

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

void Sys_SetFullscreen(bool enable)
{
    if (enable && !(s_sysState.flags & SYS_FLAGS_FULLSCREEN))
    {
        if (!RGFW_window_getPosition(s_sysState.window, &s_sysState.lastX, &s_sysState.lastY))
            return;

        s_sysState.flags |= SYS_FLAGS_FULLSCREEN;

        RGFW_monitor* monitor = RGFW_window_getMonitor(s_sysState.window);
        RGFW_window_setBorder(s_sysState.window, FALSE);
        RGFW_window_resize(s_sysState.window, monitor->mode.w, monitor->mode.h);
        RGFW_window_move(s_sysState.window, 0, 0);
    }
    else if (!enable && (s_sysState.flags & SYS_FLAGS_FULLSCREEN))
    {
        s_sysState.flags &= ~SYS_FLAGS_FULLSCREEN;
        RGFW_window_setBorder(s_sysState.window, TRUE);
        RGFW_window_resize(s_sysState.window, s_sysState.width, s_sysState.height);
        RGFW_window_move(s_sysState.window, s_sysState.lastX, s_sysState.lastY);
    }
}

bool Sys_IsFullscreen(void)
{
    return s_sysState.flags & SYS_FLAGS_FULLSCREEN;
}

void Sys_Quit(void)
{
    exit(0);
}

void Sys_ActivateContextGL(void)
{
    RGFW_window_makeCurrentContext_OpenGL(s_sysState.window);
}

void Sys_SwapGL(void)
{
    RGFW_window_swapBuffers_OpenGL(s_sysState.window);
}

sysStateRGFW_t* Sys_GetStateRGFW(void)
{
    return &s_sysState;
}
