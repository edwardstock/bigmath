/*!
 * bigmath.
 * bigint.h
 *
 * \date 09/13/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */
#ifndef BIGMATHPP_BIGINT_H
#define BIGMATHPP_BIGINT_H

#include "bigmath_config.h"
#include "utils.h"

#include <string>
#include <vector>

#if defined(HAVE_GMP)
#include <gmpxx.h>
#elif defined(HAVE_MPIR)
#include <mpirxx.h>
#else
#error "GMP or MPIR required to build bigint"
#endif

namespace bigmath {

class bigdecimal;

class BIGMATHPP_API bigint {
private:
    mpz_class m_val;
    int32_t m_base = 10;

public:
    bigint();
    explicit bigint(const std::string& val);
    bigint(const std::string& val, int32_t radix);
    bigint(const mpz_class& other, int32_t radix = 10);
    bigint(const std::vector<uint8_t>& bytes);
    bigint(const bigint& other);
    bigint(const bigdecimal& other);
    bigint(bigint&& other) noexcept;

    ENABLE_IF_CONVERTIBLE(T)
    bigint(const T& other) {
        ASSERT_CONVERTIBLE(T);
        if (bigmath::int64_compat<T>::value) {
            mpz_set_si(m_val.get_mpz_t(), (signed long int) other);
        } else if (bigmath::uint64_compat<T>::value) {
            mpz_set_ui(m_val.get_mpz_t(), (unsigned long int) other);
        } else if (std::is_enum<T>::value) {
            mpz_set_ui(m_val.get_mpz_t(), static_cast<unsigned long int>(other));
        }
    }

    bigint& operator=(const bigint& other) noexcept {
        m_val = other.m_val;
        m_base = other.m_base;
        return *this;
    }

    bigint& operator=(bigint&& other) noexcept {
        m_val = std::move(other.m_val);
        m_base = other.m_base;
        return *this;
    }

    operator std::vector<uint8_t>() const {
        return export_bytes();
    }

    operator bool() const noexcept {
        return mpz_get_si(m_val.get_mpz_t()) != 0;
    }

    operator int8_t() const {
        return int8_t(mpz_get_si(m_val.get_mpz_t()));
    }

    operator uint8_t() const {
        return uint8_t(mpz_get_ui(m_val.get_mpz_t()));
    }

    operator int16_t() const {
        return int16_t(mpz_get_si(m_val.get_mpz_t()));
    }

    operator uint16_t() const {
        return uint16_t(mpz_get_ui(m_val.get_mpz_t()));
    }

    operator int32_t() const {
        return uint32_t(mpz_get_si(m_val.get_mpz_t()));
    }

    operator uint32_t() const {
        return uint32_t(mpz_get_ui(m_val.get_mpz_t()));
    }

    operator int64_t() const {
        return int64_t(mpz_get_si(m_val.get_mpz_t()));
    }

    operator uint64_t() const {
        return uint64_t(mpz_get_ui(m_val.get_mpz_t()));
    }

    bigint& operator++() noexcept {
        ++m_val;
        return *this;
    }

    bigint operator++(int) noexcept {
        m_val++;
        return *this - bigint("1");
    }

    bigint& operator--() noexcept {
        --m_val;
        return *this;
    }

    bigint operator--(int) noexcept {
        m_val--;
        return *this + bigint("1");
    }

    bigint operator<<(int bits) const {
        return operator<<(mp_bitcnt_t(bits));
    }

    bigint operator<<(mp_bitcnt_t bits) const {
        bigint out(m_val, m_base);
        mpz_mul_2exp(out.m_val.get_mpz_t(), out.m_val.get_mpz_t(), bits);
        return out;
    }

    bigint& operator<<=(int bits) {
        return operator<<=(mp_bitcnt_t(bits));
    }

    bigint& operator<<=(mp_bitcnt_t bits) {
        mpz_mul_2exp(m_val.get_mpz_t(), m_val.get_mpz_t(), bits);
        return *this;
    }

