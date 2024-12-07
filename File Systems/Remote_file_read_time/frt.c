#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function declarations
void say_hello();
void create_file(int length);
void delete_file();
void read_file_seq(int line_size);
void read_file_random(int line_size);
void run_test(int line_size, int number_of_tests);
void get_file_size();
void flush_cache();

int main() {
    say_hello();

    int number_of_tests = 50;

    // Run tests for various line sizes
    run_test(10, number_of_tests);
    run_test(25, number_of_tests);
    run_test(50, number_of_tests);
    run_test(100, number_of_tests);
    run_test(250, number_of_tests);
    run_test(500, number_of_tests);
    run_test(1000, number_of_tests);
    run_test(1500, number_of_tests);
    run_test(2000, number_of_tests);
    run_test(2500, number_of_tests);

    printf("Program Ended\n");
    return 0;
}

// Run tests for sequential and random file access
void run_test(int line_size, int number_of_tests) {
    struct timespec start, end;

    int my_counter = 0;
    double time_seq_sum = 0;
    double time_ran_sum = 0;

    while (my_counter < number_of_tests) {
        create_file(line_size);
        flush_cache();

        // Measure sequential read time
        clock_gettime(CLOCK_MONOTONIC, &start);
        read_file_seq(line_size);
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_seq = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        time_seq_sum += time_seq;

        // Measure random read time
        clock_gettime(CLOCK_MONOTONIC, &start);
        read_file_random(line_size);
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_random = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        time_ran_sum += time_random;

        my_counter++;
    }

    printf("_________________________________________________________\n");
    printf("ON AVERAGE: read_file_seq() took %f seconds to execute\n", time_seq_sum / number_of_tests);
    printf("ON AVERAGE: read_file_random() took %f seconds to execute\n", time_ran_sum / number_of_tests);
    printf("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n");
    printf("Number of tests done: %d\n", number_of_tests);
    printf("Line size is: %d\n", line_size);

    get_file_size();
    delete_file();
}

// Create a file with the specified number of lines
void create_file(int length) {
    FILE *fpointer = fopen("blocks.txt", "w");
    if (!fpointer) {
        perror("File creation failed");
        exit(1);
    }

    for (int count = 0; count < length; count++) {
        fprintf(fpointer, "%d\n", count);
    }
    fclose(fpointer);
}

// Delete the test file
void delete_file() {
    remove("blocks.txt");
}

// Print a start message
void say_hello() {
    printf("Program Starting\n");
}

// Perform a sequential read
void read_file_seq(int line_size) {
    char strArray[line_size][line_size];

    FILE *fpointer = fopen("blocks.txt", "r");
    if (!fpointer) {
        perror("Cannot open file");
        exit(1);
    }

    char line[100];
    int count = 0;

    while (fgets(line, sizeof(line), fpointer)) {
        int i = 0;
        while (line[i] != '\n' && line[i] != '\0') {
            strArray[count][i] = line[i];
            i++;
        }
        count++;
    }
    fclose(fpointer);
}

// Perform a random read
void read_file_random(int line_size) {
    char strArray[2500][2500];

    for (int random_loop = 0; random_loop < line_size; random_loop++) {
        int r = rand() % line_size;

        char line[100];
        FILE *fpointer = fopen("blocks.txt", "r");
        if (!fpointer) {
            perror("Cannot open file");
            exit(1);
        }

        while (fgets(line, sizeof(line), fpointer)) {
            int line_as_int = atoi(line);
            if (line_as_int == r) {
                break; // Found the random line
            }
        }
        fclose(fpointer);
    }
}

// Get the file size
void get_file_size() {
    FILE *fpointer = fopen("blocks.txt", "r");
    fseek(fpointer, 0L, SEEK_END);
    printf("File size is: %ld bytes\n", ftell(fpointer));
    fclose(fpointer);
}

// Flush the file system cache
void flush_cache() {
    // For macOS, clear the file system cache using `purge`
    system("sync; sudo purge");
}
