/* 
    This benchmark measures:
        Guest user
            → store to MMIO
                → VMEXIT
                    → KVM kernel
                        → QEMU userspace (edu device)
                    → KVM kernel
                → VMRESUME
        → Guest user

    The edu device is a simple MMIO device in QEMU that just discards writes.
    The QEMU edu device needs to be enabled when starting QEMU:
        -device edu
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dirent.h>
#include <string.h>
#include <x86intrin.h>
#include "stats.h"

static inline uint64_t tsc_start(void) {
    unsigned a, d;
    asm volatile("cpuid" ::: "rax","rbx","rcx","rdx");
    asm volatile("rdtsc" : "=a"(a), "=d"(d));
    return ((uint64_t)d << 32) | a;
}

static inline uint64_t tsc_end(void) {
    unsigned a, d, c;
    asm volatile("rdtscp" : "=a"(a), "=d"(d), "=c"(c));
    asm volatile("lfence");
    return ((uint64_t)d << 32) | a;
}

static char *find_edu_resource(void) {
    static char path[512];
    DIR *d = opendir("/sys/bus/pci/devices");
    struct dirent *de;

    while ((de = readdir(d))) {
        snprintf(path, sizeof(path),
                 "/sys/bus/pci/devices/%s/vendor", de->d_name);
        FILE *f = fopen(path, "r");
        if (!f) continue;

        char buf[16];
        fgets(buf, sizeof(buf), f);
        fclose(f);

        if (strcmp(buf, "0x1234\n") == 0) {  // QEMU vendor ID
            snprintf(path, sizeof(path),
                     "/sys/bus/pci/devices/%s/device", de->d_name);
            f = fopen(path, "r");
            fgets(buf, sizeof(buf), f);
            fclose(f);

            if (strcmp(buf, "0x11e8\n") == 0) { // edu device ID
                snprintf(path, sizeof(path),
                         "/sys/bus/pci/devices/%s/resource0", de->d_name);
                closedir(d);
                return path;
            }
        }
    }
    closedir(d);
    return NULL;
}

int main(int argc, char **argv) {
    int N = (argc > 1) ? atoi(argv[1]) : 200000;

    char *res = find_edu_resource();
    if (!res) {
        fprintf(stderr, "edu device not found\n");
        return 1;
    }

    int fd = open(res, O_RDWR | O_SYNC);
    volatile uint32_t *mmio = mmap(NULL, 4096,
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    uint64_t *samples = aligned_alloc(64, N * sizeof(uint64_t));

    stats_t stats;
    stats_init(&stats, samples, N);

    for (int i = 0; i < N; i++) {
        uint64_t t0 = tsc_start();
        mmio[0x40 / 4] = 0xdeadbeef;   // arbitrary offset
        uint64_t t1 = tsc_end();
        stats_add_sample(&stats, t1 - t0);
    }

    stats_print_detailed(&stats, "MMIO(userspace)");

    free(samples);
    return 0;
}
