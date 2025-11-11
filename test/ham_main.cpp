// Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

#include "gtest.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Result " << ret;
    return ret;
}