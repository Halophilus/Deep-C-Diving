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
#include <fcntl.h>
#include "genrand.h"

// Function:	encode_string
// --------------------------
// Uses a polybius cipher to encode a string, with a table offset indicated by table_number
//
// input: some string
// table_number: conversion table offset
//
// returns: encoded string 
char *encode_string(char *input, int table_number)
{
	struct table new_table = *generate_table(table_number);
	char *encoded_string = pbEncode(input, &new_table);

	return encoded_string;
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

// Function:	process_queue
// --------------------------
// Processes strings in a queue in simultaneous batches until all elements in
// the queue have been processed
//
// queue: some queue_t instance
// batch_size: number of processes to be run simultaneously
// file_name: name of the output file
void process_queue(queue_t *queue, int batch_size, char *file_name)
{
	FILE *fp = fopen(file_name, "a");
	if (fp == NULL)
	{
		perror("Failure to open file %s in queuedriver.process_queue\n", file_name);
		exit(1);
	}

	while (get_queue_size(queue) > 0)
	{
		char *strings[batch_size];
		int i; 

		// Accumulate up to 100 strings into an array
		for (i = 0;
		     i < batch_size && get_queue_size(queue) > 0;
		     i++)
			strings[i] = (char *)pop_queue(queue);

		// Create channel of communication between parent and child
		int pipe_fd[2];
		if (pipe(pipe_fd) == -1)
		{
			perror("Failure creating pipe in queuedriver.process_queue\n");
			exit(1);
		}

		// Fork process, generate child
		int pid = fork();
		if (pid == 0) // if the current process is a child
		{
			// Close the reading end of the pipe
			close(pipe_fd[0]);

			// Redirect stdout to pipe
			dup2(pipe_fd[1], STDOUT_FILENO);
			
			// Close original writing end of the pipe;
			close(pipe_fd[1]);

			// Prepare args for cipher call
			char *args[i + 2];
			args[0] = "./cipher";
			for (int j = 0; j < i; j++)
			{
				args[j+1] = strings[j];
			}

			args[i+1] = NULL; // Terminate command
			
			// Call cipher, replacing the current process's image
			execvp(args[0], args);

			// In case the call fails
			perror("Executing cipher failed in queuedriver.process_queue()\n");
			exit(1);
		} else // In parent process
		{
			close (pipe_fd[1]); // Close the writing end
			
			// Read output from child process;
			char buffer[512];
			ssize_t bytes_read;
			while ( (bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1) )
					> 0 )
			{
				buffer[bytes_read] = '\0';
				fputs(buffer, fp);
			}
			
			// Close reading end of the pipe and wait for child to finish
			close(pipe_fd[0]);
			wait(NULL);
		}

	}

	fclose(fp);
}

// NEW 2/23
// Main method where children are created 
int main(void)
{
	// Generate a text file containing a sequence of random strings.
	global_state = *new_xorwow_state();
	append_strings("string_file.txt", 10000);

	// Convert the contents of this file into a string
	queue_t* string_queue = load_queue("string_file.txt");

	// Use multiprocessing to encode each string in batches of 100
	process_queue(string_queue, 100, "output_file");

	return 0;
}
