/*
 * messagedriver.c / Practicum 1
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 3/24/2025
 *
 * A simulation of a messaging platform
 */
#include "message.h"
#include<unistd.h>

// Function:    gen_msg
// --------------------
// Generates a random message, stores it, caches it
void gen_msg(void)
{
    int sender = rand() % 50;
    int message = rand() % 50;
    int receiver = rand() % 50;

    FILE* users;
    FILE* messages;

    users = fopen("ex_usr.txt", "r");
    messages = fopen("ex_msg.txt", "r");

    if (!users || !messages)
    {
        fprintf(stderr, "messagedriver.gen_msg: failed to open example files");
        return;
    }

    char sender_id[256];
    char receiver_id[256];
    char message_id[1024];

    // Get random sender
    for (int i = 0; i < sender; i++) fgets(sender_id, USER_SIZE, users);

    rewind(users);

    // Get random receiver
    for (int i = 0; i < receiver; i++) fgets(receiver_id, USER_SIZE, users);

    // Get random message content
    for (int i = 0; i < message; i++) fgets(message_id, CONTENT_SIZE, messages);

    // Generate a new message
    create_msg(sender_id, receiver_id, message_id);
}

// Function:    deliver_msg
// ------------------------
// Marks a random message as having been delivered
void deliver_msg(void)
{
    int max_id = get_id();
    int some_message = rand() % max_id;
    update_delivered(some_message);
}

// Main method
// -----------
// Performs 50 operations every second 20 times (1000 accesses)
// Operations are either gen_msg or deliver_msg
int main(void)
{
    make_cache();
    srand(time(NULL));
    gen_msg();

    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            int choice = rand() % 2;
            if (choice) gen_msg();
            else deliver_msg();
        }
        print_cache();
        sleep(5);
    }
}