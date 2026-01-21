#include <stdint.h>
#include <stdio.h>
#include "stats.h"

static inline void measured_function(uint64_t *var)
{
   
}

static inline uint64_t measure_start(void)
{
    unsigned cycles_low, cycles_high;
    asm volatile(
        "CPUID\n\t"
        "RDTSC\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t" : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx", "%rdx");
    return (((uint64_t)cycles_high << 32) | cycles_low);
}

static inline uint64_t measure_end(void)
{
    unsigned cycles_low, cycles_high;
    asm volatile("RDTSCP\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 "CPUID\n\t" : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx", "%rdx");
    return (((uint64_t)cycles_high << 32) | cycles_low);
}

#define MEASURE_COUNT 1000000

int main(void)
{
    uint64_t start, end;
    uint64_t variable = 0;
    uint64_t measurements[MEASURE_COUNT];

    stats_t stats;
    stats_init(&stats, measurements, MEASURE_COUNT);

    for (int i = 0; i < MEASURE_COUNT; i++)
    {
        start = measure_start();
        measured_function(&variable);
        end = measure_end();
        stats_add_sample(&stats, end - start);
    }

    stats_print_detailed(&stats, "measured_function");

    return 0;
}