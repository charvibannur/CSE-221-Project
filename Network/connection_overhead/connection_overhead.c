#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    struct timeval start, end;
    double connection_setup_time, connection_teardown_time;

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating server socket");
        return 1;
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind server socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding server socket");
        close(server_socket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) < 0) {
        perror("Error listening on server socket");
        close(server_socket);
        return 1;
    }

    // Accept a client connection
    client_addr_size = sizeof(client_addr);
    gettimeofday(&start, NULL);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
    gettimeofday(&end, NULL);
    connection_setup_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    if (client_socket < 0) {
        perror("Error accepting client connection");
        close(server_socket);
        return 1;
    }

    // Close the connection
    gettimeofday(&start, NULL);
    close(client_socket);
    gettimeofday(&end, NULL);
    connection_teardown_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    // Close the server socket
    close(server_socket);

    printf("Connection setup time: %.6f seconds\n", connection_setup_time);
    printf("Connection teardown time: %.6f seconds\n", connection_teardown_time);

    return 0;
}