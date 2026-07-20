#ifndef __SYS_H__
#define __SYS_H__

#include <core/defines.h>

ENUM(sysFlags_t, u8)
{
    SYS_FLAGS_RESIZABLE = 1 << 0,
    SYS_FLAGS_FULLSCREEN = 1 << 1,
    SYS_FLAGS_VSYNC = 1 << 2,
};

ENUM(sysEventType_t, u8)
{
    SYS_EVENT_NONE,
 	SYS_EVENT_KEY_PRESSED,
	SYS_EVENT_KEY_RELEASED,
	SYS_EVENT_KEY_CHAR,
	SYS_EVENT_WINDOW_RESIZED,
	SYS_EVENT_WINDOW_FOCUSED,
	SYS_EVENT_WINDOW_UNFOCUSED,
	SYS_EVENT_WINDOW_CLOSED,
	SYS_EVENT_MAX
};

typedef union
{
    sysEventType_t type;
    struct
    {
        sysEventType_t type;
        u8 code;
        u8 mod;
    } key;
    struct
    {
        sysEventType_t type;
        u32 value;
    } keyChar;
    struct
    {
        sysEventType_t type;
        i32 width;
        i32 height;
    } windowResized;
} sysEvent_t;

bool Sys_Initialize(const char* title, const char* appName, i32 width, i32 height, sysFlags_t flags);

bool Sys_TryPollEvent(sysEvent_t* event);

void Sys_SetFullscreen(bool enable);
bool Sys_IsFullscreen(void);

void Sys_Quit(void);

#endif // __SYS_H__
