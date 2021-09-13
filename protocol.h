#ifndef CS303_PROTOCOL_H
#define CS303_PROTOCOL_H

#include <stdio.h>

#include "ds.h"

#define DLL_NAME_MAX_LENGTH (1024)
#define FUNC_NAME_MAX_LENGTH (256)
#define FUNC_ARG_MAX_LENGTH (256)

#define REQUEST_RECEIVE_ACKNOWLEDGEMENT_SUCCESS (1)
#define REQUEST_RECEIVE_ACKNOWLEDGEMENT_FAILURE_QUEUE_FULL (2)
#define REQUEST_RECEIVE_ACKNOWLEDGEMENT_FAILURE_MALFORMED_REQUEST (3)
#define REQUEST_RECEIVE_ACKNOWLEDGEMENT_FAILURE_UNKNOWN (4)

struct request* read_request_from_stream(FILE* stream);

void write_request_to_stream(FILE* stream, struct request* req);

void write_acknowledgement_to_stream(FILE* stream, int acknowledgement);

int read_acknowledgement_from_stream(FILE* stream);

#endif