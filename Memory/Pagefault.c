#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define PAGE_SIZE 4096
#define NUM_PAGES 1000
#define NUM_ITERATIONS 5

// Function to get high-precision time in nanoseconds
static inline uint64_t get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

// Function to create a test file of specified size
int create_test_file(const char* filename, size_t size) {
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error creating file");
        return -1;
    }

    // Write random data to file
    char buffer[PAGE_SIZE];
    size_t remaining = size;
    
    while (remaining > 0) {
        size_t to_write = remaining < PAGE_SIZE ? remaining : PAGE_SIZE;
        // Fill buffer with random data
        arc4random_buf(buffer, to_write);
        
        if (write(fd, buffer, to_write) != to_write) {
            perror("Error writing to file");
            close(fd);
            return -1;
        }
        remaining -= to_write;
    }

    fsync(fd);  // Ensure data is written to disk
    close(fd);
    return 0;
}

double measure_page_fault_time(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return -1.0;
    }

    // Get file size
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error getting file stats");
        close(fd);
        return -1.0;
    }

    // Map the file into memory
    void* addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return -1.0;
    }

    // Advise the kernel to remove the pages from memory
    if (madvise(addr, st.st_size, MADV_DONTNEED) == -1) {
        perror("madvise failed");
        munmap(addr, st.st_size);
        close(fd);
        return -1.0;
    }

    // Measure time to fault in pages
    uint64_t start_time = get_time_ns();
    
    // Access each page to cause page faults
    volatile char* ptr = (volatile char*)addr;
    for (size_t i = 0; i < st.st_size; i += PAGE_SIZE) {
        char value = ptr[i];  // Force page fault
        (void)value;  // Prevent optimization
    }
    
    uint64_t end_time = get_time_ns();

    // Clean up
    munmap(addr, st.st_size);
    close(fd);

    return (double)(end_time - start_time) / (st.st_size / PAGE_SIZE);  // Average time per page
}

int main() {
    const char* filename = "/tmp/pagefault_test";
    size_t file_size = PAGE_SIZE * NUM_PAGES;

    // Create test file
    if (create_test_file(filename, file_size) != 0) {
        fprintf(stderr, "Failed to create test file\n");
        return 1;
    }

    printf("Measuring page fault service time...\n");
    printf("Page size: %d bytes\n", PAGE_SIZE);
    printf("Number of pages: %d\n", NUM_PAGES);

    // Run multiple iterations
    double total_time = 0.0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        double time_per_page = measure_page_fault_time(filename);
        if (time_per_page < 0) {
            fprintf(stderr, "Measurement failed\n");
            unlink(filename);
            return 1;
        }
        total_time += time_per_page;
        printf("Iteration %d: %.2f microseconds per page\n", i + 1, time_per_page / 1000.0);
    }

    double avg_time = total_time / NUM_ITERATIONS;
    printf("\nAverage page fault service time: %.2f microseconds per page\n", avg_time / 1000.0);
    printf("Average time per byte: %.3f nanoseconds\n", avg_time / PAGE_SIZE);

    // Clean up
    unlink(filename);
    return 0;
}