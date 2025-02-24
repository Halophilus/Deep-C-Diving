/*
 * queuedriver.c / Multiprocessing in C
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 2/23/2025
 *
 * A rewrite of queuetest.c to suit this week's application
 */
#include "queue.h"
#include "polybius.h"

// NEW 2/23
#include <unistd.h>
#include <sys/wait.h>

// Used in implementation of
typedef char* (*Conversion)(char*, char*);

// Function:	print_process
// --------------------------
// Prints the details of a process_t
//
// process: some process_t*
void print_process(process_t* process)
{
	printf("Identifier: %d\n", process->identifier);
	printf("Name: %s\n", process->name);
	printf("Runtime: %ld\n", process->runtime);
	printf("Priority: %d\n", process->priority);
}

// Function:	print_queue
// ------------------------
// Prints all the processes stored within node_t of queue_t
// MODIFIED 2/23 Prints a queue of char*
//
// queue: some queue_t* with node_t containing process_t
void print_queue(queue_t* queue)
{
	printf("%d MEMBER QUEUE\n", get_queue_size(queue));
	if (get_queue_size(queue) == 0) return;

	char *current_data;
	node_t* current_node = queue->front;
	current_data = (char *)(current_node->data);
	printf("%s\n", current_data);

	for (current_node = current_node->next; 
	     current_node != queue->front;
	     current_node = current_node->next)
	{
		printf("==================================\n");
		current_data = (char *)(current_node->data);
		printf("%s\n", current_data);
	}
}

// Function:	load_queue
// -----------------------
// Generates a queue from a text file (MODIFIED 2/23)
// Converts each line in the text file into an element in a queue
//
// filename: string of filename
queue_t* load_queue(char *filename)
{
	FILE *fp = fopen(filename, "r");
	queue_t* new_queue = create_queue();
	char buffer[512];

	if (fp == NULL)
	{
		printf("Error opening file in queuetest.load_queue\n");
		exit(1);
	}

	while (fgets(buffer, 512, fp) != NULL)
	{
		push_queue(new_queue, strdup(buffer)); 
	}

	return new_queue;
}

/* Function:	for_each NEW 2/23
 * ---------------------
 *  For each element in a queue processed in batches of size,
 *  perform function, then pipe the output to some file
 *
 *  input_queue: some queue
 *  size: number of processes to generate per batch
 *  function: some function to be performed on each value
 *
 *  returns: a queue containing the output data
*/
void for_each(queue_t* input_queue, int size, Conversion function)
{
	int pipefd[2]; // Array containing file descriptors for start and end of pipe
	pid_t pid;
	char buffer[512];

	if (pipe(pipefd) == -1)
	{
		printf("Error creating pipe in queuedriver.for_each\n");
		exit(1);
	}

	for (int i = 0; i < size; 
}	

int main(void)
{
	// Generate a 20-member queue based on data file
	// Resultant queue should reflect contents of "processes.txt"
	queue_t* new_queue = load_queue("processes.txt");
	process_t* current_process;

	// Print contents
	print_queue(new_queue);

	// Whitespace for console clarity
	printf("\n\n\n\n");

	// Remove the top 5 processes, producing a 15-member queue
	// Should remove ProcessT, ProcessS, ProcessR, ProcessQ, and ProcessP
	for (int i = 0; i < 5; i++)
	{
		current_process = remove_process(new_queue);
		print_process(current_process);
		printf("\n");
	}

	printf("\n\n\n\n");
	print_queue(new_queue);
	printf("\n\n\n\n");

	// Remove the remaining 15 processes
	// Should show them in descending order of priority
	for (int i = 0; i < 15; i++)
	{
		current_process = remove_process(new_queue);
		print_process(current_process);
		printf("\n");
	}

	printf("\n\n\n\n");
	print_queue(new_queue);// Should show a 0-member queue

	return 0;
}
