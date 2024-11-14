#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mach/mach_time.h>

#define NUM_ITERATIONS 1000

void* thread_function(void* arg) {
    return NULL;
}

uint64_t measure_thread_creation() {
    pthread_t thread;
    uint64_t start = mach_absolute_time();
    pthread_create(&thread, NULL, thread_function, NULL);
    pthread_join(thread, NULL);
    uint64_t end = mach_absolute_time();
    return end - start;
}

uint64_t measure_process_creation() {
    uint64_t start = mach_absolute_time();
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit(0);
    } else if (pid > 0) {
        // Parent process
        wait(NULL);
    } else {
        perror("fork");
        exit(1);
    }
    uint64_t end = mach_absolute_time();
    return end - start;
}

int main() {
    uint64_t total_thread_time = 0;
    uint64_t total_process_time = 0;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        total_thread_time += measure_thread_creation();
        total_process_time += measure_process_creation();
    }

    // Convert to nanoseconds
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    double nanos = (double)timebase.numer / (double)timebase.denom;

    double avg_thread_time = (total_thread_time * nanos) / NUM_ITERATIONS;
    double avg_process_time = (total_process_time * nanos) / NUM_ITERATIONS;

    printf("Average time to create and run a kernel thread: %.2f nanoseconds\n", avg_thread_time);
    printf("Average time to create and run a process: %.2f nanoseconds\n", avg_process_time);
    printf("Ratio (Process creation time / Thread creation time): %.2f\n", avg_process_time / avg_thread_time);

    return 0;
}