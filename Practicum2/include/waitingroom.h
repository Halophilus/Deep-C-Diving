
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
//#define DEBUG
#define SAFE_FREE(p) do { if (p) { free(p); p = NULL; } } while (0)

#include "queue.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// Global Variables
extern pthread_mutex_t global_map_lock; // Mutex for access to active file map
extern queue_t *file_map; // Stores file handlers for files that have been queried at runtime
extern int shutdown_signal; // Flag for terminating sleeping threads

// Function Pointer:    request_handler_fn
// ---------------------------------------
// Passed to file worker for some process related to handling client requests given a socket fd
typedef int (*request_handler_fn)(int);

// Type:        file_handler_t
// ---------------------------
// Local memory structure for keeping track of files that have been queried already
// Stores process thread for closure when server is terminated
typedef struct file_handler {
    char *filename;

    // Thread access controllers
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t tid;

    // All waiting clients
    queue_t *request_queue;

    // Process for file worker
    request_handler_fn handler_fn;
} file_handler_t;

// Type:        client_t
// --------------------
// Capsule for passing client socket fds
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
void waiting_room_init(void);

// Function:    cleanup_waiting_room
// --------------------------------
// Destroys all threads indicated by file_map
void cleanup_waiting_room(void);

#endif //WAITINGROOM_H