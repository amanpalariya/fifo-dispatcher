#include "server.h"

#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ds.h"

struct client_fd_and_queue {
    int client_fd;
    struct request_queue* queue;
};

struct client_fd_and_queue* get_client_fd_and_queue_args(int client_fd, struct request_queue* queue) {
    struct client_fd_and_queue* args = (struct client_fd_and_queue*)malloc(sizeof(struct client_fd_and_queue));
    args->client_fd = client_fd;
    args->queue = queue;
    return args;
}

void* handle_client(void* args) {
    int client_fd = ((struct client_fd_and_queue*)args)->client_fd;
    struct request_queue* queue = ((struct client_fd_and_queue*)args)->queue;
    printf("%d\n", client_fd);
    close(client_fd);
}

struct sockaddr_in get_localhost_socket_address(int port_number) {
    struct sockaddr_in addr;
    bzero((char*)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_number);
    return addr;
}

void start_listening(int socket_fd, struct request_queue* queue) {
    listen(socket_fd, MAX_CONNECTIONS);
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, get_client_fd_and_queue_args(client_fd, queue));
    }
    close(socket_fd);
}

void create_server(int port_number, struct request_queue* queue) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = get_localhost_socket_address(port_number);
    socklen_t server_addr_len = (unsigned int)sizeof(server_addr);
    if (bind(socket_fd, (struct sockaddr*)&server_addr, server_addr_len) < 0) {
        // TODO: Error on binding
        printf("Error on binding\n");
        return;
    } else {
        start_listening(socket_fd, queue);
    }
}