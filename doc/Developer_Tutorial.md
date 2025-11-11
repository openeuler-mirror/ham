# HAM Tutorial

## Best practice

## A simple demo

```c
#include "ham.h" /* The header file in src/ham.h */

#define HAM_LIB_PATH    "/usr/lib64/libham.so" /* The absolute path where you placed libham.so */

static void *handle_ham = NULL; /* dlopen handle */

/**
 * Define HAM interface functions in you code,
 * origin function definition reference to ham.h
 * */
void (*ubturbo_ham_external_log_set_ptr)(ExternalLog logFunc);
int32_t (*ubturbo_ham_register_ptr)(HamRamInfo *src, HamNumaInfo *dst,
                                 int32_t cpu_num, int32_t *cpu_start, int32_t version);
int32_t (*ubturbo_ham_migrate_ptr)(HamRamPages *ramList, size_t ram_num, int32_t step);
void (*ubturbo_ham_unregister_ptr)(void);
int32_t (*ubturbo_ham_rollback_ptr)(pid_t pid);

typedef struct dl_funcs {
    const char *func_name;
    void **func;
} dl_funcs;

/**
 * Create a mapping relationship between interface names and function types
 */
static dl_funcs ham_dlfunc_list[] = {
    {.func_name = "ubturbo_ham_external_log_set",        .func = (void**)&ubturbo_ham_external_log_set_ptr},
    {.func_name = "ubturbo_ham_register",        .func = (void**)&ubturbo_ham_register_ptr},
    {.func_name = "ubturbo_ham_migrate",          .func = (void**)&ubturbo_ham_migrate_ptr},
    {.func_name = "ubturbo_ham_unregister",         .func = (void**)&ubturbo_ham_unregister_ptr},
    {.func_name = "ubturbo_ham_rollback",         .func = (void**)&ubturbo_ham_rollback_ptr},
};

/**
 * Open and load libham.so,
 * then initialize these interface functions
 */
static int ham_dlfunc_open(void)
{
    char *error = NULL;

    ham_dlfunc_list_set_null();
    handle_ham = dlopen(HAM_LIB_PATH, RTLD_LAZY);
    if (!handle_ham) {
        print("dlopen error: %s.\n", dlerror());
        return -1;
    }

    for (int i = 0; i < ARRAY_SIZE(ham_dlfunc_list); i++) {
        *ham_dlfunc_list[i].func = dlsym(handle_ham, ham_dlfunc_list[i].func_name);
        if ((error = dlerror()) != NULL) {
            print("dlsym error: %s while getting %s.\n", error, ham_dlfunc_list[i].func_name);
            ham_dlfunc_close();
            return -1;
        }
    }

    return 0;
}

/**
 * Close the dynamic-link library when no longer in use
 */
static void ham_dlfunc_close(void)
{
    if (handle_ham) {
        (void)dlclose(handle_ham);
        handle_ham = NULL;
    }
    ham_dlfunc_list_set_null();
}

int main()
{
    int ret;
    HamRamPages ramPages[1] = {};

    ret = ham_dlfunc_open();
    if (ret < 0) {
        print("Open ham library failed.\n");
        return ret;
    }
    
    ramPages[0].uuid = 1;
    /* Assign values to other fields of ramPages */
    
    /* 1. Invoke ubturbo_ham_external_log_set */
    /* 2. Invoke ubturbo_ham_register */
    
    /* 3. Invoke ubturbo_ham_migrate */
    int ret = ubturbo_ham_migrate_ptr(ramPages, 1, HAM_MIGRATE_COMPLETION);
    if (ret != 0) {
        print(LOG_ERR, "Pages commit fail, ret:%d.\n", ret);
        ham_dlfunc_close();
        return ret;
    }
    
    /* 4. Invoke ubturbo_ham_rollback when needed */
    /* 5. Invoke ubturbo_ham_unregister */
    
    ham_dlfunc_close();
    return 0;
}
```


## Getting started

### Build HAM
Due to HAM's reliance on **libboundscheck.so**,you should get the dependent header files of 
[libboundscheck](https://gitee.com/openeuler/libboundscheck.git)
firstly and place these header files in the 3rdparty/libboundscheck/include directory of the project path.

```shell
git clone https://xxx/HAM.git
cd HAM
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j<thread-num>
```

Or directly execute the shell script:
```shell
git clone https://xxx/HAM.git
cd HAM
bash build.sh -t release
```

The only target **libham.so** is located in directory *output/ham_package/bin* of the project path.
### Deploy HAM

Just move the **libham.so** to the directory */usr/lib64* of your system.

### Run test suite
All unit test source files and relative files are located in the *test* directory.
```shell
git clone https://xxx/HAM.git
cd HAM
git submodule update --init --recursive
bash test/run_dt.sh
```

## Contribution guide
Participating in the development of HAM is highly welcomed.  

### License
HAM is licensed under Mulan PSL v2.  
You can use this software according to the terms and conditions of the Mulan PSL v2.  
You may obtain a copy of Mulan PSL v2 at: http://license.coscl.org.cn/MulanPSL2.

### Development Tools
- C programming language for coding source code.
- C++ programming language for coding test suites.
- CMake for building.