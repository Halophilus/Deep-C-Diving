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
// -------------------------
// Adds a new file_handler_t to the global map
void map_put(file_handler_t *new_fh)
{
    push_queue(file_map, new_fh);
}

// Function:    map_remove
// -----------------------
// Removes an element from file_map
//
// filename:    file path associated with file_handler_t
void map_remove(char *filename)
{
    node_t *removed = map_get_node(filename);
    remove_node(file_map, removed);
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
    pthread_mutex_lock(&global_map_lock);
    file_handler_t *handler = map_get(filename);

    if (!handler)
    {
        handler = malloc(sizeof(file_handler_t));
        pthread_mutex_init(&handler->lock, NULL);
        handler->request_queue = create_queue();
        map_put(handler);

        push_queue(file_map, handler);
        pthread_t tid;
        pthread_create(&tid, NULL, file_worker, handler);
    }
}