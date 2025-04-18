/*
 * waitingroom.c / Practicum 2
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 4/15/2025
 *
 * Thread pool for handling concurrent TCP requests
 */

#include "waitingroom.h"
queue_t *file_map;
pthread_mutex_t global_map_lock;
int shutdown_signal;

// Helper Function:    map_get
// --------------------
// Searches local file map for relevant file handlers
//
// filename:    target filename
//
// returns *file_handler_t if found, NULL if absent
file_handler_t *map_get(const char *filename)
{
    // Pull the head node
    node_t *current_node = file_map->front;
    int queue_size = get_queue_size(file_map);
    file_handler_t *node_data;

    // Iterate through list
    while(queue_size > 0)
    {
        // Coerce type to extract file handler
        node_data = (file_handler_t *)current_node->data;

        // If the filename is a match
        if (strcmp(node_data->filename, filename) == 0)
            return node_data;

        // Otherwise continue to the next node
        current_node = current_node->next;
        queue_size--;
    }

    // File handler not found
    return NULL;
}

// Helper Function:    map_get_node
// --------------------
// Searches local file map for relevant file handlers
//
// filename:    target filename
//
// returns *file_handler_t if found, NULL if absent
node_t *map_get_node(const char *filename)
{
    // Pull head node
    node_t *current_node = file_map->front;
    int queue_size = get_queue_size(file_map);
    file_handler_t *node_data;

    // Iterate through list
    while(queue_size > 0)
    {
        // If the current node contains data matching target
        node_data = (file_handler_t *)current_node->data;
        if (node_data->filename == filename)
            return current_node;

        // Otherwise proceed to next node
        current_node = current_node->next;
        queue_size--;
    }

    return NULL;
}

// Helper Function:    map_put
// ---------------------------
// Adds a new file_handler_t to the global map
void map_put(file_handler_t *new_fh)
{
    push_queue(file_map, new_fh);
}

// Debug Function:    print_map
// ----------------------------
// Prints current contents of file map
void print_map(){
    node_t *current_node = file_map->front;
    int queue_size = get_queue_size(file_map);
    file_handler_t *node_data;
    // Iterate through list
    while(queue_size > 0)
    {
        node_data = (file_handler_t *)current_node->data;
        fprintf(stdout, "%s, ", node_data->filename);
        current_node = current_node->next;
        queue_size--;
    }
    fprintf(stdout, "\n");
}

// Debug Function:  print_requests
// -------------------------------
// Prints the socket fds in a handler's request queue
//
// handler:         target file handler
void print_requests(file_handler_t *handler)
{
    queue_t *requests = handler->request_queue;
    node_t *current_node = requests->front;
    int queue_size = get_queue_size(requests);
    client_t *node_data;

    while(queue_size > 0)
    {
        node_data = (client_t *)current_node->data;
        int socket_desc = node_data->socket_desc;
        fprintf(stdout, "%d, ", socket_desc);
        current_node = current_node->next;
        queue_size--;
    }
    fprintf(stdout, "\n");
}

// Function:    make_request
// -------------------------
// Searches file_map for a file_handler_t with a matching filename
// Creates a new one and pushes to file_map if none
//
// filename:    requested filename
// socket_desc: fd for client socket
void make_request(char* filename, int socket_desc, request_handler_fn handler_fn)
{
    // Lock access to global file map and retrieve it
    pthread_mutex_lock(&global_map_lock);
    file_handler_t *handler = map_get(filename);

    // Create a new client
    client_t *client = malloc(sizeof(client_t));
    if (!client)
    {
        fprintf(stderr, "waitingroom.make_request: memory allocation failed for client_t for socket %d and file %s\n", socket_desc, filename);
        exit(1);
    }
    client->socket_desc = socket_desc;

    // If there isn't an existing corresponding handler, generate a new one
    if (!handler)
    {
#ifdef DEBUG
      fprintf(stdout, "DEBUG waitingroom.make_request: new file handler for %s created with socket %d\n", filename, socket_desc);
#endif
        // Allocate data
        handler = malloc(sizeof(file_handler_t));
        if (!handler)
        {
            fprintf(stderr, "waitingroom.make_request: memory allocation failed for file_handler_t for %s\n", filename);
            SAFE_FREE(client);
            exit(1);
        }

        // Generate fields
        pthread_mutex_init(&handler->lock, NULL);
        handler->request_queue = create_queue();
        handler->filename = strdup(filename);
        handler->handler_fn = handler_fn;

        // Add handler to global file map
        map_put(handler);

#ifdef DEBUG
        fprintf(stdout, "DEBUG waitingroom.make_request: socket %d added to existing handler for %s\n", socket_desc, filename);
        fprintf(stdout, "DEBUG waitingroom.make_request: current requests queue for %s handler\n", filename);
        print_requests(handler);
#endif
        // Add client to request queue
        push_queue(handler->request_queue, client);

        // Create pthread and store thread ID
        pthread_create(&handler->tid, NULL, file_worker, handler);
    }
    else // If there is already a matching file handler
    {
        // Lock access to the handler and add the request to the handler's request queue
        pthread_mutex_lock(&handler->lock);
        push_queue(handler->request_queue, client);
        pthread_cond_signal(&handler->cond);

#ifdef DEBUG
        fprintf(stdout, "DEBUG waitingroom.make_request: socket %d added to handler for %s", socket_desc, filename);
        fprintf(stdout, "DEBUG waitingroom.make_request: current requests queue for %s handler\n", filename);
        print_requests(handler);
#endif

        // Release the handler mutex
        pthread_mutex_unlock(&handler->lock);
    }

#ifdef DEBUG
    fprintf(stdout, "DEBUG waitingroom.make_request: current file map contents\n");
    print_map();
#endif

    // Release the global file map mutex
    pthread_mutex_unlock(&global_map_lock);
}

