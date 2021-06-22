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

/**
 * \brief This class is a С++ backport to work with mpdecimal 2.4.2, as only 2.5.0 have c++ implementation
 */

#ifndef BIGMATHPP_MPDECIMAL_H
#define BIGMATHPP_MPDECIMAL_H

#include "bd_context.h"
#include "bigint.h"
#include "errors.h"
#include "mpdecimal_backport.h"
#include "utils.h"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#undef isfinite    /* math.h */
#undef isnan       /* math.h */
#undef issubnormal /* math.h */
#undef isspecial   /* ctype.h */

/******************************************************************************/
/*                              Decimal object                                */
/******************************************************************************/

namespace bigmath {

constexpr mpd_ssize_t MINALLOC = 4;

class BIGMATHPP_API bigdecimal {
private:
    mpd_uint_t data[MINALLOC] = {0};

    mpd_t value{
        MPD_STATIC | MPD_STATIC_DATA | MPD_SNAN, /* flags */
        0,                                       /* exp */
        0,                                       /* digits */
        0,                                       /* len */
        MINALLOC,                                /* alloc */
        data                                     /* data */
    };

    /* mpd_t accessors */
    ALWAYS_INLINE
    bool isstatic() const {
        return value.data == data;
    }

    /* reset rhs to snan after moving data to lhs */
    ALWAYS_INLINE
    void reset() {
        value = {
            MPD_STATIC | MPD_STATIC_DATA | MPD_SNAN, /* flags */
            0,                                       /* exp */
            0,                                       /* digits */
            0,                                       /* len */
            MINALLOC,                                /* alloc */
            data                                     /* data */
        };
    }

    /* Copy flags, preserving memory attributes of result. */
    ALWAYS_INLINE
    uint8_t copy_flags(const uint8_t rflags, const uint8_t aflags) {
        return (rflags & (MPD_STATIC | MPD_DATAFLAGS)) |
               (aflags & ~(MPD_STATIC | MPD_DATAFLAGS));
    }

    ALWAYS_INLINE
    void copy_value(const mpd_t* const src, const bool fastcopy) {
        assert(mpd_isstatic(&value));
        assert(mpd_isstatic(src));
        assert(value.alloc >= MINALLOC);
        assert(src->alloc >= MINALLOC);
        assert(MPD_MINALLOC == MINALLOC);
        if (fastcopy) {
            value.data[0] = src->data[0];
            value.data[1] = src->data[1];
            value.data[2] = src->data[2];
            value.data[3] = src->data[3];
            value.flags = copy_flags(value.flags, src->flags);
            value.exp = src->exp;
            value.digits = src->digits;
            value.len = src->len;
        } else {
            if (!mpd_qcopy_cxx(&value, src)) {
                context.raise(MPD_Malloc_error);
            }
        }
    }

    ALWAYS_INLINE
    void move_value(const mpd_t* const src, const bool fastcopy) {
        if (!mpd_isstatic(src)) {
            return;
        }
        assert(mpd_isstatic(&value));
        assert(mpd_isstatic(src));
        assert(value.alloc >= MINALLOC);
        assert(src->alloc >= MINALLOC);
        assert(MPD_MINALLOC == MINALLOC);
        if (fastcopy) {
            value.data[0] = src->data[0];
            value.data[1] = src->data[1];
            value.data[2] = src->data[2];
            value.data[3] = src->data[3];
            value.flags = copy_flags(value.flags, src->flags);
            value.exp = src->exp;
            value.digits = src->digits;
            value.len = src->len;
        } else {
            assert(mpd_isdynamic_data(src));
            if (mpd_isdynamic_data(&value)) {
                mpd_free(value.data);
            }
            value = *src;
            assert(mpd_isstatic(&value));
            assert(mpd_isdynamic_data(&value));
        }
    }

    ALWAYS_INLINE bigdecimal unary_func_status(
        int (*func)(mpd_t*, const mpd_t*, uint32_t*)) const {
        bigdecimal result;
        uint32_t status = 0;
        if (!func(result.get(), getconst(), &status)) {
            throw malloc_error("out of memory");
        }
        return result;
    }

