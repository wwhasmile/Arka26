#ifndef __SYS_RGFW_H__
#define __SYS_RGFW_H__

#include "sys.h"

#define RGFW_INT_DEFINED
#define RGFW_OPENGL
#include <ext/RGFW.h>

typedef struct
{
    bool running;
    const char* title;
    const char* appName;
    i32 width;
    i32 height;
    i32 lastX;
    i32 lastY;
    sysFlags_t flags;
    RGFW_initFlags rgfwInitFlags;
    RGFW_windowFlags rgfwWindowFlags;
    RGFW_window* window;
} sysStateRGFW_t;

void Sys_ActivateContextGL(void);

void Sys_SwapGL(void);

sysStateRGFW_t* Sys_GetStateRGFW(void);

#endif // __SYS_RGFW_H__
