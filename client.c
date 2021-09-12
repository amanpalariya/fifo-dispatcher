#include "client.h"

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ds.h"
#include "logger.h"

void send_request_through_stream(FILE* stream, struct request* req) {
}

bool has_server_received_request(FILE* stream) {
}

int connect_to_port_number(int port_number) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) return socket_fd;

    struct hostent* server = gethostbyname("localhost");
    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr_list[0], (char*)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port_number);
    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        // TODO: Error in connecting
        return -1;
    }
    return socket_fd;
}

void send_request(int port_number, struct request* req) {
    int socket_fd = connect_to_port_number(port_number);
    if (socket_fd < 0) {
        // TODO: Error
        return;
    }
    FILE* socket_stream = fdopen(socket_fd, "w+");
    send_request_through_stream(socket_stream, req);
    socket_stream = fdopen(socket_fd, "r");
    if (has_server_received_request(socket_stream)) {
        // TODO: Print acknowledgement receipt
    } else {
        // TODO: Print denial receipt
    }
}