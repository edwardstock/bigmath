/*!
 * bigmath.
 * bigdecimal_test.cpp
 *
 * \date 09/13/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */

#include <bigmath/bigdecimal.h>
#include <gtest/gtest.h>
#include <iostream>

using namespace bigmath;
using bigdec18 = bigdecimal;

TEST(BigDecimal, Division) {
    //66634859979403239086722692970
    bigint total("999522899691048586300907250");
    bigint base("1000000000000000000");
    bigdec18 totalD(total);
    bigdec18 baseD(base);
    bigdec18 result = bigdec18(total) / bigdec18(base);
    bigdec18 cmp("999522899.69104858630090725");

    ASSERT_EQ(cmp, result);

    std::cout << result << std::endl;

    //    std::cout << result.format(".18f") << std::endl;
}

TEST(BigDecimal, FromDouble) {
    bigdec18 baseFee(0.001);
    ASSERT_EQ(bigdec18("0.001"), baseFee);
}

TEST(BigDecimal, ToBigint) {
    bigdec18 val("124.450200000000000200");
    bigdec18 base("1000000000000000000");
    bigdec18 res = val * base;
    std::cout << res.to_bigint() << std::endl;
    std::cout << val.format(".6f") << std::endl;
}