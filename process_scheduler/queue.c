/*
 * queue.c / Program Execution and Process queue
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 2/8/2025
 *
 * Implementing a process queue
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

// Function:	create_process
// ---------------------------
// Generates a process from input and returns a process_t*
//
// identifier: integer identifier
// name: string name
// runtime: long runtime
// priority: unsigned int priority
//
// returns: process_t*
process_t* create_process(int identifier, char *name, long runtime, unsigned int priority)
{
	char *new_name = (char *)malloc(32 * sizeof(char));
	memcpy(new_name, name, 32);

	process_t* new_process = (process_t *)malloc(sizeof(process_t));

	new_process->identifier = identifier;
	new_process->name = new_name;
	new_process->runtime = runtime;
	new_process->priority = priority;

	return new_process;
}

// Function:	create_node
// ------------------------
// Creates a new free-floating node
//
// data: pointer to some data
//
// returns: node_t* in a single-element circular queue
node_t* create_node(void* data)
{
	node_t* new_node = (node_t *)malloc(sizeof(node_t));
	if (new_node == NULL)
	{
		printf("Error allocating memory in queue.create_node()\n");
		exit(1);
	}
	new_node->next = new_node;
	new_node->prev = new_node;
	new_node->data = data;

	return new_node;
}

// Function:	create_queue
// -------------------------
// Creates a new queue from a node object
//
// returns: queue_t* of empty circular queue
queue_t* create_queue(void)
{
	queue_t* new_queue = (queue_t *)malloc(sizeof(queue_t));
	if (new_queue == NULL)
	{
		printf("Error allocating memory in queue.create_queue()\n");
		exit(1);
	}

	new_queue->back = NULL;
	new_queue->front = NULL;
	new_queue->size = 0;

	return new_queue;
}

// Function:	get_queue_size
// ---------------------------
// Returns the size of a queue_t in terms of number of elements
//
// queue: some queue_t
//
// returns: int size of queue
int get_queue_size(queue_t* queue)
{
	return queue->size;
}

// Function:	push_queue
// -----------------------
// Creates a node from a data element and pushes it to the queue
//
// queue: some queue_t
// element: some data element of anonymous type
void push_queue(queue_t* queue, void* element)
{
	node_t* new_node = create_node(element);
	
	// Pushing to an empty queue is a special case where front = back
	if ( get_queue_size(queue) == 0 )
	{
		queue->front = new_node;
		queue->back = new_node;
	} else // For queues with 1 or more elements
	{
		node_t* back_node = queue->back;

		new_node->next = back_node->next;
		new_node->prev = back_node;
		
		back_node->next = new_node;
		queue->back = new_node;
	}
	
	queue->size++;
}

// Function:	remove_node
// ------------------------
// Removes a node from an arbitrary position within a queue
// Assumes that the node is present in the queue
//
// queue: some queue_t
// node: some node_t within queue
void remove_node(queue_t* queue, node_t* node)
{
	// Cannot perform action on an empty queue
	if (get_queue_size(queue) == 0)
	{
		printf("queue.remove_node() called on an empty queue\n");
		return;
	}

	// Returns an empty queue
	if (get_queue_size(queue) == 1)
	{
		free(node);
		queue->front = NULL;
		queue->back = NULL;
		queue->size = 0;
		return;
	}

	// Store locational context
	node_t* next_node = node->next;
	node_t* prev_node = node->prev;

	// Close gap produced by removed node
	next_node->prev = prev_node;
	prev_node->next = next_node;

	// Decrement the queue size
	queue->size--;

	// In case the node being removed represents front or back of queue 
	if (node == queue->front)
	{
		queue->front = next_node;
	} else 
	if (node == queue->back)
	{
		queue->back = prev_node;
	}

	free(node);
}

// Function:	pop_queue
// ----------------------
// Pops the top element from the queue
//
// queue: some queue_t*
void* pop_queue(queue_t* queue)
{
	node_t* top_node = queue->front;
	remove_node(queue, top_node);

	return top_node->data;
}

// Function:	remove_process
// ----------------------------
// Removes the process with the highest priority and returns the node
//
// queue: any queue_t*
//
// returns: process_t* of highest priority
process_t* remove_process(queue_t* queue)
{
	node_t* current_node;	
	node_t* priority_node;

	process_t* current_process;
	process_t* max_process;
	
	int priority_max;

	// If the queue is empty
	if (get_queue_size(queue) == 0)
	{
		printf("queue.remove_process() called on an empty queue!\n");
		return NULL;
	}
	
	// Pull data from the front element in the queue
	current_node = queue->front;
	priority_node = current_node;
	max_process = (process_t *)(current_node->data);	
	priority_max = max_process->priority;

	// For all other circumstances
	for (current_node = current_node->next; 
	     current_node != queue->front; // Base case for a circular queue
	     current_node = current_node->next)
	{
		// Cast the data at the current node
		current_process = ( (process_t *)(current_node->data) );
		
		// If the priority of this process is higher than max
		if (current_process->priority > priority_max)
		{
			priority_max = current_process->priority;
			priority_node = current_node;
			max_process = current_process;
		}
	}
	
	// Remove the node with the highest priority
	remove_node(queue, priority_node);
	return max_process;
}	


