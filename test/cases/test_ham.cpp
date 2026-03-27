/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * Description: HAM user ham ut code
 */
#include <sys/ioctl.h>
#include <fcntl.h>
#include "gtest/gtest.h"
#include "mockcpp/mokc.h"
#include "ham_comm.h"

extern "C" int32_t g_fd;

constexpr size_t SIZE_2MB = 2097152;
constexpr size_t SIZE_2KB = 4096;

using namespace std;
class TestHam : public ::testing::Test {
protected:
    void SetUp() override
    {
        cout << "[Phase SetUp Begin]" << endl;
        cout << "[Phase SetUp End]" << endl;
    }
    void TearDown() override
    {
        cout << "[Phase TearDown Begin]" << endl;
        GlobalMockObject::verify();
        cout << "[Phase TearDown End]" << endl;
    }
};

extern "C" {
    int32_t HamCheckRamBlock(HamRamInfo *src, HamNumaInfo *dst);
    int32_t HandleIoctl(int fd, unsigned long req, uintptr_t data);
}

TEST_F(TestHam, checkRamBlockTask)
{
    HamRamInfo src;
    HamNumaInfo dst;
    dst.numaList[0].numaId = 1;
    src.num = 0;
    EXPECT_EQ(HamCheckRamBlock(&src, &dst), -ERR_CHECK_PARAMETERS);
    src.num = BATCH_NUM + 1;
    EXPECT_EQ(HamCheckRamBlock(&src, &dst), -ERR_CHECK_PARAMETERS);
    src.num = 1;
    dst.num = 0;
    EXPECT_EQ(HamCheckRamBlock(&src, &dst), -ERR_CHECK_PARAMETERS);
    dst.num = BATCH_NUM + 1;
    EXPECT_EQ(HamCheckRamBlock(&src, &dst), -ERR_CHECK_PARAMETERS);
    dst.num = 1;
    src.blockList[0].uuid = 1;
    EXPECT_EQ(HamCheckRamBlock(&src, &dst), -ERR_CHECK_PARAMETERS);
    src.blockList[0].uuid = 0;
    src.blockList[0].size = 0;
    EXPECT_EQ(HamCheckRamBlock(&src, &dst), -ERR_CHECK_PARAMETERS);
    src.blockList[0].size = SIZE_2MB;

    dst.numaList[0].size = 0;
    EXPECT_EQ(HamCheckRamBlock(&src, &dst), -ERR_CHECK_PARAMETERS);
    dst.numaList[0].size = SIZE_2KB;
    EXPECT_EQ(HamCheckRamBlock(&src, &dst), -ERR_CHECK_PARAMETERS);
    dst.numaList[0].size = SIZE_2MB;
    EXPECT_EQ(HamCheckRamBlock(&src, &dst), SUCCESS);
}

TEST_F(TestHam, HandleIoctl)
{
    int ret;
    int fd = -1;
    int data = 0;
    MOCKER((int (*)(int, int, int *))ioctl).stubs().will(returnValue(-1)).then(returnValue(0));

    ret = HandleIoctl(fd, HAM_STOP_MIGRATION, (uintptr_t) &data);
    EXPECT_EQ(ret, -ERR_OPERATE_DEVICE);

    fd = 1;
    ret = HandleIoctl(fd, HAM_STOP_MIGRATION, (uintptr_t) &data);
    EXPECT_EQ(ret, -errno);

    fd = 1;
    ret = HandleIoctl(fd, HAM_STOP_MIGRATION, (uintptr_t) &data);
    EXPECT_EQ(ret, SUCCESS);
}

TEST_F(TestHam, startMigrationTask)
{
    int ret;
    HamRamInfo src = { .num = 1, .blockList = { { .uuid = 0, .size = SIZE_2KB } } };
    HamNumaInfo dst = { .num = 1, .numaList = { { .numaId = 0, .size = SIZE_2KB } } };

    MOCKER((int (*)(int, int, int *))open).stubs().will(returnValue(-1)).then(returnValue(1));
    MOCKER((int (*)(int, int, int *))ioctl).stubs().will(returnValue(0));
    MOCKER(close).stubs().will(returnValue(0));

    /* open device failed */
    g_fd = -1;
    ret = ubturbo_ham_register(&src, &dst);
    EXPECT_EQ(ret, -ERR_OPERATE_DEVICE);

    /* success */
    ret = ubturbo_ham_register(&src, &dst);
    EXPECT_EQ(ret, 0);

    /* parameters invalid */
    src.num = 0;
    ret = ubturbo_ham_register(&src, &dst);
    EXPECT_EQ(ret, -ERR_CHECK_PARAMETERS);
}

TEST_F(TestHam, migratePagesTask)
{
    int32_t ret;

    /* not open the device */
    g_fd = -1;
    ret = ubturbo_ham_migrate(nullptr, 0, HAM_MIGRATE_COMPLETION);
    EXPECT_EQ(ret, -ERR_OPERATE_DEVICE);

    /* migration not completed */
    g_fd = 1;
    MOCKER(HandleIoctl).stubs().with(eq(g_fd), eq(HAM_MIGRATE_PAGES), any()).will(returnValue(0));
    ret = ubturbo_ham_migrate(nullptr, 0, HAM_MIGRATE_PRECOPY);
    EXPECT_EQ(ret, SUCCESS);

    /* modify page table failed */
    MOCKER(HandleIoctl).stubs().with(eq(g_fd), eq(HAM_MODIFY_PAGETABLE), any()).will(returnValue(-1))
        .then(returnValue(0)).then(returnValue(0)).then(returnValue(0));
    ret = ubturbo_ham_migrate(nullptr, 0, HAM_MIGRATE_COMPLETION);
    EXPECT_EQ(ret, -ERR_MODIFY_PAGETABLE);

    /* cache clear failed */
    MOCKER(HandleIoctl).stubs().with(eq(g_fd), eq(HAM_CACHE_CLEAR), any()).will(returnValue(-1))
        .then(returnValue(0)).then(returnValue(0));
    ret = ubturbo_ham_migrate(nullptr, 0, HAM_MIGRATE_COMPLETION);
    EXPECT_EQ(ret, -ERR_CLEAR_CACHE);

    /* all success */
    ret = ubturbo_ham_migrate(nullptr, 0, HAM_MIGRATE_COMPLETION);
    EXPECT_EQ(ret, SUCCESS);
}

TEST_F(TestHam, stopMigrationTask)
{
    MOCKER(HandleIoctl).stubs().will(returnValue(0));
    MOCKER(close).expects(once()).will(returnValue(0));
    ubturbo_ham_unregister();
    EXPECT_EQ(g_fd, -1);
}

TEST_F(TestHam, ubturbo_ham_rollback)
{
    MOCKER((int (*)(int, int, int *))open).stubs().will(returnValue(1));
    MOCKER(HandleIoctl).stubs().will(returnValue(0));
    MOCKER(close).expects(once()).will(returnValue(0));
    EXPECT_EQ(ubturbo_ham_rollback(1), SUCCESS);
}
