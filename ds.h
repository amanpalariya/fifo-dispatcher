#include <stdbool.h>

#ifndef CS303_DS_H
#define CS303_DS_H

struct request {
    char* dll_name;
    char* func_name;
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
};

void init_request(struct request* req, char* dll_name, char* func_name, char** func_args);

struct request* get_new_empty_request();

struct request* get_new_request(char* dll_name, char* func_name, char** func_args);

void free_request(struct request* req);

bool is_queue_full(struct request_queue* queue);

bool is_queue_empty(struct request_queue* queue);

void init_queue(struct request_queue* queue, int max_size);

struct request_queue* get_new_empty_queue(int max_size);

bool enqueue(struct request_queue* queue, struct request new_req);

struct request* dequeue(struct request_queue* queue);

struct request* peek_queue(struct request_queue* queue);

void free_queue(struct request_queue* queue);

void print_queue(struct request_queue* q);

void print_request(struct request* req);

#endif