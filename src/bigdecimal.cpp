/*
 * Copyright (c) 2020 Stefan Krah. All rights reserved.
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

#include "bigmath/bigdecimal.h"

#include <cstdint>
#include <iostream>
#include <sstream>

namespace bigmath {

/*****************************************************************************/
/*                               Library init                                */
/*****************************************************************************/

class LibraryInit {
public:
    LibraryInit() {
        mpd_setminalloc(bigmath::MINALLOC);
    }
};

const LibraryInit init;

/*****************************************************************************/
/*                                Decimal API                                */
/*****************************************************************************/

bigdecimal bigdecimal::exact(const char* const s, bd_context& c) {
    bigdecimal result;
    uint32_t status = 0;

    if (s == nullptr) {
        throw value_error("bigdecimal::exact: string argument is NULL");
    }

    mpd_qset_string_exact(result.get(), s, &status);
    c.raise(status);
    return result;
}

bigdecimal bigdecimal::exact(const std::string& s, bd_context& c) {
    return bigdecimal::exact(s.c_str(), c);
}

bigdecimal bigdecimal::ln10(int64_t n, bd_context& c) {
    bigdecimal result;
    uint32_t status = 0;

    mpd_ssize_t nn = bigmath::safe_downcast<mpd_ssize_t, int64_t>(n);
    mpd_qln10(result.get(), nn, &status);

    c.raise(status);
    return result;
}

int32_t bigdecimal::radix() {
    return 10;
}

std::string bigdecimal::repr(bool capitals) const {
    std::string s = to_sci(capitals);

    return "bigmath::bigdecimal(\"" + s + "\")";
}

std::ostream& operator<<(std::ostream& os, const bigdecimal& dec) {
    os << dec.format("f");
    return os;
}

} // namespace bigmath
