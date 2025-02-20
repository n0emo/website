#include <stdio.h>

#define mewtest const char *

#define mewassert(message, ...) do { \
    if (!(__VA_ARGS__)) return "Assertion `" #__VA_ARGS__ "` failed: " message; \
} while (0)

#define mewtest_run(...) do { \
    const char *message = (__VA_ARGS__)(); \
    printf("Running %s... ", #__VA_ARGS__); \
    if (message) { \
        printf("Error: %s\n", message); \
    } else { \
        printf("OK\n"); \
    } \
} while (0)
