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

TEST(BigDecimal, Adding) {
    bigdec18 res("250.222222222222222222");
    bigdec18 a("125.111111111111111111");
    bigdec18 b("125.111111111111111111");

    ASSERT_EQ(res, a + b);
    a += b;
    ASSERT_EQ(res, a);
}

TEST(BigDecimal, AddingHighPrec) {
    bigdec18 res0("1914.264799228445797420");
    bigdec18 res1("227098.162215212855674869");

    bigdec18 a0("251.878985371658936099");
    bigdec18 v0("1662.385813856786861321");
    bigdec18 a1("30001.967206053160430979");
    bigdec18 v1("197096.195009159695243890");

    ASSERT_EQ(res0, a0 + v0);
    ASSERT_EQ(res1, a1 + v1);
    a0 += v0;
    a1 += v1;
    ASSERT_EQ(res0, a0);
    ASSERT_EQ(res1, a1);
}

TEST(BigDecimal, Division) {
    // 66634859979403239086722692970
    bigint total("999522899691048586300907250");
    bigint base("1000000000000000000");
    bigdec18 totalD(total);
    bigdec18 baseD(base);
    bigdec18 result = bigdec18(total) / bigdec18(base);
    bigdec18 cmp("999522899.69104858630090725");

    ASSERT_EQ(cmp, result);

    std::cout << result << std::endl;
}

TEST(BigDecimal, FromDouble) {
    bigdec18 baseFee(0.001);
    ASSERT_EQ(bigdec18("0.001"), baseFee);
}

TEST(BigDecimal, FromString) {
    bigdec18 val("5.000000000000000000");
    ASSERT_EQ(bigdec18("5.0"), val);
}

TEST(BigDecimal, MoveFromString) {
    bigdec18 target;

    target = bigdec18("5.000000000000000000");

    ASSERT_EQ(bigdec18("5.0"), target);
}

TEST(BigDecimal, ToBigint) {
    bigdec18 val("124.450200000000000200");
    bigdec18 base("1000000000000000000");
    bigdec18 res = val * base;
    std::cout << res.to_bigint() << std::endl;
    std::cout << val.format(".6f") << std::endl;
    std::cout << val.to_sci() << std::endl;
    std::cout << val.to_eng(false) << std::endl;
}

TEST(BigDecimal, Move) {
    bigdec18 a("100");
    bigdec18 b = std::move(a);
    // NaN
    ASSERT_TRUE(a.isnan());
    ASSERT_EQ(bigdec18("100"), b);
}

TEST(BigDecimal, MulAndAdd) {
    bigint sfee("1000000000000000000");
    bigdec18 fee = bigdec18(sfee) / bigdec18("1000000000000000000");
    bigdec18 mul(bigint("1"));
    bigdec18 extra_fee(bigdec18(bigint("200000000000000000")) / bigdec18("1000000000000000000"));

    bigdec18 res = fee * mul + extra_fee;
    ASSERT_EQ(bigdec18("1.20"), res);
}
