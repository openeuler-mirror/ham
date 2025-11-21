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

#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "ham_comm.h"

#if defined(DEBUG_UT)
#define STATIC
#else
#define STATIC static
#endif

#define NUMA_NO_NODE (-1)

STATIC pid_t g_pid;
STATIC uint16_t g_scna;
STATIC int32_t g_fd = -1;

STATIC int32_t HamCheckRamBlock(HamRamInfo *src, HamNumaInfo *dst)
{
    uint32_t i;

    if (!src || src->num == 0 || src->num > BATCH_NUM) {
        HAM_LOGERROR("HamRamInfo invalid\n");
        return -ERR_CHECK_PARAMETERS;
    }

    if (dst && dst->num != src->num) {
        HAM_LOGERROR("Parameter cnt  mismatch\n");
        return -ERR_CHECK_PARAMETERS;
    }

    size_t totalSize = 0;
    for (i = 0; i < src->num; i++) {
        if (src->blockList[i].uuid != i) {
            HAM_LOGERROR("The src uuid is invalid\n");
            return -ERR_CHECK_PARAMETERS;
        }
        if (dst && src->blockList[i].size != dst->numaList[i].size) {
            HAM_LOGERROR("The ram block size is not equal to the numa size, i: %d\n", i);
            return -ERR_CHECK_PARAMETERS;
        }
        if (src->blockList[i].size == 0) {
            HAM_LOGERROR("The src size is invalid\n");
            return -ERR_CHECK_PARAMETERS;
        }
        if (totalSize > (ULONG_MAX - src->blockList[i].size)) {
            HAM_LOGERROR("Integer overflow, srcSize:%lu, blockList[%d].size:%lu\n",
                         totalSize, i, src->blockList[i].size);
            return -ERR_CHECK_PARAMETERS;
        }
        totalSize += src->blockList[i].size;
    }

    return SUCCESS;
}

STATIC int32_t HandleIoctl(int fd, unsigned long req, uintptr_t data)
{
    int32_t ret;
    if (fd < 0) {
        HAM_LOGERROR("Device %s not opened.\n", HAM_DEVICE);
        return -ERR_OPERATE_DEVICE;
    }

    ret = ioctl(fd, req, data);
    if (ret < 0) {
        HAM_LOGERROR("Call ham ioctl failed, request_no: %ld, errno: %d.\n", req, errno);
        return -errno;
    }
    return SUCCESS;
}

int32_t ubturbo_ham_register(HamRamInfo *src, HamNumaInfo *dst)
{
    int32_t ret;
    uint32_t i;
    MigrationParam migration;

    ret = HamCheckRamBlock(src, dst);
    if (ret != SUCCESS) {
        HAM_LOGERROR("Ramblock desc info is invalid.\n");
        return ret;
    }

    g_pid = src->pid;
    g_scna = src->scna;

    migration.pid = src->pid;
    migration.cnt = src->num;

    for (i = 0; i < migration.cnt; i++) {
        migration.ramBlocks[i].numaId = dst ? dst->numaList[i].numaId : NUMA_NO_NODE;
        migration.ramBlocks[i].size = src->blockList[i].size;
        migration.ramBlocks[i].hvaStart = src->blockList[i].hva;
    }

    if (g_fd != -1) {
        close(g_fd);
    }
    g_fd = open(HAM_DEVICE, O_RDWR);
    if (g_fd < 0) {
        HAM_LOGERROR("Failed to open device %s, errno: %d.\n", HAM_DEVICE, errno);
        return -ERR_OPERATE_DEVICE;
    }
    ret = HandleIoctl(g_fd, HAM_START_MIGRATION, (uintptr_t) &migration);
    if (ret) {
        HAM_LOGERROR("Failed to call kernel interface of start migration.\n");
        close(g_fd);
        g_fd = -1;
        return -ERR_REGISTER;
    }
    return SUCCESS;
}

int32_t ubturbo_ham_migrate(HamRamPages *ramList, size_t ramNum, int32_t step)
{
    int ret;
    MaintainInfo mtInfo = { .pid = g_pid, .cacheable = false };

    if (step > HAM_MIGRATE_BUTT) {
        HAM_LOGERROR("Invalid step value.\n", errno);
        return -ERR_CHECK_PARAMETERS;
    }
    ret = HandleIoctl(g_fd, HAM_MIGRATE_PAGES, (uintptr_t) &g_pid);
    if (ret) {
        HAM_LOGERROR("Failed to migrate pages, errno: %d.\n", errno);
        return ret == -ERR_OPERATE_DEVICE ? -ERR_OPERATE_DEVICE : -ERR_MIGRATE_PAGES;
    }
    HAM_LOGINFO("migrate pages completed\n");

    if (step == HAM_MIGRATE_COMPLETION) {
        ret = HandleIoctl(g_fd, HAM_MODIFY_PAGETABLE, (uintptr_t) &mtInfo);
        if (ret) {
            HAM_LOGERROR("Failed to modify page table, errno: %d.\n", errno);
            return -ERR_MODIFY_PAGETABLE;
        }
        ret = HandleIoctl(g_fd, HAM_CACHE_CLEAR, (uintptr_t) &g_pid);
        if (ret) {
            HAM_LOGERROR("Failed to cache clear, errno: %d.\n", errno);
            return -ERR_CLEAR_CACHE;
        }
    }

    return SUCCESS;
}

int32_t ubturbo_ham_pgtable_modify(bool cacheable)
{
    int32_t ret;
    MaintainInfo mtInfo = { .pid = g_pid, .cacheable = cacheable };

    ret = HandleIoctl(g_fd, HAM_MODIFY_PAGETABLE, (uintptr_t) &mtInfo);
    if (ret) {
        HAM_LOGERROR("Failed to modify page table, errno: %d.\n", errno);
        return -ERR_MODIFY_PAGETABLE;
    }
    return SUCCESS;
}

void ubturbo_ham_unregister(void)
{
    int ret;

    ret = HandleIoctl(g_fd, HAM_STOP_MIGRATION, (uintptr_t) &g_pid);
    if (ret) {
        HAM_LOGERROR("Failed to stop migration, errno: %d.\n", errno);
    }

    if (g_fd >= 0) {
        close(g_fd);
        g_fd = -1;
    }
    g_pid = 0;
    g_scna = 0;
}

int32_t ubturbo_ham_rollback(pid_t srcPid)
{
    int32_t fd, ret = SUCCESS;
    fd = open(HAM_DEVICE, O_RDWR);
    if (fd < 0) {
        HAM_LOGERROR("Failed to open device %s, errno: %d.\n", HAM_DEVICE, errno);
        return -ERR_OPERATE_DEVICE;
    }
    ret = HandleIoctl(fd, HAM_ROLLBACK_PAGES, (uintptr_t) &srcPid);
    if (ret) {
        HAM_LOGERROR("Failed to rollback pages, errno: %d.\n", errno);
        ret = -ERR_ROLLBACK_PAGES;
    }
    close(fd);
    return ret;
}