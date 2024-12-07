#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define PORT 1234
#define MESSAGE "Ping"
#define BUFSIZE 1024
#define ITERATIONS 1000

double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1.0e6;
}

void server_mode() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[BUFSIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 1);

    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int len = recv(client_fd, buffer, BUFSIZE, 0);
        if (len <= 0) break;
        send(client_fd, buffer, len, 0);
    }

    close(client_fd);
    close(server_fd);
}

void client_mode(const char *server_ip) {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFSIZE];
    double total_rtt = 0;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < ITERATIONS; i++) {
        double start = get_time_ms();
        send(sock_fd, MESSAGE, strlen(MESSAGE), 0);
        recv(sock_fd, buffer, BUFSIZE, 0);
        double end = get_time_ms();
        total_rtt += (end - start);
    }

    printf("Average RTT: %.2f ms\n", total_rtt / ITERATIONS);
    close(sock_fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <server|client> [server_ip]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "server") == 0) {
        server_mode();
    } else if (strcmp(argv[1], "client") == 0 && argc == 3) {
        client_mode(argv[2]);
    } else {
        printf("Invalid arguments\n");
        return EXIT_FAILURE;
    }

    return 0;
}
