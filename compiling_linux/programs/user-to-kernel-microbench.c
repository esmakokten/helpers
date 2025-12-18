#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include "stats.h"

static inline uint64_t rdtsc_serialized_start(void) {
    unsigned int a, d;
    // CPUID;RDTSC is nicely serialized
    asm volatile("cpuid" : : "a"(0) : "rbx","rcx","rdx");
    asm volatile("rdtsc" : "=a"(a), "=d"(d));
    return ((uint64_t)d<<32) | a;
}
static inline uint64_t rdtsc_serialized_end(void) {
    unsigned int a, d, c;
    asm volatile("rdtscp" : "=a"(a), "=d"(d), "=c"(c));
    asm volatile("lfence");
    return ((uint64_t)d<<32) | a;
}

// Define the IOCTL commands (must match the kernel module)
#define IOCTL_RUN_VMCALL   _IOW('v', 1, unsigned long)  // does vmcall
#define IOCTL_RUN_CPUID     _IOW('v', 2, unsigned long)  // does cpuid (fast path)
#define IOCTL_RUN_OUTB     _IOW('v', 3, unsigned long)  // does out 0xE9 from kernel

#define DEVICE_PATH "/dev/kvm-fake"

int main(int argc, char *argv[]) {
    int fd;
    int ret;

    const int N = (argc>1)?atoi(argv[1]):200000;

    uint64_t *s1 = aligned_alloc(64, N*sizeof(uint64_t));
    uint64_t *s2 = aligned_alloc(64, N*sizeof(uint64_t));
    uint64_t *s3 = aligned_alloc(64, N*sizeof(uint64_t));

    stats_t stats1, stats2, stats3;
    stats_init(&stats1, s1, N);
    stats_init(&stats2, s2, N);
    stats_init(&stats3, s3, N);

    printf("=== User to Kernel Microbenchmark ===\n");
    printf("Number of iterations: %d\n\n", N);

    // Open the device
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error: Failed to open device %s: %s\n", 
                DEVICE_PATH, strerror(errno));
        fprintf(stderr, "Make sure the kernel module is loaded.\n");
        fprintf(stderr, "Try: sudo insmod modules/mesurement-module.ko\n");
        return 1;
    }

    printf("Device opened successfully.\n\n");

    // Test 1: CPUID (Fast Path)
    printf("Running Test 1: CPUID instruction (fast path)...\n");
    for (int i=0;i<N;i++) {
        uint64_t t0 = rdtsc_serialized_start();
        ret = ioctl(fd, IOCTL_RUN_CPUID, N);
        uint64_t t1 = rdtsc_serialized_end();
        if (ret < 0) {
            fprintf(stderr, "Error: IOCTL_RUN_CPUID failed: %s\n", strerror(errno));
            close(fd);
            return 1;
        }
        stats_add_sample(&stats1, t1 - t0);
    }
    printf("  ✓ Completed\n\n");

    // Test 2: VMCALL
    printf("Running Test 2: VMCALL instruction...\n");
    for (int i=0;i<N;i++) {
        uint64_t t0 = rdtsc_serialized_start();
        ret = ioctl(fd, IOCTL_RUN_VMCALL, N);
        uint64_t t1 = rdtsc_serialized_end();
        if (ret < 0) {
            fprintf(stderr, "Error: IOCTL_RUN_VMCALL failed: %s\n", strerror(errno));
            close(fd);
            return 1;
        }
        stats_add_sample(&stats2, t1 - t0);
    }
    printf("  ✓ Completed\n\n");

    // Test 3: OUT instruction (Slow Path)
    printf("Running Test 3: OUT instruction to port 0xE9 (slow path)...\n");
    for (int i=0;i<N;i++) {
        uint64_t t0 = rdtsc_serialized_start();
        ret = ioctl(fd, IOCTL_RUN_OUTB, N);
        uint64_t t1 = rdtsc_serialized_end();
        if (ret < 0) {
            fprintf(stderr, "Error: IOCTL_RUN_OUTB failed: %s\n", strerror(errno));
            close(fd);
            return 1;
        }
        stats_add_sample(&stats3, t1 - t0);
    }
    printf("  ✓ Completed\n\n");

    stats_print_detailed(&stats1, "CPUID(user-kernel, fast)");
    stats_print_detailed(&stats2, "VMCALL(user-kernel, medium)");
    stats_print_detailed(&stats3, "OUT 0xE9(user-kernel, slow)");

    close(fd);
    return 0;
}
