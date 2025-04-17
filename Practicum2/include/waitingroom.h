
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
#define DEBUG

#include "queue.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>


// Storing active files
extern pthread_mutex_t global_map_lock;
extern queue_t *file_map;
extern int shutdown_signal;

// Type alias for a function pointer
typedef int (*request_handler_fn)(int);

typedef struct file_handler {
    char *filename;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    queue_t *request_queue;
    request_handler_fn handler_fn;
    pthread_t tid;
} file_handler_t;

typedef struct client {
    int socket_desc;
} client_t;

// Function:    make_request
// -------------------------
// Searches file_map for a file_handler_t with a matching filename
// Creates a new one and pushes to file_map if none
//
// filename:    requested filename
// socket_desc: fd for client socket
void make_request(char* filename, int socket_desc, request_handler_fn handler_fn);

// Function:    file_worker
// ------------------------
// Iteratively processes clients in a handler's queue
void *file_worker(void *arg);

// Function:    map_init
// ---------------------
// Initializes file_map
void waiting_room_init();

// Function:    cleanup_waiting_room
// --------------------------------
// Destroys all threads indicated by file_map
void cleanup_waiting_room(void);

#endif //WAITINGROOM_H