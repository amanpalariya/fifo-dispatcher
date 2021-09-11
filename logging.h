#ifndef CS303_LOGGING_H
#define CS303_LOGGING_H

#define LOG_LEVEL_DEBUG (0)
#define LOG_LEVEL_INFO (1)
#define LOG_LEVEL_WARNING (2)
#define LOG_LEVEL_ERROR (3)

#include <stdio.h>

extern int __log_level__;
extern int __log_level_backup__;
extern FILE* __log_stream__;

void set_log_stream(FILE* stream);

void set_log_level(int new_log_level);

void mute_logs();

void unmute_logs();

void console_log(char* msg, int level);

void log_debug(char* msg);

void log_info(char* msg);

void log_warning(char* msg);

void log_error(char* msg);

#endif