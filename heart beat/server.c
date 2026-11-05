#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define TIMEOUT_SEC 15
#define BUFFER_SIZE 1024

typedef struct {
    int fd;
    time_t last_activity;
} client_t;

int main() {
    int listen_fd;
    client_t clients[MAX_CLIENTS];

    // Initialize tracking state table
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
        clients[i].last_activity = 0;
    }

    // 1. Create TCP socket, set SO_REUSEADDR, bind, and start listening
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 10) < 0) {
        perror("Listen failed");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    printf("Concurrent Heartbeat Echo Server running on port %d...\n", PORT);

    while (1) {

        // 2. Initialize fd_set variables for select() multiplexing
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);

        int max_fd = listen_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0) {
                FD_SET(clients[i].fd, &readfds);
                if (clients[i].fd > max_fd)
                    max_fd = clients[i].fd;
            }
        }

        // Configure timeout for select()
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int activity = select(max_fd + 1, &readfds, NULL, NULL, &tv);

        if (activity < 0 && errno != EINTR) {
            perror("Select error");
        }

        time_t now = time(NULL);

        // 3a. Accept new incoming connections on listen_fd
        if (FD_ISSET(listen_fd, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);

            int new_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen);

            if (new_fd >= 0) {
                int added = 0;

                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].fd == -1) {
                        clients[i].fd = new_fd;
                        clients[i].last_activity = now;

                        printf("New client connected on slot %d (fd: %d)\n", i, new_fd);

                        added = 1;
                        break;
                    }
                }

                if (!added) {
                    printf("Server full. Rejecting client fd: %d\n", new_fd);
                    close(new_fd);
                }
            }
        }

        // 3b. Handle incoming client messages (Echo vs PING/PONG)
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i].fd;

            if (fd > 0 && FD_ISSET(fd, &readfds)) {
                char buffer[BUFFER_SIZE];

                ssize_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);

                if (bytes_read <= 0) {
                    // Graceful teardown on disconnect or read error
                    printf("Client disconnected on slot %d (fd: %d)\n", i, fd);

                    close(fd);
                    clients[i].fd = -1;
                    clients[i].last_activity = 0;

                } else {
                    buffer[bytes_read] = '\0';
                    clients[i].last_activity = now;

                    // Check for Heartbeat Protocol Command
                    if (strcmp(buffer, "PING\n") == 0 ||
                        strcmp(buffer, "PING\r\n") == 0) {

                        send(fd, "PONG\n", 5, 0);

                    } else {
                        // Standard Echo Service
                        send(fd, buffer, bytes_read, 0);
                    }
                }
            }
        }

        // 3c. Check client activity timestamps and evict timed-out connections
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0) {
                if (difftime(now, clients[i].last_activity) > TIMEOUT_SEC) {

                    printf("Client on slot %d (fd: %d) timed out (>15s inactivity). Evicting...\n",
                           i, clients[i].fd);

                    send(clients[i].fd, "TIMEOUT\n", 8, 0);

                    close(clients[i].fd);
                    clients[i].fd = -1;
                    clients[i].last_activity = 0;
                }
            }
        }
    }

    close(listen_fd);
    return 0;
}