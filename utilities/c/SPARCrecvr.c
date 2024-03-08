/*
 * Copyright (c) 2023 2024 RoyalGraphX, John Millikin
 * All rights reserved.
 *
 * This software is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the project root for details.
 */

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int split_server_address(server_address, server_ip, server_port)
    char *server_address;
    unsigned long *server_ip;
    unsigned short *server_port;
{
    char *port_str;
    char *port_extra;
    long port_raw;

    port_str = strchr(server_address, ':');
    if (port_str == NULL) {
        return -1;
    }
    *(port_str++) = '\0';

    *server_ip = inet_addr(server_address);
    if (*server_ip == -1) {
        return -1;
    }

    port_raw = strtol(port_str, &port_extra, 10);
    if (port_raw < 1 || port_raw > 65535) {
        return -1;
    }
    if (*port_extra != '\0') {
        return -1;
    }
    *server_port = (unsigned short)port_raw;

    return 0;
}

int recv_file(server_ip, server_port, output_path)
    unsigned long server_ip;
    unsigned short server_port;
    char *output_path;
{
    int socket_fd;
    int output_fd;
    struct sockaddr_in server;
    char buffer[2048];
    long total_bytes_transferred = 0;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        return -1;
    }

    memset((char *)&server, 0, sizeof server);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = server_ip;
    server.sin_port = htons(server_port);

    if (connect(socket_fd, (struct sockaddr *)&server, sizeof server) == -1) {
        return -1;
    }

    printf("Connected to %s with port %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
    printf("File transfer beginning...\n");
    printf("Saving to: '%s'\n", output_path);

    output_fd = open(output_path, O_WRONLY | O_CREAT, 0600);
    if (output_fd == -1) {
        return -1;
    }

    printf("Bytes transferred so far: %ld", total_bytes_transferred);
    fflush(stdout);

    while (1) {
        int n = read(socket_fd, buffer, sizeof buffer);
        if (n == -1) {
            close(output_fd);
            return -1;
        }
        if (n == 0) {
            close(output_fd);
            printf("\nTransfer completed. Total bytes transferred: %ld\n", total_bytes_transferred);
            return 0;
        }
        write(output_fd, buffer, n);
        total_bytes_transferred += n;

        printf("\rBytes transferred so far: %ld", total_bytes_transferred);
        fflush(stdout);
    }
}

int main(argc, argv)
    int argc;
    char **argv;
{
    unsigned long server_ip;
    unsigned short server_port;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_address:port> <output_path>\n", *argv);
        return 1;
    }

    if (split_server_address(argv[1], &server_ip, &server_port) == -1) {
        fprintf(stderr, "Invalid server address \"%s\"\n", argv[1]);
        return 1;
    }

    if (recv_file(server_ip, server_port, argv[2]) == -1) {
        perror("Error receiving file");
        return 1;
    }

    return 0;
}