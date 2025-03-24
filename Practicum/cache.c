/*
 * cache.c / Practricum 1
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 3/24/2025
 *
 * Implementation of cache behavior
 */

#include "message.h"

// Function:	make_cache
// -----------------------
// Forms cache structure in memory
void make_cache()
{
	msg_cache.size = 0;
	msg_cache.head = msg_cache.tail = NULL;

	for (int i = 0; i < CACHE_SIZE; i++)
	{
		msg_cache.messages[i] = NULL;
	}
}

// Function:	cache_fetch
// ------------------------
// Looks for message with matching ID in cache
//
// id:		Integer target message ID
//
// returns:	Message object pointer if hit, NULL if miss
Message* cache_fetch(int id)
{
	if (msg_cache.head == NULL)
	{
		fprintf(stderr, "Fetch for message %d attempted on empty cache in cache.cache_fetch\n", id);
		return NULL;
	}

	Message* current_element = cache.head;

	while(current_element->next != NULL)
	{
		if (current_element->id == id)
		{
			return current_element;
		}
		
		current_element = current_element->next;
	}

	fprintf(stderr, "cache.cache fetch: ID %d not present in cache\n", id);
	return NULL;
}

// Function:	cache_push
// -----------------------
// Pushes a new Message object to the cache
//
// id:		integer target message ID
void cache_push(const Message* msg)
{
	if (msg == NULL)
	{
		fprintf(stderr, "cache.cache_push: Null pointer argument exception\n");
		return;
	}

	// If the cache is already full
	if (msg_cache.size == CACHE_SIZE)
	{
Message* evicted = msg_cache.tail;
		msg_cache.tail = evicted->next;
		free(evicted);
		msg_cache.size--;
	}  

	msg->next = msg_cache.head;
	msg_cach.head = msg;
	
	if (msg_cache.size == 0) msg_cache.tail = msg;

	msg_cache.size++;
}

// Function:	cache_bump
// -----------------------
// Updates a message's location in the cache
void cache_bump(int id)
{
	Message* 
}
