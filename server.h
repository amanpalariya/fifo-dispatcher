#ifndef CS303_SERVER_H
#define CS303_SERVER_H

#include "ds.h"

#define MAX_CONNECTIONS (5)

void create_server(int port_number, struct request_queue* queue);

#endif