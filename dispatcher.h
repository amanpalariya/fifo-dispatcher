#ifndef CS303_DISPATCHER_H
#define CS303_DISPATCHER_H

#define THREAD_LIMIT (8)
#define MEMORY_LIMIT_IN_KB (100000)
#define FILES_LIMIT (8)

#include "ds.h"

void start_dispatching(struct request_queue* queue);

#endif