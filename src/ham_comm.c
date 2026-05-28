/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.

 * ham is licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *      http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include "ham_comm.h"
#include <stdarg.h>

static ExternalLog g_hamLog = NULL;

#define HAM_LOG_BUF_LEN 512

void ubturbo_ham_external_log_set(ExternalLog logFunc)
{
    g_hamLog = logFunc;
}

void HamLogFunc(int logLevel, const char *funcName, int line, const char *format, ...)
{
    if (g_hamLog == NULL) {
        return;
    }

    va_list argPtr;
    char logBuf[HAM_LOG_BUF_LEN];
    int ret;

    va_start(argPtr, format);
    ret = vsnprintf_s(logBuf, sizeof(logBuf), sizeof(logBuf) - 1, format, argPtr);
    if (ret < 0) {
        g_hamLog(logLevel, funcName, line, "vsnprintf_s failed. ");
        va_end(argPtr);
        return;
    }
    va_end(argPtr);

    logBuf[HAM_LOG_BUF_LEN - 1] = '\0';

    g_hamLog(logLevel, funcName, line, logBuf);
}
