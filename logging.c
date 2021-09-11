#include "logging.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

int __log_level__ = 0;
int __log_level_backup__ = 0;
FILE* __log_stream__ = NULL;

void set_log_stream(FILE* stream) {
    __log_stream__ = stream;
}

void set_log_level(int new_log_level) {
    __log_level__ = new_log_level;
}

void mute_logs() {
    __log_level_backup__ = __log_level__;
    set_log_level(0x1000);
}

void unmute_logs() {
    __log_level__ = __log_level_backup__;
}

void console_log(char* msg, int level) {
    if (!__log_stream__) set_log_stream(stdout);
    if (level < __log_level__) return;
    time_t rawtime;
    time(&rawtime);
    struct tm* t = localtime(&rawtime);
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    switch (level) {
        case LOG_LEVEL_INFO:
            fprintf(__log_stream__, "INFO  %s: %s\n", timeStr, msg);
            break;
        case LOG_LEVEL_WARNING:
            fprintf(__log_stream__, "WARN  %s: %s\n", timeStr, msg);
            break;
        case LOG_LEVEL_DEBUG:
            fprintf(__log_stream__, "DEBUG %s: %s\n", timeStr, msg);
            break;
        case LOG_LEVEL_ERROR:
            fprintf(__log_stream__, "ERROR %s: %s\n", timeStr, msg);
            break;
        default:
            break;
    }
}

void log_debug(char* msg) { console_log(msg, LOG_LEVEL_DEBUG); }

void log_info(char* msg) { console_log(msg, LOG_LEVEL_INFO); }

void log_warning(char* msg) { console_log(msg, LOG_LEVEL_WARNING); }

void log_error(char* msg) { console_log(msg, LOG_LEVEL_ERROR); }
