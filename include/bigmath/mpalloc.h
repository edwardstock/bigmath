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

#ifndef LIBMPDECPP_BACKPORT_MPALLOC_H_
#define LIBMPDECPP_BACKPORT_MPALLOC_H_

#include "mpdecimal_backport.h"

#include <cstdint>

#if (MPD_MINOR_VERSION == 4)
/* Internal header file: all symbols have local scope in the DSO */
MPD_PRAGMA(MPD_HIDE_SYMBOLS_START)

namespace bigmath {

int mpd_switch_to_dyn(mpd_t* result, mpd_ssize_t size, uint32_t* status);
int mpd_switch_to_dyn_zero(mpd_t* result, mpd_ssize_t size, uint32_t* status);
int mpd_realloc_dyn(mpd_t* result, mpd_ssize_t size, uint32_t* status);

int mpd_switch_to_dyn_cxx(mpd_t* result, mpd_ssize_t size);
int mpd_realloc_dyn_cxx(mpd_t* result, mpd_ssize_t size);

//void* mpd_callocfunc_em(size_t nmemb, size_t size);
//void* mpd_alloc(mpd_size_t nmemb, mpd_size_t size);
//void* mpd_calloc(mpd_size_t nmemb, mpd_size_t size);
//void* mpd_realloc(void* ptr, mpd_size_t nmemb, mpd_size_t size, uint8_t* err);
//void* mpd_sh_alloc(mpd_size_t struct_size, mpd_size_t nmemb, mpd_size_t size);
//mpd_t* mpd_qnew_size(mpd_ssize_t nwords);
//mpd_t* mpd_qnew(void);

} // namespace bigmath

MPD_PRAGMA(MPD_HIDE_SYMBOLS_END) /* restore previous scope rules */

#endif //check mpd 2.4.2

#endif /* LIBMPDECPP_BACKPORT_MPALLOC_H_ */
