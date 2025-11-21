# ubturbo_ham_register: 初始化确定性迁移

## 库 LIBRARY

ham库 (libham.so)

## 摘要 SYNOPSIS

```c
#include <ham.h>
int32_t ubturbo_ham_register(HamRamInfo *src, HamNumaInfo *dst);
```

## 描述 DESCRIPTION

初始化确定性迁移，完成迁移前准备工作。

## 参数 Parameters

| name | IN/OUT | description        |
|------|--------|--------------------|
| src  | IN     | 源端虚机ramblock的描述信息  |
| dst  | IN     | 目的端虚机借用到源端的内存的描述信息 |

## 返回值 RETURN VALUE

返回 `SUCCESS` 表示成功，返回其他值表示失败，请见`错误 ERRORS`

## 错误 ERRORS

| Error                | Description |
|----------------------|-------------|
| ERR_CHECK_PARAMETERS | 入参校验失败      |
| ERR_OPERATE_DEVICE   | 设备打开失败      |
| ERR_REGISTER         | 内核初始化失败     |

## 约束 CONSTRAINTS

暂无

## 附注 NOTES

暂无

## 样例 EXAMPLES

以下程序初始化确定性迁移，将相关信息传入做迁移前的准备工作。

```c
#include <stdio.h>
#include <unistd.h>
#include <ham.h>

// 以下参数值都需根据实际情况修改，下面仅为示例
int main(void)
{
    int32_t ret;
    HamRamInfo srcInfo = {
            .pid = 100,
            .scna = 1,
            .num = 2,
            .blockList = {
                    {.uuid = 1, .hva = 0x1000, .size = 4096}
            }
    };

    HamNumaInfo dstInfo = {
            .pid = 101,
            .num = 1,
            .numaList = {
                    {.numaId = 34, .size = 4096}
            }
    };

    int32_t result = ubturbo_ham_register(&srcInfo, &dstInfo);
    printf("Migration result: %d\n", result);

    /* Do your work here... */

    return 0;
}
```

# ubturbo_ham_migrate: 触发确定性迁移

## 库 LIBRARY

ham库 (libham.so)

## 摘要 SYNOPSIS

```c
#include <ham.h>
int32_t ubturbo_ham_migrate(HamRamPages *ramList, size_t ramNum, int32_t step);
```

## 描述 DESCRIPTION

触发确定性迁移

## 参数 PARAMETERS

| name    | IN/OUT | description        |
|---------|--------|--------------------|
| ramList | IN     | 源端虚机的待迁移页面描述信息     |
| ramNum  | IN     | 源端虚机的待迁移页面描述信息数组长度 |
| step    | IN     | 区分precopy阶段和停机中断阶段 |

## 返回值 RETURN VALUE

返回 `SUCCESS` 表示成功，返回其他值表示失败，请见`错误 ERRORS`

## 错误 ERRORS

| Error                | Description |
|----------------------|-------------|
| ERR_CHECK_PARAMETERS | 入参校验失败      |
| ERR_OPERATE_DEVICE   | 设备打开失败      |
| ERR_MIGRATE_PAGES    | 页面迁移失败      |
| ERR_MODIFY_PAGETABLE | 修改页表属性失败    |
| ERR_CLEAR_CACHE      | 刷cache失败    |

## 约束 CONSTRAINTS

暂无

## 附注 NOTES

暂无

## 样例 EXAMPLES

以下程序进行虚机页面迁移。

```c
#include <stdio.h>
#include <stdlib.h>
#include <ham.h>

// 以下参数值都需根据实际情况修改，下面仅为示例
int main() {
    const size_t num_ram_lists = 1;
    const size_t pages_per_list = 2;

    HamRamPages *ramLists = (HamRamPages *)malloc(num_ram_lists * sizeof(HamRamPages));
    if (!ramLists) {
        perror("Failed to allocate memory for ramLists");
        return -1;
    }

    ramLists[0].uuid = 1;
    ramLists[0].hvaNum = pages_per_list;
    ramLists[0].hvaList = (uintptr_t *)malloc(pages_per_list * sizeof(uintptr_t));
    if (!ramLists[0].hvaList) {
        perror("Failed to allocate hvaList for ramLists[0]");
        free(ramLists);
        return -1;
    }
    ramLists[0].hvaList[0] = 0x1000;
    ramLists[0].hvaList[1] = 0x2000;

    int32_t result = ubturbo_ham_migrate(ramLists, num_ram_lists, 0);
    printf("Migration result: %d\n", result);

    for (size_t i = 0; i < num_ram_lists; i++) {
        free(ramLists[i].hvaList);
    }
    free(ramLists);

    /* Do your work here... */

    return 0;
}
```

