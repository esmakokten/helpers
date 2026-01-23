#!/bin/sh
echo "This is a placeholder initrd script."
echo "Replace this with your actual initrd initialization code."

# Create essential directories first
mkdir -p /dev /proc /sys /bin /modules /programs 

# Mount essential filesystems
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev 2>/dev/null || {
    echo "devtmpfs not available, will create device nodes manually"
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