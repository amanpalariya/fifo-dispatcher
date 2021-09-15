#include "dispatcher.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dll_runner.h"
#include "ds.h"
#include "logger.h"

struct run_dll_function_args {
    struct request* req;
    bool* finish_thread_signal;
};

struct run_dll_function_args* get_run_dll_function_args(struct request* req, bool* finish_thread_signal) {
    struct run_dll_function_args* args = (struct run_dll_function_args*)malloc(sizeof(struct run_dll_function_args));
    args->req = req;
    args->finish_thread_signal = finish_thread_signal;
    return args;
}

int get_current_memory_usage_in_kb() {
    char command[20];
    sprintf(command, "ps -o rss= %d", getpid());
    FILE* output_stream = popen(command, "r");
    int memory_usage_in_kb;
    fscanf(output_stream, "%d", &memory_usage_in_kb);
    fclose(output_stream);
    return memory_usage_in_kb;
}

int get_current_number_of_files() {
    char command[40];
    sprintf(command, "ls /proc/%d/fd | wc -l", getpid());
    FILE* output_stream = popen(command, "r");
    int number_of_files;
    fscanf(output_stream, "%d", &number_of_files);
    fclose(output_stream);
    return number_of_files;
}

void* run_dll_function(void* args) {
    struct run_dll_function_args* _args = (struct run_dll_function_args*)args;
    handle_request(_args->req);
    *_args->finish_thread_signal = true;
    pthread_exit(NULL);
}

void start_dispatching(struct request_queue* queue, int thread_limit, int memory_limit_in_kb, int files_limit) {
    if (thread_limit < 0 || memory_limit_in_kb < 0 || files_limit < 0) {
        log_error("Limits on resources should be positive, got %d on number of threads, %d KB on memory, %d on number of files", thread_limit, memory_limit_in_kb, files_limit);
    }
    int prev_size = 0;
    int base_memory_usage_in_kb = get_current_memory_usage_in_kb();
    int base_files_usage = get_current_number_of_files();
    pthread_t thread_id[thread_limit];
    bool is_thread_available[thread_limit];
    bool is_thread_about_to_finish[thread_limit];
    for (int i = 0; i < thread_limit; i++) {
        is_thread_about_to_finish[i] = false;
        is_thread_available[i] = true;
    }
    while (true) {
        if (queue->size == 0 ||
            (get_current_memory_usage_in_kb() - base_memory_usage_in_kb) >= memory_limit_in_kb ||
            (get_current_number_of_files() - base_files_usage) >= files_limit) continue;
        for (int i = 0; i < thread_limit; i++) {
            if (is_thread_about_to_finish[i]) {
                pthread_join(thread_id[i], NULL);
                is_thread_about_to_finish[i] = false;
                is_thread_available[i] = true;
            }
            if (is_thread_available[i]) {
                struct request* req = dequeue(queue);
                if (req != NULL) {
                    int retval = pthread_create(&thread_id[i], NULL, run_dll_function, get_run_dll_function_args(req, &is_thread_about_to_finish[i]));
                    if (retval == 0) {
                        is_thread_available[i] = false;
                    }
                }
            }
        }
    }
}
