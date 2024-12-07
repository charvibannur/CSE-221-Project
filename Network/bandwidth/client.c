#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mach/mach_time.h>

#define PORT 8080
#define BLOCK_SIZE (1 * 1024 * 1024) // 1MB
#define ITERATIONS 10
#define ROUNDS 10

double timebase_factor;

void initialize_timebase() {
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    timebase_factor = (double)timebase.numer / (double)timebase.denom;
}

void measure_bandwidth(const char *server_ip) {
    char *buffer = (char *)malloc(BLOCK_SIZE);
    if (!buffer) {
        perror("Failed to allocate buffer");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 'A', BLOCK_SIZE);

    uint64_t start, end;
    double total_time_ns = 0;
    double total_per_round_ns = 0;

    for (int round = 0; round < ROUNDS; round++) {
        total_per_round_ns = 0;
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Socket creation failed");
            free(buffer);
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
            perror("Invalid address or address not supported");
            close(sock);
            free(buffer);
            exit(EXIT_FAILURE);
        }

        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            close(sock);
            free(buffer);
            exit(EXIT_FAILURE);
        }

        printf("Connected to server, round %d.\n", round + 1);

        for (int i = 0; i < ITERATIONS; i++) {
            start = mach_absolute_time();

            ssize_t bytes_sent = send(sock, buffer, BLOCK_SIZE, 0);
            if (bytes_sent != BLOCK_SIZE) {
                perror("Failed to send data");
                close(sock);
                free(buffer);
                exit(EXIT_FAILURE);
            }

            end = mach_absolute_time();
            double elapsed_ns = (end - start) * timebase_factor; // Convert to nanoseconds
            total_time_ns += elapsed_ns;
            total_per_round_ns+=elapsed_ns;
            printf("Iteration %d: Sent %ld bytes in %.6f ms.\n", i + 1, bytes_sent, elapsed_ns / 1e6);
        }
        printf("Average time for round %d: %.6f\n",round+1,(total_per_round_ns/10)); 
        printf("Round %d completed.\n", round + 1);
        close(sock); // Gracefully close the socket
    }

    free(buffer);

    double avg_time_sec = (total_time_ns / ITERATIONS / ROUNDS) / 1e9; // Convert to seconds
    double bandwidth_mbps = (BLOCK_SIZE / 1024.0 / 1024.0) / avg_time_sec; // MB/s

    printf("Average Time: %.3f ms\n", avg_time_sec * 1e3); // Convert to milliseconds
    printf("Bandwidth: %.2f MB/s\n", bandwidth_mbps);
}

int main() {
    initialize_timebase(); // Initialize the timebase for mach_absolute_time

    //const char *server_ip = "127.0.0.1"; // Localhost for loopback testing
    const char *server_ip = "192.168.1.103";
    // Replace with the server's IP for remote testing, e.g., "192.168.1.100"

    printf("Testing bandwidth to server at %s...\n", server_ip);
    measure_bandwidth(server_ip);

    return 0;
}