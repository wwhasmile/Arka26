#include "memory.h"

#include <core/log.h>
#include <core/assert.h>

#include <stdlib.h>

#define MEMORY_ALIGNMENT 0x10
static const u64 BASE_CHUNK_SIZE = 128 * 1024 * 1024;
static const u64 BASE_ARENA_SIZE = 96 * 1024 * 1024;

ENUM(memoryChunkType_t, u32)
{
    MEMORY_CHUNK_TYPE_MANUAL = 'MEMM',
    MEMORY_CHUNK_TYPE_ROOT = 'MEMR',
    MEMORY_CHUNK_TYPE_NODE = 'MEMN',
};

typedef struct memoryChunk_t
{
    memoryChunkType_t type;
    bool taken;
    u64 size;
    struct memoryChunk_t* next;
    struct memoryChunk_t* previous;
} memoryChunk_t;

typedef struct memoryBump_t
{
    u64 size;
    u64 origin;
    u64 taken;
    struct memoryBump_t* previous;
} memoryBump_t;

static struct
{
    memoryChunk_t* rootChunk;
    memoryBump_t* currentBump;
} s_memoryState;

static memoryChunk_t* Memory_ChunkBlockCreate(u64 size, memoryChunk_t* previous, memoryChunkType_t type);
static memoryBump_t* Memory_BumpCreate(u64 size, memoryBump_t* previous, u64 origin);

bool Memory_Initialize(void)
{
    ASSERT_MESSAGE(s_memoryState.rootChunk == NULL, "Attempted to initialize memory system after it was initialized");

    s_memoryState.rootChunk = Memory_ChunkBlockCreate(BASE_CHUNK_SIZE - sizeof(memoryChunk_t), NULL, MEMORY_CHUNK_TYPE_ROOT);
    if (s_memoryState.rootChunk == NULL)
    {
        LOG_FATAL("Failed to initialize primary memory arena");
        return FALSE;
    }

    s_memoryState.currentBump = Memory_BumpCreate(BASE_ARENA_SIZE, NULL, 0);
    if (s_memoryState.currentBump == NULL)
    {
        LOG_FATAL("Failed to initialize bump allocator");
        free(s_memoryState.rootChunk);
        s_memoryState.rootChunk = NULL;
        return FALSE;
    }

    LOG_SUCCESS("Memory system has been initialized successfully");
    return TRUE;
}

void* Memory_Malloc(u64 size)
{
    ASSERT_MESSAGE(size > 0, "Attempted to allocate a memory block of size 0.");
    size = ALIGN(size, MEMORY_ALIGNMENT);

    memoryChunk_t* chunk;
    if (s_memoryState.rootChunk == NULL)
    {
        chunk = Memory_ChunkBlockCreate(size, NULL, MEMORY_CHUNK_TYPE_MANUAL);
        if (chunk == NULL)
            return NULL;
        return chunk + 1;
    }

    chunk = s_memoryState.rootChunk;
    while (chunk != NULL && (chunk->taken || chunk->size < size))
    {
        if (chunk->next == NULL)
        {
            u64 toAllocate = size > (BASE_CHUNK_SIZE - sizeof(memoryChunk_t)) ?
                (size + BASE_CHUNK_SIZE - sizeof(memoryChunk_t)) :
                (BASE_CHUNK_SIZE - sizeof(memoryChunk_t));
            Memory_ChunkBlockCreate(toAllocate, chunk, MEMORY_CHUNK_TYPE_ROOT);
        }
        chunk = chunk->next;
    }
    if (chunk == NULL)
        return NULL;

    chunk->taken = TRUE;
    if (chunk->size - size > sizeof(memoryChunk_t) + MEMORY_ALIGNMENT)
    {
        memoryChunk_t* next = (memoryChunk_t*)((u8*)(chunk + 1) + size);
        memoryChunk_t result = {
            .type = MEMORY_CHUNK_TYPE_NODE,
            .taken = FALSE,
            .size = chunk->size - size - sizeof(memoryChunk_t),
            .next = chunk->next,
            .previous = chunk,
        };
        if (result.next != NULL)
            result.next->previous = next;
        *next = result;
        chunk->next = next;
        chunk->size = size;
    }
    return chunk + 1;
}

