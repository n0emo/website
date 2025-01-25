#ifndef THRDPOOL_H_
#define THRDPOOL_H_

#include <stdatomic.h>
#include <stdbool.h>

#include <pthread.h>

typedef int (JobExecutor)(void *arg);

typedef struct ThreadPoolJob {
    JobExecutor *executor;
    void *arg;
    struct ThreadPoolJob *next;
} ThreadPoolJob;

typedef struct ThreadPoolQueue {
    ThreadPoolJob *first;
    ThreadPoolJob *last;
    atomic_size_t count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    atomic_bool about_to_destroy;
} ThreadPoolQueue;

void queue_init(ThreadPoolQueue *queue);
void queue_destroy(ThreadPoolQueue *queue);
void queue_push(ThreadPoolQueue *queue, ThreadPoolJob job);
ThreadPoolJob queue_pop(ThreadPoolQueue *queue, bool *ok);

typedef struct ThreadPool {
    ThreadPoolQueue queue;
    pthread_t *threads;
    size_t thread_count;
    atomic_size_t threads_alive;
    atomic_bool cancel;
} ThreadPool;

void thrdpool_init(ThreadPool *pool, size_t thread_count);
void thrdpool_destroy(ThreadPool *pool);
void thrdpool_add_job(ThreadPool *pool, JobExecutor *executor, void *arg);

#endif // THRDPOOL_H_
