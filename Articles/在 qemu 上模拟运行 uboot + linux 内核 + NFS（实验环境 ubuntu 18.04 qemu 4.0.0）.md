实验环境 ubuntu 18.04 qemu 4.0.0

本文只是记录在 ubuntu 18.04 qemu 4.0.0 版本下搭建调试环境时的重要步骤和出现的一些 bug，并没有详细地描述整个构建细节。下载 linux 内核与 uboot 源码等细节可参考其他介绍文档。

## 1. 在 qemu 上模拟运行 linux 内核

### 非图形化启动
```
qemu-system-arm -M \
        vexpress-a9 -m 512M \
        -dtb linux-4.20.17/arch/arm/boot/dts/vexpress-v2p-ca9.dtb \
        -kernel linux-4.20.17/arch/arm/boot/zImage \
        -nographic \
        -append "console=ttyAMA0" 
```

### 图形化启动
- 4.0.0 版本直接启动会出现图形界面没有显示的情况，原因是 SDL 库的版本不够新，4.0.0 版本需要安装 libsdl2-dev - Simple DirectMedia Layer development files 版本，1.2 版本已经不能用了，导致 qemu 配置时 SDL 支持为 no ，安装 2 版本后，再进行配置就会发现 SDL 支持为 yes，重新安装即可。

