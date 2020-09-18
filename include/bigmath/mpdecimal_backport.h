/*!
 * trash.
 * mpdecimal_backport.h
 *
 * \date 09/13/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */
#ifndef LIBMPDECPP_BACKPORT_BACKPORT_H
#define LIBMPDECPP_BACKPORT_BACKPORT_H

#include "bigmath_config.h"

// old mpdecimal thinks we're still using MSVC 6
#ifdef _MSC_VER
#define _TMP_MSC_VER _MSC_VER
#undef _MSC_VER
#include <mpdecimal.h>
#define _MSC_VER _TMP_MSC_VER
#else
#include <mpdecimal.h>
#endif

#if (MPD_MINOR_VERSION == 4)

void mpd_qset_string_exact(mpd_t* dec, const char* s, uint32_t* status);
void mpd_qset_i64_exact(mpd_t* result, int64_t a, uint32_t* status);
int mpd_qcopy_cxx(mpd_t* result, const mpd_t* a);

#endif

#endif // LIBMPDECPP_BACKPORT_BACKPORT_H
