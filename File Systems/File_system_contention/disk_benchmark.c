#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#define BLOCK_SIZE 4096  // Standard filesystem block size
#define NUM_BLOCKS 1000  // Number of blocks to read per process
#define MAX_PROCESSES 8  // Maximum number of concurrent processes to test
#define FILE_SIZE (BLOCK_SIZE * NUM_BLOCKS)

// Shared memory structure
struct shared_data {
    double times[MAX_PROCESSES];
};

// Function to create a test file with random data
void create_test_file(const char* filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error creating file");
        exit(1);
    }
    
    char buffer[BLOCK_SIZE];
    for (int i = 0; i < NUM_BLOCKS; i++) {
        // Fill buffer with random data
        for (int j = 0; j < BLOCK_SIZE; j++) {
            buffer[j] = rand() % 256;
        }
        write(fd, buffer, BLOCK_SIZE);
    }
    close(fd);
}

// Function to measure read time for one process
double measure_read_time(const char* filename) {
    char buffer[BLOCK_SIZE];
    struct timeval start, end;
    
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }
    
    // Ensure file is not cached
    fcntl(fd, F_NOCACHE, 1);
    
    gettimeofday(&start, NULL);
    
    // Read all blocks
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (read(fd, buffer, BLOCK_SIZE) != BLOCK_SIZE) {
            perror("Read error");
            exit(1);
        }
    }
    
    gettimeofday(&end, NULL);
    close(fd);
    
    return ((end.tv_sec - start.tv_sec) * 1000000.0 + 
            (end.tv_usec - start.tv_usec)) / NUM_BLOCKS;  // Average time per block in microseconds
}

int main() {
    char filename[256];
    pid_t pids[MAX_PROCESSES];

    // Create shared memory
    struct shared_data *shared = mmap(NULL, sizeof(struct shared_data),
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANON, -1, 0);
    if (shared == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    
    // Create test files
    for (int i = 0; i < MAX_PROCESSES; i++) {
        snprintf(filename, sizeof(filename), "test_file_%d", i);
        create_test_file(filename);
    }
    
    printf("Number of Processes | Average Time per Block (microseconds)\n");
    printf("------------------------------------------------\n");
    
    // Test with different numbers of concurrent processes
    for (int num_procs = 1; num_procs <= MAX_PROCESSES; num_procs++) {
        // Fork processes
        for (int i = 0; i < num_procs; i++) {
            pids[i] = fork();
            if (pids[i] == 0) {  // Child process
                snprintf(filename, sizeof(filename), "test_file_%d", i);
                shared->times[i] = measure_read_time(filename);
                exit(0);
            } else if (pids[i] < 0) {
                perror("Fork failed");
                exit(1);
            }
        }
        
        // Wait for all children and calculate average
        for (int i = 0; i < num_procs; i++) {
            waitpid(pids[i], NULL, 0);
        }
        
        // Calculate average time
        double total_time = 0;
        for (int i = 0; i < num_procs; i++) {
            total_time += shared->times[i];
        }
        
        printf("%17d | %f\n", num_procs, total_time / num_procs);
    }
    
    // Cleanup
    munmap(shared, sizeof(struct shared_data));
    
    // Cleanup test files
    for (int i = 0; i < MAX_PROCESSES; i++) {
        snprintf(filename, sizeof(filename), "test_file_%d", i);
        unlink(filename);
    }
    
    return 0;
}