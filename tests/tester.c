#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../dll_runner.h"
#include "../ds.h"
#include "../logger.h"
#include "../protocol.h"

char* TEMPORARY_FILENAME = NULL;
int total_tests = 0;
int passed_tests = 0;
int failed_tests = 0;

void test_log(char* test_name, bool success) {
    total_tests++;
    if (success) {
        printf("\033[0;32m[✓]: %s\033[0m\n", test_name);
        passed_tests++;
    } else {
        printf("\033[0;31m[ ]: %s\033[0m\n", test_name);
        failed_tests++;
    }
}

void print_test_section(char* section) {
    printf("\n\033[0;1m%s\033[0m\n\n", section);
}

void test_get_new_request() {
    char* DLL_NAME = "testdll.so";
    char* FUNC_NAME = "testfunc";
    int NUM_ARGS = 0;
    struct request* req = get_new_request(DLL_NAME, FUNC_NAME, NUM_ARGS, NULL);
    test_log("get_new_request()", strcmp(DLL_NAME, req->dll_name) == 0 && strcmp(FUNC_NAME, req->func_name) == 0 && NUM_ARGS == req->num_args);
}

void test_queue() {
    int MAX_SIZE = 2;
    struct request_queue* queue = get_new_empty_queue(MAX_SIZE);
    test_log("get_new_empty_queue()", queue->size == 0 && queue->max_size == MAX_SIZE);

    enqueue(queue, get_new_request("A0", "B0", 0, NULL));
    struct request* front = peek_queue(queue);
    test_log("(1/2) enqueue()", queue->size == 1 && strcmp(front->dll_name, "A0") == 0 && strcmp(front->func_name, "B0") == 0 && front->num_args == 0);

    enqueue(queue, get_new_request("A1", "B1", 1, NULL));
    front = peek_queue(queue);
    struct request* back = queue->head->req;
    test_log("(2/2) enqueue()", queue->size == 2 && strcmp(front->dll_name, "A0") == 0 && strcmp(front->func_name, "B0") == 0 && front->num_args == 0 && strcmp(back->dll_name, "A1") == 0 && strcmp(back->func_name, "B1") == 0 && back->num_args == 1);

    test_log("Queue max size check", enqueue(queue, get_new_request("A2", "B2", 2, NULL)) == false);

    front = dequeue(queue);
    test_log("(1/2) dequeue()", queue->size == 1 && strcmp(front->dll_name, "A0") == 0 && strcmp(front->func_name, "B0") == 0 && front->num_args == 0);
    free_request(front);

    free_request(dequeue(queue));
    test_log("(2/2) dequeque()", queue->size == 0 && dequeue(queue) == NULL);

    free_queue(queue);
}

void test_ds() {
    test_get_new_request();
    test_queue();
}

void test_dll_runner() {
    char** arg = (char**)(malloc(sizeof(char*) * 2));
    arg[0] = (char*)malloc(sizeof(char) * 10);
    arg[1] = (char*)malloc(sizeof(char) * 10);
    arg[0] = "1.57";
    arg[1] = "2";
    struct request* good_req = get_new_request("/lib/x86_64-linux-gnu/libm.so.6", "cos", 1, arg);
    struct request* unsupported_req = get_new_request("/lib/x86_64-linux-gnu/libm.so.6", "pow", 2, arg);
    struct request* bad_req = get_new_request("this library does not exist", "this function does not exist", 0, NULL);
    test_log("Request handled", handle_request(good_req) == true);
    test_log("Unsupported request rejected", handle_request(unsupported_req) == false);
    test_log("Invalid request rejected", handle_request(bad_req) == false);
    free_request(good_req);
    free_request(unsupported_req);
    free_request(bad_req);
}

