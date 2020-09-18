/*!
 * trash.
 * mpdecimal_backport.cpp
 *
 * \date 09/13/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */

#include "bigmath/mpdecimal_backport.h"

#include <cassert>
#include <cstring>

#if (MPD_MINOR_VERSION == 4)

#include "bigmath/mpalloc.h"

/* convert a character string to a decimal, use a maxcontext for conversion */
void mpd_qset_string_exact(mpd_t* dec, const char* s, uint32_t* status) {
    mpd_context_t maxcontext;

    mpd_maxcontext(&maxcontext);
    mpd_qset_string(dec, s, &maxcontext, status);

    if (*status & (MPD_Inexact | MPD_Rounded | MPD_Clamped)) {
        /* we want exact results */
        mpd_seterror(dec, MPD_Invalid_operation, status);
    }
    *status &= MPD_Errors;
}

/* quietly set a decimal from an int64_t, use a maxcontext for conversion */
void mpd_qset_i64_exact(mpd_t* result, int64_t a, uint32_t* status) {
    mpd_context_t maxcontext;

    mpd_maxcontext(&maxcontext);
#ifdef CONFIG_64
    mpd_qset_ssize(result, a, &maxcontext, status);
#else
    _c32_qset_i64(result, a, &maxcontext, status);
#endif

    if (*status & (MPD_Inexact | MPD_Rounded | MPD_Clamped)) {
        /* we want exact results */
        mpd_seterror(result, MPD_Invalid_operation, status);
    }
    *status &= MPD_Errors;
}

/* Same as mpd_qresize, but do not set the result no NaN on failure. */
static ALWAYS_INLINE int mpd_qresize_cxx(mpd_t* result, mpd_ssize_t nwords) {
    assert(!mpd_isconst_data(result));  /* illegal operation for a const */
    assert(!mpd_isshared_data(result)); /* illegal operation for a shared */
    assert(MPD_MINALLOC <= result->alloc);

    nwords = (nwords <= MPD_MINALLOC) ? MPD_MINALLOC : nwords;
    if (nwords == result->alloc) {
        return 1;
    }
    if (mpd_isstatic_data(result)) {
        if (nwords > result->alloc) {
            return bigmath::mpd_switch_to_dyn_cxx(result, nwords);
        }
        return 1;
    }

    return bigmath::mpd_realloc_dyn_cxx(result, nwords);
}

int mpd_qcopy_cxx(mpd_t* result, const mpd_t* a) {
    if (result == a)
        return 1;

    if (!mpd_qresize_cxx(result, a->len)) {
        return 0;
    }

    mpd_copy_flags(result, a);
    result->exp = a->exp;
    result->digits = a->digits;
    result->len = a->len;
    memcpy(result->data, a->data, a->len * (sizeof *result->data));

    return 1;
}

#endif //check mpdecimal 2.4.2