// Function:    file_worker
// ------------------------
// Iteratively processes clients in a handler's queue
// Spawned whenever a new file is being handled
// Sleeps for 10 seconds before parsing requests when in debug mode
void *file_worker(void *arg)
{
    // Capture the file handler object, and pull active requests and worker function
    file_handler_t *handler = (file_handler_t *)arg;
    queue_t *requests = handler->request_queue;
    request_handler_fn handler_process = handler->handler_fn;

#ifdef DEBUG
    pthread_mutex_lock(&global_map_lock);
    fprintf(stdout, "DEBUG waitingroom.file_worker: entering file worker for %s, current file_map: \n", handler->filename);
    print_map();
    pthread_mutex_unlock(&global_map_lock);
#endif

    while (1) // Loop indefinitely
    {
        // Lock mutex handler
        pthread_mutex_lock(&handler->lock);

        // If the file worker runs out of requests
        while (get_queue_size(requests) == 0)
        {

#ifdef DEBUG
            fprintf(stdout, "DEBUG waitingroom.file_worker: requests queue depleted for %s\n", handler->filename);
#endif

            // Put the thread on hold until a signal to wake is provided
            pthread_cond_wait(&handler->cond, &handler->lock);

            // Permits an "out" for the function to exit before joining when terminating the program
            if (shutdown_signal) return NULL;
        }

        // Pull most recent request
        client_t *req = (client_t *)pop_queue(requests);

#ifdef DEBUG
        fprintf(stdout, "DEBUG waitingroom.file_worker: processing socket %d request for file %s\n", req->socket_desc, handler->filename);
        fprintf(stdout, "DEBUG waitingroom.file_worker: remaining requests for %s: ", handler->filename);
        print_requests(handler);
        sleep(10);
#endif

        // Release mutex handler and perform operation on released request
        pthread_mutex_unlock(&handler->lock);
        int result = handler_process(req->socket_desc);

        if (result) fprintf(stderr, "waitingroom.file_worker: operation failed\n");

        // Free request once completed
        SAFE_FREE(req);
    }
}

// Function:    waiting_room_init
// ---------------------
// Initializes file_map
void waiting_room_init()
{
    file_map = create_queue();
    pthread_mutex_init(&global_map_lock, NULL);
    shutdown_signal = 0;
}

// Function:    cleanup_waiting_room
// --------------------------------
// Destroys all threads indicated by file_map
void cleanup_waiting_room(void)
{
    // Flag for shutdown
    shutdown_signal = 1;

    // While there are remaining file threads
    while(get_queue_size(file_map) != 0)
    {
        // Get file handler
        file_handler_t *handler = (file_handler_t *)pop_queue(file_map);

        // Terminate and join any active threads
        pthread_cond_signal(&handler->cond); // Wake up babe, the shutdown flag was raised
        pthread_join(handler->tid, NULL); // Join the threads

        // Destroy mutex/conditional
        pthread_mutex_destroy(&handler->lock);
        pthread_cond_destroy(&handler->cond);

        // Destroy request queue
        destroy_queue(handler->request_queue);

        // Free remaining pointers
        SAFE_FREE(handler->filename);
        SAFE_FREE(handler);
    }

    SAFE_FREE(file_map);
    pthread_mutex_destroy(&global_map_lock);
}