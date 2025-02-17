#include "thrdpool.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "log.h"

void queue_init(ThreadPoolQueue *queue) {
    queue->first = NULL;
    queue->last = NULL;
    queue->count = 0;
    queue->about_to_destroy = false;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
}

void queue_destroy(ThreadPoolQueue *queue) {
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    ThreadPoolJob *node = queue->first;
    ThreadPoolJob *next = NULL;
    while (node) {
        next = node->next;
        free(node);
        node = next;
    }
}

void queue_push(ThreadPoolQueue *queue, ThreadPoolJob job) {
    pthread_mutex_lock(&queue->mutex);

    ThreadPoolJob *node = malloc(sizeof(*node));
    memcpy(node, &job, sizeof(*node));

    if (queue->count == 0) {
        queue->first = node;
        queue->last = node;
        queue->count += 1;
    } else {
        queue->last->next = node;
        queue->last = node;
        queue->count += 1;
    }
    pthread_mutex_unlock(&queue->mutex);
    pthread_cond_signal(&queue->not_empty);
}

ThreadPoolJob queue_pop(ThreadPoolQueue *queue, bool *ok) {
    ThreadPoolJob result = {0};
    pthread_mutex_lock(&queue->mutex);
    while (queue->count == 0) {
        if (queue->about_to_destroy) {
            if (ok != NULL) *ok = false;
            return result;
        }
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    assert(queue->count > 0);
    queue->count -= 1;
    ThreadPoolJob *first = queue->first;
    queue->first = queue->first->next;
    memcpy(&result, first, sizeof(result));
    free(first);

    pthread_mutex_unlock(&queue->mutex);

    if (ok != NULL) *ok = true;
    return result;
}

void *thread_func(void *arg) {
    ThreadPool *pool = (ThreadPool *) arg;
    pthread_t current_thrd = pthread_self();

    pool->threads_alive += 1;

    while (!pool->cancel) {
        ThreadPoolJob job = queue_pop(&pool->queue, NULL);
        if (pool->cancel) break;

        int res = job.executor(job.arg);

        if (res != 0) {
            log_error("Job returned status %d (thread %" PRIu64 ")", res, (uint64_t) current_thrd);
        }
    }

    pool->threads_alive -= 1;
    return 0;
}

void thrdpool_init(ThreadPool *pool, size_t thread_count) {
    queue_init(&pool->queue);
    pool->thread_count = thread_count;
    pool->threads = malloc(sizeof(pthread_t) * thread_count);
    pool->threads_alive = 0;
    pool->cancel = false;
    for (size_t i = 0; i < thread_count; i++) {
        pthread_create(&pool->threads[i], NULL, thread_func, pool);
    }
}

void thrdpool_destroy(ThreadPool *pool) {
    pool->queue.about_to_destroy = true;
    pool->cancel = true;
    while (pool->threads_alive > 0) {
        pthread_cond_broadcast(&pool->queue.not_empty);
    }
    for (size_t i = 0; i < pool->thread_count; i++) {
        void *res;
        pthread_join(pool->threads[i], &res);
    }
    queue_destroy(&pool->queue);
    free(pool->threads);
}

void thrdpool_add_job(ThreadPool *pool, JobExecutor *executor, void *arg) {
    ThreadPoolJob job = { executor, arg, NULL };
    queue_push(&pool->queue, job);
}
