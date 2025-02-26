/*
 * pqueuepmain.c / Complex synchronization
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 2/25/2025
 *
 * Part 3: Synchronization with monitors
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// Expand to 15
#define BUF_SIZE 15    

int buffer[BUF_SIZE];						
int num = 0;									
pthread_mutex_t mut;	
pthread_cond_t cond; 

// Keep track of how many items have been produced/consumed
int num_produced = 0;
int num_consumed = 0;
int index_value = 0;

void *producer(void *param);
void *consumer(void *param);

// Function:	print_buffer
// -------------------------
// Prints the contents of buffer for debugging
void print_buffer()
{
	for (int i = 0; i < BUF_SIZE; i++)
	{
		printf("%d ", buffer[i]);
	}
	printf("\n");
}

void main (int argc, char *argv[])
{
	// Add two threads
	pthread_t t1, t2, t3, t4;

	if(pthread_mutex_init(&mut, NULL) != 0) {
		perror("pthread_mutex_init");
		exit(1);
	}
	if(pthread_cond_init(&cond, NULL) != 0) {
		perror("pthread_cond_init");
		exit(1);
	}
	
	// Add one producer
	if (pthread_create(&t1, NULL, producer, NULL) != 0)
	{
		perror("pthread_create");
		exit (1);
	}

	if (pthread_create(&t3, NULL, producer, NULL) != 0)
	{
		perror("pthread_create");
		exit (1);
	}

	// Add one consumer
	if (pthread_create(&t2, NULL, consumer, NULL) != 0)
       	{
		perror("pthread_create");
		exit (1);
	}

	
	if (pthread_create(&t4, NULL, consumer, NULL) != 0)
       	{
		perror("pthread_create");
		exit (1);
	}

	// Join all threads before exiting
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	pthread_join(t4, NULL);

	printf("WE ARE DONE\n");
	pthread_mutex_destroy(&mut);
	pthread_cond_destroy(&cond);
}

void *producer(void *param)
{
	while(1) {
		pthread_mutex_lock (&mut);
		
		if (num > BUF_SIZE) exit(1);	
		
		// Unlock mutex, signal to waiting threads, return to caller
		if (num_produced >= 30)
		{
			pthread_mutex_unlock(&mut);
			printf("Exiting producer thread");
			pthread_cond_signal(&cond);
			return NULL;
		}

		// Wait if the buffer is full
		while (num == BUF_SIZE)			
			pthread_cond_wait (&cond, &mut);

		// Produce an item for the buffer 
		buffer[num] = num_produced;
		num_produced++;
		num++;

		// Indicate production with a print statement
		printf("ELEMENT PRODUCED: %d\n", num_produced-1);
		print_buffer();
		pthread_mutex_unlock (&mut);
		pthread_cond_signal (&cond);
		sleep(1);
	}
}

void *consumer(void *param)
{
	while (1) {
		pthread_mutex_lock (&mut);
		
		// Unlock mutex, signal to waiting threads, return to caller
		if (num_consumed >= 30)
		{
			pthread_mutex_unlock(&mut);
			printf("Exiting consumer thread");
			pthread_cond_signal(&cond);
			return NULL;
		}
		
		if (num < 0) exit(1);	 
		while (num == 0)		
			pthread_cond_wait (&cond, &mut);
		
		// Consume some element of the buffer
		int item = buffer[num-1];
		buffer[num-1] = 0;
		num--;
		num_consumed++;

		pthread_mutex_unlock (&mut);
		pthread_cond_signal (&cond);
		
		printf("ELEMENT CONSUMED: %d\n", item);
		print_buffer();
		sleep(3);
	}
}

