#ifndef __SYS_H__
#define __SYS_H__

#include <core/defines.h>

typedef enum
{
    SYS_FLAGS_RESIZABLE = 1 << 0,
    SYS_FLAGS_FULLSCREEN = 1 << 1,
    SYS_FLAGS_VSYNC = 1 << 2,
} sysFlags;

typedef enum
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
} sysEventType;

typedef union
{
    sysEventType type;
    struct
    {
        sysEventType type;
        u8 code;
        u8 mod;
    } key;
    struct
    {
        sysEventType type;
        u32 value;
    } keyChar;
    struct
    {
        sysEventType type;
        i32 width;
        i32 height;
    } windowResized;
} sysEvent;

bool Sys_Initialize(const char* title, const char* appName, i32 width, i32 height, sysFlags flags);

bool Sys_TryPollEvent(sysEvent* event);

#endif // __SYS_H__
