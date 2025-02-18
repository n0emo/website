#ifndef LOG_H_
#define LOG_H_

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
} LogLevel;

void log_init();

const char *log_level_str(LogLevel level);
void log_simple(LogLevel level, const char *format, ...) __attribute__((format(printf, 2, 3)));

#ifdef LOG_WITH_FILE
void log_with_file(LogLevel level, const char *file, int line, const char *format, ...) __attribute__((format(printf, 4, 5)));
#endif

#ifdef LOG_WITH_FILE
#   define LOG_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#   define log(level, ...) log_with_file(level, LOG_FILENAME, __LINE__, __VA_ARGS__)
#else
#   define log(level, ...) log_simple(level, __VA_ARGS__)
#endif

#define log_trace(...) log(LOG_TRACE, __VA_ARGS__)
#define log_debug(...) log(LOG_DEBUG, __VA_ARGS__)
#define log_info(...) log(LOG_INFO, __VA_ARGS__)
#define log_warn(...) log(LOG_WARN, __VA_ARGS__)
#define log_error(...) log(LOG_ERROR, __VA_ARGS__)

#endif // LOG_H_
