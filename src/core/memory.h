#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <core/defines.h>

bool Memory_Initialize(void);

void* Memory_Malloc(u64 size);

void Memory_Free(void* block);

void* Memory_BumpAlloc(u64 size);

u64 Memory_BumpMarker(void);

void Memory_BumpReset(u64 marker);

// As silly as it is, it's here for the fuller API.
// If the memory system hasn't been initialized then it will use free().
void Memory_BumpFree(void* block);

#endif // __MEMORY_H__
