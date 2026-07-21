#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <core/defines.h>

bool Memory_Initialize(void);

void* Memory_Malloc(u64 size);

void Memory_Free(void* block);

#endif // __MEMORY_H__
