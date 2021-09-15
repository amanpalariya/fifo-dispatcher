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
#include "protocol.h"

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
        return -1;
    }
    return socket_fd;
}

void send_request(int port_number, struct request* req) {
    if (req == NULL) {
        log_error("NULL request, aborting");
        return;
    }
    int socket_fd = connect_to_port_number(port_number);
    if (socket_fd < 0) {
        log_error("Could not connect to the server at port %d", port_number);
        return;
    }
    FILE* socket_stream = fdopen(socket_fd, "w");
    log_info("Sending request to server");
    write_request_to_stream(socket_stream, req);
    log_info("Request sent");
    socket_stream = fdopen(socket_fd, "r");
    int acknowledgement = read_acknowledgement_from_stream(socket_stream);
    switch (acknowledgement) {
        case REQUEST_RECEIVE_ACKNOWLEDGEMENT_SUCCESS:
            log_info("Request queued by the server");
            break;
        case REQUEST_RECEIVE_ACKNOWLEDGEMENT_FAILURE_QUEUE_FULL:
            log_warning("Request could not be queued because the queue was full");
            break;
        case REQUEST_RECEIVE_ACKNOWLEDGEMENT_FAILURE_MALFORMED_REQUEST:
            log_warning("Malformed request received by the server");
            break;
        case REQUEST_RECEIVE_ACKNOWLEDGEMENT_FAILURE_UNKNOWN:
            log_warning("Unknown error while processing request at the server");
            break;
        default:
            log_error("Invalid acknowledgement received from the server");
            break;
    }
}