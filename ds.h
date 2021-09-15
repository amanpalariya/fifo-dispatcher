#ifndef CS303_DS_H
#define CS303_DS_H

#include <pthread.h>
#include <stdbool.h>

struct request {
    char* dll_name;
    char* func_name;
    int num_args;
    char** func_args;
};

struct request_queue_node {
    struct request* req;
    struct request_queue_node* prev;
    struct request_queue_node* next;
};

struct request_queue {
    int max_size;
    int size;
    struct request_queue_node* head;
    struct request_queue_node* tail;
    pthread_mutex_t* mutex;
};

void init_request(struct request* req, char* dll_name, char* func_name, int num_args, char** func_args);

struct request* get_new_empty_request();

struct request* get_new_request(char* dll_name, char* func_name, int num_args, char** func_args);

void free_request(struct request* req);

bool is_queue_full(struct request_queue* queue);

bool is_queue_empty(struct request_queue* queue);

void init_queue(struct request_queue* queue, int max_size);

struct request_queue* get_new_empty_queue(int max_size);

bool enqueue(struct request_queue* queue, struct request* req);

struct request* dequeue(struct request_queue* queue);

struct request* peek_queue(struct request_queue* queue);

void free_queue(struct request_queue* queue);

#endif