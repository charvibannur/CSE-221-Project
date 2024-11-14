#include <mach/mach_time.h>
#include <stdio.h>

int main() {
    // Convert to nanoseconds
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    double nanos_per_tick = (double)timebase.numer / timebase.denom;
    
    // Measure overhead of mach_absolute_time()
    uint64_t start, end;
    uint64_t total = 0;
    int iterations = 1000;
    
    for (int i = 0; i < iterations; i++) {
        start = mach_absolute_time();
        end = mach_absolute_time();
        total += (end - start);
    }
    
    double avg_overhead_ns = (total * nanos_per_tick) / iterations;
    printf("Average overhead: %.2f nanoseconds\n", avg_overhead_ns);
    return 0;
}