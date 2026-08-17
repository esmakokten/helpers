#!/bin/sh
echo "This is a placeholder initrd script."
echo "Replace this with your actual initrd initialization code."

# Create essential directories first
mkdir -p /dev /proc /sys /bin /modules /programs 

# Create console device node early (before mounting)
mknod -m 600 /dev/console c 5 1 2>/dev/null || true
mknod -m 666 /dev/null c 1 3 2>/dev/null || true

# Mount essential filesystems
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev 2>/dev/null || {
    echo "devtmpfs not available, device nodes created manually"
}

sleep 1

echo "Running programs..."

# Below are example commands to run your programs.
# Replace these with actual program invocations as needed.
/programs/rtdsc.o

# End of initrd script
echo "Programs complete. System will halt."
# Instead of exec /bin/init (which doesn't exist), just sleep
while true; do sleep 1000; done