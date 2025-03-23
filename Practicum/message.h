/*
 * message.h / Practicum 1
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 3/23/2025
 *
 * Data structure to store a message
 */
#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Struct:	Message
// --------------------
// Contains all relevant information for a message
typedef struct Message {
	int id;
	time_t timestamp;
	char sender[128];
	char receiver[128];
	char content[1024];
	bool delivered;
} Message;

// Function:	create_msg
// -----------------------
// Dynamically allocates a Message struct
//
// sender:	String of sender ID
// receiver:	String of receiver ID
// content:	String of message content
//
// returns: Message object from heap memory
Message* create_msg(const char* sender, const char* receiver const char* content);

// Function:	store_message
// --------------------------
// Stores copy of generated message object in a local text file
//
// msg:		Message object pointer
//
// returns: 0 on success, -1 on failure
int store_msg(const Message* msg);

// Function:	retrieve_message
// -----------------------------
// Retrieves a message from local storage based on ID
//
// id:		Integer Message.id target value
//
// returns:	Message* object pointer
Message* retrieve_message(int id);

#endif
