/* rawr_arena.h - v0.1.0 */

/*
 * BSD 1-Clause License
 *
 * Copyright (c) 2025 rawr developers
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* TODO: add a description */
/* TODO: add support for custom allocators */
/* TODO: implement growable arena */

#ifndef RAWR_ARENA_H
#define RAWR_ARENA_H

#include <stddef.h>

typedef struct RawrArena {
    size_t used;
    size_t capacity;
} RawrArena;

RawrArena *rawrArenaCreate(size_t capacity);
void *rawrArenaAlloc(RawrArena *arena, size_t size, size_t alignment);

/* NOTE: alignment expected to be the same that you provided on alloc */
void *rawrArenaRealloc(
    RawrArena *arena,
    void *ptr,
    size_t new_size,
    size_t old_size,
    size_t alignment
);

void rawrArenaReset(RawrArena *arena);
void rawrArenaDestroy(RawrArena *arena);

#ifdef RAWR_ARENA_IMPLEMENTATION

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void *rawrArenaData(RawrArena *arena) {
    return arena + 1;
}

static void *rawrArenaTop(RawrArena *arena) {
    return (char *)rawrArenaData(arena) + arena->used;
}

static size_t rawrArenaCalculatePadding(void *ptr, size_t alignment) {
    return (alignment - (uintptr_t)ptr % alignment) % alignment;
}

RawrArena *rawrArenaCreate(size_t capacity) {
    RawrArena *arena = malloc(sizeof(*arena) + capacity);
    if (!arena) {
        return NULL;
    }

    *arena = (RawrArena){
        .used = 0,
        .capacity = capacity,
    };

    return arena;
}

void *rawrArenaAlloc(RawrArena *arena, size_t size, size_t alignment) {
    size_t padding = rawrArenaCalculatePadding(rawrArenaTop(arena), alignment);
    if (arena->used + padding + size > arena->capacity) {
        return NULL;
    }

    void *ptr = (char *)rawrArenaTop(arena) + padding;
    arena->used += padding + size;
    return ptr;
}

void *rawrArenaRealloc(
    RawrArena *arena,
    void *ptr,
    size_t new_size,
    size_t old_size,
    size_t alignment
) {
    assert((uintptr_t)ptr % alignment == 0);

    if (new_size <= old_size) {
        return ptr;
    }

    void *allocation_end = (char *)ptr + old_size;
    if (allocation_end == rawrArenaTop(arena)) {
        size_t delta_size = new_size - old_size;
        if (arena->used + delta_size > arena->capacity) {
            return NULL;
        }

        arena->used += delta_size;
        return ptr;
    }

    void *new_ptr = rawrArenaAlloc(arena, new_size, alignment);
    if (!new_ptr) {
        return NULL;
    }

    return memcpy(new_ptr, ptr, old_size);
}

void rawrArenaReset(RawrArena *arena) {
    arena->used = 0;
}

void rawrArenaDestroy(RawrArena *arena) {
    free(arena);
}

#endif /* RAWR_ARENA_IMPLEMENTATION */
#endif /* RAWR_ARENA_H */
