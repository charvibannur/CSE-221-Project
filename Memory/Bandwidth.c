#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <mach/mach_time.h>

// Define array size (make it large enough to exceed cache)
#define ARRAY_SIZE (64 * 1024 * 1024)  // 64MB
#define ITERATIONS 100
#define UNROLL_FACTOR 64

// Timer functions for macOS
static inline uint64_t get_time(void) {
    return mach_absolute_time();
}

double get_elapsed_time(uint64_t start, uint64_t end) {
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    uint64_t elapsed = end - start;
    return (double)elapsed * timebase.numer / timebase.denom / 1e9; // Convert to seconds
}

// Memory fence to prevent reordering
static inline void memory_fence(void) {
    __sync_synchronize();
}

// Function to measure read bandwidth
double measure_read_bandwidth(int* arr, size_t size) {
    uint64_t start, end;
    volatile int sink = 0; // Prevent optimization
    
    memory_fence();
    start = get_time();
    
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < size; i += UNROLL_FACTOR) {
            sink += arr[i];
            sink += arr[i + 1];
            sink += arr[i + 2];
            sink += arr[i + 3];
            sink += arr[i + 4];
            sink += arr[i + 5];
            sink += arr[i + 6];
            sink += arr[i + 7];
            sink += arr[i + 8];
            sink += arr[i + 9];
            sink += arr[i + 10];
            sink += arr[i + 11];
            sink += arr[i + 12];
            sink += arr[i + 13];
            sink += arr[i + 14];
            sink += arr[i + 15];
            sink += arr[i + 16];
            sink += arr[i + 17];
            sink += arr[i + 18];
            sink += arr[i + 19];
            sink += arr[i + 20];
            sink += arr[i + 21];
            sink += arr[i + 22];
            sink += arr[i + 23];
            sink += arr[i + 24];
            sink += arr[i + 25];
            sink += arr[i + 26];
            sink += arr[i + 27];
            sink += arr[i + 28];
            sink += arr[i + 29];
            sink += arr[i + 30];
            sink += arr[i + 31];
            sink += arr[i + 32];
            sink += arr[i + 33];
            sink += arr[i + 34];
            sink += arr[i + 35];
            sink += arr[i + 36];
            sink += arr[i + 37];
            sink += arr[i + 38];
            sink += arr[i + 39];
            sink += arr[i + 40];
            sink += arr[i + 41];
            sink += arr[i + 42];
            sink += arr[i + 43];
            sink += arr[i + 44];
            sink += arr[i + 45];
            sink += arr[i + 46];
            sink += arr[i + 47];
            sink += arr[i + 48];
            sink += arr[i + 49];
            sink += arr[i + 50];
            sink += arr[i + 51];
            sink += arr[i + 52];
            sink += arr[i + 53];
            sink += arr[i + 54];
            sink += arr[i + 55];
            sink += arr[i + 56];
            sink += arr[i + 57];
            sink += arr[i + 58];
            sink += arr[i + 59];
            sink += arr[i + 60];
            sink += arr[i + 61];
            sink += arr[i + 62];
            sink += arr[i + 63];
        }
    }
    
    memory_fence();
    end = get_time();
    
    double elapsed = get_elapsed_time(start, end);
    double bytes_processed = (double)size * sizeof(int) * ITERATIONS;
    return bytes_processed / elapsed; // Return bytes per second
}

// Function to measure write bandwidth
double measure_write_bandwidth(int* arr, size_t size) {
    uint64_t start, end;
    int value = 42;
    
    memory_fence();
    start = get_time();
    
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < size; i += UNROLL_FACTOR) {
            arr[i] = value;
            arr[i + 1] = value;
            arr[i + 2] = value;
            arr[i + 3] = value;
            arr[i + 4] = value;
            arr[i + 5] = value;
            arr[i + 6] = value;
            arr[i + 7] = value;
            arr[i + 7] = value;
            arr[i + 8] = value;
            arr[i + 9] = value;
            arr[i + 10] = value;
            arr[i + 11] = value;
            arr[i + 12] = value;
            arr[i + 13] = value;
            arr[i + 14] = value;
            arr[i + 15] = value;
            arr[i + 16] = value;
            arr[i + 17] = value;
            arr[i + 18] = value;
            arr[i + 19] = value;
            arr[i + 20] = value;
            arr[i + 21] = value;
            arr[i + 22] = value;
            arr[i + 23] = value;
            arr[i + 24] = value;
            arr[i + 25] = value;
            arr[i + 26] = value;
            arr[i + 27] = value;
            arr[i + 28] = value;
            arr[i + 29] = value;
            arr[i + 30] = value;
            arr[i + 31] = value;
            arr[i + 32] = value;
            arr[i + 33] = value;
            arr[i + 34] = value;
            arr[i + 35] = value;
            arr[i + 36] = value;
            arr[i + 37] = value;
            arr[i + 38] = value;
            arr[i + 39] = value;
            arr[i + 40] = value;
            arr[i + 41] = value;
            arr[i + 42] = value;
            arr[i + 43] = value;
            arr[i + 44] = value;
            arr[i + 45] = value;
            arr[i + 46] = value;
            arr[i + 47] = value;
            arr[i + 48] = value;
            arr[i + 49] = value;
            arr[i + 50] = value;
            arr[i + 51] = value;
            arr[i + 52] = value;
            arr[i + 53] = value;
            arr[i + 54] = value;
            arr[i + 55] = value;
            arr[i + 56] = value;
            arr[i + 57] = value;
            arr[i + 58] = value;
            arr[i + 59] = value;
            arr[i + 60] = value;
            arr[i + 61] = value;
            arr[i + 62] = value;            
            arr[i + 63] = value;
        }
    }
    
    memory_fence();
    end = get_time();
    
    double elapsed = get_elapsed_time(start, end);
    double bytes_processed = (double)size * sizeof(int) * ITERATIONS;
    return bytes_processed / elapsed; // Return bytes per second
}

int main() {
    // Allocate memory aligned to page size
    int* arr = NULL;
    posix_memalign((void**)&arr, 4096, ARRAY_SIZE * sizeof(int));
    
    if (arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // Initialize array
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = i;
    }
    
    // Measure read bandwidth
    double read_bw = measure_read_bandwidth(arr, ARRAY_SIZE);
    printf("Read Bandwidth: %.2f GB/s\n", read_bw / 1e9);
    
    // Measure write bandwidth
    double write_bw = measure_write_bandwidth(arr, ARRAY_SIZE);
    printf("Write Bandwidth: %.2f GB/s\n", write_bw / 1e9);
    
    free(arr);
    return 0;
}