# HAM介绍

确定性虚机热迁移基于灵衢内存池化能力将目的端虚机内存上线到源端os，并利用内核migrate_pages接口迁移内存，期间通过灵衢总线远端内存访问能力保活虚机；迁移中虚机数据无副本，可去除迭代清脏依赖，以实现确定性时长迁移。HAM作为确定性迁移关键组件，提供迁移任务管理、内存精准迁移能力。

# HAM插件安装步骤

## 前提条件
已安装OBMM、SMAP、RMRS、MXE。
## 安装步骤
1. 安装libvirt和QEMU相关包。

   ```bash
   yum install -y libvirt*
   yum install -y qemu*
   yum install -y edk2-aarch64.noarch
   ```

2. 配置QEMU和libvirt

   配置文件及配置步骤可参考相关组件,主要修改`/etc/libvirt/qemu.conf`,增加对应字符设备。
   ```c
   cgroup_device_acl = [
   "/dev/ham_migrate"
   ]
   ```

3. 安装HAM rpm包，如在安装SMAP时已执行3.5.2章节，可不执行此步骤。

   ```bash
   yum install -y ham
   ```
