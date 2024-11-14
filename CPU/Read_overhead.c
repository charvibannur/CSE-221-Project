#include <mach/mach_time.h>
#include <stdio.h>
#include <stdint.h>

#define NUM_ITERATIONS 1000  // Number of measurements to perform

int main() {
    // Get timebase info for conversion to nanoseconds
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    double nanos_per_tick = (double)timebase.numer / timebase.denom;
    
    uint64_t total_overhead = 0;
    uint64_t min_overhead = UINT64_MAX;
    uint64_t max_overhead = 0;
    
    // Warm up the CPU
    for (int i = 0; i < 1000; i++) {
        mach_absolute_time();
    }
    
    // Measure mach_absolute_time overhead
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        uint64_t start = mach_absolute_time();
        uint64_t end = mach_absolute_time();
        
        uint64_t overhead = end - start;
        total_overhead += overhead;
        
        // Track min and max overhead
        if (overhead < min_overhead) min_overhead = overhead;
        if (overhead > max_overhead) max_overhead = overhead;
    }
    
    // Calculate average overhead in both ticks and nanoseconds
    double avg_overhead_ticks = (double)total_overhead / NUM_ITERATIONS;
    double avg_overhead_ns = avg_overhead_ticks * nanos_per_tick;
    double min_overhead_ns = min_overhead * nanos_per_tick;
    double max_overhead_ns = max_overhead * nanos_per_tick;
    
    // Print results
    printf("mach_absolute_time() overhead measurements:\n");
    printf("Number of iterations: %d\n", NUM_ITERATIONS);
    printf("Average overhead: %.2f ticks (%.2f ns)\n", avg_overhead_ticks, avg_overhead_ns);
    printf("Minimum overhead: %lu ticks (%.2f ns)\n", min_overhead, min_overhead_ns);
    printf("Maximum overhead: %lu ticks (%.2f ns)\n", max_overhead, max_overhead_ns);
    
    return 0;
}