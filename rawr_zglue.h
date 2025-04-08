/* rawr_zglue.h - v0.1.0 */

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

#ifndef RAWR_ZGLUE_H
#define RAWR_ZGLUE_H

#if defined(RAWR_ARENA_H) && defined(RAWR_DYNARR_H)
#define RAWR_ZGLUE_ARENA_AND_DYNARR
#endif /* defined(RAWR_ARENA_H) && defined(RAWR_DYNARR_H) */

#ifdef RAWR_ZGLUE_ARENA_AND_DYNARR
RawrDynarrAllocator rawrZglueArenaDynarrAllocator(RawrArena *arena);
#endif /* RAWR_ZGLUE_ARENA_AND_DYNARR */

#ifdef RAWR_ZGLUE_IMPLEMENTATION
#ifdef RAWR_ZGLUE_ARENA_AND_DYNARR
#include <stdalign.h>

static void *rawrZglueArenaDynarrAllocatorRealloc(
    void *ptr,
    size_t new_size,
    size_t old_size,
    void *user_data
) {
    if (ptr) {
        void *new_ptr = rawrArenaRealloc(
            user_data,
            ptr,
            new_size,
            old_size,
            alignof(max_align_t)
        );
        return new_size > 0 ? new_ptr : NULL;
    } else if (!ptr && new_size > 0) {
        return rawrArenaAlloc(user_data, new_size, alignof(max_align_t));
    }

    return NULL;
}

RawrDynarrAllocator rawrZglueArenaDynarrAllocator(RawrArena *arena) {
    return (RawrDynarrAllocator){
        .user_data = arena,
        .realloc = rawrZglueArenaDynarrAllocatorRealloc,
    };
}

#endif /* RAWR_ZGLUE_ARENA_AND_DYNARR */
#endif /* RAWR_ZGLUE_IMPLEMENTATION */
#endif /* RAWR_ZGLUE_H */
