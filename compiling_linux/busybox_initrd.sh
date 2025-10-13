#!/bin/bash

set -euo pipefail

# --- Versions / paths ---------------------------------------------------------
BUSYBOX_VERSION="${BUSYBOX_VERSION:-1_37_0}"
WORK="${WORK:-$PWD/busybox_initrd}"
BB_BUILD="$WORK/busybox-$BUSYBOX_VERSION"
OUT="$WORK/out"

# --- Echo vars ---
echo "BUSYBOX_VERSION: $BUSYBOX_VERSION"
echo "WORK: $WORK"
echo "BB_BUILD: $BB_BUILD"
echo "OUT: $OUT"

mkdir -p "$WORK" "$OUT"

# --- Fetch BusyBox ------------------------------------------------------------
# TODO: Original link seems down. "https://busybox.net/downloads/busybox-$BUSYBOX_VERSION.tar.bz2"
cd "$WORK"
if [ ! -f "busybox-$BUSYBOX_VERSION.tar.bz2" ]; then
  curl -L -o "busybox-$BUSYBOX_VERSION.tar.bz2" \
    "https://git.busybox.net/busybox/snapshot/busybox-$BUSYBOX_VERSION.tar.bz2" 
fi
if [ ! -d "$BB_BUILD" ]; then
  tar xf "busybox-$BUSYBOX_VERSION.tar.bz2"
fi

# --- Configure BusyBox (static) ----------------------------------------------
cd "$BB_BUILD"
make distclean >/dev/null 2>&1 || true
make defconfig

# Enable static build by modifying .config directly
sed -i 's/# CONFIG_STATIC is not set/CONFIG_STATIC=y/' .config

# Disable problematic networking tools that have compilation issues
sed -i 's/CONFIG_TC=y/# CONFIG_TC is not set/' .config

# Ensure we have the configuration we want
make oldconfig < /dev/null

# --- Build & install BusyBox --------------------------------------------------
make -j"$(nproc)"

# --- Minimal rootfs layout ----------------------------------------------------
rm -rf "$WORK/initrd"
make install CONFIG_PREFIX="$WORK/initrd"
cd "$WORK"/initrd
mkdir -p etc proc sys dev

# --- Simple /etc/inittab (BusyBox init handles everything) -------------------
cat > etc/inittab <<'EOF'
::sysinit:/bin/mount -t proc proc /proc
::sysinit:/bin/mount -t sysfs sysfs /sys  
::sysinit:/bin/mount -t devtmpfs devtmpfs /dev
::respawn:-/bin/sh
EOF

# Create minimal /init script that calls BusyBox init
cat > init <<'EOF'
#!/bin/sh
exec /sbin/init
EOF
chmod +x init

# --- Pack initramfs -----------------------------------------------------------
cd "$WORK/initrd"
find . -print0 \
 | cpio --null -ov --format=newc \
 | gzip -9 > "$OUT/initramfs.cpio.gz"

echo "Built initramfs at: $OUT/initramfs.cpio.gz"
echo "Rootfs size:"
du -sh "$WORK/initrd"

