#include "ds.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void init_request(struct request* req, char* dll_name, char* func_name, int num_args, char** func_args) {
    req->dll_name = dll_name;
    req->func_name = func_name;
    req->num_args = num_args;
    req->func_args = func_args;
}

struct request* get_new_empty_request() {
    return (struct request*)malloc(sizeof(struct request));
}

struct request* get_new_request(char* dll_name, char* func_name, int num_args, char** func_args) {
    struct request* req = get_new_empty_request();
    init_request(req, dll_name, func_name, num_args, func_args);
    return req;
}

void free_request(struct request* req) {
    free(req->dll_name);
    free(req->func_name);
    free(req->func_args);
    free(req);
}

void init_request_queue_node(struct request_queue_node* node, struct request* req, struct request_queue_node* prev, struct request_queue_node* next) {
    node->req = req;
    node->prev = prev;
    node->next = next;
}

struct request_queue_node* get_new_empty_request_queue_node() {
    return (struct request_queue_node*)malloc(sizeof(struct request_queue_node));
}

struct request_queue_node* get_new_request_queue_node(struct request* req, struct request_queue_node* prev, struct request_queue_node* next) {
    struct request_queue_node* node = get_new_empty_request_queue_node();
    init_request_queue_node(node, req, prev, next);
    return node;
}

void free_request_queue_node(struct request_queue_node* node) {
    free_request(node->req);
    free(node);
}

bool is_queue_full(struct request_queue* queue) {
    return queue->size == queue->max_size;
}

bool is_queue_empty(struct request_queue* queue) {
    return queue->size == 0;
}

void init_queue(struct request_queue* queue, int max_size) {
    queue->max_size = max_size;
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    queue->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(queue->mutex, NULL);
}

struct request_queue* get_new_empty_queue(int max_size) {
    struct request_queue* queue = (struct request_queue*)malloc(sizeof(struct request_queue));
    init_queue(queue, max_size);
    return queue;
}

bool __enqueue(struct request_queue* queue, struct request* req) {
    struct request_queue_node* node = get_new_request_queue_node(req, NULL, queue->head);
    if (is_queue_empty(queue)) {
        queue->head = node;
        queue->tail = node;
        queue->size = 1;
        return true;
    } else if (is_queue_full(queue)) {
        return false;
    } else {
        queue->head->prev = node;
        queue->head = node;
        queue->size += 1;
        return true;
    }
}

bool enqueue(struct request_queue* queue, struct request* req) {
    pthread_mutex_lock(queue->mutex);
    bool response = __enqueue(queue, req);
    pthread_mutex_unlock(queue->mutex);
    return response;
}

struct request* __dequeue(struct request_queue* queue) {
    if (is_queue_empty(queue)) {
        return NULL;
    } else if (queue->size == 1) {
        struct request_queue_node* removed_node = queue->tail;
        struct request* req = removed_node->req;
        queue->head = NULL;
        queue->tail = NULL;
        queue->size = 0;
        free(removed_node);
        return req;
    } else {
        struct request_queue_node* removed_node = queue->tail;
        struct request* req = removed_node->req;
        queue->tail = queue->tail->prev;
        queue->tail->next = NULL;
        queue->size -= 1;
        free(removed_node);
        return req;
    }
}

struct request* dequeue(struct request_queue* queue) {
    pthread_mutex_lock(queue->mutex);
    struct request* response = __dequeue(queue);
    pthread_mutex_unlock(queue->mutex);
    return response;
}

struct request* peek_queue(struct request_queue* queue) {
    if (is_queue_empty(queue)) {
        return NULL;
    } else {
        return queue->tail->req;
    }
}

void free_queue(struct request_queue* queue) {
    struct request_queue_node* node = queue->head;
    while (node != NULL) {
        free_request_queue_node(node);
        node = node->next;
    }
    pthread_mutex_destroy(queue->mutex);
    free(queue->mutex);
    free(queue);
}