    ALWAYS_INLINE bigdecimal unary_func(
        void (*func)(mpd_t*, const mpd_t*, const mpd_context_t*, uint32_t*),
        bd_context& c = context) const {
        bigdecimal result;
        uint32_t status = 0;
        func(result.get(), getconst(), c.getconst(), &status);
        c.raise(status);
        return result;
    }

    ALWAYS_INLINE bigdecimal binary_func_noctx(
        int (*func)(mpd_t*, const mpd_t*, const mpd_t*),
        const bigdecimal& other) const {
        bigdecimal result;
        (void) func(result.get(), getconst(), other.getconst());
        return result;
    }

    ALWAYS_INLINE bigdecimal int_binary_func(
        int (*func)(mpd_t*, const mpd_t*, const mpd_t*, const mpd_context_t*, uint32_t*),
        const bigdecimal& other,
        bd_context& c = context) const {
        bigdecimal result;
        uint32_t status = 0;
        (void) func(result.get(), getconst(), other.getconst(), c.getconst(), &status);
        c.raise(status);
        return result;
    }

    ALWAYS_INLINE bigdecimal binary_func(
        void (*func)(mpd_t*, const mpd_t*, const mpd_t*, const mpd_context_t*, uint32_t*),
        const bigdecimal& other,
        bd_context& c = context) const {
        bigdecimal result;
        uint32_t status = 0;
        func(result.get(), getconst(), other.getconst(), c.getconst(), &status);
        c.raise(status);
        return result;
    }

    ALWAYS_INLINE bigdecimal binary_func(
        // func
        void (*func)(mpd_t*, const mpd_t*, const mpd_t*, const mpd_context_t*, uint32_t*),
        // other
        const bigdecimal& other,
        // context
        bd_context&& c) const {
        bigdecimal result;
        uint32_t status = 0;
        func(result.get(), getconst(), other.getconst(), c.getconst(), &status);
        c.raise(status);
        return result;
    }
    ALWAYS_INLINE bigdecimal& inplace_binary_func_move_ctx(
        void (*func)(mpd_t*, const mpd_t*, const mpd_t*, const mpd_context_t*, uint32_t*),
        const bigdecimal& other,
        bd_context&& c) {
        uint32_t status = 0;
        func(get(), getconst(), other.getconst(), c.getconst(), &status);
        c.raise(status);
        return *this;
    }

    ALWAYS_INLINE bigdecimal& inplace_binary_func(
        void (*func)(mpd_t*, const mpd_t*, const mpd_t*, const mpd_context_t*, uint32_t*),
        const bigdecimal& other,
        bd_context& c = context) {
        uint32_t status = 0;
        func(get(), getconst(), other.getconst(), c.getconst(), &status);
        c.raise(status);
        return *this;
    }

    ALWAYS_INLINE bigdecimal inplace_shiftl(const int64_t n, bd_context& c = context) {
        uint32_t status = 0;
        mpd_ssize_t nn = bigmath::safe_downcast<mpd_ssize_t, int64_t>(n);
        mpd_qshiftl(get(), getconst(), nn, &status);
        c.raise(status);
        return *this;
    }

    ALWAYS_INLINE bigdecimal inplace_shiftr(const int64_t n, bd_context& c = context) {
        uint32_t status = 0;
        mpd_ssize_t nn = bigmath::safe_downcast<mpd_ssize_t, int64_t>(n);
        mpd_qshiftr(get(), getconst(), nn, &status);
        c.raise(status);
        return *this;
    }

public:
    /***********************************************************************/
    /*               Exact conversions regardless of context               */
    /***********************************************************************/

    /* Implicit */
    bigdecimal() {
    }
    bigdecimal(const bigdecimal& other) {
        *this = other;
    }
    bigdecimal(bigdecimal&& other) noexcept {
        *this = std::move(other);
    }

    ENABLE_IF_SIGNED(T)
    explicit bigdecimal(const T& other) {
        ASSERT_SIGNED(T);
        uint32_t status = 0;
        mpd_qset_i64_exact(&value, other, &status);
        context.raise(status);
    }

    ENABLE_IF_UNSIGNED(T)
    explicit bigdecimal(const T& other) {
        ASSERT_UNSIGNED(T);
        uint32_t status = 0;
        mpd_qset_u64_exact(&value, other, &status);
        context.raise(status);
    }

