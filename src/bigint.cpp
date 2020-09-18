/*!
 * trash.
 * bigint.cpp
 *
 * \date 09/13/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */
#include "bigmath/bigint.h"

#include "bigmath/bigdecimal.h"

const bigmath::bigint bigmath::ZERO = bigmath::bigint("0");
const bigmath::bigint bigmath::ONE = bigmath::bigint("1");
const bigmath::bigint bigmath::TWO = bigmath::bigint("2");
const bigmath::bigint bigmath::THREE = bigmath::bigint("3");

bigmath::bigint::bigint()
    : m_val(), m_base(10) {
    m_val.set_str("0", 10);
}

bigmath::bigint::bigint(const std::string& val)
    : m_val(), m_base(10) {
    m_val.set_str(val, 10);
}
bigmath::bigint::bigint(const std::string& val, int32_t radix)
    : m_val(), m_base(radix) {
    m_val.set_str(val, radix);
}

bigmath::bigint::bigint(const std::vector<uint8_t>& bytes)
    : m_val() {
    import_bytes(bytes);
}
bigmath::bigint::bigint(const mpz_class& other, int32_t radix)
    : m_val(other),
      m_base(radix) {
}
bigmath::bigint::bigint(const bigmath::bigint& other) {
    m_val = other.m_val;
    m_base = other.m_base;
}

bigmath::bigint::bigint(const bigmath::bigdecimal& other) {
    *this = other.to_bigint();
}

bigmath::bigint::bigint(bigmath::bigint&& other) noexcept {
    m_val = std::move(other.m_val);
    m_base = other.m_base;
}
int32_t bigmath::bigint::get_radix() const {
    return m_base;
}
mp_bitcnt_t bigmath::bigint::get_precision() const {
    return m_val.get_prec();
}
std::string bigmath::bigint::str() const {
    return m_val.get_str(m_base);
}
std::vector<uint8_t> bigmath::bigint::export_bytes() const {
    std::vector<uint8_t> data;
    data.resize(get_precision());
    size_t count = 0;
    mpz_export(data.data(), &count, 1, sizeof(uint8_t), 1, 0, m_val.get_mpz_t());

    std::vector<uint8_t> out;
    out.resize(count);
    for (size_t i = 0; i < count; i++) {
        out[i] = data[i];
    }
    if (count == 0) {
        out.resize(1);
        out[0] = (uint8_t) 0x00;
    }

    return out;
}
void bigmath::bigint::import_bytes(const std::vector<uint8_t>& input) {
    mpz_import(m_val.get_mpz_t(), input.size(), 1, sizeof(uint8_t), 1, 0, input.data());
}