    bigint operator>>(int bits) const {
        return operator>>(mp_bitcnt_t(bits));
    }

    bigint operator>>(mp_bitcnt_t bits) const {
        bigint out(m_val, m_base);
        mpz_tdiv_q_2exp(out.m_val.get_mpz_t(), out.m_val.get_mpz_t(), bits);
        return out;
    }

    bigint& operator>>=(int bits) {
        return operator>>=(mp_bitcnt_t(bits));
    }

    bigint& operator>>=(mp_bitcnt_t bits) {
        mpz_tdiv_q_2exp(m_val.get_mpz_t(), m_val.get_mpz_t(), bits);
        return *this;
    }

    ENABLE_IF_CONVERTIBLE(T)
    bigint& operator|=(const T& other) {
        ASSERT_CONVERTIBLE(T);
        auto tmp = bigint(other);
        mpz_ior(m_val.get_mpz_t(), m_val.get_mpz_t(), tmp.m_val.get_mpz_t());
        return *this;
    }

    ENABLE_IF_CONVERTIBLE(T)
    bigint operator|(const T& other) {
        ASSERT_CONVERTIBLE(T);
        bigint out(*this);
        auto tmp = bigint(other);
        mpz_ior(out.m_val.get_mpz_t(), out.m_val.get_mpz_t(), tmp.m_val.get_mpz_t());
        return out;
    }

    bigint& operator|=(const bigint& other) {
        mpz_ior(m_val.get_mpz_t(), m_val.get_mpz_t(), other.m_val.get_mpz_t());
        return *this;
    }

    bigint operator|(const bigint& other) {
        bigint out(*this);
        mpz_ior(out.m_val.get_mpz_t(), out.m_val.get_mpz_t(), other.m_val.get_mpz_t());
        return out;
    }

    ENABLE_IF_CONVERTIBLE(T)
    bigint& operator&=(const T& other) {
        ASSERT_CONVERTIBLE(T);
        auto tmp = bigint(other);
        mpz_and(m_val.get_mpz_t(), m_val.get_mpz_t(), tmp.m_val.get_mpz_t());
        return *this;
    }

    ENABLE_IF_CONVERTIBLE(T)
    bigint operator&(const T& other) {
        ASSERT_CONVERTIBLE(T);
        bigint out(*this);
        auto tmp = bigint(other);
        mpz_and(out.m_val.get_mpz_t(), out.m_val.get_mpz_t(), tmp.m_val.get_mpz_t());
        return out;
    }

    bigint& operator&=(const bigint& other) {
        mpz_and(m_val.get_mpz_t(), m_val.get_mpz_t(), other.m_val.get_mpz_t());
        return *this;
    }

    bigint operator&(const bigint& other) {
        bigint out(*this);
        mpz_and(out.m_val.get_mpz_t(), out.m_val.get_mpz_t(), other.m_val.get_mpz_t());
        return out;
    }

    bigint operator+(const bigint& other) const {
        bigint out(m_val);
        out.m_val += other.m_val;
        return out;
    }

    ENABLE_IF_CONVERTIBLE(T)
    bigint& operator+=(const T& other) {
        return *this += bigint(other);
    }

    bigint& operator+=(const bigint& other) {

        m_val += other.m_val;
        return *this;
    }

    bigint operator-(const bigint& other) const {
        bigint out(m_val);
        out.m_val -= other.m_val;
        return out;
    }

    ENABLE_IF_CONVERTIBLE(T)
    bigint& operator-=(const T& other) {
        return *this -= bigint(other);
    }

    bigint& operator-=(const bigint& other) {
        m_val -= other.m_val;
        return *this;
    }

    bigint operator%(const bigint& other) const {
        bigint out(m_val);
        out.m_val %= other.m_val;
        return out;
    }

    ENABLE_IF_CONVERTIBLE(T)
    bigint& operator%=(const T& other) {
        return *this %= bigint(other);
    }