    explicit bigdecimal(double value) {
        std::stringstream ss;
        ss << value;
        set_str(ss.str());
    }

    /* Explicit */
    explicit bigdecimal(const char* const s) {
        if (s == nullptr) {
            throw value_error("bigdecimal: string argument in constructor is NULL");
        }
        set_str(s);
    }

    explicit bigdecimal(const std::string& s) {
        set_str(s);
    }

    explicit bigdecimal(const bigmath::bigint& s) {
        set_str(s.str());
    }

    /***********************************************************************/
    /*                inexact_error conversions that use a context               */
    /***********************************************************************/
    explicit bigdecimal(const bigdecimal& other, bd_context& c) {
        const mpd_context_t* ctx = c.getconst();

        *this = other;

        if (mpd_isnan(&value) && value.digits > ctx->prec - ctx->clamp) {
            /* Special case: too many NaN payload digits */
            mpd_setspecial(&value, MPD_POS, MPD_NAN);
            c.raise(MPD_Conversion_syntax);
        } else {
            uint32_t status = 0;
            mpd_qfinalize(&value, ctx, &status);
            c.raise(status);
        }
    }

    ENABLE_IF_SIGNED(T)
    explicit bigdecimal(const T& other, bd_context& c) {
        ASSERT_SIGNED(T);
        uint32_t status = 0;
        mpd_qset_i64(&value, other, c.getconst(), &status);
        c.raise(status);
    }

    ENABLE_IF_UNSIGNED(T)
    explicit bigdecimal(const T& other, bd_context& c) {
        ASSERT_UNSIGNED(T);
        uint32_t status = 0;
        mpd_qset_u64(&value, other, c.getconst(), &status);
        c.raise(status);
    }

    explicit bigdecimal(const char* const s, bd_context& c) {
        uint32_t status = 0;
        if (s == nullptr) {
            throw value_error("bigdecimal: string argument in constructor is NULL");
        }
        mpd_qset_string(&value, s, c.getconst(), &status);
        c.raise(status);
    }

    explicit bigdecimal(const std::string& s, bd_context& c) {
        uint32_t status = 0;
        mpd_qset_string(&value, s.c_str(), c.getconst(), &status);
        c.raise(status);
    }

    /***********************************************************************/
    /*                              Accessors                              */
    /***********************************************************************/
    mpd_t* get() {
        return &value;
    }
    const mpd_t* getconst() const {
        return &value;
    }

    ALWAYS_INLINE int sign() const {
        return mpd_isnegative(&value) ? -1 : 1;
    }

    ALWAYS_INLINE bigdecimal coeff() const {
        if (isspecial()) {
            throw value_error("coefficient is undefined for special values");
        }

        bigdecimal result = *this;
        mpd_set_positive(&result.value);
        result.value.exp = 0;
        return result;
    }

    ALWAYS_INLINE int64_t exponent() const {
        if (isspecial()) {
            throw value_error("exponent is undefined for special values");
        }

        return value.exp;
    }

    ALWAYS_INLINE bigdecimal payload() const {
        if (!isnan()) {
            throw value_error("payload is only defined for NaNs");
        }
        if (value.len == 0) {
            return bigdecimal(0);
        }

        bigdecimal result = *this;
        mpd_set_flags(&result.value, 0);
        result.value.exp = 0;
        return result;
    }

    void set_str(const std::string& s) {
        set_str(s.c_str());
    }

    void set_str(const char* s) {
        mpd_context_t maxcontext;

        uint32_t status = 0;
        mpd_maxcontext(&maxcontext);
        mpd_qset_string(&value, s, &maxcontext, &status);

        if (status & (MPD_Inexact | MPD_Rounded | MPD_Clamped)) {
            /* we want exact results */
            mpd_seterror(&value, MPD_Invalid_operation, &status);
        }
        status &= MPD_Errors;
        context.raise(status);
    }

    /***********************************************************************/
    /*                             Destructor                              */
    /***********************************************************************/
    ~bigdecimal() {
        if (value.data != data)
            mpd_del(&value);
    }

    /***********************************************************************/
    /*                         Assignment operators                        */
    /***********************************************************************/
    ALWAYS_INLINE bigdecimal& operator=(const bigdecimal& other) {
        copy_value(other.getconst(), other.isstatic());
        return *this;
    }