void Memory_Free(void* block)
{
    ASSERT_MESSAGE(block != NULL, "Attempted to deallocate a null pointer");

    memoryChunk_t* chunk = (memoryChunk_t*)block - 1;
    if (chunk->type == MEMORY_CHUNK_TYPE_MANUAL)
    {
        free(chunk);
        return;
    }

    ASSERT_MESSAGE((chunk->type != MEMORY_CHUNK_TYPE_ROOT && chunk->type != MEMORY_CHUNK_TYPE_NODE) || !chunk->taken,
        "Attempted to deallocate an invalid memory chunk");

    chunk->taken = FALSE;

    memoryChunk_t* next = chunk->next;
    if (next != NULL && next->type == MEMORY_CHUNK_TYPE_NODE && !next->taken)
    {
        chunk->size += sizeof(memoryChunk_t) + next->size;
        chunk->next = next->next;
        if (next->next != NULL)
            next->next->previous = chunk;
        next = chunk->next;
    }

    memoryChunk_t* previous = chunk->previous;
    if (previous == NULL)
        return;

    if (chunk->type == MEMORY_CHUNK_TYPE_ROOT)
    {
        if (next == NULL || next->type != MEMORY_CHUNK_TYPE_NODE)
        {
            previous->next = next;
            if (next != NULL)
                next->previous = previous;
            free(chunk);
        }
        return;
    }

    if (previous->taken)
        return;

    if (previous->type == MEMORY_CHUNK_TYPE_ROOT && previous->previous != NULL)
    {
        chunk = previous;
        previous = chunk->previous;
        previous->next = next;
        if (next != NULL)
            next->previous = previous;
        free(chunk);
        return;
    }

    previous->size += sizeof(memoryChunk_t) + chunk->size;
    previous->next = next;
    if (next != NULL)
        next->previous = previous;
}

void* Memory_BumpAlloc(u64 size)
{
    ASSERT_MESSAGE(size > 0, "Attempted to allocate 0 bytes on bump");
    size = ALIGN(size, MEMORY_ALIGNMENT);

    if (s_memoryState.currentBump == NULL)
        return Memory_Malloc(size);
    memoryBump_t* bump = s_memoryState.currentBump;

    if (size > bump->size - bump->taken)
    {
        u64 toAllocate = size > BASE_ARENA_SIZE ? size + BASE_ARENA_SIZE : BASE_ARENA_SIZE;
        memoryBump_t* next = Memory_BumpCreate(toAllocate, bump, bump->origin + bump->size);
        if (next == NULL)
            return NULL;
        bump->taken = s_memoryState.currentBump->size;
        s_memoryState.currentBump = next;
        bump = next;
    }

    void* chunk = (u8*)(bump + 1) + bump->taken;
    bump->taken += size;
    return chunk;
}

u64 Memory_BumpMarker(void)
{
    if (s_memoryState.currentBump == NULL)
        return 0;

    return s_memoryState.currentBump->taken + s_memoryState.currentBump->origin;
}

void Memory_BumpReset(u64 marker)
{
    marker = ALIGN(marker, MEMORY_ALIGNMENT);

    if (s_memoryState.currentBump == NULL)
        return;
    memoryBump_t* bump = s_memoryState.currentBump;

    ASSERT_MESSAGE(marker >= bump->origin + bump->size,
        "Attempted to reset bump allocator to an address larger than the current one");

    while (marker < bump->origin)
    {
        bump = bump->previous;
        Memory_Free(s_memoryState.currentBump);
        s_memoryState.currentBump = bump;
    }
    bump->taken = marker - bump->origin;
}

void Memory_BumpFree(void* block)
{
    ASSERT_MESSAGE(block != NULL, "Attempted to deallocate a null pointer");

    memoryBump_t* bump = s_memoryState.currentBump;
    while (bump != NULL)
    {
        if ((u8*)block >= (u8*)(bump + 1) && (u8*)block < (u8*)(bump + 1) + bump->size)
            return;
        bump = bump->previous;
    }

    Memory_Free(block);
}

memoryChunk_t* Memory_ChunkBlockCreate(u64 size, memoryChunk_t* previous, memoryChunkType_t type)
{
    size = ALIGN(size, MEMORY_ALIGNMENT);
    memoryChunk_t* chunk = (memoryChunk_t*)malloc(size + sizeof(memoryChunk_t));
    if (chunk == NULL)
    {
        LOG_ERROR("Failed to allocate a new heap chunk of size %ull bytes", size);
        return NULL;
    }

    memoryChunk_t result = {
        .type = type,
        .taken = FALSE,
        .size = size,
        .next = NULL,
        .previous = previous,
    };
    if (previous != NULL)
        previous->next = chunk;
    *chunk = result;
    return chunk;
}

memoryBump_t* Memory_BumpCreate(u64 size, memoryBump_t* previous, u64 origin)
{
    size = ALIGN(size, MEMORY_ALIGNMENT);
    memoryBump_t* bump = (memoryBump_t*)Memory_Malloc(sizeof(memoryBump_t) + size);
    if (bump == NULL)
    {
        LOG_ERROR("Failed to allocate a new bump allocator of %ull bytes", size);
        return NULL;
    }

    memoryBump_t result = {
        .size = size,
        .origin = origin,
        .taken = 0,
        .previous = previous,
    };
    *bump = result;
    return bump;
}
