#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_line_from_stream(FILE* stream, char* s, int max_len) {
    char* response = fgets(s, max_len, stream);
    int n = strlen(s);
    if (s[n - 1] == '\n') s[strlen(s) - 1] = '\0';
    return response;
}

struct request* read_request_from_stream(FILE* stream) {
    struct request* req = get_new_empty_request();
    req->dll_name = (char*)malloc(DLL_NAME_MAX_LENGTH);
    req->func_name = (char*)malloc(FUNC_NAME_MAX_LENGTH);

    read_line_from_stream(stream, req->dll_name, DLL_NAME_MAX_LENGTH);

    read_line_from_stream(stream, req->func_name, FUNC_NAME_MAX_LENGTH);

    char buf[32];
    read_line_from_stream(stream, buf, 32);
    req->num_args = atoi(buf);

    req->func_args = (char**)malloc(sizeof(char*) * (req->num_args));

    char buffer[FUNC_ARG_MAX_LENGTH + 1];
    int i;
    for (i = 0; i < req->num_args; i++) {
        if (read_line_from_stream(stream, buffer, FUNC_ARG_MAX_LENGTH + 1) == NULL) return NULL;
        req->func_args[i] = (char*)malloc(FUNC_ARG_MAX_LENGTH + 1);
        strcpy(req->func_args[i], buffer);
    }
    return req;
}

void write_request_to_stream(FILE* stream, struct request* req) {
    fprintf(stream, "%s\n", req->dll_name);
    fprintf(stream, "%s\n", req->func_name);
    fprintf(stream, "%d\n", req->num_args);
    for (int i = 0; i < req->num_args; i++) {
        fprintf(stream, "%s\n", req->func_args[i]);
    }
    fflush(stream);
}

void write_acknowledgement_to_stream(FILE* stream, int acknowledgement) {
    fprintf(stream, "%d\n", acknowledgement);
    fflush(stream);
}

int read_acknowledgement_from_stream(FILE* stream) {
    int acknowledgement;
    fscanf(stream, "%d", &acknowledgement);
    return acknowledgement;
}