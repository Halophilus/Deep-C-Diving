/*
 * waitingroom.h / Practicum 2
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 4/15/2025
 *
 * Thread pool for handling concurrent TCP requests
 */

#ifndef WAITINGROOM_H
#define WAITINGROOM_H
#include "queue.h"
#include <pthread.h>

// Storing active files
pthread_mutex_t global_map_lock = PTHREAD_MUTEX_INITIALIZER;
extern queue_t *file_map;

// Type alias for a function pointer
typedef void (*request_handler_fn)(int);

typedef struct file_handler {
    char *filename;
    pthread_mutex_t lock;
    queue_t *request_queue;
    request_handler_fn handler_fn;
} file_handler_t;






#endif WAITINGROOM_H
