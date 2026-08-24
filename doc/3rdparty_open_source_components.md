# HAM 开源三方件清单

> 本文档梳理本仓库依赖的开源三方件、来源、许可证及引用方式。发布件 `libham.so` 不静态链接三方件；静态引用的三方件仅存在于 DT 可执行文件 `ham_dt` 中，不随产品分发。

## 1. 依赖总览

| 三方件 | 版本 | 许可证 | 来源 | 引用方式 | 使用范围 |
|--------|------|--------|------|----------|----------|
| libboundscheck | 系统安装版本 | MulanPSL-2.0 | openEuler 发行版软件包 `libboundscheck`（`yum install libboundscheck`） | 动态链接（`libboundscheck.so`） | 主库与 DT |
| googletest | 1.10.9004 | BSD-3-Clause | openEuler 社区开源件 [src-openeuler/googletest](https://gitcode.com/src-openeuler/googletest)（humble 分支，`ros-humble-gtest-vendor_1.10.9004.orig.tar.gz`） | **静态链接** | 仅 DT |
| mockcpp | v2.7 | Apache-2.0 | 开源项目 mockcpp，维持原有获取方式（git submodule / 手动放置于 `test/3rdparty/mockcpp`） | **静态链接** | 仅 DT |
| pthread | glibc | LGPL（glibc） | 操作系统自带 | 动态链接 | 仅 DT |

## 2. 静态引用的开源三方件

### 2.1 googletest

- **引入位置**：`test/3rdparty/CMakeLists.txt` 中 `EXTERNALPROJECT_ADD(gtest ...)`
- **获取方式**：构建 DT 时从 openEuler 社区开源仓库的 humble 分支下载 `ros-humble-gtest-vendor_1.10.9004.orig.tar.gz`，并校验 SHA256（`79b5f1bd841f84ac1b289ab362031cb18ef396fb6234ddf003af09bca3e050fa`）；也可将源码手动放置于 `test/3rdparty/googletest/`，构建时会优先使用本地源码
- **构建开关**：`-DBUILD_SHARED_LIBS=OFF -DINSTALL_GTEST=ON -DGOOGLETEST_VERSION=1.10.0`
- **链接产物**：`libgtest.a`（Debug 构建为 `libgtestd.a`）静态链接进 `ham_dt`
- **许可证**：BSD-3-Clause（源码包内 `LICENSE`）

### 2.2 mockcpp

- **引入位置**：`test/3rdparty/CMakeLists.txt` 中 `EXTERNALPROJECT_ADD(mockcpp ...)`
- **获取方式**：源码通过 git submodule / 手动放置于 `test/3rdparty/mockcpp/`；构建前应用 `mockcpp_support_arm64.patch`
- **链接产物**：`libmockcpp.a` 静态链接进 `ham_dt`
- **许可证**：Apache-2.0（源码内 `COPYING`）

## 3. 动态链接三方件

### 3.1 libboundscheck

- 来源：openEuler 发行版软件包 `libboundscheck`，提供 `/usr/lib64/libboundscheck.so` 和 `/usr/include/securec.h`
- `libham.so` 与 DT 可执行文件 `ham_dt` 均动态链接该库
- CMake 配置阶段通过 `find_path` 检查 `securec.h`，未安装时会给出明确错误

## 4. 发布件依赖结论

RPM 发布件 `libham.so` 动态链接 openEuler 开源件 `libboundscheck.so`，不存在静态链接的三方件。googletest 和 mockcpp 仅静态链接进测试可执行文件 `ham_dt`，不进入发布件。

## 5. 变更记录

| 日期 | 变更内容 |
|------|----------|
| 2026-08 | libboundscheck 移除 submodule，改用 openEuler 发行版软件包（动态链接） |
| 2026-08 | googletest 移除 submodule，切换为 openEuler 社区开源件，构建时自动拉取并校验 |
| 2026-08 | mockcpp 维持原有静态依赖方式 |
