#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BLOCK_SIZE (1 * 1024 * 1024) // 1MB
#define ITERATIONS 10
#define ROUNDS 50
#define WARMUP_ROUNDS 40

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Server socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    for (int round = 0; round < ROUNDS; round++) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (new_socket < 0) {
            perror("Accept failed");
            continue; // Skip this round if there's an error
        }

        printf("Accepted connection for round %d.\n", round + 1);

        char buffer[BLOCK_SIZE];
        for (int i = 0; i < ITERATIONS; i++) {
            ssize_t total_bytes_received = 0;

            while (total_bytes_received < BLOCK_SIZE) {
                ssize_t bytes_received = recv(new_socket, buffer + total_bytes_received, BLOCK_SIZE - total_bytes_received, 0);
                if (bytes_received <= 0) {
                    if (bytes_received == 0) {
                        printf("Client disconnected prematurely.\n");
                    } else {
                        perror("Data reception failed");
                    }
                    break;
                }
                total_bytes_received += bytes_received;
            }

            if (total_bytes_received == BLOCK_SIZE) {
                printf("Iteration %d: Received full block of %ld bytes.\n", i + 1, total_bytes_received);
            } else {
                printf("Iteration %d: Warning - Incomplete block received (%ld bytes).\n", i + 1, total_bytes_received);
            }
        }

        close(new_socket);
        printf("Round %d completed.\n", round + 1);
    }

    close(server_fd);
    printf("Server finished.\n");
    return 0;
}
