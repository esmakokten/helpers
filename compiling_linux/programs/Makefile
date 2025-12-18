# Simple Makefile for BusyBox initramfs with custom programs
KERNEL_VERSION = 6.5.12
INITRD = ../busybox_initrd/initrd
PROGRAMS_DIR = .
KERNEL_BUILD = ../tinylinux/linux-$(KERNEL_VERSION)
OUT = ../busybox_initrd/out/initramfs.cpio.gz

# Default: build everything
all: $(OUT)

# Build initramfs
$(OUT): busybox programs ko scripts
	cd $(INITRD) && find . | cpio -o -H newc | gzip > ../out/initramfs.cpio.gz
	@echo "Built: $(OUT)"

# Build BusyBox (only if needed)
busybox:
	@if [ ! -f $(INITRD)/bin/busybox ]; then \
		echo "Building BusyBox..."; \
		./busybox_initrd.sh; \
	fi

# Compile programs
programs:
	@mkdir -p $(INITRD)/programs
	@for f in $(PROGRAMS_DIR)/*.c; do \
		if [ -f "$$f" ]; then \
			name=$$(basename "$$f" .c); \
			echo "Compiling $$name..."; \
			gcc -static -o "$$name.o" "$$f" -lm; \
			cp "$$name.o" $(INITRD)/programs/; \
		fi; \
	done

# Compile modules 
ko:
# Create Kbuild file to include only .c files and skip .mod.c files 
	@mkdir -p $(INITRD)/modules
	@echo "# Auto-generated Kbuild" > $(PROGRAMS_DIR)/modules/Kbuild 
	@for f in $(PROGRAMS_DIR)/modules/*.c; do \
		if [ -f "$$f" ]; then \
			name=$$(basename "$$f" .c); \
			case "$$name" in *.mod) continue ;; esac; \
			echo "obj-m += $$name.o" >> $(PROGRAMS_DIR)/modules/Kbuild; \
		fi; \
	done
	@# Build all modules at once
	if [ -f $(PROGRAMS_DIR)/modules/Kbuild ]; then \
		make -C "$(KERNEL_BUILD)" M="$(PWD)/$(PROGRAMS_DIR)/modules" modules; \
		cp $(PROGRAMS_DIR)/modules/*.ko $(INITRD)/modules/ 2>/dev/null || true; \
	fi

scripts:
	@echo "Copying scripts..."
	@mkdir -p $(INITRD)/scripts
	@cp $(PROGRAMS_DIR)/scripts/* $(INITRD)/scripts/ || :

# Clean everything
clean:
	rm -rf busybox_initrd
	rm -f $(PROGRAMS_DIR)/*.o
	rm -rf $(PROGRAMS_DIR)/modules/*.o $(PROGRAMS_DIR)/modules/*.ko $(PROGRAMS_DIR)/modules/*.mod $(PROGRAMS_DIR)/modules/*.mod.c $(PROGRAMS_DIR)/modules/*.mod.o
	rm -f $(PROGRAMS_DIR)/modules/Kbuild $(PROGRAMS_DIR)/modules/Module.symvers $(PROGRAMS_DIR)/modules/modules.order
	rm -f $(PROGRAMS_DIR)/modules/.*.cmd

.PHONY: all busybox programs ko scripts clean