# Simple Makefile for BusyBox initramfs with custom programs

INITRD = ../busybox_initrd/initrd
PROGRAMS_DIR = .
OUT = ../busybox_initrd/out/initramfs.cpio.gz

# Default: build everything
all: $(OUT)

# Build initramfs
$(OUT): busybox programs
	cd $(INITRD) && find . | cpio -o -H newc | gzip > ../out/initramfs.cpio.gz
	@echo "Built: $(OUT)"

# Build BusyBox (only if needed)
busybox:
	@if [ ! -f $(INITRD)/bin/busybox ]; then \
		echo "Building BusyBox..."; \
		./busybox_initrd.sh; \
	fi

# Compile programs
$(PROGRAMS_DIR)/%.c:
 	
	@mkdir -p $(INITRD)/programs
	@for f in $(PROGRAMS_DIR)/*.c; do \
		if [ -f "$$f" ]; then \
			name=$$(basename "$$f" .c); \
			echo "Compiling $$name..."; \
			gcc -static -o "$$name.o" "$$f"; \
			cp "$$name.o" $(INITRD)/programs/; \
		fi; \
	done

programs:$(PROGRAMS_DIR)/%.c
# Clean everything
clean:
	-rm -rf $(INITRD)
	-rm $(OUT)
	-rm *.s
	-rm *.o
.PHONY: all busybox programs clean
