/*!
 * bigmath.
 * errors.h
 *
 * \date 09/15/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */
#ifndef BIGMATHPP_ERRORS_H
#define BIGMATHPP_ERRORS_H

#include "mpdecimal_backport.h"

#include <exception>
#include <string>

namespace bigmath {

/******************************************************************************/
/*                          Constants from libmpdec                           */
/******************************************************************************/

enum round {
    ROUND_UP = MPD_ROUND_UP,               /* round away from 0               */
    ROUND_DOWN = MPD_ROUND_DOWN,           /* round toward 0 (truncate)       */
    ROUND_CEILING = MPD_ROUND_CEILING,     /* round toward +infinity          */
    ROUND_FLOOR = MPD_ROUND_FLOOR,         /* round toward -infinity          */
    ROUND_HALF_UP = MPD_ROUND_HALF_UP,     /* 0.5 is rounded up               */
    ROUND_HALF_DOWN = MPD_ROUND_HALF_DOWN, /* 0.5 is rounded down             */
    ROUND_HALF_EVEN = MPD_ROUND_HALF_EVEN, /* 0.5 is rounded to even          */
    ROUND_05UP = MPD_ROUND_05UP,           /* round zero or five away from 0  */
    ROUND_TRUNC = MPD_ROUND_TRUNC,         /* truncate, but set infinity      */
    ROUND_GUARD = MPD_ROUND_GUARD
};

/*
 * Aliases for a spelling that is consistent with the exceptions below.
 * Use whichever form you prefer.
 */
constexpr uint32_t DecClamped = MPD_Clamped;
constexpr uint32_t DecConversionSyntax = MPD_Conversion_syntax;
constexpr uint32_t DecDivisionByZero = MPD_Division_by_zero;
constexpr uint32_t DecDivisionImpossible = MPD_Division_impossible;
constexpr uint32_t DecDivisionUndefined = MPD_Division_undefined;
constexpr uint32_t DecFpuError = MPD_Fpu_error; /* unused */
constexpr uint32_t DecInexact = MPD_Inexact;
constexpr uint32_t DecInvalidContext = MPD_Invalid_context;
constexpr uint32_t DecInvalidOperation = MPD_Invalid_operation;
constexpr uint32_t DecMallocError = MPD_Malloc_error;
constexpr uint32_t DecNotImplemented = MPD_Not_implemented; /* unused */
constexpr uint32_t DecOverflow = MPD_Overflow;
constexpr uint32_t DecRounded = MPD_Rounded;
constexpr uint32_t DecSubnormal = MPD_Subnormal;
constexpr uint32_t DecUnderflow = MPD_Underflow;

/* Flag sets */
constexpr uint32_t DecIEEEInvalidOperation = MPD_IEEE_Invalid_operation;
/* DecConversionSyntax */
/* DecDivisionImpossible */
/* DecDivisionUndefined */
/* DecFpuError */
/* DecInvalidContext */
/* DecInvalidOperation */
/* DecMallocError */

constexpr uint32_t DecErrors = MPD_Errors;
/* DecIEEEInvalidOperation */
/* DecDivisionByZero */

constexpr uint32_t DecMaxStatus = MPD_Max_status;
/* All flags */

/* IEEEContext(): common arguments */
constexpr int DECIMAL32 = MPD_DECIMAL32;
constexpr int DECIMAL64 = MPD_DECIMAL64;
constexpr int DECIMAL128 = MPD_DECIMAL128;

/* IEEEContext(): maximum argument value */
constexpr int IEEE_CONTEXT_MAX_BITS = MPD_IEEE_CONTEXT_MAX_BITS;

/******************************************************************************/
/*                             Decimal Exceptions                             */
/******************************************************************************/

class bigdecimal_exception : public std::exception {
private:
    std::string m_msg;

public:
    bigdecimal_exception()
        : m_msg("") {
    }
    explicit bigdecimal_exception(const std::string& msg)
        : m_msg(msg) {
    }

    virtual const char* what() const throw() {
        return m_msg.c_str();
    }
};

/* Signals */
class IEEE_invalid_operation : public bigdecimal_exception {
    using bigdecimal_exception::bigdecimal_exception;
};

class division_by_zero : public bigdecimal_exception {
    using bigdecimal_exception::bigdecimal_exception;
};

class overflow_error : public bigdecimal_exception {
    using bigdecimal_exception::bigdecimal_exception;
};

class underflow_error : public bigdecimal_exception {
    using bigdecimal_exception::bigdecimal_exception;
};

class subnormal_error : public bigdecimal_exception {
    using bigdecimal_exception::bigdecimal_exception;
};

class inexact_error : public bigdecimal_exception {
    using bigdecimal_exception::bigdecimal_exception;
};

class rounder_error : public bigdecimal_exception {
    using bigdecimal_exception::bigdecimal_exception;
};

class clamped_error : public bigdecimal_exception {
    using bigdecimal_exception::bigdecimal_exception;
};

/* Conditions */
class invalid_operation_error : public IEEE_invalid_operation {
    using IEEE_invalid_operation::IEEE_invalid_operation;
};

class conversion_syntax_error : public IEEE_invalid_operation {
    using IEEE_invalid_operation::IEEE_invalid_operation;
};

class division_impossible_error : public IEEE_invalid_operation {
    using IEEE_invalid_operation::IEEE_invalid_operation;
};

class division_undefined_error : public IEEE_invalid_operation {
    using IEEE_invalid_operation::IEEE_invalid_operation;
};

/******************************************************************************/
/*                               Other Exceptions                             */
/******************************************************************************/

class value_error : public std::exception {
private:
    std::string m_msg;

public:
    value_error()
        : m_msg("") {
    }
    explicit value_error(const std::string& msg)
        : m_msg(msg) {
    }

    virtual const char* what() const throw() {
        return m_msg.c_str();
    }
};

class runtime_error : public std::exception {
private:
    std::string m_msg;

public:
    runtime_error()
        : m_msg("") {
    }
    explicit runtime_error(const std::string& msg)
        : m_msg(msg) {
    }

    virtual const char* what() const throw() {
        return m_msg.c_str();
    }
};

class malloc_error : public std::exception {
private:
    std::string m_msg;

public:
    malloc_error()
        : m_msg("") {
    }
    explicit malloc_error(const std::string& msg)
        : m_msg(msg) {
    }

    virtual const char* what() const throw() {
        return m_msg.c_str();
    }
};

} // namespace bigmath

#endif //BIGMATHPP_ERRORS_H
