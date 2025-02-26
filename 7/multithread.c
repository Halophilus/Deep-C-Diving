/*
 * multithread.c / Complex synchronization
 *
 * Ben Henshaw / CS54600 / Northeastern University
 * Spring 2025 / 2/25/2025
 *
 * Part 1: Create threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int shared_value = 0;

void *worker(void *data)
{
    char *name = (char*)data;
 
    for (int i = 0; i < 120; i++)
    {
        usleep(50000);

	// Demonstrate how threads can access and modify shared memory
	if (shared_value == 0)
	{
		printf("%s: Ping!\n", name);
		shared_value = 1;
	} else 
	{
		printf("%s: Pong!\n", name);
		shared_value = 0;
	}
    }

    printf("Thread %s done!\n", name);
    return NULL;
}
 
int main(void)
{
    pthread_t th1, th2;
    pthread_create(&th1, NULL, worker, "X");
    usleep(25000); // Offset thread initiation to maximize ping/pong behavior
    pthread_create(&th2, NULL, worker, "Y");

    sleep(5);
    // Cancel th1, thread X
    printf("====> Cancelling Thread %lu!!\n", (unsigned long int)th1);
    int x_cancellation = pthread_cancel(th1);
    usleep(100000);
    
    // Cancel th2, thread Y
    printf("====> Cancelling Thread %lu!\n", (unsigned long int)th2);
    int y_cancellation = pthread_cancel(th2);

    // Verify thread cancellation success
    if (y_cancellation == 0 && x_cancellation == 0)
	    printf("Threads X and Y closed successfully!\n");

    printf("Exiting from main program\n");
    return 0;
}
