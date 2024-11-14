#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mach/mach_time.h>

#define ITERATIONS 1000  // Number of repetitions for each stride length
#define KB 1024
#define MB 1024 * KB
#define GB 1024 * MB

// Stride lengths to test, in bytes
const size_t strides[] = {128, 1 * KB, 4 * KB, 16 * KB, 512 * KB, 1 * MB, 4 * MB, 32 * MB};
const int num_strides = sizeof(strides) / sizeof(strides[0]);

uint64_t get_time_ns() {
    static mach_timebase_info_data_t timebase;
    if (timebase.denom == 0) {
        mach_timebase_info(&timebase);  // Get timebase information only once
    }
    uint64_t time = mach_absolute_time();
    return time * timebase.numer / timebase.denom;  // Convert ticks to nanoseconds
}

void measure_latency(size_t size, size_t stride, FILE *output) {
    int *array = (int *)malloc(size);  // Allocate memory region
    if (!array) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    size_t steps = size / sizeof(int);
    volatile int sink;  // Prevents compiler optimizations
    uint64_t total_latency = 0;

    for (size_t i = 0; i < ITERATIONS; i++) {
        uint64_t start = get_time_ns();
        sink = array[(i * (stride / sizeof(int))) % steps];  // Access with the specified stride
        uint64_t end = get_time_ns();
        
        total_latency += (end - start);  // Accumulate latency for averaging
    }
    
    double average_latency = (double)total_latency / ITERATIONS;
    fprintf(output, "%zu,%zu,%f\n", size, stride, average_latency);  // Write array size, stride, and average latency
    
    free((void *)array);
}

int main() {
    FILE *output = fopen("latency_results_average.csv", "w");
    if (!output) {
        perror("File open failed");
        return EXIT_FAILURE;
    }

    // Write header row
    fprintf(output, "Size(Bytes),Stride(Bytes),Average_Latency(ns)\n");

    // Start from 32 KB and go up to 1 GB with ~1.5x increments each time
    for (size_t size = 32 * KB; size <= 1 * GB; size = (size * 3) / 2) {
        for (int s = 0; s < num_strides; s++) {
            measure_latency(size, strides[s], output);
        }
    }

    fclose(output);
    printf("Average latency measurement completed. Results saved to latency_results_average.csv\n");
    return 0;
}