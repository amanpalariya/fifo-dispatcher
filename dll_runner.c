#include "dll_runner.h"

#include <dlfcn.h>

#include "ds.h"
#include "logger.h"

void handle_request(struct request* req) {
    void* handle = dlopen(req->dll_name, RTLD_LAZY);
    if (!handle) {
        log_error(dlerror());
        return;
    }
    void (*func)(char** args) = dlsym(handle, req->func_name);
    char* error = dlerror();
    if (error) {
        log_error(error);
        return;
    }
    (*func)(req->func_args);
    dlclose(handle);
}