# ubturbo_ham_unregister: 结束确定性迁移

## 库 LIBRARY

ham库 (libham.so)

## 摘要 SYNOPSIS

```c
#include <ham.h>
void ubturbo_ham_unregister(void);
```

## 描述 DESCRIPTION

确定性迁移结束，释放清理资源

## 参数 PARAMETERS

无

## 返回值 RETURN VALUE

无

## 错误 ERRORS

无

## 约束 CONSTRAINTS

暂无

## 附注 NOTES

暂无

## 样例 EXAMPLES

以下程序迁移结束，释放相关资源。

```c
#include <stdio.h>
#include <ham.h>

int main() {
    ubturbo_ham_unregister();

    /* Do your work here... */

    return 0;
}
```

# ubturbo_ham_rollback: 页面回迁检查处理

## 库 LIBRARY

ham库 (libham.so)

## 摘要 SYNOPSIS

```c
#include <ham.h>
int32_t ubturbo_ham_rollback(pid_t srcPid);
```

## 描述 DESCRIPTION

如果迁移失败，将已经源端迁出的页迁回，清理释放资源

## 参数 PARAMETERS

| name   | IN/OUT | description |
|--------|--------|-------------|
| srcPid | IN     | 源端虚机pid     |

## 返回值 RETURN VALUE

返回 `SUCCESS` 表示成功，返回其他值表示失败，请见`错误 ERRORS`

## 错误 ERRORS

| Error              | Description |
|--------------------|-------------|
| ERR_OPERATE_DEVICE | 设备打开失败      |
| ERR_ROLLBACK_PAGES | 页面回迁失败      |

## 约束 CONSTRAINTS

暂无

## 附注 NOTES

暂无

## 样例 EXAMPLES

以下程序回滚已迁移到借用内存的虚机，将页面全部回迁到源numa。

```c
#include <stdio.h>
#include <stdlib.h>
#include <ham.h>

// 以下参数值都需根据实际情况修改，下面仅为示例
int main() {
    int32_t ret;
    pid_t pid = 100;
    ret = ubturbo_ham_rollback(pid);
    printf("Rollback pages result: %d\n", ret);

    /* Do your work here... */

    return 0;
}
```

# ubturbo_ham_external_log_set: 日志注册

## 库 LIBRARY

ham库 (libham.so)

## 摘要 SYNOPSIS

```c
#include <ham.h>
void ubturbo_ham_external_log_set(ExternalLog logFunc);
```

## 描述 DESCRIPTION

将日志函数注册到HAM中，方便HAM日志输出

## 参数 PARAMETERS

| name    | IN/OUT | description |
|---------|--------|-------------|
| logFunc | IN     | 日志函数指针      |

## 返回值 RETURN VALUE

无

## 错误 ERRORS

无

## 约束 CONSTRAINTS

暂无

## 附注 NOTES

暂无

## 样例 EXAMPLES

以下程序注册日志函数到ham，后续ham日志通过对应函数输出。

```c
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <ham.h>

// 该函数仅为示例使用
static void ham_external_log(int level, const char *funcname, int linenr, const char *logBuf)
{
    if (level == HAM_LOG_DEBUG) {
        syslog(LOG_DEBUG, "[%s:%d] %s", funcname, linenr, logBuf);
    } else if (level == HAM_LOG_INFO) {
        syslog(LOG_INFO, "[%s:%d] %s", funcname, linenr, logBuf);
    } else if (level == HAM_LOG_WARNING) {
        syslog(LOG_WARNING, "[%s:%d] %s", funcname, linenr, logBuf);
    } else {
        syslog(LOG_ERR, "[%s:%d] %s", funcname, linenr, logBuf);
    }
}

int main() {
    ubturbo_ham_external_log_set(ham_external_log);

    /* Do your work here... */

    return 0;
}
```