    bigint& operator%=(const bigint& other) {
        m_val %= other.m_val;
        return *this;
    }

    bigint operator*(const bigint& other) const {
        bigint out(m_val);
        out.m_val *= other.m_val;
        return out;
    }

    ENABLE_IF_CONVERTIBLE(T)
    bigint& operator*=(const T& other) {
        return *this *= bigint(other);
    }

    bigint& operator*=(const bigint& other) {
        m_val *= other.m_val;
        return *this;
    }

    bigint operator/(const bigint& other) const {
        bigint out(m_val);
        out.m_val /= other.m_val;
        return out;
    }

    ENABLE_IF_CONVERTIBLE(T)
    bigint& operator/=(const T& other) {
        return *this /= bigint(other);
    }

    bigint& operator/=(const bigint& other) {
        m_val /= other.m_val;
        return *this;
    }

    bool operator>(const bigint& other) const {
        return m_val > other.m_val;
    }

    bool operator>=(const bigint& other) const {
        return m_val >= other.m_val;
    }

    bool operator<(const bigint& other) const {
        return m_val < other.m_val;
    }

    bool operator<=(const bigint& other) const {
        return m_val <= other.m_val;
    }

    bool operator==(const bigint& other) const {
        return m_val == other.m_val;
    }

    bool operator!=(const bigint& other) const {
        return m_val != other.m_val;
    }

    /***********************************************************************/
    /*                      Comparison operators                   */
    /***********************************************************************/
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bool operator==(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return bigint(other) == (*this);
    }
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bool operator!=(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return bigint(other) != (*this);
    }
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bool operator<(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return (*this) < bigint(other);
    }
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bool operator<=(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return (*this) >= bigint(other);
    }
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bool operator>=(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return (*this) >= bigint(other);
    }
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bool operator>(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return (*this) > bigint(other);
    }

    /***********************************************************************/
    /*                      Arithmetic operators                   */
    /***********************************************************************/
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bigint operator+(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return bigint(other) + (*this);
    }
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bigint operator-(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return bigint(other) - (*this);
    }
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bigint operator*(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return bigint(other) * (*this);
    }
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bigint operator/(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return bigint(other) / (*this);
    }
    ENABLE_IF_CONVERTIBLE(T)
    ALWAYS_INLINE bigint operator%(const T& other) {
        ASSERT_CONVERTIBLE(T);
        return bigint(other) % (*this);
    }

    int32_t get_radix() const;

    mp_bitcnt_t get_precision() const;

    std::string str() const;

    std::vector<uint8_t> export_bytes() const;

    void import_bytes(const std::vector<uint8_t>& input);

    friend std::ostream& operator<<(std::ostream& os, const bigint& val) {
        os << val.str();
        return os;
    }
};

BIGMATHPP_API extern const bigint ZERO;
BIGMATHPP_API extern const bigint ONE;
BIGMATHPP_API extern const bigint TWO;
BIGMATHPP_API extern const bigint THREE;

/***********************************************************************/
/*                      Reverse comparison operators                   */
/***********************************************************************/
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bool operator==(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) == self;
}
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bool operator!=(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) != self;
}
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bool operator<(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) < self;
}
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bool operator<=(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) <= self;
}
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bool operator>=(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) >= self;
}
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bool operator>(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) > self;
}

/***********************************************************************/
/*                      Reverse arithmetic operators                   */
/***********************************************************************/
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bigint operator+(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) + self;
}
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bigint operator-(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) - self;
}
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bigint operator*(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) * self;
}
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bigint operator/(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) / self;
}
ENABLE_IF_CONVERTIBLE(T)
ALWAYS_INLINE bigint operator%(const T& other, const bigint& self) {
    ASSERT_CONVERTIBLE(T);
    return bigint(other) % self;
}

} // namespace bigmath

#endif // BIGMATHPP_BIGINT_H
