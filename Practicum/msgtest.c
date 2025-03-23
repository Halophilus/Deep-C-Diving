/*
 * msgtest_new.c / Practicum
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 3/23/2025
 *
 * New Testing Driver for message.c
 */


#include <stdio.h>
#include <stdlib.h>
#include "message.h"

void test_create_message();
void test_store_and_retrieve_message();
void test_update_delivered();

int main() {
    printf("Starting tests...\n\n");
    
    test_create_message();
    test_store_and_retrieve_message();
    test_update_delivered();
    
    printf("\nAll tests completed.\n");
    return 0;
}

void test_create_message() {
    printf("\nTesting create_msg()...\n");
    Message* msg = create_msg("Alice", "Bob", "Hello Bob, this is a test.");
    
    if (msg == NULL) {
        fprintf(stderr, "Error: create_msg failed!\n");
        return;
    }
    
    print_msg(msg);
    free(msg);
}

void test_store_and_retrieve_message() {
    printf("\nTesting store_msg() and retrieve_msg()...\n");
    Message* msg = create_msg("Charlie", "Dave", "Testing message storage and retrieval.");
    
    if (store_msg(msg) != 0) {
        fprintf(stderr, "Error: store_msg failed!\n");
        free(msg);
        return;
    }
    
    Message* retrieved_msg = retrieve_msg(msg->id);
    if (retrieved_msg == NULL) {
        fprintf(stderr, "Error: retrieve_msg failed!\n");
        free(msg);
        return;
    }
    
    printf("Retrieved message:\n");
    print_msg(retrieved_msg);
    
    free(msg);
    free(retrieved_msg);
}

void test_update_delivered() {
    printf("\nTesting update_delivered()...\n");
    
    int id = get_id(); // Assume last message ID is the one to update
    if (update_delivered(id) != 0) {
        fprintf(stderr, "Error: update_delivered failed!\n");
        return;
    }
    
    Message* updated_msg = retrieve_msg(id);
    if (updated_msg == NULL) {
        fprintf(stderr, "Error: retrieve_msg after update failed!\n");
        return;
    }
    
    printf("Updated message:\n");
    print_msg(updated_msg);
    
    free(updated_msg);
}

