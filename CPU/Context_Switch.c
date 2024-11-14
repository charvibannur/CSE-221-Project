#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mach/mach_time.h>

#define NUM_ITERATIONS 100000
#define NUM_SWITCHES 2

// Structure to hold pipe file descriptors
typedef struct {
    int read_fd;
    int write_fd;
} pipe_fds;

// Function to measure process context switch time
void measure_process_switch(pipe_fds *pipes) {
    char buf[1];
    if (fork() == 0) {  // Child process
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            read(pipes[0].read_fd, buf, 1);
            write(pipes[1].write_fd, buf, 1);
        }
        exit(0);
    } else {  // Parent process
        uint64_t start = mach_absolute_time();
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            write(pipes[0].write_fd, buf, 1);
            read(pipes[1].read_fd, buf, 1);
        }
        uint64_t end = mach_absolute_time();
        wait(NULL);

        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        double nanos = (double)timebase.numer / (double)timebase.denom;
        double total_time = (end - start) * nanos;
        printf("Average process context switch time: %.2f ns\n", total_time / (NUM_ITERATIONS * NUM_SWITCHES));
    }
}

// Structure to pass data to threads
typedef struct {
    pipe_fds *pipes;
    int iterations;
} thread_data;

// Thread function
void* thread_func(void* arg) {
    thread_data* data = (thread_data*)arg;
    char buf[1];
    for (int i = 0; i < data->iterations; i++) {
        read(data->pipes[0].read_fd, buf, 1);
        write(data->pipes[1].write_fd, buf, 1);
    }
    return NULL;
}

// Function to measure thread context switch time
void measure_thread_switch(pipe_fds *pipes) {
    pthread_t thread;
    char buf[1];
    thread_data data = {pipes, NUM_ITERATIONS};

    pthread_create(&thread, NULL, thread_func, &data);

    uint64_t start = mach_absolute_time();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        write(pipes[0].write_fd, buf, 1);
        read(pipes[1].read_fd, buf, 1);
    }
    uint64_t end = mach_absolute_time();

    pthread_join(thread, NULL);

    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    double nanos = (double)timebase.numer / (double)timebase.denom;
    double total_time = (end - start) * nanos;
    printf("Average thread context switch time: %.2f ns\n", total_time / (NUM_ITERATIONS * NUM_SWITCHES));
}

int main() {
    pipe_fds pipes[2];

    // Create pipes
    if (pipe((int*)&pipes[0]) == -1 || pipe((int*)&pipes[1]) == -1) {
        perror("pipe");
        exit(1);
    }

    printf("Measuring process context switch time...\n");
    measure_process_switch(pipes);

    printf("Measuring thread context switch time...\n");
    measure_thread_switch(pipes);

    // Close pipes
    close(pipes[0].read_fd);
    close(pipes[0].write_fd);
    close(pipes[1].read_fd);
    close(pipes[1].write_fd);

    return 0;
}