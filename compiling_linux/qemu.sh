#!/bin/bash
# This script is to trigger qemu with the right arguments
set -euo pipefail

# --- Versions / paths ---------------------------------------------------------
KERNEL_VERSION="${KERNEL_VERSION:-6.5.12}"
KERNEL_IMAGE="tinylinux/out/vmlinuz-$KERNEL_VERSION"
INITRD_IMAGE="busybox_initrd/out/initramfs.cpio.gz"

# --- Echo vars ---
echo "KERNEL_VERSION: $KERNEL_VERSION"
echo "KERNEL_IMAGE: $KERNEL_IMAGE"
echo "INITRD_IMAGE: $INITRD_IMAGE"

# --- Run QEMU -----------------------------------------------------------------

# Taskset to bind QEMU to a specific CPU core (optional)
#TASKSET_CMD=("taskset" -cpu-list 1) # Bind to CPU core 1

# QEMU options
QEMU_OPTS=()
QEMU_OPTS+=("-enable-kvm")
QEMU_OPTS+=("-cpu" "host")
QEMU_OPTS+=("-smp" "2")
QEMU_OPTS+=("-debugcon" "file:debugcon.log" "-global" "isa-debugcon.iobase=0xe9")



# Launch QEMU
echo "Launching QEMU: Command:"
cat << EOF
${TASKSET_CMD[@]}
qemu-system-x86_64
    -kernel $KERNEL_IMAGE
    -initrd $INITRD_IMAGE
    -append "console=ttyS0"
    -nographic
    ${QEMU_OPTS[@]}
    $@
EOF

"${TASKSET_CMD[@]}" \
qemu-system-x86_64 \
    -kernel "$KERNEL_IMAGE" \
    -initrd "$INITRD_IMAGE" \
    -append "console=ttyS0 acpi.debug_level=ACPI_DEBUG smp.debug_level=SMP_DEBUG ignore_loglevel" \
    -nographic \
    "${QEMU_OPTS[@]}" \
    "$@"
