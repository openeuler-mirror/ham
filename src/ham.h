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

#ifndef HAM_HAM_H
#define HAM_HAM_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BATCH_NUM 4

enum HamErrCode {
    SUCCESS = 0,
    ERR_CHECK_PARAMETERS = 0x1000,
    ERR_OPERATE_DEVICE = 0x2000,
    ERR_REGISTER = 0x3000,
    ERR_MIGRATE_PAGES = 0x4000,
    ERR_MODIFY_PAGETABLE = 0x4001,
    ERR_CLEAR_CACHE = 0x4002,
    ERR_DRAIN_UB = 0x4003,
    ERR_ROLLBACK_PAGES = 0x5000,
};

/**
 * The single ram block info of the src process.
 */
typedef struct {
    uint32_t uuid; /* start from 0 generally */
    uintptr_t hva;
    size_t size;
} HamRamBlock;

/**
 * The complete info of the src process.
 */
typedef struct {
    pid_t pid;
    uint16_t scna;
    uint32_t num; /* the actual number of the ram blocks list below */
    HamRamBlock blockList[BATCH_NUM];
} HamRamInfo;

/**
 * The single numa info of the destination.
 */
typedef struct {
    uint32_t numaId;
    size_t size; /* must equal to matched block size */
} HamNuma;

typedef struct {
    pid_t pid;
    uint32_t num; /* the actual number of the numa list below, and must equal to the ram block number */
    HamNuma numaList[BATCH_NUM];
} HamNumaInfo;

/**
 * Deprecated
 */
typedef struct {
    int32_t uuid;
    size_t hvaNum;
    uintptr_t *hvaList;
} HamRamPages;

typedef enum {
    HAM_MIGRATE_PRECOPY = 0,
    HAM_MIGRATE_COMPLETION = 1,
    HAM_MIGRATE_BUTT
} HamMigrateStep;

/**
 * The custom log printing function type definition.
 * @funcname and @linenr have default value
 * @logBuf's max size is 512Bytes
 */
typedef void (*ExternalLog)(int level, const char *funcname, int linenr, const char *logBuf);

/**
 * Used to set a custom log printing function with the same type as the function defined above
 * @param logFunc
 */
void ubturbo_ham_external_log_set(ExternalLog logFunc);

/* 迁移过程：预处理，HamVmState标识主存ramblock，src和dst一一关联对应 */
/**
 * Migration preprocessing.
 * Mainly configure huge tables, establish mapping relationships between the src and dst pages
 * and allocate new folios for migration.
 */
int32_t ubturbo_ham_register(HamRamInfo *src, HamNumaInfo *dst);

/* 迁移过程：页面迁移，上层传入全量ramblock赃页信息，Ham冷热识别内部收敛，
   step:0代表precopy阶段(刷赃页)，step:1代表completion阶段(刷Cache) */
/**
 * Migration processing.
 * @ramList and @ramNum are deprecated.
 */
int32_t ubturbo_ham_migrate(HamRamPages *ramList, size_t ramNum, int32_t step);

/**
 * Migration processing.
 * Currently used only at the destination end for restoring pgtable attributes.
 */
int32_t ubturbo_ham_pgtable_modify(bool cacheable);

/**
 * Migration postprocessing.
 * Mainly clean temporary resources, such as close the HAM device...
 */
void ubturbo_ham_unregister(void);

/* 迁移异常结束时：提供给libvirt的资源清理接口（包含：页面回迁或管理资源清理） */
int32_t ubturbo_ham_rollback(pid_t srcPid);

#ifdef __cplusplus
}
#endif
#endif
