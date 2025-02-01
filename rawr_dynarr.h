/* rawr_dynarr.h - v0.0.1 */

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

#ifndef RAWR_DYNARR_H
#define RAWR_DYNARR_H

#include <stddef.h>

typedef struct RawrDynarrHeader {
    size_t length;
    size_t capacity;
} RawrDynarrHeader;

typedef struct RawrDynarrGeneralPointer {
    RawrDynarrHeader **header_pp;
    size_t element_size;
    size_t padded_header_size;
} RawrDynarrGeneralPointer;

#define RAWR_DYNARR_GEN(type, name) \
    struct name { \
        RawrDynarrHeader h; /* header */ \
        type d[]; /* data */ \
    }

typedef enum {
    RAWR_DYNARR_RESULT_SUCCESS,
    RAWR_DYNARR_RESULT_FAIL_ALLOC,
} RawrDynarrResult;

RawrDynarrResult rawrDynarrCreate(RawrDynarrGeneralPointer gp);

RawrDynarrResult rawrDynarrCreateWithCapacity(
    RawrDynarrGeneralPointer gp,
    size_t capacity
);

RawrDynarrResult rawrDynarrCreateWithLength(
    RawrDynarrGeneralPointer gp,
    size_t length
);

RawrDynarrResult rawrDynarrCreateWithLengthExact(
    RawrDynarrGeneralPointer gp,
    size_t length
);

void rawrDynarrDestroy(RawrDynarrGeneralPointer gp);

RawrDynarrResult rawrDynarrCapacitySet(
    RawrDynarrGeneralPointer gp,
    size_t new_capacity
);

RawrDynarrResult rawrDynarrResize(
    RawrDynarrGeneralPointer gp,
    size_t new_length
);

RawrDynarrResult rawrDynarrResizeExact(
    RawrDynarrGeneralPointer gp,
    size_t new_length
);

RawrDynarrResult rawrDynarrExtend(RawrDynarrGeneralPointer gp, size_t n);
void rawrDynarrContract(RawrDynarrGeneralPointer gp, size_t n);
size_t rawrDynarrLastIndex(RawrDynarrHeader header);

/* Dummy function for type checking in macros */
static inline int rawrDynarrTypecheckDummy(RawrDynarrHeader *header_p) {
    (void)header_p;
    return 0;
}

#define RAWR_DYNARR_NOEVAL(x) (sizeof(x) - sizeof(x))

#define RAWR_DYNARR_GENERAL_POINTER(dynarr_pp) \
    ((RawrDynarrGeneralPointer) { \
        (RawrDynarrHeader **)(dynarr_pp), \
        sizeof((**(dynarr_pp)).d[0]), \
        sizeof((**(dynarr_pp))) \
        + /* zero */ RAWR_DYNARR_NOEVAL( \
            rawrDynarrTypecheckDummy(&(**(dynarr_pp)).h) \
        ) \
    })

#define RAWR_DYNARR_LAST_INDEX(dynarr_p) rawrDynarrLastIndex((dynarr_p)->h)
#define RAWR_DYNARR_LAST(dynarr_p) \
    ((dynarr_p)->d[RAWR_DYNARR_LAST_INDEX(dynarr_p)])

#ifdef RAWR_DYNARR_IMPLEMENTATION

#include <assert.h>
#include <stdlib.h>

RawrDynarrResult rawrDynarrCreate(RawrDynarrGeneralPointer gp) {
    return rawrDynarrCreateWithCapacity(gp, 0);
}

RawrDynarrResult rawrDynarrCreateWithCapacity(
    RawrDynarrGeneralPointer gp,
    size_t capacity
) {
    *gp.header_pp = NULL;
    return rawrDynarrCapacitySet(gp, capacity);
}

RawrDynarrResult rawrDynarrCreateWithLength(
    RawrDynarrGeneralPointer gp,
    size_t length
) {
    *gp.header_pp = NULL;
    return rawrDynarrResize(gp, length);
}

RawrDynarrResult rawrDynarrCreateWithLengthExact(
    RawrDynarrGeneralPointer gp,
    size_t length
) {
    *gp.header_pp = NULL;
    return rawrDynarrResizeExact(gp, length);
}

void rawrDynarrDestroy(RawrDynarrGeneralPointer gp) {
    free(*gp.header_pp);
    *gp.header_pp = NULL;
}

RawrDynarrResult rawrDynarrCapacitySet(
    RawrDynarrGeneralPointer gp,
    size_t new_capacity
) {
    RawrDynarrHeader *new_header_p = realloc(
        *gp.header_pp,
        gp.padded_header_size + new_capacity * gp.element_size
    );

    if (!new_header_p) {
        return RAWR_DYNARR_RESULT_FAIL_ALLOC;
    }

    if (*gp.header_pp == NULL) {
        new_header_p->length = 0;
    }
    *gp.header_pp = new_header_p;

    if (new_header_p->length > new_capacity) {
        new_header_p->length = new_capacity;
    }
    new_header_p->capacity = new_capacity;

    return RAWR_DYNARR_RESULT_SUCCESS;
}

RawrDynarrResult rawrDynarrResize(
    RawrDynarrGeneralPointer gp,
    size_t new_length
) {
    if (*gp.header_pp && new_length <= (**gp.header_pp).capacity) {
        (**gp.header_pp).length = new_length;
        return RAWR_DYNARR_RESULT_SUCCESS;
    }

    RawrDynarrResult result = rawrDynarrCapacitySet(gp, new_length * 3 / 2);
    if (result) {
        return result;
    }

    (**gp.header_pp).length = new_length;
    return RAWR_DYNARR_RESULT_SUCCESS;
}

RawrDynarrResult rawrDynarrResizeExact(
    RawrDynarrGeneralPointer gp,
    size_t new_length
) {
    RawrDynarrResult result = rawrDynarrCapacitySet(gp, new_length);
    if (result) {
        return result;
    }

    (**gp.header_pp).length = new_length;
    return RAWR_DYNARR_RESULT_SUCCESS;
}

RawrDynarrResult rawrDynarrExtend(RawrDynarrGeneralPointer gp, size_t n) {
    return rawrDynarrResize(gp, (**gp.header_pp).length + n);
}

void rawrDynarrContract(RawrDynarrGeneralPointer gp, size_t n) {
    assert(n <= (*gp.header_pp)->length);
    (*gp.header_pp)->length -= n;
}

size_t rawrDynarrLastIndex(RawrDynarrHeader header) {
    assert(header.length);
    return header.length - 1;
}

#endif /* RAWR_DYNARR_IMPLEMENTATION */
#endif /* RAWR_DYNARR_H */
