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

echo "Loading kernel modules..."
insmod /modules/fake-module.ko
insmod /modules/mesurement-module.ko

# Give the system a moment to create device nodes
sleep 1

# Create device nodes manually if they don't exist
# First, try to create /tmp for temporary files
mkdir -p /tmp

if [ ! -c /dev/kvm-microbench ]; then
    echo "Creating /dev/kvm-microbench manually"
    # Use cut instead of awk for more reliable field extraction
    MAJOR=$(grep 'kvm-microbench' /proc/devices | cut -d' ' -f1 | tr -d ' ')
    echo "Extracted major number: '$MAJOR' (length: ${#MAJOR})"
    if [ -n "$MAJOR" ]; then
        echo "Creating device node with major number $MAJOR"
        mknod /dev/kvm-microbench c $MAJOR 0 && echo "mknod succeeded" || echo "mknod failed: $?"
        chmod 666 /dev/kvm-microbench
        if [ -c /dev/kvm-microbench ]; then
            echo "SUCCESS: Device node created"
            ls -l /dev/kvm-microbench
        else
            echo "FAILED: Device node not created"
        fi
    else
        echo "ERROR: Could not extract valid major number"
        # Fallback: try common major numbers
        echo "Trying fallback major number 253"
        mknod /dev/kvm-microbench c 253 0 2>/dev/null
        chmod 666 /dev/kvm-microbench 2>/dev/null
    fi
fi

if [ ! -c /dev/kvm-fake ]; then
    echo "Creating /dev/kvm-fake manually"
    # Use cut instead of awk for more reliable field extraction
    MAJOR=$(grep 'kvm-fake' /proc/devices | cut -d' ' -f1 | tr -d ' ')
    echo "Extracted major number: '$MAJOR' (length: ${#MAJOR})"
    if [ -n "$MAJOR" ]; then
        echo "Creating device node with major number $MAJOR"
        mknod /dev/kvm-fake c $MAJOR 0 && echo "mknod succeeded" || echo "mknod failed: $?"
        chmod 666 /dev/kvm-fake
        if [ -c /dev/kvm-fake ]; then
            echo "SUCCESS: Device node created"
            ls -l /dev/kvm-fake
        else
            echo "FAILED: Device node not created"
        fi
    else
        echo "ERROR: Could not extract valid major number"
        # Fallback: try common major numbers
        echo "Trying fallback major number 254"
        mknod /dev/kvm-fake c 254 0 2>/dev/null
        chmod 666 /dev/kvm-fake 2>/dev/null
    fi
fi

# Verify device nodes
echo "Checking device nodes..."
ls -l /dev/kvm-* 2>/dev/null || echo "Warning: device nodes not found"

echo "Running benchmarks..."
#/programs/kernel-space-microbench.o
#/programs/user-space-microbench.o
#/programs/user-to-kernel-microbench.o
/programs/vmexit-vmresume-microbench.o

# End of initrd script
echo "Benchmarks complete. System will halt."
# Instead of exec /bin/init (which doesn't exist), just sleep
while true; do sleep 1000; done