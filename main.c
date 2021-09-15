#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "dispatcher.h"
#include "ds.h"
#include "logger.h"
#include "protocol.h"
#include "server.h"

int PORT_NUMBER = 9988;
int THREAD_LIMIT = 8;
int MEMORY_LIMIT_IN_KB = 10000;
int FILES_LIMIT = 10;
int QUEUE_SIZE = 10;

void* create_server_async(void* args) {
    create_server(PORT_NUMBER, (struct request_queue*)args);
}

void* start_dispatcher_async(void* args) {
    start_dispatching((struct request_queue*)args, THREAD_LIMIT, MEMORY_LIMIT_IN_KB, FILES_LIMIT);
}

void start_server_and_dispatcher() {
    struct request_queue* queue = get_new_empty_queue(QUEUE_SIZE);
    pthread_t st, dt;
    pthread_create(&st, NULL, create_server_async, queue);
    pthread_create(&dt, NULL, start_dispatcher_async, queue);
    pthread_join(st, NULL);
    pthread_join(dt, NULL);
    free_queue(queue);
}

void start_client() {
    struct request* req = read_request_from_stream(stdin);
    if (req == NULL) {
        log_error("NULL request\n");
        return;
    } else if (strcmp(req->dll_name, "") == 0) {
        log_error("Empty DLL name");
        return;
    } else if (strcmp(req->func_name, "") == 0) {
        log_error("Empty function name");
        return;
    } else if (req->num_args < 0) {
        log_error("Number of arguments in request is invalid");
        return;
    }
    send_request(PORT_NUMBER, req);
}

int main(int argc, char** argv) {
    char* command_format = "%s server|client [<PORT>] [-l <MAX_THREADS> <MAX_MEMORY_IN_KB> <MAX_FILES> <MAX_QUEUE_SIZE>]";
    char command[1000];
    sprintf(command, command_format, argv[0]);
    if (argc <= 1) {
        printf("Invalid command, please use the following format\n\n%s\n", command);
        return 0;
    }
    if (strcmp(argv[1], "server") == 0) {
        if (argc > 2) {
            if (strcmp(argv[2], "-l") == 0) {
                if (argc < 7) {
                    printf("Limits missing after -l, please use the following format\n\n%s\n", command);
                    return 0;
                }
                THREAD_LIMIT = atoi(argv[3]);
                MEMORY_LIMIT_IN_KB = atoi(argv[4]);
                FILES_LIMIT = atoi(argv[5]);
                QUEUE_SIZE = atoi(argv[6]);
            } else {
                PORT_NUMBER = atoi(argv[2]);
                if (argc > 3 && strcmp(argv[3], "-l") == 0) {
                    if (argc < 8) {
                        printf("Limits missing after -l, please use the following format\n\n%s\n", command);
                        return 0;
                    }
                    THREAD_LIMIT = atoi(argv[4]);
                    MEMORY_LIMIT_IN_KB = atoi(argv[5]);
                    FILES_LIMIT = atoi(argv[6]);
                    QUEUE_SIZE = atoi(argv[7]);
                }
            }
        }
        if (THREAD_LIMIT <= 0 || MEMORY_LIMIT_IN_KB <= 0 || FILES_LIMIT <= 0 || QUEUE_SIZE <= 0) {
            printf("Please ensure that limits are positive integers\n");
            return 0;
        }
        if (argc == 2) {
            printf("Using default port %d\n", PORT_NUMBER);
        } else {
            printf("Using port %d\n", PORT_NUMBER);
        }
        printf("Limits:\n\t%d threads\n\t%d KB memory\n\t%d files\n", THREAD_LIMIT, MEMORY_LIMIT_IN_KB, FILES_LIMIT);
        printf("Maximum queue size is %d\n", QUEUE_SIZE);
        start_server_and_dispatcher();
    } else if (strcmp(argv[1], "client") == 0) {
        if (argc == 2) {
            printf("Using default port %d\n", PORT_NUMBER);
        } else {
            PORT_NUMBER = atoi(argv[2]);
            printf("Using port %d\n", PORT_NUMBER);
        }
        start_client();
    } else {
        printf("Invalid command, please use the following format\n\n%s\n", command);
        return 0;
    }
    return 0;
}