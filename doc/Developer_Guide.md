# HAM介绍

确定性虚机热迁移基于灵衢内存池化能力将目的端虚机内存上线到源端os，并利用内核migrate_pages接口迁移内存，期间通过灵衢总线远端内存访问能力保活虚机；迁移中虚机数据无副本，可去除迭代清脏依赖，以实现确定性时长迁移。HAM作为确定性迁移关键组件，提供迁移任务管理、内存精准迁移能力。

# HAM编译

1. 下载源码后，在根目录下执行
    ```bash
   sh build.sh 
   ```

2. 编译完成后根目录下会生成output文件夹，编译生成的so存放在output文件夹下的ham_package文件夹中，目录结构如下：
    ```bash
    ham_package
    ├── include
    │   └── ham.h
    └── lib
        └── libham.so
   ```


## HAM接口设计

### 接口结构体说明

#### HamRamBlock

```c
typedef struct {
    uint32_t uuid; // ramblock的id
    uintptr_t hva; // ramblock的起始地址
    size_t size; // ramblock的大小
} HamRamBlock; 
```

#### HamRamInfo

```c
typedef struct {
    pid_t pid; // 迁移进程pid
    uint16_t scna; // 源端ub_controller标识符
    uint32_t num; // 源端待迁移ramblock数量
    HamRamBlock blockList[BATCH_NUM]; // 源端待迁移ramblock数组
} HamRamInfo;
```

#### HamNuma

```c
typedef struct {
    uint32_t numaId; // 借用内存的numaId
    size_t size; // 借用内存的大小
} HamNuma;
```

#### HamNumaInfo

```c
typedef struct {
    pid_t pid; // 迁移进程的pid
    uint32_t num; // HamNuma的个数
    HamNuma numaList[BATCH_NUM]; // HamNuma数组
} HamNumaInfo;
```

#### HamRamPages

```c
typedef struct {
    int32_t uuid; // ramblock的id
    size_t hvaNum; // hva数量
    uintptr_t *hvaList; // 存储hva的数组
} HamRamPages;
```

#### HamMigrateStep

```c
typedef enum {
    HAM_MIGRATE_PRECOPY = 0, // 当前处于precopy阶段
    HAM_MIGRATE_COMPLETION = 1, // 当前处于迁移结束刷cache阶段
    HAM_MIGRATE_BUTT
} HamMigrateStep;
```

### 对外接口说明

#### 启动确定性迁移接口

```c
/* *
 * @ src: 源端虚机ramblock的描述信息
 * @ dst: 目的端虚机借用内存的描述信息
 * @ return: 0成功 非0失败
 * @ function: 启动确定性迁移，完成迁移前准备工作
 */
int32_t ubturbo_ham_register(HamRamInfo *src, HamNumaInfo *dst);
```

#### 触发确定性迁移接口

```c
/* *
 * @ ram: 源端虚机的待迁移页面描述信息
 * @ step: 区分precopy阶段和停机中断阶段
 * @ return: 0成功 非0失败
 * @ function: 触发确定性迁移
 */
int32_t ubturbo_ham_migrate(HamRamPages *ramList, size_t ramNum, int32_t step);
```

#### 结束确定性迁移

```c
/* *
 * @ function: 确定性迁移结束，释放清理资源
 */
void ubturbo_ham_unregister(void);
```

#### 页面回迁检查处理

```c
/* *
 * @ srcPid: 源端虚机pid
 * @ return: 0成功 非0失败
 * @ function: 如果迁移失败，将已经源端迁出的页迁回，清理释放资源
 */
int32_t ubturbo_ham_rollback(pid_t srcPid);
```