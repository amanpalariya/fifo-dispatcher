#ifndef CS303_DISPATCHER_H
#define CS303_DISPATCHER_H

#include "ds.h"

void start_dispatching(struct request_queue* queue, int thread_limit, int memory_limit_in_kb, int files_limit);

#endif