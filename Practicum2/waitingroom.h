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

// Struct:  job
// ------------
// Jobs to push to queue
typedef struct job
{
    void (*function)(void*);
    void* arg;
};

typedef struct thread_pool {
    pthread_t* threads;
    int thread_count;
    int shutdown;

    queue_t* job_queue; // your generic queue

    pthread_mutex_t lock;
    pthread_cond_t notify;
} thread_pool_t;

queue_t active_files;

socket_queue_t *find_socket_queue(char *filename);

int oldest_client(socket_queue_t *);

void handle_inbound(int socket_desc);



#endif WAITINGROOM_H
