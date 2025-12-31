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

#include "MxBase/Utils/Singleton.h"

class MyClass {};

class Test1 {
public:
    explicit Test1(const int i)
    {
        std::cout<< "Test1" <<std::endl;
    }

    std::string DoTest() const
    {
        return "Test1";
    }
};

namespace mxbase {
TEST(TestSingleton, TestBasicSingletonNotUsed)
{
    MyClass obj1;
    MyClass obj2;
    if (&obj1 == &obj2) {
        std::cout << "Obj1 == Obj2" << std::endl;
    } else {
        std::cout << "Obj1 != Obj2" << std::endl;
    }
    ASSERT_EQ(false, &obj1 == &obj2);
}

TEST(TestSingleton, TestBasicSingletonNormal)
{
    MyClass& ptr1 = Singleton<MyClass>::GetInstance();
    MyClass& ptr2 = Singleton<MyClass>::GetInstance();
    ASSERT_EQ(true, &ptr1 == &ptr2);
}

TEST(TestSingleton, TestBasicSingletonT)
{
    auto test1 = Singleton<Test1>::GetInstance(1);
    std::string str = test1.DoTest();
    ASSERT_STREQ("Test1", str.c_str());
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}