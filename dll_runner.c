#include "dll_runner.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ds.h"
#include "logger.h"

bool handle_request(struct request* req) {
    void* handle = dlopen(req->dll_name, RTLD_LAZY);
    if (!handle) {
        log_error("\"%s\" while executing \"%s\" from \"%s\"", dlerror(), req->func_name, req->dll_name);
        return false;
    }
    double (*func)(double arg) = dlsym(handle, req->func_name);
    char* error = dlerror();
    if (error) {
        log_error("\"%s\" while executing \"%s\" from \"%s\"", error, req->func_name, req->dll_name);
    } else {
        if (req->num_args == 1) {
            double arg = atof(req->func_args[0]);
            double retval = func(arg);
            log_info("From \"%s\", %s(%f) -> %f", req->dll_name, req->func_name, arg, retval);
            dlclose(handle);
            return true;
        } else {
            log_error("DLL runner supports only (double)(double) functions, got %d args while executing \"%s\" from \"%s\"", req->num_args, req->func_name, req->dll_name);
        }
    }

    dlclose(handle);
    return false;
}