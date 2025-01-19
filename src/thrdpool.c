#include "thrdpool.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "log.h"

void queue_init(ThreadPoolQueue *queue) {
    queue->first = NULL;
    queue->last = NULL;
    queue->count = 0;
    queue->about_to_destroy = false;
    mtx_init(&queue->mutex, mtx_plain);
    cnd_init(&queue->not_empty);
}

void queue_destroy(ThreadPoolQueue *queue) {
    mtx_destroy(&queue->mutex);
    cnd_destroy(&queue->not_empty);
    ThreadPoolJob *node = queue->first;
    ThreadPoolJob *next = NULL;
    while (node) {
        next = node->next;
        free(node);
        node = next;
    }
}

void queue_push(ThreadPoolQueue *queue, ThreadPoolJob job) {
    mtx_lock(&queue->mutex);

    ThreadPoolJob *node = malloc(sizeof(*node));
    memcpy(node, &job, sizeof(*node));

    if (queue->count == 0) {
        queue->first = node;
        queue->last = node;
        queue->count += 1;
        mtx_unlock(&queue->mutex);
        cnd_signal(&queue->not_empty);
    } else {
        queue->last->next = node;
        queue->last = node;
        queue->count += 1;
        mtx_unlock(&queue->mutex);
        cnd_signal(&queue->not_empty);
    }
}

ThreadPoolJob queue_pop(ThreadPoolQueue *queue, bool *ok) {
    ThreadPoolJob result = {0};
    mtx_lock(&queue->mutex);
    while (queue->count == 0) {
        if (queue->about_to_destroy) {
            if (ok != NULL) *ok = false;
            return result;
        }
        cnd_wait(&queue->not_empty, &queue->mutex);
    }

    assert(queue->count > 0);
    queue->count -= 1;
    ThreadPoolJob *first = queue->first;
    queue->first = queue->first->next;
    memcpy(&result, first, sizeof(result));
    free(first);

    mtx_unlock(&queue->mutex);

    if (ok != NULL) *ok = true;
    return result;
}

int thread_func(void *arg) {
    ThreadPool *pool = (ThreadPool *) arg;
    thrd_t current_thrd = thrd_current();

    pool->threads_alive += 1;

    while (!pool->cancel) {
        ThreadPoolJob job = queue_pop(&pool->queue, NULL);
        if (pool->cancel) break;

        int res = job.executor(job.arg);

        if (res != 0) {
            log_error("Job returned status %d (thread %lu)", res, current_thrd);
        }
    }

    pool->threads_alive -= 1;
    return 0;
}

void thrdpool_init(ThreadPool *pool, size_t thread_count) {
    queue_init(&pool->queue);
    pool->thread_count = thread_count;
    pool->threads = malloc(sizeof(*pool->threads) * thread_count);
    pool->threads_alive = 0;
    pool->cancel = false;
    for (size_t i = 0; i < thread_count; i++) {
        thrd_create(&pool->threads[i], thread_func, pool);
    }
}

void thrdpool_destroy(ThreadPool *pool) {
    pool->queue.about_to_destroy = true;
    pool->cancel = true;
    while (pool->threads_alive > 0) {
        cnd_broadcast(&pool->queue.not_empty);
    }
    for (size_t i = 0; i < pool->thread_count; i++) {
        int res;
        thrd_join(pool->threads[i], &res);
    }
    queue_destroy(&pool->queue);
    free(pool->threads);
}

void thrdpool_add_job(ThreadPool *pool, JobExecutor *executor, void *arg) {
    ThreadPoolJob job = { executor, arg, NULL };
    queue_push(&pool->queue, job);
}
