/*
 * message.h / Practicum 1
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 3/23/2025
 *
 * Header file for message handling and caching
 */

#ifndef MESSAGE_H
#define MESSAGE_H
#define CACHE_SIZE 16


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Message structure definition
typedef struct 
{
	int id;
	time_t timestamp;
	char sender[256];
	char receiver[256];
	char content[1024];
	int delivered; // 0 (undelivered) or 1 (delivered)
	
	// Linked list 
	struct Message* next;
} Message;

// Cache structure definition
typedef struct Cache 
{
	Message* messages[CACHE_SIZE];
	Message* head;
	Message* tail;
	int size;
} Cache;


// Function prototypes for Message behavior
Message* create_msg(const char* sender, const char* receiver, const char* content);
Message* parse_row(char* row);
int store_msg(const Message* msg);
char* generate_msg_string(const Message* msg);
Message* retrieve_msg(int id);
int update_delivered(int id);
int get_id(void);
void print_msg(const Message* msg);

// Function prototypes for Cache behavior
void make_cache();
Message* cache_fetch(int id);
void cache_push(int id);
void cache_evict();
void cache_bump(int id);


Cache msg_cache;
#endif // MESSAGE_H

