/*
 * cache.c / Practicum 1
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 3/24/2025
 *
 * Implementation of cache behavior
 */
#include "message.h"

// Cache performance trackers
int accesses = 0;
int hits = 0;
int misses = 0;
int cache_prints = 0;

// Function:	make_cache
// -----------------------
// Forms cache structure in memory
void make_cache()
{
	msg_cache.size = 0;
	msg_cache.head = msg_cache.tail = NULL;
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
    // Empty cache
	if (msg_cache.head == NULL)
	{
		fprintf(stderr, "Cache.cache_fetch: Fetch for message %d attempted on empty cache\n", id);
		return NULL;
	}

    accesses++;
	Message* current_element = msg_cache.head;

    // Search LL for matching id
	while(current_element != NULL)
	{
        // Hit
		if (current_element->id == id)
		{
            hits++;
            cache_bump(current_element);
            return current_element;
		}
		
		current_element = current_element->next;
	}

    // Miss
    misses++;
	fprintf(stderr, "Cache.cache_fetch: ID %d not present in cache\n", id);
	return NULL;
}

// Function:    cache_fetch_previous
// ---------------------------------
// Helper function, obtains the previous element in the LL cache
//
// msg:         Message object pointer
//
// Returns:     Message prior to msg in LL
Message* cache_fetch_previous(Message* msg)
{
    // Null msg
    if (msg == NULL)
    {
        fprintf(stderr, "Cache.cache_fetch_previous: Null pointer argument exception\n");
        return NULL;
    }

    Message* current_element = msg_cache.head;

    // Empty cache
    if (!current_element)
    {
        fprintf(stderr, "Cache.cache_fetch_previous: Called on empty cache\n");
        return NULL;
    }

    // No previous element
    if (current_element == msg)
    {
        fprintf(stderr, "Cache.cache_fetch_previous: target message is at head of cache\n");
        return NULL;
    }

    // Search cache for element
    while(current_element->next != msg){
        // End of list reached
        if (current_element->next == NULL)
        {
            fprintf(stderr,"Cache.cache_fetch_previous: msg not found in cache");
            return NULL;
        }
        current_element = current_element->next;
    }

    return current_element;
}

// Function:	cache_push
// -----------------------
// Pushes a new Message object to the cache
//
// id:		integer target message ID
int cache_push(Message* msg)
{
	if (msg == NULL)
	{
		fprintf(stderr, "cache.cache_push: Null pointer argument exception\n");
		return -1;
	}

    // Check to see if it's already in the cache
    Message* cache_msg = cache_fetch(msg->id);
    if (cache_msg)
    {
        fprintf(stderr, "Cache.cache_push: msg %d already in cache\n", msg->id);
        return -1;
    }

	// If the cache is already full
	if (msg_cache.size == CACHE_SIZE)
	{
        // Evict the tail of the cache
        Message* evicted = msg_cache.tail;
		msg_cache.tail = cache_fetch_previous(evicted);
        msg_cache.tail->next = NULL;
		free(evicted);
		msg_cache.size--;
	}  

	msg->next = msg_cache.head;
	msg_cache.head = msg;
	
	if (msg_cache.size == 0) msg_cache.tail = msg;

	msg_cache.size++;
    return 0;
}

// Function:	cache_bump
// -----------------------
// Puts message to head of cache
void cache_bump(Message* msg)
{
    // If msg is already at the head, no need to bump
    if (msg == msg_cache.head) {
        fprintf(stderr, "Cache.cache_bump: msg already at head of cache\n");
        return;
    }

    // Find the previous message
    Message* prev_msg = cache_fetch_previous(msg);
    if (!prev_msg) {
        fprintf(stderr, "Cache.cache_bump: Unable to find previous message\n");
        return;
    }

    // If the target message is the tail, we need to update the tail to the new position
    if (msg == msg_cache.tail) {
        msg_cache.tail = prev_msg;
        prev_msg->next = NULL;
    }

    // Update the next pointer of the previous message to skip the target message
    prev_msg->next = msg->next;

    // Move msg to the head of the cache
    msg->next = msg_cache.head;
    msg_cache.head = msg;
}

// Function:    print_cache
// ------------------------
// Prints contents of the cache
void print_cache(void)
{
    printf("CACHE PRINT %d: ", cache_prints);
    Message* current_msg = msg_cache.head;

    // Empty cache
    if (!current_msg)
    {
        printf("EMPTY\n");
    }

    // n >= 1
    while (current_msg)
    {
        print_msg(current_msg);
        current_msg = current_msg->next;
    }

    // Cache feedback
    double hit_ratio = hits / accesses;
    printf("HIT RATIO: %f\n", hit_ratio);
    printf("ACCESSES: %d\nHITS: %d\nMISSES: %d\n", accesses, hits, misses);
    printf("-------------");
}