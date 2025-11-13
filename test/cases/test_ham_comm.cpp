/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * Description: HAM user comm ut code
 */
#include "gtest/gtest.h"
#include "mockcpp/mokc.h"
#include "ham_comm.h"

using namespace std;

class TestHamComm : public ::testing::Test {
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

static void StubExternalLog(int level, const char *funcname, int linenr, const char *logBuf)
{
}

TEST_F(TestHamComm, setLogTask)
{
    ubturbo_ham_external_log_set(nullptr);
}

TEST_F(TestHamComm, logFuncTask)
{
    HamLogFunc(0, nullptr, 0, nullptr);
    ubturbo_ham_external_log_set(StubExternalLog);
    MOCKER(vsnprintf_s).stubs().will(returnValue(-1)).then(returnValue(0));
    HamLogFunc(0, nullptr, 0, nullptr);
    HamLogFunc(0, nullptr, 0, nullptr);
}
