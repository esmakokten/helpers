# ==========================
# Configuration
# ==========================
KERNEL_VERSION ?= 6.5.12
WORK ?= $(shell pwd)/tinylinux
KERNEL := tinylinux/out/vmlinuz-$(KERNEL_VERSION)
INITRD := busybox_initrd/out/initramfs.cpio.gz
CORES ?= 1

# QEMU runtime flags
QEMU_FLAGS =  -smp $(CORES) \
              -kernel $(KERNEL) -initrd $(INITRD) \
              -append "console=ttyS0" -nographic


.PHONY: all run kernel initrd clean reset

all: run

# Run QEMU with built kernel + initrd
run: programs 
	qemu-system-x86_64 -enable-kvm -cpu host $(QEMU_FLAGS)
run_naive: programs 
	qemu-system-x86_64 $(QEMU_FLAGS)

programs: $(KERNEL) $(INITRD)
	$(MAKE) -C programs



# ==========================
# Kernel build
# ==========================
$(KERNEL):
	./tinylinux.sh

kernel: $(KERNEL)

# ==========================
# Initrd build
# ==========================

$(INITRD):
	./busybox_initrd.sh
	@true

initrd: $(INITRD)

# ==========================
# Cleanup
# ==========================

clean: 
	$(MAKE) -C programs clean

reset:
	@echo ">>> Full reset"
	rm -rf tinylinux/
	rm -rf busybox_initrd/

