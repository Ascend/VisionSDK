/*
* -------------------------------------------------------------------------
*  This file is part of the Vision SDK project.
* Copyright (c) 2025 Huawei Technologies Co.,Ltd.
*
* Vision SDK is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*
*           http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
* -------------------------------------------------------------------------
 * Description: Gtest unit cases.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include "MxBase/Maths/FastMath.h"

using namespace std;

namespace {
class FastMathTest : public testing::Test {
public:
};

TEST_F(FastMathTest, sigmoidTest)
{
    const float num = 2.0;
    float sig = fastmath::sigmoid(num);
    const float expect = 0.880797;
    EXPECT_FLOAT_EQ(sig, expect);
}

TEST_F(FastMathTest, expTest)
{
    const float num = -2.0;
    float exp = fastmath::exp(num);
    const float expect = 0.13533528;
    EXPECT_FLOAT_EQ(exp, expect);
}

TEST_F(FastMathTest, expAndsigmoidTest)
{
    const float num1 = -3;
    float exp = fastmath::exp(num1);
    const float expect1 = 0.049787067;
    EXPECT_FLOAT_EQ(exp, expect1);
    const float num2 = 3;
    const float expect2 = 0.952574;
    float sig = fastmath::sigmoid(num2);
    EXPECT_FLOAT_EQ(sig, expect2);
    const float expect3 = 0.952574;
    EXPECT_NEAR(1.0 / (1.0 + exp), sig, expect3);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}