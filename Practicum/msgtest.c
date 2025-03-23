/*
 * msgtest.c / Practicum
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 3/23/2025
 *
 * Testing message.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "message.h"

int main() {
    // Test create_msg function
    printf("Testing create_msg...\n");
    Message* msg1 = create_msg("Alice", "Bob", "Hello, Bob! This is a test message.");
    if (msg1 == NULL) {
        fprintf(stderr, "Error creating message\n");
        return 1;
    }

    printf("Message created successfully:\n");
    printf("ID: %d\n", msg1->id);
    printf("Sender: %s\n", msg1->sender);
    printf("Receiver: %s\n", msg1->receiver);
    printf("Content: %s\n", msg1->content);
    printf("Delivered: %d\n", msg1->delivered);
    printf("Timestamp: %s\n", ctime(&(msg1->timestamp)));

    // Test store_msg function
    printf("\nTesting store_msg...\n");
    if (store_msg(msg1) != 0) {
        fprintf(stderr, "Error storing message\n");
        return 1;
    }
    printf("Message stored successfully.\n");

    // Test retrieve_msg function
    printf("\nTesting retrieve_msg...\n");
    Message* retrieved_msg = retrieve_msg(msg1->id);
    if (retrieved_msg == NULL) {
        fprintf(stderr, "Error retrieving message\n");
        return 1;
    }

    printf("Message retrieved successfully:\n");
    printf("ID: %d\n", retrieved_msg->id);
    printf("Sender: %s\n", retrieved_msg->sender);
    printf("Receiver: %s\n", retrieved_msg->receiver);
    printf("Content: %s\n", retrieved_msg->content);
    printf("Delivered: %d\n", retrieved_msg->delivered);
    printf("Timestamp: %s\n", ctime(&(retrieved_msg->timestamp)));

    // Test update_delivered function
    printf("\nTesting update_delivered...\n");
    if (update_delivered(msg1->id) != 0) {
        fprintf(stderr, "Error updating delivered flag\n");
        return 1;
    }

    // Retrieve and display the message again to check if the delivered flag was updated
    retrieved_msg = retrieve_msg(msg1->id);
    if (retrieved_msg == NULL) {
        fprintf(stderr, "Error retrieving message after update\n");
        return 1;
    }

    printf("Message after updating delivered flag:\n");
    printf("ID: %d\n", retrieved_msg->id);
    printf("Sender: %s\n", retrieved_msg->sender);
    printf("Receiver: %s\n", retrieved_msg->receiver);
    printf("Content: %s\n", retrieved_msg->content);
    printf("Delivered: %d\n", retrieved_msg->delivered);
    printf("Timestamp: %s\n", ctime(&(retrieved_msg->timestamp)));

    // Clean up dynamically allocated memory
    free(msg1);
    free(retrieved_msg);

    return 0;
}

