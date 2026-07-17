#ifndef __SYS_H__
#define __SYS_H__

#include <core/defines.h>

typedef enum
{
    SYS_FLAGS_RESIZABLE = 1 << 0,
    SYS_FLAGS_FULLSCREEN = 1 << 1,
    SYS_FLAGS_VSYNC = 1 << 2,
    SYS_FLAGS_MOUSE = 1 << 3,
} sysFlags;

bool Sys_Initialize(const char* title, const char* appName, i32 width, i32 height, sysFlags flags);

#endif // __SYS_H__
