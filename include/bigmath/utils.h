/*!
 * bigmath.
 * utils.h
 *
 * \date 09/15/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */
#ifndef BIGMATHPP_UTILS_H
#define BIGMATHPP_UTILS_H

#include "errors.h"
#include "mpdecimal_backport.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>

/******************************************************************************/
/*                                    Util                                    */
/******************************************************************************/

namespace bigmath {

inline size_t digits2bits(uint32_t d) {
    const double LOG2_10 = 3.3219280948873624;

    return size_t(std::ceil(d * LOG2_10));
}

inline uint32_t bits2digits(size_t b) {
    const double LOG10_2 = 0.30102999566398119;

    return int(std::floor(b * LOG10_2));
}

template<typename dest_t, typename src_t>
inline dest_t safe_downcast(src_t v) {
    if (v < std::numeric_limits<dest_t>::min() ||
        v > std::numeric_limits<dest_t>::max()) {
        throw bigmath::value_error();
    }

    return static_cast<dest_t>(v);
}

inline std::shared_ptr<const char> shared_cp(const char* cp) {
    if (cp == nullptr) {
        throw runtime_error("invalid nullptr argument");
    }

    return std::shared_ptr<const char>(cp, [](const char* s) { mpd_free(const_cast<char*>(s)); });
}

inline std::string string_from_cp(const char* cp) {
    const auto p = shared_cp(cp);
    return std::string(p.get());
}

template<typename T>
struct int64_compat {
#define INT64_SUBSET(T) \
    (INT64_MIN <= std::numeric_limits<T>::min() && std::numeric_limits<T>::max() <= INT64_MAX)

    static const bool value = std::is_same<T, int8_t>::value ||
                              std::is_same<T, int16_t>::value ||
                              std::is_same<T, int32_t>::value ||
                              std::is_same<T, int64_t>::value ||
                              (std::is_same<T, signed char>::value && INT64_SUBSET(signed char)) ||
                              (std::is_same<T, short>::value && INT64_SUBSET(short)) ||
                              (std::is_same<T, int>::value && INT64_SUBSET(int)) ||
                              (std::is_same<T, long>::value && INT64_SUBSET(long)) ||
                              (std::is_same<T, long long>::value && INT64_SUBSET(long long));
};

template<typename T>
struct uint64_compat {
#define UINT64_SUBSET(T) (std::numeric_limits<T>::max() <= UINT64_MAX)

    static const bool value = std::is_same<T, uint8_t>::value ||
                              std::is_same<T, uint16_t>::value ||
                              std::is_same<T, uint32_t>::value ||
                              std::is_same<T, uint64_t>::value ||
                              (std::is_same<T, unsigned char>::value && UINT64_SUBSET(unsigned char)) ||
                              (std::is_same<T, unsigned short>::value && UINT64_SUBSET(unsigned short)) ||
                              (std::is_same<T, unsigned int>::value && UINT64_SUBSET(unsigned int)) ||
                              (std::is_same<T, unsigned long>::value && UINT64_SUBSET(unsigned long)) ||
                              (std::is_same<T, unsigned long long>::value && UINT64_SUBSET(unsigned long long));
};

#define ENABLE_IF_SIGNED(T) \
    template<typename T,    \
             typename = typename std::enable_if<bigmath::int64_compat<T>::value>::type>

#define ENABLE_IF_UNSIGNED(T) \
    template<typename T,      \
             typename = typename std::enable_if<bigmath::uint64_compat<T>::value>::type, typename = void>

#define ENABLE_IF_CONVERTIBLE(T)                \
    template<                                   \
        typename T,                             \
        typename = typename std::enable_if<     \
            bigmath::uint64_compat<T>::value || \
            bigmath::int64_compat<T>::value ||  \
            std::is_enum<T>::value>::type,      \
        typename = void>

#define ENABLE_IF_INTEGRAL(T) \
    template<typename T,      \
             typename = typename std::enable_if<bigmath::int64_compat<T>::value || bigmath::uint64_compat<T>::value>::type>

#define ASSERT_SIGNED(T)                           \
    static_assert(bigmath::int64_compat<T>::value, \
                  "internal error: selected type is not int64 compatible")

#define ASSERT_UNSIGNED(T)                          \
    static_assert(bigmath::uint64_compat<T>::value, \
                  "internal error: selected type is not uint64 compatible")

#define ASSERT_INTEGRAL(T)                                                             \
    static_assert(bigmath::int64_compat<T>::value || bigmath::uint64_compat<T>::value, \
                  "internal error: selected type is not a suitable integer type")

#define ASSERT_CONVERTIBLE(T)                            \
    static_assert(bigmath::uint64_compat<T>::value ||    \
                      bigmath::int64_compat<T>::value || \
                      std::is_enum<T>::value,            \
                  "Unknown value")

} // namespace bigmath

#endif // BIGMATHPP_UTILS_H
