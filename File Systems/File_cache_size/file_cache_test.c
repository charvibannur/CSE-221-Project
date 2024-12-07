#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

// Constants for buffer size, file sizes, and clock speed
#define BUF_SIZE 65536 // 64 KB
#define GB (1024L * 1024 * 1024) // 1 GB in bytes
#define GHz 4.05 // MacBook Air M3 clock speed in GHz
#define SIZES 40 // Number of file sizes to test
#define SAMPLES 10 // Number of samples for averaging

// Timer structure to measure time
struct Timer {
    struct timespec start;
    struct timespec end;
};

// Start the timer
void start_timer(struct Timer *timer) {
    clock_gettime(CLOCK_MONOTONIC, &timer->start);
}

// End the timer
void end_timer(struct Timer *timer) {
    clock_gettime(CLOCK_MONOTONIC, &timer->end);
}

// Calculate elapsed time in nanoseconds
long double calc_diff(struct Timer *timer) {
    long sec = timer->end.tv_sec - timer->start.tv_sec;
    long nsec = timer->end.tv_nsec - timer->start.tv_nsec;
    return sec * 1e9 + nsec;
}

// Measure read time for a specific file size
long double measure_read_time(int fd, long double file_size, char *buffer) {
    struct Timer timer;

    // Reset file pointer to the beginning
    lseek(fd, 0, SEEK_SET);

    // Start timing
    start_timer(&timer);

    // Read the file in chunks
    for (long double i = 0; i < file_size; i += BUF_SIZE) {
        read(fd, buffer, BUF_SIZE);
    }

    // Stop timing
    end_timer(&timer);

    return calc_diff(&timer);
}

int main() {
    // Array to store average times for different file sizes
    long double averages[SIZES];

    // File name (change this to your test file)
    const char *file_name = "testFile";

    // Open the file
    int file = open(file_name, O_RDONLY);
    if (file < 0) {
        perror("Failed to open file");
        return 1;
    }

    // Allocate a buffer for reading
    char *buffer = (char *)malloc(BUF_SIZE);
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(file);
        return 1;
    }

    // Test for multiple file sizes
    for (int i = 10; i < SIZES; i++) {
        long double file_size = GB + (i * GB); // Increment file size by 1 GB
        printf("\nTesting file size: %Lf GB\n", file_size / GB);

        long double times[SAMPLES];
        for (int j = 0; j < SAMPLES; j++) {
            // Initial read to load data into the cache
            measure_read_time(file, file_size, buffer);

            // Measure cached read time
            times[j] = measure_read_time(file, file_size, buffer);
        }

        // Calculate average time for the current file size
        long double total_time = 0;
        for (int j = 0; j < SAMPLES; j++) {
            total_time += times[j];
        }
        averages[i] = total_time / SAMPLES;

        // Output average time in cycles and milliseconds
        long double avg_cycles = averages[i] * GHz / 1e9;
        printf("Average cycles: %Lf\n", avg_cycles);
        printf("Average milliseconds: %Lf\n", averages[i] / 1e6);
    }

    // Clean up
    free(buffer);
    close(file);

    return 0;
}
