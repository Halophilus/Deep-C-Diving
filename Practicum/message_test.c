#include "message.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test: Creating a message
void test_create_msg()
{
    Message* msg = create_msg("Alice", "Bob", "Hello, Bob!");
    assert(msg != NULL);
    assert(msg->id > 0);  // ID should be greater than 0
    assert(strcmp(msg->sender, "Alice") == 0);
    assert(strcmp(msg->receiver, "Bob") == 0);
    assert(strcmp(msg->content, "Hello, Bob!") == 0);
    assert(msg->delivered == 0);  // Default delivered flag should be 0
    printf("test_create_msg passed\n");
    free(msg);
}

// Test: Parsing a row from CSV into a Message struct
void test_parse_row()
{
    const char* row = "1,Sun Mar 23 14:45:22 2025,Charlie,David,Hey there,0\n";
    Message* msg = parse_row((char*)row);
    assert(msg != NULL);
    assert(msg->id == 1);
    assert(strcmp(msg->sender, "Charlie") == 0);
    assert(strcmp(msg->receiver, "David") == 0);
    assert(strcmp(msg->content, "Hey there") == 0);
    assert(msg->delivered == 0);
    printf("test_parse_row passed\n");
    free(msg);
}

// Test: Storing a message in CSV
void test_store_msg()
{
    Message* msg = create_msg("Eve", "Frank", "Important message!");
    int result = store_msg(msg);
    assert(result == 0);  // store_msg should return 0 on success
    print_msg(msg);
    // Retrieve the stored message and check it
    Message* retrieved_msg = retrieve_msg(msg->id);
    assert(retrieved_msg != NULL);

    print_msg(retrieved_msg);
    assert(retrieved_msg->id == msg->id);
    assert(strcmp(retrieved_msg->sender, msg->sender) == 0);
    assert(strcmp(retrieved_msg->receiver, msg->receiver) == 0);
    assert(strcmp(retrieved_msg->content, msg->content) == 0);

    printf("test_store_msg passed\n");

    // Clean up
    free(msg);
    free(retrieved_msg);
}

// Test: Updating the 'delivered' flag of a message
void test_update_delivered()
{
    Message* msg = create_msg("Grace", "Heidi", "Message to deliver");
    store_msg(msg);

    // Update the delivered flag
    int result = update_delivered(msg->id);
    assert(result == 0);  // update_delivered should return 0 on success

    // Retrieve the message again and check the delivered flag
    Message* retrieved_msg = retrieve_msg(msg->id);
    assert(retrieved_msg != NULL);
    assert(retrieved_msg->delivered == 1);  // The flag should be updated to 1

    printf("test_update_delivered passed\n");

    // Clean up
    free(msg);
    free(retrieved_msg);
}

// Test: Retrieving a message with an invalid ID (should return NULL)
void test_retrieve_invalid_msg()
{
    Message* msg = retrieve_msg(999);  // Assume ID 999 doesn't exist
    assert(msg == NULL);  // Should return NULL for invalid ID
    printf("test_retrieve_invalid_msg passed\n");
}

// Test: Handling a null pointer in `store_msg`
void test_store_null_msg()
{
    int result = store_msg(NULL);
    assert(result == -1);  // store_msg should return -1 when passed NULL
    printf("test_store_null_msg passed\n");
}

// Test: Handle parsing of an invalid row (incorrect CSV format)
void test_parse_invalid_row()
{
    const char* invalid_row = "1,,Charlie,David,,0\n";  // Missing timestamp and content
    Message* msg = parse_row((char*)invalid_row);
    assert(msg == NULL);  // Should return NULL due to invalid row
    printf("test_parse_invalid_row passed\n");
}

// Test: Edge case with empty message content
void test_empty_message_content()
{
    Message* msg = create_msg("Alice", "Bob", "");
    assert(msg != NULL);
    assert(strcmp(msg->sender, "Alice") == 0);
    assert(strcmp(msg->receiver, "Bob") == 0);
    assert(strcmp(msg->content, "") == 0);  // Empty content should be handled properly
    assert(msg->delivered == 0);
    printf("test_empty_message_content passed\n");
    free(msg);
}

// Test: Check the correct format of the CSV string generation
void test_generate_msg_string_format()
{
    Message* msg = create_msg("John", "Alice", "This is a test message");
    char* csv_string = generate_msg_string(msg);
    assert(csv_string != NULL);

    // Check if the CSV string has the correct format
    assert(strstr(csv_string, "John") != NULL);
    assert(strstr(csv_string, "Alice") != NULL);
    assert(strstr(csv_string, "This is a test message") != NULL);

    printf("test_generate_msg_string_format passed\n");

    free(csv_string);
    free(msg);
}


int main()
{
    // Run all tests
    test_create_msg();
    test_parse_row();
    test_store_msg();
    test_update_delivered();
    test_retrieve_invalid_msg();
    test_store_null_msg();
    test_parse_invalid_row();
    test_empty_message_content();
    test_generate_msg_string_format();

    printf("All tests passed!\n");
    return 0;
}

