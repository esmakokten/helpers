#!/bin/bash

set -euo pipefail

# --- Versions / paths ---------------------------------------------------------
KERNEL_VERSION="${KERNEL_VERSION:-6.5.12}"
WORK="${WORK:-$PWD/tinylinux}"
KERNEL_BUILD="$WORK/linux-$KERNEL_VERSION"
OUT="$WORK/out"

# --- Echo vars ---
echo "KERNEL_VERSION: $KERNEL_VERSION"
echo "WORK: $WORK"
echo "KERNEL_BUILD: $KERNEL_BUILD"
echo "OUT: $OUT"

mkdir -p "$WORK" "$OUT"

# --- Fetch Linux kernel -------------------------------------------------------
cd "$WORK"
if [ ! -f "linux-$KERNEL_VERSION.tar.xz" ]; then
    curl -L -o "linux-$KERNEL_VERSION.tar.xz" \
        "https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-$KERNEL_VERSION.tar.xz"
fi
if [ ! -d "$KERNEL_BUILD" ]; then
    tar xf "linux-$KERNEL_VERSION.tar.xz"
fi
# --- Configure Linux kernel ---------------------------------------------------
cd "$KERNEL_BUILD"
make mrproper
make defconfig
# TODO: Use tinyconfig and enable options manually 
# make tinyconfig
# scripts/config --file .config \
#   -e CONFIG_BLK_DEV_INITRD \
#   -e CONFIG_INITRAMFS_SOURCE="$WORK/initrd" \
# ...

# --- Build Linux kernel -------------------------------------------------------
make -j"$(nproc)" || exit 1
# --- Copy kernel image --------------------------------------------------------
cp arch/x86/boot/bzImage "$OUT/vmlinuz-$KERNEL_VERSION"
