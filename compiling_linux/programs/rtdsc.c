#include <stdint.h>
#include <stdio.h>
#include <sys/io.h>
static inline void measured_function_outb(uint64_t *var)
{
	__asm__ __volatile__ (
	"outb %b0, %w1"           // Assembly instruction: outb source, destination
	:                       // No output operands
	: "a"('T'), "Nd"(0xe9) // Input operands: %0 gets 'data' in 'a' register, %1 gets 0xe9 in 'd' register
    	: "memory"
   );
}
static inline void measured_function_cpuid(uint64_t *var)
{
	__asm__ __volatile__ (
   		"cpuid"
	);
}
#define MEASURED_FUNCTION measured_function_cpuid

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
    iopl(3);
    uint64_t start, end;
    uint64_t variable = 0;
    uint64_t measurements[MEASURE_COUNT];
    
    //Warmup the cache
    for (int i = 0; i < MEASURE_COUNT; i++)
    {
        start = measure_start();
        MEASURED_FUNCTION(&variable);
        end = measure_end();
        measurements[i] = end - start;
    }
    
 
    //Test for real
    for (int i = 0; i < MEASURE_COUNT; i++)
    {
        start = measure_start();
        MEASURED_FUNCTION(&variable);
        end = measure_end();
	measurements[i] = end - start;
    }

    uint64_t total = 0;
    for (int i = 0; i < MEASURE_COUNT; i++)
    {
        total += measurements[i];
    }
    double average = (double)total / MEASURE_COUNT;
    printf("\n\nAverage cycles for measured_function: %.2f\n", average);
    printf("All measurements:\n");
    for (int i = 0; i < MEASURE_COUNT; i++)
    {
        printf("%lu", measurements[i]);
        if (i < MEASURE_COUNT - 1)
            printf(", ");
        if ((i + 1) % 10 == 0)
            printf("\n");  
    }

    return 0;
}
