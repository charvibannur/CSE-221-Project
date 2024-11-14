#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <mach/mach_time.h>

// Define message types
#define _GENERIC_INFO 0
#define _GENERIC_ERROR 1

// Timer structure
struct Timer {
    uint64_t start;
    uint64_t end;
};

// Utility function to read time
static inline uint64_t get_time(void) {
    return mach_absolute_time();
}

// Timer functions
void start_timer(struct Timer* timer) {
    timer->start = get_time();
}

void end_timer(struct Timer* timer) {
    timer->end = get_time();
}

uint64_t calc_diff(struct Timer* timer) {
    return timer->end - timer->start;
}

// Print functions
void print_message(int type, const char* message) {
    if (type == _GENERIC_ERROR) {
        fprintf(stderr, "%s", message);
    } else {
        printf("%s", message);
    }
}

void print_uint64(uint64_t value) {
    printf("%lu", value);
}

int main() {
    // Get timebase info for conversion to nanoseconds
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    double nanos_per_unit = (double)timebase.numer / (double)timebase.denom;

    // Set process priority to minimize interference
    nice(-20);

    print_message(_GENERIC_INFO, "Start System Call Overhead Time Test: getpid()\n");
    
    // Number of iterations for more accurate measurement
    #define ITERATIONS 1000
    uint64_t total_time = 0;
    
    // Warm up the cache and system
    for (int i = 0; i < 1000; i++) {
        getpid();
    }
    
    // Perform actual measurements
    for (int i = 0; i < ITERATIONS; i++) {
        static struct Timer timer;
        
        start_timer(&timer);
        getpid();  // Minimal system call
        end_timer(&timer);
        
        total_time += calc_diff(&timer);
    }
    
    // Calculate average time in nanoseconds
    double average_ns = (total_time * nanos_per_unit) / ITERATIONS;
    
    print_message(_GENERIC_INFO, "Average time per system call (ns): ");
    printf("%.2f\n", average_ns);
    
    // Also perform a single measurement for comparison
    static struct Timer single_timer;
    start_timer(&single_timer);
    getpid();
    end_timer(&single_timer);
    uint64_t single_time = calc_diff(&single_timer);
    
    print_message(_GENERIC_INFO, "Single measurement (ns): ");
    printf("%.2f\n", single_time * nanos_per_unit);
    
    // Additional measurements with different system calls for comparison
    print_message(_GENERIC_INFO, "\nComparison with other minimal system calls:\n");
    
    // Measure getppid()
    total_time = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        static struct Timer timer;
        start_timer(&timer);
        getppid();
        end_timer(&timer);
        total_time += calc_diff(&timer);
    }
    average_ns = (total_time * nanos_per_unit) / ITERATIONS;
    print_message(_GENERIC_INFO, "getppid() average (ns): ");
    printf("%.2f\n", average_ns);
    
    // Measure geteuid()
    total_time = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        static struct Timer timer;
        start_timer(&timer);
        geteuid();
        end_timer(&timer);
        total_time += calc_diff(&timer);
    }
    average_ns = (total_time * nanos_per_unit) / ITERATIONS;
    print_message(_GENERIC_INFO, "geteuid() average (ns): ");
    printf("%.2f\n", average_ns);

    return 0;
}