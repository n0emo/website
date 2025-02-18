#include "mew/log.h"

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

static pthread_mutex_t mtx;

void log_init() {
    pthread_mutex_init(&mtx, NULL);
}

const char *log_level_str(LogLevel level) {
    switch (level) {
        case LOG_TRACE: return "TRACE";
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO:  return "INFO";
        case LOG_WARN:  return "WARN";
        case LOG_ERROR: return "ERROR";
    }

    return "UNKNOWN";
}

void log_simple(LogLevel level, const char *format, ...) {
    FILE *stream = stdout;
    if (level == LOG_WARN || level == LOG_ERROR) {
        stream = stderr;
    }

    time_t timer;
    struct tm timeinfo;
    time(&timer);
    localtime_r(&timer, &timeinfo);

    pthread_mutex_lock(&mtx);
    fprintf(
        stream,
        "[%04d:%02d:%02d %02d:%02d:%02d] %s: ",
        timeinfo.tm_year+1900,
        timeinfo.tm_mon+1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec,
        log_level_str(level)
    );

    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);

    fprintf(stream, "\n");

    pthread_mutex_unlock(&mtx);
}

#ifdef LOG_WITH_FILE
void log_with_file(LogLevel level, const char *file, int line, const char *format, ...) {
    FILE *stream = stdout;
    if (level == LOG_WARN || level == LOG_ERROR) {
        stream = stderr;
    }

    // TODO: consider timed lock
    pthread_mutex_lock(&mtx);
    time_t my_time;
    struct tm *timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);

    fprintf(
        stream,
        "[%04d:%02d:%02d %02d:%02d:%02d] %s %s:%d: ",
        timeinfo->tm_year+1900,
        timeinfo->tm_mon+1,
        timeinfo->tm_mday,
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec,
        log_level_str(level),
        file,
        line
    );

    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);

    fprintf(stream, "\n");
    pthread_mutex_unlock(&mtx);
}
#endif
