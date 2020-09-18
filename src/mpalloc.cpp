/*
 * Copyright (c) 2008-2020 Stefan Krah. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "bigmath/mpalloc.h"

#if (MPD_MINOR_VERSION == 4)

#include "bigmath/typearith.h"
#include "mpdecimal.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(_MSC_VER)
#pragma warning(disable : 4232)
#endif

/*
 * Input: 'result' is a static mpd_t with a static coefficient.
 * Assumption: 'nwords' >= result->alloc.
 *
 * Resize the static coefficient to a larger dynamic one and copy the
 * existing data. If successful, the value of 'result' is unchanged.
 * Otherwise, set 'result' to NaN and update 'status' with MPD_Malloc_error.
 */
int bigmath::mpd_switch_to_dyn(mpd_t* result, mpd_ssize_t nwords, uint32_t* status) {
    mpd_uint_t* p = result->data;

    assert(nwords >= result->alloc);

    result->data = (mpd_uint_t*) mpd_alloc(nwords, sizeof *result->data);
    if (result->data == NULL) {
        result->data = p;
        mpd_set_qnan(result);
        mpd_set_positive(result);
        result->exp = result->digits = result->len = 0;
        *status |= MPD_Malloc_error;
        return 0;
    }

    memcpy(result->data, p, result->alloc * (sizeof *result->data));
    result->alloc = nwords;
    mpd_set_dynamic_data(result);
    return 1;
}

/*
 * Input: 'result' is a static mpd_t with a static coefficient.
 *
 * Convert the coefficient to a dynamic one that is initialized to zero. If
 * malloc fails, set 'result' to NaN and update 'status' with MPD_Malloc_error.
 */
int bigmath::mpd_switch_to_dyn_zero(mpd_t* result, mpd_ssize_t nwords, uint32_t* status) {
    mpd_uint_t* p = result->data;

    result->data = (mpd_uint_t*) mpd_calloc(nwords, sizeof *result->data);
    if (result->data == NULL) {
        result->data = p;
        mpd_set_qnan(result);
        mpd_set_positive(result);
        result->exp = result->digits = result->len = 0;
        *status |= MPD_Malloc_error;
        return 0;
    }

    result->alloc = nwords;
    mpd_set_dynamic_data(result);

    return 1;
}

/*
 * Input: 'result' is a static or a dynamic mpd_t with a dynamic coefficient.
 * Resize the coefficient to length 'nwords':
 *   Case nwords > result->alloc:
 *     If realloc is successful:
 *       'result' has a larger coefficient but the same value. Return 1.
 *     Otherwise:
 *       Set 'result' to NaN, update status with MPD_Malloc_error and return 0.
 *   Case nwords < result->alloc:
 *     If realloc is successful:
 *       'result' has a smaller coefficient. result->len is undefined. Return 1.
 *     Otherwise (unlikely):
 *       'result' is unchanged. Reuse the now oversized coefficient. Return 1.
 */
int bigmath::mpd_realloc_dyn(mpd_t* result, mpd_ssize_t nwords, uint32_t* status) {
    uint8_t err = 0;

    result->data = (mpd_uint_t*) mpd_realloc(result->data, nwords, sizeof *result->data, &err);
    if (!err) {
        result->alloc = nwords;
    } else if (nwords > result->alloc) {
        mpd_set_qnan(result);
        mpd_set_positive(result);
        result->exp = result->digits = result->len = 0;
        *status |= MPD_Malloc_error;
        return 0;
    }

    return 1;
}

/*
 * Input: 'result' is a static mpd_t with a static coefficient.
 * Assumption: 'nwords' >= result->alloc.
 *
 * Resize the static coefficient to a larger dynamic one and copy the
 * existing data.
 *
 * On failure the value of 'result' is unchanged.
 */
int bigmath::mpd_switch_to_dyn_cxx(mpd_t* result, mpd_ssize_t nwords) {
    assert(nwords >= result->alloc);

    mpd_uint_t* data = (mpd_uint_t*) mpd_alloc(nwords, sizeof *result->data);
    if (data == NULL) {
        return 0;
    }

    memcpy(data, result->data, result->alloc * (sizeof *result->data));
    result->data = data;
    result->alloc = nwords;
    mpd_set_dynamic_data(result);
    return 1;
}

/*
 * Input: 'result' is a static or a dynamic mpd_t with a dynamic coefficient.
 * Resize the coefficient to length 'nwords':
 *   Case nwords > result->alloc:
 *     If realloc is successful:
 *       'result' has a larger coefficient but the same value. Return 1.
 *     Otherwise:
 *       'result' has a the same coefficient. Return 0.
 *   Case nwords < result->alloc:
 *     If realloc is successful:
 *       'result' has a smaller coefficient. result->len is undefined. Return 1.
 *     Otherwise (unlikely):
 *       'result' is unchanged. Reuse the now oversized coefficient. Return 1.
 */
int bigmath::mpd_realloc_dyn_cxx(mpd_t* result, mpd_ssize_t nwords) {
    uint8_t err = 0;

    mpd_uint_t* p = (mpd_uint_t*) mpd_realloc(result->data, nwords, sizeof *result->data, &err);
    if (!err) {
        result->data = p;
        result->alloc = nwords;
    } else if (nwords > result->alloc) {
        return 0;
    }

    return 1;
}

#endif