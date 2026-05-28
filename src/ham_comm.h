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

#ifndef HAM_COMM_H
#define HAM_COMM_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "ham.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAM_DEVICE "/dev/ham_migrate"

#define HAM_START_MIGRATION _IOW('N', 0, unsigned long)
#define HAM_MIGRATE_PAGES _IOW('N', 1, unsigned long)
#define HAM_ROLLBACK_PAGES _IOW('N', 2, unsigned long)
#define HAM_STOP_MIGRATION _IOW('N', 3, unsigned long)
#define HAM_MODIFY_PAGETABLE _IOW('N', 4, unsigned long)
#define HAM_CACHE_CLEAR _IOW('N', 5, unsigned long)

typedef enum {
    HAM_LOG_DEBUG = 0,
    HAM_LOG_INFO,
    HAM_LOG_WARNING,
    HAM_LOG_ERROR,
    HAM_LOG_BUTT
} HamLogLevel;

typedef struct {
    uint32_t numaId;
    uint64_t size;
    uint64_t hvaStart;
} RamBlockInfo;

typedef struct {
    pid_t pid;
    uint32_t cnt;
    RamBlockInfo ramBlocks[BATCH_NUM];
} MigrationParam; // 内核匹配：struct migration_param

typedef struct {
    pid_t pid;
    bool cacheable;
} MaintainInfo;

void HamLogFunc(int logLevel, const char *funcName, int line, const char *format, ...)
    __attribute__((format(printf, 4, 5)));

#define HAM_LOGINFO(...)                                               \
    do {                                                               \
        HamLogFunc(HAM_LOG_INFO, __FUNCTION__, __LINE__, __VA_ARGS__); \
    } while (0)

#define HAM_LOGDEBUG(...)                                               \
    do {                                                                \
        HamLogFunc(HAM_LOG_DEBUG, __FUNCTION__, __LINE__, __VA_ARGS__); \
    } while (0)

#define HAM_LOGWARN(...)                                                  \
    do {                                                                  \
        HamLogFunc(HAM_LOG_WARNING, __FUNCTION__, __LINE__, __VA_ARGS__); \
    } while (0)

#define HAM_LOGERROR(...)                                               \
    do {                                                                \
        HamLogFunc(HAM_LOG_ERROR, __FUNCTION__, __LINE__, __VA_ARGS__); \
    } while (0)

#ifdef __cplusplus
}
#endif
#endif