- 参考帖子 [在Ubuntu下安装QEMU完毕后去测试arm，结果只输出一行VNC](https://www.crifan.com/qemu_test_arm_vnc_server_running_on_127_0_0_1_5900_no_other_output/)

```
qemu-system-arm -M \
        vexpress-a9 -m 512M \
        -dtb linux-4.20.17/arch/arm/boot/dts/vexpress-v2p-ca9.dtb \
        -kernel linux-4.20.17/arch/arm/boot/zImage \
        -append "root=/dev/mmcblk0 rw console=tty0" \
        -sd rootfs.ext3
```

## 2. 制作根文件系统

mkdir rootfs
mkdir rootfs/lib
cp –r _install/* rootfs
cp -p /usr/arm-linux-gnueabi/lib/* rootfs/lib
mkdir -p rootfs/dev/
mknod –m 666 tty1 c 4 1
mknod –m 666 tty2 c 4 2
mknod –m 666 tty3 c 4 3
mknod –m 666 tty4 c 4 4
mknod –m 666 console c 5 1
mknod –m 666 null c 1 3

### 添加开机启动文件 rcS
echo '------------------------------------'
echo 'Hello SummerGift!'
echo '------------------------------------'

## 3. u-boot 配置编译与启动配置

### 3.1 重新编译内核为 uImage，修改装载地址

make LOADADDR=0x60003000 uImage -j4

### 3.2 修改启动命令，并重新编译 uboot
```
#undef CONFIG_BOOTCOMMAND

#define CONFIG_BOOTCOMMAND \
"tftp 0x60003000 uImage;tftp 0x60500000 vexpress-v2p-ca9.dtb; \
setenv bootargs 'root=/dev/mmcblk0 console=ttyAMA0'; \
bootm 0x60003000 - 0x60500000; "

#define CONFIG_IPADDR 192.168.1.66
#define CONFIG_NETMASK 255.255.255.0
#define CONFIG_SERVERIP 192.168.1.198
```

### 3.3 启动 uboot 运行
```
qemu-system-arm -M vexpress-a9 \
-kernel u-boot \
-nographic \
-m 512M
```

### 3.4 uboot 通过 tftp 下载内核后启动脚本
- 实际测试 4.0.0 版本会报错 vlan 参数无效，实际修改为下面的启动参数即可
```
    -net nic \
    -net tap,ifname=tap0 \
```

- 全部启动参数
```
cp   /home/summergift/linux_arm/linux-4.20.17/arch/arm/boot/uImage /home/summergift/linux_arm/tftpboot
cp   /home/summergift/linux_arm/linux-4.20.17/arch/arm/boot/dts/vexpress-v2p-ca9.dtb /home/summergift/linux_arm/tftpboot
cp   /home/summergift/linux_arm/rootfs.ext3 /home/summergift/linux_arm/tftpboot

qemu-system-arm \
    -M vexpress-a9 \
    -kernel u-boot-2019.04/u-boot \
    -m 512M \
    -nographic \
    -net nic \
    -net tap,ifname=tap0 \
    -sd rootfs.ext3
```

如果想要图形化启动则去掉 -nographic 参数，并且修改启动命令中的 ttyAMA0 为 tty0。

## 4. 使用 nfs 挂载根文件系统

### 4.1 修改 uboot 启动参数并重新编译

### 4.2 开启内核 nfs 功能并重新编译

VFS: Unable to mount root fs via NFS, trying floppy.
VFS: Cannot open root device "nfs" or unknown-block(2,0): error -6
Please append a correct "root=" boot option; here are the available partitions:
1f00          131072 mtdblock0 
 (driver?)
1f01           32768 mtdblock1 
 (driver?)
b300           32768 mmcblk0 
 driver: mmcblk
Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(2,0)
CPU: 0 PID: 1 Comm: swapper/0 Not tainted 4.20.17 #3
Hardware name: ARM-Versatile Express
[<80110914>] (unwind_backtrace) from [<8010c80c>] (show_stack+0x10/0x14)
[<8010c80c>] (show_stack) from [<80705078>] (dump_stack+0x88/0x9c)
[<80705078>] (dump_stack) from [<801207ac>] (panic+0x110/0x27c)
[<801207ac>] (panic) from [<80a01598>] (mount_block_root+0x1ec/0x2d8)
[<80a01598>] (mount_block_root) from [<80a017c8>] (mount_root+0x144/0x160)
[<80a017c8>] (mount_root) from [<80a01934>] (prepare_namespace+0x150/0x198)
[<80a01934>] (prepare_namespace) from [<80a01180>] (kernel_init_freeable+0x340/0x354)
[<80a01180>] (kernel_init_freeable) from [<8071bbb0>] (kernel_init+0x8/0x114)
[<8071bbb0>] (kernel_init) from [<801010e8>] (ret_from_fork+0x14/0x2c)
Exception stack(0x9f48ffb0 to 0x9f48fff8)
ffa0:                                     00000000 00000000 00000000 00000000
ffc0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
ffe0: 00000000 00000000 00000000 00000000 00000013 00000000
---[ end Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(2,0) ]---


### 4.3 从网上搜索的解决办法

在 nfsroot 配置项后面加上 proto=tcp,nfsvers=3,nolock 即可
```
#undef CONFIG_BOOTCOMMAND

#define CONFIG_BOOTCOMMAND \
"tftp 0x60003000 uImage;tftp 0x60500000 vexpress-v2p-ca9.dtb; \
setenv bootargs 'root=/dev/nfs rw \
nfsroot=192.168.1.198:/home/summergift/linux_arm/rootfs,proto=tcp,nfsvers=3,nolock init=/linuxrc \
ip=192.168.1.66 console=tty0'; \
bootm 0x60003000 - 0x60500000; "

#define CONFIG_IPADDR 192.168.1.66
#define CONFIG_NETMASK 255.255.255.0
#define CONFIG_SERVERIP 192.168.1.198
```

```
sudo mount -t nfs 192.168.1198:/home/summergift/linux_arm/rootfs /nfs_mount -o nolock
```

### 4.4 图形化启动 uboot + kernel + rootfs + nfs

- uboot 启动参数
```
#undef CONFIG_BOOTCOMMAND

#define CONFIG_BOOTCOMMAND \
"tftp 0x60003000 uImage;tftp 0x60500000 vexpress-v2p-ca9.dtb; \
setenv bootargs 'root=/dev/nfs rw \
nfsroot=192.168.1.198:/home/summergift/linux_arm/rootfs,proto=tcp,nfsvers=3,nolock init=/linuxrc \
ip=192.168.1.66 console=tty0'; \
bootm 0x60003000 - 0x60500000; "

#define CONFIG_IPADDR 192.168.1.66
#define CONFIG_NETMASK 255.255.255.0
#define CONFIG_SERVERIP 192.168.1.198
```

- qemu 启动参数项
```
make -C /home/summergift/linux_arm/linux-4.20.17 LOADADDR=0x60003000 uImage -j4
cp   /home/summergift/linux_arm/linux-4.20.17/arch/arm/boot/uImage /home/summergift/linux_arm/tftpboot
cp   /home/summergift/linux_arm/linux-4.20.17/arch/arm/boot/dts/vexpress-v2p-ca9.dtb /home/summergift/linux_arm/tftpboot
cp   /home/summergift/linux_arm/rootfs.ext3 /home/summergift/linux_arm/tftpboot

qemu-system-arm \
    -M vexpress-a9 \
    -kernel u-boot-2019.04/u-boot \
    -m 512M \
    -net nic \
    -net tap,ifname=tap0  \
    -sd rootfs.ext3
```

### linux 启动顺序

init->linuxrc->inittab->fstab->console

各种文件系统挂载之后就可以通过 cat 命令来查看系统信息了。