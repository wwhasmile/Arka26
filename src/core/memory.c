#include "memory.h"

#include <stdlib.h>

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

typedef struct memoryArena_t
{
    u64 size;
    u64 origin;
    u64 taken;
    struct memoryArena_t* previous;
} memoryArena_t;

static struct
{
    memoryChunk_t* rootChunk;
    memoryArena_t* currentArena;
} s_memoryState;

static memoryChunk_t* Memory_ChunkBlockCreate(u64 size, memoryChunk_t* previous, memoryChunkType_t type);

bool Memory_Initialize(void)
{
    if (s_memoryState.rootChunk != NULL)
        return FALSE;

    s_memoryState.rootChunk = Memory_ChunkBlockCreate(BASE_CHUNK_SIZE - sizeof(memoryChunk_t), NULL, MEMORY_CHUNK_TYPE_ROOT);
    if (s_memoryState.rootChunk == NULL)
        return FALSE;

    return TRUE;
}

void* Memory_Malloc(u64 size)
{
    size = ALIGN(size, 0x10);

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
                size : (BASE_CHUNK_SIZE - sizeof(memoryChunk_t));
            Memory_ChunkBlockCreate(toAllocate, chunk, MEMORY_CHUNK_TYPE_ROOT);
        }
        chunk = chunk->next;
    }
    if (chunk == NULL)
        return NULL;

    chunk->taken = TRUE;
    if (chunk->size - size > sizeof(memoryChunk_t) + 0x10)
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
    if (block == NULL)
        return;

    memoryChunk_t* chunk = (memoryChunk_t*)block - 1;
    if (chunk->type == MEMORY_CHUNK_TYPE_MANUAL)
    {
        free(chunk);
        return;
    }

    if (!chunk->taken)
        return;

    chunk->taken = FALSE;

    memoryChunk_t* next = chunk->next;
    if (next != NULL && next->type == MEMORY_CHUNK_TYPE_NODE && !next->taken)
    {
        chunk->size += sizeof(memoryChunk_t) + next->size;
        chunk->next = next->next;
        if (next->next != NULL)
            next->next->previous = chunk;
    }

    memoryChunk_t* previous = chunk->previous;
    if (previous != NULL)
    {
        if (chunk->type == MEMORY_CHUNK_TYPE_ROOT && chunk->next == NULL)
        {
            previous->next = NULL;
            free(chunk);
            return;
        }

        if (chunk->type != MEMORY_CHUNK_TYPE_ROOT && !previous->taken)
        {
            previous->size += sizeof(memoryChunk_t) + chunk->size;
            previous->next = chunk->next;
            if (chunk->next != NULL)
                chunk->next->previous = previous;
        }
    }
}

memoryChunk_t* Memory_ChunkBlockCreate(u64 size, memoryChunk_t* previous, memoryChunkType_t type)
{
    size = ALIGN(size, 0x10);
    memoryChunk_t* chunk = (memoryChunk_t*)malloc(size + sizeof(memoryChunk_t));
    if (chunk == NULL)
        return NULL;

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
