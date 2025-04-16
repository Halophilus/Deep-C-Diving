/*
 * waitingroom.c / Practicum 2
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 4/15/2025
 *
 * Thread pool for handling concurrent TCP requests
 */

#include "waitingroom.h"

// Helper Function:    map_get
// --------------------
// Searches local file map for relevant file handlers
//
// filename:    target filename
//
// returns *file_handler_t if found, NULL if absent
file_handler_t *map_get(const char *filename)
{
    node_t *current_node = file_map->front;
    file_handler_t *node_data;

    // Iterate through list
    while(current_node != NULL)
    {
        node_data = (file_handler_t *)current_node->data;
        if (node_data->filename == filename)
            return node_data;
        current_node = current_node->next;
    }

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
    node_t *current_node = file_map->front;
    file_handler_t *node_data;

    // Iterate through list
    while(current_node != NULL)
    {
        node_data = (file_handler_t *)current_node->data;
        if (node_data->filename == filename)
            return current_node;
        current_node = current_node->next;
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

// Helper Function:    map_remove
// ------------------------------
// Removes an element from file_map
//
// filename:    file path associated with file_handler_t
void map_remove(char *filename)
{
    node_t *removed = map_get_node(filename);
    file_handler_t *handler = removed->data;
    free(handler->request_queue);
    free(handler);
    remove_node(file_map, removed);
}

// Debug Function:    print_map
// ----------------------------
// Prints current contents of file map
void print_map(){
    node_t *current_node = file_map->front;
    file_handler_t *node_data;
    // Iterate through list
    while(current_node != NULL)
    {
        node_data = (file_handler_t *)current_node->data;
        fprintf(stdout, "%s\n", node_data->filename)
        current_node = current_node->next;
    }
}

// Debug Function:  print_requests
//

// Function:    make_request
// -------------------------
// Searches file_map for a file_handler_t with a matching filename
// Creates a new one and pushes to file_map if none
//
// filename:    requested filename
// socket_desc: fd for client socket
void make_request(char* filename, int socket_desc, request_handler_fn handler_fn)
{
    pthread_mutex_lock(&global_map_lock);
    file_handler_t *handler = map_get(filename);
    client_t *client = malloc(sizeof(client_t));
    client->socket_desc = socket_desc;

    if (!handler) // If there isn't a corresponding handler
    {
#ifdef DEBUG
      fprintf(stdout, "DEBUG waitingroom.make_request: new file handler for %s created at socket %d", filename, socket_desc);
#endif
        handler = malloc(sizeof(file_handler_t)); //

        pthread_mutex_init(&handler->lock, NULL);
        handler->request_queue = create_queue();
        handler->handler_fn = handler_fn;
        map_put(handler);

#ifdef DEBUG
        fprintf(stdout, "DEBUG waitingroom.make_request: socket %d added to existing handler for %s", socket_desc, filename);
#endif

        push_queue(handler->request_queue, client);


        pthread_t tid;
        pthread_create(&tid, NULL, file_worker, handler);
    }
    else
    {
        pthread_mutex_lock(&handler->lock);
        push_queue(handler->request_queue, client);
        pthread_mutex_unlock(&handler->lock);
    }

#ifdef DEBUG
    fprintf(stdout, "DEBUG waitingroom.make_request: current file map contents\n");
    print_map();
#endif

    pthread_mutex_unlock(&global_map_lock);

}

// Function:    file_worker
// ------------------------
// Iteratively processes clients in a handler's queue
void *file_worker(void *arg)
{
    file_handler_t *handler = (file_handler_t *)arg;
    queue_t *requests = handler->request_queue;
    request_handler_fn handler_process = handler->handler_fn;

    while (1) // Loop as long as there are items left in the queue
    {
        pthread_mutex_lock(&handler->lock); // Lock mutex for handler

        if (get_queue_size(requests) == 0) // If there are no requests left
        {
            pthread_mutex_unlock(&handler->lock);
            break;
        }

        client_t *req = (client_t *)pop_queue(requests); // Dequeue top request
        pthread_mutex_unlock(&handler->lock); // Unlock mutex handler
        handler_process(req->socket_desc);

        free(req);
    }

    // Once empty, remove the element from the file map
    pthread_mutex_lock(&global_map_lock);
    map_remove(handler->filename);
    pthread_mutex_unlock(&global_map_lock);

    // Destroy the mutex
    pthread_mutex_destroy(&handler->lock);

    // Request queue should be empty, so it can be freed directly
    free(requests);
    free(handler);

    return NULL;
}

// Function:    map_init
// ---------------------
// Initializes