void test_protocol() {
    char** arg = (char**)(malloc(sizeof(char*) * 2));
    arg[0] = (char*)malloc(sizeof(char) * 10);
    arg[1] = (char*)malloc(sizeof(char) * 10);
    arg[0] = "1.57";
    arg[1] = "2";
    struct request* req;
    struct request* read_req;
    char filename[] = "/tmp/temp-stream.XXXXXX";
    FILE* out;

    req = get_new_request("test_dll", "test_func", 1, arg);
    out = fopen(TEMPORARY_FILENAME, "w");
    write_request_to_stream(out, req);
    fclose(out);
    out = fopen(TEMPORARY_FILENAME, "r");
    read_req = read_request_from_stream(out);
    fclose(out);
    test_log("(1/2) Request read + write over stream", strcmp(req->dll_name, read_req->dll_name) == 0 && strcmp(req->func_name, read_req->func_name) == 0 && req->num_args == read_req->num_args && strcmp(req->func_args[0], read_req->func_args[0]) == 0);
    free_request(req);
    free_request(read_req);

    req = get_new_request("test_dll", "test_func", 2, arg);
    out = fopen(TEMPORARY_FILENAME, "w");
    write_request_to_stream(out, req);
    fclose(out);
    out = fopen(TEMPORARY_FILENAME, "r");
    read_req = read_request_from_stream(out);
    fclose(out);
    test_log("(2/2) Request read + write over stream", strcmp(req->dll_name, read_req->dll_name) == 0 && strcmp(req->func_name, read_req->func_name) == 0 && req->num_args == read_req->num_args && strcmp(req->func_args[0], read_req->func_args[0]) == 0 && strcmp(req->func_args[1], read_req->func_args[1]) == 0);
    free_request(req);
    free_request(read_req);

    int ack, read_ack;

    ack = REQUEST_RECEIVE_ACKNOWLEDGEMENT_FAILURE_MALFORMED_REQUEST;
    out = fopen(TEMPORARY_FILENAME, "w");
    write_acknowledgement_to_stream(out, ack);
    fclose(out);
    out = fopen(TEMPORARY_FILENAME, "r");
    read_ack = read_acknowledgement_from_stream(out);
    fclose(out);
    test_log("(1/4) Acknowledgement read + write over stream", ack == read_ack);

    ack = REQUEST_RECEIVE_ACKNOWLEDGEMENT_FAILURE_QUEUE_FULL;
    out = fopen(TEMPORARY_FILENAME, "w");
    write_acknowledgement_to_stream(out, ack);
    fclose(out);
    out = fopen(TEMPORARY_FILENAME, "r");
    read_ack = read_acknowledgement_from_stream(out);
    fclose(out);
    test_log("(2/4) Acknowledgement read + write over stream", ack == read_ack);

    ack = REQUEST_RECEIVE_ACKNOWLEDGEMENT_FAILURE_UNKNOWN;
    out = fopen(TEMPORARY_FILENAME, "w");
    write_acknowledgement_to_stream(out, ack);
    fclose(out);
    out = fopen(TEMPORARY_FILENAME, "r");
    read_ack = read_acknowledgement_from_stream(out);
    fclose(out);
    test_log("(3/4) Acknowledgement read + write over stream", ack == read_ack);

    ack = REQUEST_RECEIVE_ACKNOWLEDGEMENT_SUCCESS;
    out = fopen(TEMPORARY_FILENAME, "w");
    write_acknowledgement_to_stream(out, ack);
    fclose(out);
    out = fopen(TEMPORARY_FILENAME, "r");
    read_ack = read_acknowledgement_from_stream(out);
    fclose(out);
    test_log("(4/4) Acknowledgement read + write over stream", ack == read_ack);
}

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    TEMPORARY_FILENAME = argv[1];

    mute_logs();

    print_test_section("Testing common data structures");
    test_ds();
    print_test_section("Testing DLL runner");
    test_dll_runner();
    print_test_section("Testing protocol");
    test_protocol();

    printf("\n%d/%d tests passed\n", passed_tests, total_tests);
    return 0;
}