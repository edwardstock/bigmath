/*!
 * bigmath.
 * bigint_test.cpp
 *
 * \date 09/13/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */

#include "bigmath/bigdecimal.h"

#include <bigmath/bigint.h>
#include <gtest/gtest.h>

using namespace bigmath;

TEST(BigInt, DefaultEqualsToZero) {
    bigint v;

    ASSERT_EQ(bigint("0"), v);
}

TEST(BigInt, FromFloatString) {
    bigint v("100.500");
    ASSERT_EQ(bigint("0"), v);

    v = bigint("100");
    ASSERT_EQ(bigint("100"), v);
}

TEST(BigInt, Add) {
    bigint a("100");
    uint32_t b = 200;

    bigint a2("100");
    bigint b2(200);

    a += b;
    ASSERT_EQ(bigint("300"), a);

    a2 += b2;
    ASSERT_EQ(bigint("300"), a);

    bigint one("1");
    bigint two = one + bigint("1");
    ASSERT_EQ(bigint("2"), two);

    one += bigint("1");
    ASSERT_EQ(bigint("2"), one);
    int nv = 2;

    one++;
    nv++;
    ASSERT_EQ(bigint("3"), one);
    ASSERT_EQ(3, nv);

    ++one;
    ++nv;
    ASSERT_EQ(bigint("4"), one);
    ASSERT_EQ(4, nv);

    ASSERT_EQ(bigint("4"), one++);
    ASSERT_EQ(4, nv++);
    ASSERT_EQ(bigint("5"), one);
    ASSERT_EQ(5, nv);
}

TEST(BigInt, Sub) {
    bigint ten("10");
    bigint res = ten - bigint("1");
    ASSERT_EQ(bigint("9"), res);

    ten -= bigint("1");
    ASSERT_EQ(bigint("9"), ten);
    int nv = 9;

    ten--;
    nv--;
    ASSERT_EQ(bigint("8"), ten);
    ASSERT_EQ(8, nv);

    --ten;
    --nv;
    ASSERT_EQ(bigint("7"), ten);
    ASSERT_EQ(7, nv);

    ASSERT_EQ(bigint("7"), ten--);
    ASSERT_EQ(7, nv--);
    ASSERT_EQ(bigint("6"), ten);
    ASSERT_EQ(6, nv);
}

TEST(BigInt, Comparing) {
    bigint value("10");
    uint8_t cmp = 1;
    int8_t cmp2 = 2;
    uint64_t cmp3 = 325;

    ASSERT_TRUE(value > cmp);
    ASSERT_TRUE(value >= cmp);
    ASSERT_TRUE(cmp < value);
    ASSERT_TRUE(cmp <= value);
    ASSERT_TRUE(value > cmp2);
    ASSERT_TRUE(value >= cmp2);
    ASSERT_TRUE(cmp2 < value);
    ASSERT_TRUE(cmp2 <= value);
    ASSERT_FALSE(value > cmp3);
    ASSERT_FALSE(value >= cmp3);
    ASSERT_FALSE(cmp3 < value);
    ASSERT_FALSE(cmp3 <= value);
}

TEST(BigInt, CopyAssignment) {
    bigint v1("100");
    bigint v2 = v1;

    std::cout << v2 << std::endl;
    std::cout << v1 << std::endl;
}

std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    std::vector<uint8_t> bytes;

    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        auto byte = (uint8_t) strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }

    return bytes;
}

std::string bytes_to_hex(const uint8_t* data, size_t len) {
    std::stringstream ss;
    ss << std::hex;
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << std::setfill('0') << (int) data[i];
    }

    return ss.str();
}

TEST(BigInt, ExportBytes) {
    bigint v0("0");
    bigint v1("100");
    bigint v2("66634859979403239086722692970");
    bigint v3("1000000000000000000");
    bigint v4("255");
    bigint v5("256");
    bigint v6("127");
    bigint v7("128");

    std::vector<uint8_t> res0 = v0.export_bytes();
    std::vector<uint8_t> res1 = v1.export_bytes();
    std::vector<uint8_t> res2 = v2.export_bytes();
    std::vector<uint8_t> res3 = v3.export_bytes();
    std::vector<uint8_t> res4 = v4.export_bytes();
    std::vector<uint8_t> res5 = v5.export_bytes();
    std::vector<uint8_t> res6 = v6.export_bytes();
    std::vector<uint8_t> res7 = v7.export_bytes();

    const std::string hex0 = bytes_to_hex(&res0[0], res0.size());
    const std::string hex1 = bytes_to_hex(&res1[0], res1.size());
    const std::string hex2 = bytes_to_hex(&res2[0], res2.size());
    const std::string hex3 = bytes_to_hex(&res3[0], res3.size());
    const std::string hex4 = bytes_to_hex(&res4[0], res4.size());
    const std::string hex5 = bytes_to_hex(&res5[0], res5.size());
    const std::string hex6 = bytes_to_hex(&res6[0], res6.size());
    const std::string hex7 = bytes_to_hex(&res7[0], res7.size());

    ASSERT_STREQ("00", hex0.c_str());
    ASSERT_STREQ("64", hex1.c_str());
    ASSERT_STREQ("d74f1075a595f3ffcbf4b76a", hex2.c_str());
    ASSERT_STREQ("0de0b6b3a7640000", hex3.c_str());
    ASSERT_STREQ("ff", hex4.c_str());
    ASSERT_STREQ("0100", hex5.c_str());
    ASSERT_STREQ("7f", hex6.c_str());
    ASSERT_STREQ("80", hex7.c_str());

    //0: 00
    //100: 64
    //66634859979403239086722692970: d74f1075a595f3ffcbf4b76a
    //1000000000000000000: 0de0b6b3a7640000
    //255: ff
    //256: 0100
    //127: 7f
    //128: 80
}