    ALWAYS_INLINE bigdecimal& operator=(bigdecimal&& other) noexcept {
        if (this != &other) {
            move_value(other.getconst(), other.isstatic());
            other.reset();
        }
        return *this;
    };

    ALWAYS_INLINE bigdecimal& operator+=(const bigdecimal& other) {
        return inplace_binary_func_move_ctx(mpd_qadd, other, calc_precision(*this, other));
    }
    ALWAYS_INLINE bigdecimal& operator-=(const bigdecimal& other) {
        return inplace_binary_func(mpd_qsub, other);
    }
    ALWAYS_INLINE bigdecimal& operator*=(const bigdecimal& other) {
        return inplace_binary_func_move_ctx(mpd_qmul, other, calc_precision(*this, other));
    }
    ALWAYS_INLINE bigdecimal& operator/=(const bigdecimal& other) {
        return inplace_binary_func_move_ctx(mpd_qdiv, other, calc_precision(*this, other));
    }
    ALWAYS_INLINE bigdecimal& operator%=(const bigdecimal& other) {
        return inplace_binary_func(mpd_qrem, other);
    }

    /***********************************************************************/
    /*                         Comparison operators                        */
    /***********************************************************************/
    ALWAYS_INLINE bool operator==(const bigdecimal& other) const {
        uint32_t status = 0;
        const int r = mpd_qcmp(getconst(), other.getconst(), &status);
        if (r == INT_MAX) {
            if (issnan() || other.issnan()) {
                context.raise(status);
            }
            return false;
        }
        return r == 0;
    }

    ALWAYS_INLINE bool operator!=(const bigdecimal& other) const {
        uint32_t status = 0;
        const int r = mpd_qcmp(getconst(), other.getconst(), &status);
        if (r == INT_MAX) {
            if (issnan() || other.issnan()) {
                context.raise(status);
            }
            return true;
        }
        return r != 0;
    }

    ALWAYS_INLINE bool operator<(const bigdecimal& other) const {
        uint32_t status = 0;
        const int r = mpd_qcmp(getconst(), other.getconst(), &status);
        if (r == INT_MAX) {
            context.raise(status);
            return false;
        }
        return r < 0;
    }

    ALWAYS_INLINE bool operator<=(const bigdecimal& other) const {
        uint32_t status = 0;
        const int r = mpd_qcmp(getconst(), other.getconst(), &status);
        if (r == INT_MAX) {
            context.raise(status);
            return false;
        }
        return r <= 0;
    }

    ALWAYS_INLINE bool operator>=(const bigdecimal& other) const {
        uint32_t status = 0;
        const int r = mpd_qcmp(getconst(), other.getconst(), &status);
        if (r == INT_MAX) {
            context.raise(status);
            return false;
        }
        return r >= 0;
    }

    ALWAYS_INLINE bool operator>(const bigdecimal& other) const {
        uint32_t status = 0;
        const int r = mpd_qcmp(getconst(), other.getconst(), &status);
        if (r == INT_MAX) {
            context.raise(status);
            return false;
        }
        return r > 0;
    }

    /***********************************************************************/
    /*                      Unary arithmetic operators                     */
    /***********************************************************************/
    ALWAYS_INLINE bigdecimal operator-() const {
        return unary_func(mpd_qminus);
    }
    ALWAYS_INLINE bigdecimal operator+() const {
        return unary_func(mpd_qplus);
    }

    /***********************************************************************/
    /*                      Binary arithmetic operators                    */
    /***********************************************************************/
    ALWAYS_INLINE bigdecimal operator+(const bigdecimal& other) const {
        return binary_func(mpd_qadd, other, calc_precision(*this, other));
    }
    ALWAYS_INLINE bigdecimal operator-(const bigdecimal& other) const {
        return binary_func(mpd_qsub, other, calc_precision(*this, other));
    }
    ALWAYS_INLINE bigdecimal operator*(const bigdecimal& other) const {
        return binary_func(mpd_qmul, other, calc_precision(*this, other));
    }
    ALWAYS_INLINE bigdecimal operator/(const bigdecimal& other) const {
        return binary_func(mpd_qdiv, other, calc_precision(*this, other));
    }
    ALWAYS_INLINE bigdecimal operator%(const bigdecimal& other) const {
        return binary_func(mpd_qrem, other);
    }

