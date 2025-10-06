
# Build a minimal Linux kernel with Busybox initramfs

This guide will help you compile a minimal Linux kernel with Busybox as the initramfs.

## Prerequisites

- A Linux machine with the necessary build tools installed (gcc, make, etc.). Otherwise you'll get errors while compiling the kernel.
- Basic knowledge of Linux kernel configuration and compilation

## Scripts
- `tinylinux.sh`: Script to compile the Linux kernel.
- `busybox_initrd.sh`: Script to create a minimal initramfs using Busybox

### Linux Kernel Compilation

**`tinylinux.sh`**
1. Download and extract the Linux kernel source code.
2. make a minimal configuration using `make defconfig`.
```
TODO: Here we can use `make tinyconfig` for smallest kernel. Then manually enable some options. I started to build up a list of essential options to enable:
    > Enable 64-bit kernel
    > Device Drivers > Character devices > Enable the TTY for console support
    > General setup > Enable initramfs
    > General setup > Configure standard kernel features (expert users) > Enable support for printk

This work goes into understanding linux kernel configuration better, leave it for now. and use default config `make defconfig` for now.
```
3. Modify the kernel configuration to include necessary options (see below).
4. Compile the kernel using `make -j$(nproc)`.
5. Copy the compiled kernel image to the output directory.

### Initramfs with Busybox

As initramfs and init process we are going to use Busybox. Busybox is a software that provides several stripped-down Unix tools in a single executable file. It is often used in embedded Linux systems due to its small size.
Example: `/bin/busybox sh` will start a shell, `/bin/busybox ls` will list files, etc.

**`busybox_initrd.sh`**
1. Download and extract the Busybox source code. Note that the original download link seems to be down, so we are using an alternative link from the Busybox git repository.
2. Configure Busybox to build a static binary using `make defconfig` and modifying the configuration.
3. Compile Busybox using `make -j$(nproc)`.
4. Create a minimal root filesystem structure:
    - `/bin`
    - `/proc`
    - `/sys`
    - `/dev`
5. To get rid of `busybox command` prefix, create symlinks for each Busybox applet in `/bin`.
6. Create an `init` script that will be executed by the kernel during boot. The `init` script should:
    - Mount necessary filesystems (`proc`, `sysfs`, `devtmpfs`).
    - Start a shell or any other process.
8. Create the initramfs image using `cpio` and compress it with `gzip`.
9. Copy the initramfs image to the output directory.

### Booting the Kernel
To try out the build, you can boot the compiled kernel with the initramfs using QEMU:
```bash
qemu-system-x86_64 -kernel tinylinux/out/vmlinuz-6.5.12 -initrd busybox_initrd/out/initramfs.cpio.gz -append "console=ttyS0" -nographic
```