/*
 * Copyright (c) 2024 RoyalGraphX
 * All rights reserved.
 *
 * This software is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the project root for details.
 */

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_address:port> <file_to_serve>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *host_server_address_port = argv[1];
    char *file_to_serve_path = argv[2];

    // Parsing host server address and port
    char *delimiter = strchr(host_server_address_port, ':');
    if (delimiter == NULL) {
        fprintf(stderr, "SPARCsendr: Invalid server_address:port format\n");
        exit(EXIT_FAILURE);
    }
    *delimiter = '\0';
    char *host_server_address = host_server_address_port;
    int port = atoi(delimiter + 1);

    // Open the file for reading
    FILE *input_file = fopen(file_to_serve_path, "rb");
    if (input_file == NULL) {
        perror("SPARCsendr: Failed to open input file");
        exit(EXIT_FAILURE);
    }

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("SPARCsendr: Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    struct sockaddr_in server_addr;
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Bind socket to port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("SPARCsendr: Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 5) < 0) {
        perror("SPARCsendr: Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    printf("SPARCsendr: listening on %s:%d\n", host_server_address, port);

    // Accept incoming connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (clientfd < 0) {
        perror("SPARCsendr: Failed to accept connection");
        exit(EXIT_FAILURE);
    }

    printf("SPARCsendr: Client connected\n");

    // Send file contents
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), input_file)) > 0) {
        if (send(clientfd, buffer, bytes_read, 0) < 0) {
            perror("SPARCsendr: Error sending file");
            exit(EXIT_FAILURE);
        }
    }

    printf("SPARCsendr: File sent successfully\n");

    // Close sockets and file
    close(clientfd);
    close(sockfd);
    fclose(input_file);

    return 0;
}