    bd_context calc_precision(const bigdecimal& a, const bigdecimal& b) const {
        bd_context maxcontext{
            std::max(a.getconst()->digits, b.getconst()->digits),
            MPD_MAX_EMAX,
            MPD_MIN_EMIN,
            MPD_ROUND_HALF_EVEN,
            MPD_IEEE_Invalid_operation,
            0,
            0};
        return maxcontext;
    }

    /***********************************************************************/
    /*                             Predicates                              */
    /***********************************************************************/
    /* Predicates, no context arg */
    ALWAYS_INLINE bool iscanonical() const {
        return mpd_iscanonical(getconst());
    }
    ALWAYS_INLINE bool isfinite() const {
        return mpd_isfinite(getconst());
    }
    ALWAYS_INLINE bool isinfinite() const {
        return mpd_isinfinite(getconst());
    }
    ALWAYS_INLINE bool isspecial() const {
        return mpd_isspecial(getconst());
    }
    ALWAYS_INLINE bool isnan() const {
        return mpd_isnan(getconst());
    }
    ALWAYS_INLINE bool isqnan() const {
        return mpd_isqnan(getconst());
    }
    ALWAYS_INLINE bool issnan() const {
        return mpd_issnan(getconst());
    }
    ALWAYS_INLINE bool issigned() const {
        return mpd_issigned(getconst());
    }
    ALWAYS_INLINE bool iszero() const {
        return mpd_iszero(getconst());
    }
    ALWAYS_INLINE bool isinteger() const {
        return mpd_isinteger(getconst());
    }

    /* Predicates, optional context arg */
    ALWAYS_INLINE bool isnormal(const bd_context& c = context) const {
        return mpd_isnormal(getconst(), c.getconst());
    }
    ALWAYS_INLINE bool issubnormal(const bd_context& c = context) const {
        return mpd_issubnormal(getconst(), c.getconst());
    }

    /***********************************************************************/
    /*                           Unary functions                           */
    /***********************************************************************/
    /* Unary functions, no context arg */
    ALWAYS_INLINE int64_t adjexp() const {
        if (isspecial()) {
            throw value_error("adjusted exponent undefined for special values");
        }
        return mpd_adjexp(getconst());
    }

    ALWAYS_INLINE bigdecimal canonical() const {
        return *this;
    }
    ALWAYS_INLINE bigdecimal copy() const {
        return unary_func_status(mpd_qcopy);
    }
    ALWAYS_INLINE bigdecimal copy_abs() const {
        return unary_func_status(mpd_qcopy_abs);
    }
    ALWAYS_INLINE bigdecimal copy_negate() const {
        return unary_func_status(mpd_qcopy_negate);
    }

    /* Unary functions, optional context arg */
    ALWAYS_INLINE std::string number_class(bd_context& c = context) const {
        return mpd_class(getconst(), c.getconst());
    }

