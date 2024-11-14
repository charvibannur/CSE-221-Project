#include <stdio.h>
#include <stdint.h>
#include <mach/mach_time.h>
#include <unistd.h>

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

// Function declarations with different parameter counts
// Using __attribute__((noinline)) to prevent optimization
__attribute__((noinline)) void zero_parameter(void) {
}

__attribute__((noinline)) void one_parameter(int a) {
}

__attribute__((noinline)) void two_parameter(int a, int b) {
}

__attribute__((noinline)) void three_parameter(int a, int b, int c) {
}

__attribute__((noinline)) void four_parameter(int a, int b, int c, int d) {
}

__attribute__((noinline)) void five_parameter(int a, int b, int c, int d, int e) {
}

__attribute__((noinline)) void six_parameter(int a, int b, int c, int d, int e, int f) {
}

__attribute__((noinline)) void seven_parameter(int a, int b, int c, int d, int e, int f, int g) {
}

// Function to measure and print results for a specific parameter count
void measure_function(const char* desc, void (*func)(), int param_count) {
    print_message(_GENERIC_INFO, desc);
    
    // Run multiple iterations to get more stable measurements
    #define ITERATIONS 1000
    uint64_t total_time = 0;
    
    // Get timebase info for conversion to nanoseconds
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    double nanos_per_unit = (double)timebase.numer / (double)timebase.denom;
    
    for (int i = 0; i < ITERATIONS; i++) {
        static struct Timer timer;
        start_timer(&timer);
        
        switch(param_count) {
            case 0:
                zero_parameter();
                break;
            case 1:
                one_parameter(1);
                break;
            case 2:
                two_parameter(1, 2);
                break;
            case 3:
                three_parameter(1, 2, 3);
                break;
            case 4:
                four_parameter(1, 2, 3, 4);
                break;
            case 5:
                five_parameter(1, 2, 3, 4, 5);
                break;
            case 6:
                six_parameter(1, 2, 3, 4, 5, 6);
                break;
            case 7:
                seven_parameter(1, 2, 3, 4, 5, 6, 7);
                break;
        }
        
        end_timer(&timer);
        total_time += calc_diff(&timer);
    }
    
    // Convert to nanoseconds and calculate average
    double average_ns = (total_time * nanos_per_unit) / ITERATIONS;
    
    print_message(_GENERIC_INFO, "Average time (ns): ");
    printf("%.2f", average_ns);
    print_message(_GENERIC_INFO, "\n");
}

int main() {
    // Try to set maximum priority to minimize interference
    nice(-20);
    
    print_message(_GENERIC_INFO, "Starting Procedure Call Overhead Measurement\n");
    print_message(_GENERIC_INFO, "Running 1,000,000 iterations per measurement\n\n");
    
    // Measure each function with different parameter counts
    measure_function("Zero Parameter Test\n", (void(*)())zero_parameter, 0);
    measure_function("One Parameter Test\n", (void(*)())one_parameter, 1);
    measure_function("Two Parameter Test\n", (void(*)())two_parameter, 2);
    measure_function("Three Parameter Test\n", (void(*)())three_parameter, 3);
    measure_function("Four Parameter Test\n", (void(*)())four_parameter, 4);
    measure_function("Five Parameter Test\n", (void(*)())five_parameter, 5);
    measure_function("Six Parameter Test\n", (void(*)())six_parameter, 6);
    measure_function("Seven Parameter Test\n", (void(*)())seven_parameter, 7);
    
    return 0;
}