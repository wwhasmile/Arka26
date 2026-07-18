#ifndef __SYS_RGFW_H__
#define __SYS_RGFW_H__

#include "sys.h"

#define RGFW_INT_DEFINED
#define RGFW_OPENGL
#include <ext/RGFW.h>

typedef struct
{
    bool running;
    char* title;
    char* appName;
    i32 width;
    i32 height;
    sysFlags flags;
    RGFW_window* window;
} sysState_RGFW;

sysState_RGFW* Sys_GetStateRGFW(void);

#endif // __SYS_RGFW_H__
