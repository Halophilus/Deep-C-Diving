/*
 * queuetest.c / Program execution and process queue
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 2/8/2025
 *
 * Testing file for queue.h and queue.c
 */
#include "queue.h"

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
//
// queue: some queue_t* with node_t containing process_t
void print_queue(queue_t* queue)
{
	printf("%d MEMBER QUEUE\n", get_queue_size(queue));
	if (get_queue_size(queue) == 0) return;

	process_t* current_process;
	node_t* current_node = queue->front;
	current_process = (process_t *)(current_node->data);
	print_process(current_process);

	for (current_node = current_node->next; 
	     current_node != queue->front;
	     current_node = current_node->next)
	{
		printf("==================================\n");
		current_process = (process_t *)(current_node->data);
		print_process(current_process);
	}
}

// Function:	load_queue
// -----------------------
// Generates a queue from a text file
// Each line represents a process, with each attribute separated by whitespace
// i.e., [IDENTIFIER] [NAME] [RUNTIME] [PRIORITY] 
//
// filename: string of filename
queue_t* load_queue(char *filename)
{
	FILE *fp = fopen(filename, "r");
	queue_t* new_queue = create_queue();
	char buffer[256];
	char *token;
	const char delimiter[] = " ";
	int id;
	char nom[32];
	long rt;
	unsigned int pr;
	process_t* current_process;

	if (fp == NULL)
	{
		printf("Error opening file in queuetest.load_queue\n");
		exit(1);
	}

	while (fgets(buffer, 256, fp) != NULL)
	{
		token = strtok(buffer, delimiter);
		id = atoi(token);

		token = strtok(NULL, delimiter);
		strcpy(nom, token);

		char *endptr;
		token = strtok(NULL, delimiter);
		rt = strtol(token, &endptr, 10);

		token = strtok(NULL, delimiter);
		pr = atoi(token);

		current_process = create_process(id, nom, rt, pr);
		push_queue(new_queue, current_process); 

	}

	return new_queue;
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