    ALWAYS_INLINE bigdecimal abs(bd_context& c = context) const {
        return unary_func(mpd_qabs, c);
    }
    ALWAYS_INLINE bigdecimal ceil(bd_context& c = context) const {
        return unary_func(mpd_qceil, c);
    }
    ALWAYS_INLINE bigdecimal exp(bd_context& c = context) const {
        return unary_func(mpd_qexp, c);
    }
    ALWAYS_INLINE bigdecimal floor(bd_context& c = context) const {
        return unary_func(mpd_qfloor, c);
    }
    ALWAYS_INLINE bigdecimal invroot(bd_context& c = context) const {
        return unary_func(mpd_qinvroot, c);
    }
    ALWAYS_INLINE bigdecimal logical_invert(bd_context& c = context) const {
        return unary_func(mpd_qinvert, c);
    }
    ALWAYS_INLINE bigdecimal ln(bd_context& c = context) const {
        return unary_func(mpd_qln, c);
    }
    ALWAYS_INLINE bigdecimal log10(bd_context& c = context) const {
        return unary_func(mpd_qlog10, c);
    }
    ALWAYS_INLINE bigdecimal logb(bd_context& c = context) const {
        return unary_func(mpd_qlogb, c);
    }
    ALWAYS_INLINE bigdecimal minus(bd_context& c = context) const {
        return unary_func(mpd_qminus, c);
    }
    ALWAYS_INLINE bigdecimal next_minus(bd_context& c = context) const {
        return unary_func(mpd_qnext_minus, c);
    }
    ALWAYS_INLINE bigdecimal next_plus(bd_context& c = context) const {
        return unary_func(mpd_qnext_plus, c);
    }
    ALWAYS_INLINE bigdecimal plus(bd_context& c = context) const {
        return unary_func(mpd_qplus, c);
    }
    ALWAYS_INLINE bigdecimal reduce(bd_context& c = context) const {
        return unary_func(mpd_qreduce, c);
    }
    ALWAYS_INLINE bigdecimal to_integral(bd_context& c = context) const {
        return unary_func(mpd_qround_to_int, c);
    }
    ALWAYS_INLINE bigint to_bigint(bd_context& c = context) const {
        return bigint(to_integral(c).format("f"));
    }
    ALWAYS_INLINE bigdecimal to_integral_exact(bd_context& c = context) const {
        return unary_func(mpd_qround_to_intx, c);
    }
    ALWAYS_INLINE bigdecimal sqrt(bd_context& c = context) const {
        return unary_func(mpd_qsqrt, c);
    }
    ALWAYS_INLINE bigdecimal trunc(bd_context& c = context) const {
        return unary_func(mpd_qtrunc, c);
    }

    /***********************************************************************/
    /*                           Binary functions                          */
    /***********************************************************************/
    /* Binary functions, no context arg */
    ALWAYS_INLINE bigdecimal compare_total(const bigdecimal& other) const {
        return binary_func_noctx(mpd_compare_total, other);
    }
    ALWAYS_INLINE bigdecimal compare_total_mag(const bigdecimal& other) const {
        return binary_func_noctx(mpd_compare_total_mag, other);
    }