TEST(BigInt, ImportBytes) {
    std::vector<bigint> values = {
        bigint("0"),
        bigint("100"),
        bigint("66634859979403239086722692970"),
        bigint("1000000000000000000"),
        bigint("255"),
        bigint("256"),
        bigint("127"),
        bigint("128"),
    };

    std::vector<std::vector<uint8_t>> bytes_results;
    for (const auto& v : values) {
        bytes_results.push_back(v.export_bytes());
    }

    std::vector<bigint> results;
    for (const auto& v : bytes_results) {
        results.push_back(bigint(v));
    }

    for (size_t i = 0; i < values.size(); i++) {
        ASSERT_EQ(values[i], results[i]);
    }
}

TEST(BigInt, FromNumericEnum) {
    enum some : uint8_t {
        A = 0x01,
        B,
        C
    };

    bigint a(some::A);
    bigint b(some::B);
    bigint c(some::C);

    ASSERT_EQ(bigint("01", 16), a);
    ASSERT_EQ(bigint("02", 16), b);
    ASSERT_EQ(bigint("03", 16), c);

    std::cout << a << std::endl;
}

TEST(BigInt, FromHexString) {
    bigint hex("80", 16);
    std::cout << hex << std::endl;

    bigint dec("16");

    hex += dec;
    ASSERT_EQ(bigint("90", 16), hex);

    bigint hex2("80", 16);
    hex2 += bigint("15", 10);
    ASSERT_EQ(bigint("8f", 16), hex2);
}

TEST(BigInt, Bitwise) {
    {
        // repat this ops for bigint
        uint32_t rgba_src = 0xFFFFFFFF;
        uint8_t a = (rgba_src >> 24) & 0xFF;
        uint8_t r = (rgba_src >> 16) & 0xFF;
        uint8_t g = (rgba_src >> 8) & 0xFF;
        uint8_t b = (rgba_src) &0xFF;
        uint32_t rgba_target = 0;
        rgba_target |= a << 24;
        rgba_target |= r << 16;
        rgba_target |= g << 8;
        rgba_target |= b;

        ASSERT_EQ(rgba_src, rgba_target);
    }

    {
        bigint rgba_src("FFEEDDBB", 16);
        bigint a = (rgba_src >> 24) & 0xFF;
        bigint r = (rgba_src >> 16) & 0xFF;
        bigint g = (rgba_src >> 8) & 0xFF;
        bigint b = (rgba_src) &0xFF;

        ASSERT_EQ(bigint("FF", 16), a);
        ASSERT_EQ(bigint("EE", 16), r);
        ASSERT_EQ(bigint("DD", 16), g);
        ASSERT_EQ(bigint("BB", 16), b);

        bigint rgba_target = 0;
        rgba_target |= a << 24;
        rgba_target |= r << 16;
        rgba_target |= (g << 8);
        rgba_target |= b;

        ASSERT_EQ(rgba_src, rgba_target);

        std::cout << rgba_target << std::endl;
    }
}

TEST(BigInt, ZeroValue) {
    bigint zero("0");
    const std::string s = "0";
    ASSERT_TRUE(s == zero.str());
}

TEST(BigInt, BoolOperator) {
    bigint zero("0");
    bool valid = false;
    if (!zero) {
        valid = true;
    }
    ASSERT_TRUE(valid);
}

TEST(BigInt, FromBigDecimal) {
    bigint converted(bigdecimal("100.500") + bigdecimal("200"));
    ASSERT_EQ(bigint("300"), converted);

    bigdecimal mul_res = bigdecimal("1000000000000000000") * bigdecimal("100");
    ASSERT_EQ(bigdecimal("100000000000000000000"), mul_res);
    bigint conv2(mul_res);
    ASSERT_EQ(bigint("100000000000000000000"), conv2);
}

TEST(BigInt, DefaultCtor) {
    bigint val;

    ASSERT_EQ(bigint("0"), val);
}