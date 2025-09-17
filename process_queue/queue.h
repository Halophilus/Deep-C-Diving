/*
 * queue.h / Program Execution and Process queue
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 2/8/2025
 *
 * Header file for process queue
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct:	process_t
// ----------------------
// Stores a process
typedef struct
{
	int identifier;
	char *name;
	long runtime;
	unsigned int priority;
} process_t;

// Struct:	node_t
// -------------------
// An individual node in a DLL queue, can store any data
typedef struct node
{
	struct node* next;
	struct node* prev;

	void* data;
} node_t;

// Struct:	queue_t
// --------------------
// A doubly-linked queue data structure
//
// Data structure:
// front -next-> <-prev- node1 -next-> ... <-prev- back
//   ...<-prev- back -next-> <-prev- front -next->...
typedef struct
{
	node_t* back;
	node_t* front;

	unsigned int size;
} queue_t;

// Function:	create_node
// ------------------------
// Creates a new free-floating node
//
// data: pointer to some data
//
// returns: node_t* in a single-element circular queue
node_t* create_node(void* data);

// Function:	create_queue
// -------------------------
// Creates a new queue from a node object
//
// returns: queue_t* of empty circular queue
queue_t* create_queue(void);

// Function:    create_process
// ---------------------------
// Generates a process from input and returns a process_t*
//
// identifier: integer identifier
// name: string name
// runtime: long runtime
// priority: unsigned int priority
//
// returns: process_t*
process_t* create_process(int identifier, char *name, long runtime, unsigned int priority);

// Function:	get_queue_size
// ---------------------------
// Returns the size of a queue_t in terms of number of elements
//
// queue: some queue_t
//
// returns: int size of queue
int get_queue_size(queue_t* queue);

// Function:	push_queue
// -----------------------
// Creates a node from a data element and pushes it to the queue
//
// queue: some queue_t
// element: some data element of anonymous type
void push_queue(queue_t* queue, void* element);

// Function:	remove_node
// ------------------------
// Removes a node from an arbitrary position within a queue
// Assumes that the node is present in the queue
//
// queue: some queue_t
// node: some node_t within queue
void remove_node(queue_t* queue, node_t* node);

// Function:	pop_queue
// ----------------------
// Pops the top element from the queue
//
// queue: some queue_t*
void* pop_queue(queue_t* queue);

// Function:	remove_process
// ----------------------------
// Removes the process with the highest priority and returns the node
//
// queue: any queue_t*
//
// returns: process_t* of highest priority
process_t* remove_process(queue_t* queue);