    /* Binary arithmetic functions, optional context arg */
    ALWAYS_INLINE bigdecimal add(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qadd, other, c);
    }
    ALWAYS_INLINE bigdecimal div(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qdiv, other, c);
    }
    ALWAYS_INLINE bigdecimal divint(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qdivint, other, c);
    }
    ALWAYS_INLINE bigdecimal compare(const bigdecimal& other, bd_context& c = context) const {
        return int_binary_func(mpd_qcompare, other, c);
    }
    ALWAYS_INLINE bigdecimal compare_signal(const bigdecimal& other, bd_context& c = context) const {
        return int_binary_func(mpd_qcompare_signal, other, c);
    }
    ALWAYS_INLINE bigdecimal logical_and(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qand, other, c);
    }
    ALWAYS_INLINE bigdecimal logical_or(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qor, other, c);
    }
    ALWAYS_INLINE bigdecimal logical_xor(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qxor, other, c);
    }
    ALWAYS_INLINE bigdecimal max(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qmax, other, c);
    }
    ALWAYS_INLINE bigdecimal max_mag(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qmax_mag, other, c);
    }
    ALWAYS_INLINE bigdecimal min(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qmin, other, c);
    }
    ALWAYS_INLINE bigdecimal min_mag(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qmin_mag, other, c);
    }
    ALWAYS_INLINE bigdecimal mul(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qmul, other, c);
    }
    ALWAYS_INLINE bigdecimal next_toward(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qnext_toward, other, c);
    }
    ALWAYS_INLINE bigdecimal pow(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qpow, other, c);
    }
    ALWAYS_INLINE bigdecimal quantize(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qquantize, other, c);
    }
    ALWAYS_INLINE bigdecimal rem(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qrem, other, c);
    }
    ALWAYS_INLINE bigdecimal rem_near(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qrem_near, other, c);
    }
    ALWAYS_INLINE bigdecimal rotate(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qrotate, other, c);
    }
    ALWAYS_INLINE bigdecimal scaleb(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qscaleb, other, c);
    }
    ALWAYS_INLINE bigdecimal shift(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qshift, other, c);
    }
    ALWAYS_INLINE bigdecimal sub(const bigdecimal& other, bd_context& c = context) const {
        return binary_func(mpd_qsub, other, c);
    }

    /* Binary arithmetic function, two return values */
    ALWAYS_INLINE std::pair<bigdecimal, bigdecimal> divmod(const bigdecimal& other, bd_context& c = context) const {
        std::pair<bigdecimal, bigdecimal> result;
        uint32_t status = 0;
        mpd_qdivmod(result.first.get(), result.second.get(), getconst(), other.getconst(), c.getconst(), &status);
        c.raise(status);
        return result;
    }

    /***********************************************************************/
    /*                          Ternary functions                          */
    /***********************************************************************/
    ALWAYS_INLINE bigdecimal fma(const bigdecimal& other, const bigdecimal& third, bd_context& c = context) const {
        bigdecimal result;
        uint32_t status = 0;
        mpd_qfma(result.get(), getconst(), other.getconst(), third.getconst(), c.getconst(), &status);
        c.raise(status);
        return result;
    }

    ALWAYS_INLINE bigdecimal powmod(const bigdecimal& other, const bigdecimal& third, bd_context& c = context) const {
        bigdecimal result;
        uint32_t status = 0;
        mpd_qpowmod(result.get(), getconst(), other.getconst(), third.getconst(), c.getconst(), &status);
        c.raise(status);
        return result;
    }

    /***********************************************************************/
    /*                         Irregular functions                         */
    /***********************************************************************/
    ALWAYS_INLINE bigdecimal apply(bd_context& c = context) const {
        bigdecimal result = *this;
        uint32_t status = 0;

        mpd_qfinalize(result.get(), c.getconst(), &status);
        c.raise(status);
        return result;
    }

    ALWAYS_INLINE int cmp(const bigdecimal& other) const {
        uint32_t status = 0;
        return mpd_qcmp(getconst(), other.getconst(), &status);
    }

    ALWAYS_INLINE int cmp_total(const bigdecimal& other) const {
        return mpd_cmp_total(getconst(), other.getconst());
    }

    ALWAYS_INLINE int cmp_total_mag(const bigdecimal& other) const {
        return mpd_cmp_total_mag(getconst(), other.getconst());
    }

    ALWAYS_INLINE bigdecimal copy_sign(const bigdecimal& other) const {
        bigdecimal result;
        uint32_t status = 0;
        if (!mpd_qcopy_sign(result.get(), getconst(), other.getconst(), &status)) {
            throw malloc_error("out of memory");
        }
        return result;
    }

    ALWAYS_INLINE bigdecimal rescale(const int64_t exp, bd_context& c = context) const {
        bigdecimal result;
        uint32_t status = 0;
        mpd_ssize_t xexp = bigmath::safe_downcast<mpd_ssize_t, int64_t>(exp);
        mpd_qrescale(result.get(), getconst(), xexp, c.getconst(), &status);
        c.raise(status);
        return result;
    }

    ALWAYS_INLINE bool same_quantum(const bigdecimal& other) const {
        return mpd_same_quantum(getconst(), other.getconst());
    }

    ALWAYS_INLINE bigdecimal shiftn(const int64_t n, bd_context& c = context) const {
        bigdecimal result;
        uint32_t status = 0;
        mpd_ssize_t nn = bigmath::safe_downcast<mpd_ssize_t, int64_t>(n);
        mpd_qshiftn(result.get(), getconst(), nn, c.getconst(), &status);
        c.raise(status);
        return result;
    }

    ALWAYS_INLINE bigdecimal shiftl(const int64_t n, bd_context& c = context) const {
        bigdecimal result;
        uint32_t status = 0;
        mpd_ssize_t nn = bigmath::safe_downcast<mpd_ssize_t, int64_t>(n);
        mpd_qshiftl(result.get(), getconst(), nn, &status);
        c.raise(status);
        return result;
    }

    ALWAYS_INLINE bigdecimal shiftr(const int64_t n, bd_context& c = context) const {
        bigdecimal result;
        uint32_t status = 0;
        mpd_ssize_t nn = bigmath::safe_downcast<mpd_ssize_t, int64_t>(n);
        mpd_qshiftr(result.get(), getconst(), nn, &status);
        c.raise(status);
        return result;
    }

    static bigdecimal exact(const char* const s, bd_context& c);
    static bigdecimal exact(const std::string& s, bd_context& c);
    static bigdecimal ln10(int64_t n, bd_context& c = context);
    static int32_t radix();

    /***********************************************************************/
    /*                          Integer conversion                         */
    /***********************************************************************/
    ALWAYS_INLINE int64_t i64() const {
        uint32_t status = 0;
        const int64_t result = mpd_qget_i64(getconst(), &status);
        if (status) {
            throw value_error("cannot convert to int64_t");
        }
        return result;
    }

    ALWAYS_INLINE int32_t i32() const {
        uint32_t status = 0;
        const int32_t result = mpd_qget_i32(getconst(), &status);
        if (status) {
            throw value_error("cannot convert to int32_t");
        }
        return result;
    }

    ALWAYS_INLINE uint64_t u64() const {
        uint32_t status = 0;
        const uint64_t result = mpd_qget_u64(getconst(), &status);
        if (status) {
            throw value_error("cannot convert to uint64_t");
        }
        return result;
    }

    ALWAYS_INLINE uint32_t u32() const {
        uint32_t status = 0;
        const uint32_t result = mpd_qget_u32(getconst(), &status);
        if (status) {
            throw value_error("cannot convert to uint32_t");
        }
        return result;
    }

    /***********************************************************************/
    /*                          String conversion                          */
    /***********************************************************************/
    /* String representations */
    std::string repr(bool capitals = true) const;

    inline std::string to_sci(bool capitals = true) const {
        const char* cp = mpd_to_sci(getconst(), capitals);
        if (cp == NULL) {
            throw malloc_error("out of memory");
        }
        return string_from_cp(cp);
    }

    inline std::string to_eng(bool capitals = true) const {
        const char* cp = mpd_to_eng(getconst(), capitals);
        if (cp == NULL) {
            throw malloc_error("out of memory");
        }
        return string_from_cp(cp);
    }

    /// \brief Format output by Python formatting standard
    /// \param fmt
    /// \see https://www.python.org/dev/peps/pep-3101/
    /// \param c context, by default getting static
    /// \return
    inline std::string format(const char* fmt, const bd_context& c = context) const {
        uint32_t status = 0;
        mpd_context_t ctx;

        if (fmt == NULL) {
            throw value_error("bigdecimal.format: fmt argument is NULL");
        }

        mpd_maxcontext(&ctx);
        ctx.round = c.getconst()->round;
        ctx.traps = 0;

        const char* cp = mpd_qformat(getconst(), fmt, &ctx, &status);
        if (cp == NULL) {
            if (status & MPD_Malloc_error) {
                throw malloc_error("out of memory");
            } else if (status & MPD_Invalid_operation) {
                throw value_error("invalid format string");
            } else {
                throw runtime_error("internal error: unexpected status");
            }
        }
        return string_from_cp(cp);
    }

    inline std::string format(const std::string& s, const bd_context& c = context) const {
        return format(s.c_str(), c);
    }

    friend std::ostream& operator<<(std::ostream& os, const bigdecimal& self);
};

/***********************************************************************/
/*                      Reverse comparison operators                   */
/***********************************************************************/
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bool operator==(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) == self;
}
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bool operator!=(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) != self;
}
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bool operator<(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) < self;
}
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bool operator<=(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) <= self;
}
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bool operator>=(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) >= self;
}
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bool operator>(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) > self;
}

/***********************************************************************/
/*                      Reverse arithmetic operators                   */
/***********************************************************************/
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bigdecimal operator+(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) + self;
}
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bigdecimal operator-(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) - self;
}
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bigdecimal operator*(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) * self;
}
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bigdecimal operator/(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) / self;
}
ENABLE_IF_INTEGRAL(T)
ALWAYS_INLINE bigdecimal operator%(const T& other, const bigdecimal& self) {
    ASSERT_INTEGRAL(T);
    return bigdecimal(other) % self;
}

#undef INT64_SUBSET
#undef UINT64_SUBSET
#undef ENABLE_IF_SIGNED
#undef ENABLE_IF_UNSIGNED
#undef ENABLE_IF_INTEGRAL
#undef ASSERT_SIGNED
#undef ASSERT_UNSIGNED
#undef ASSERT_INTEGRAL
} // namespace bigmath

#endif // BIGMATHPP_MPDECIMAL_H
