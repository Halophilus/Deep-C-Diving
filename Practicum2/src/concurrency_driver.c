/*
 * concurrency_driver.c / Practicum 2
 *
 * Ben Henshaw / CS5600 / Northeastern Univeristy
 * Spring 2025 / 4/13/2025
 *
 * Demonstration of concurrency
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

// State macros
#define NUM_OPERATIONS 5 // Number of concurrent operations forked
#define NUM_CYCLES 5 // Number of waves of operations
#define NUM_CLIENT_FILES 3
#define NUM_SERVER_FILES 3

// Files
const char *client_files[] = {"client1.txt", "client2.txt", "client3.txt"};
const char *server_files[] = {"server1.txt", "server2.txt", "server3.txt"};

// Command-line rfs arguments
const char *commands[] = {"WRITE", "GET", "RM"};

// Helper Function:     spawn_rfs_command
// --------------------------------------
// Forks process and invokes rfs in child with random params
//
// cmd:     1st clarg in rfs call
// arg1:    2nd clarg
// arg2:    3rd clarg
void spawn_rfs_command(const char *cmd, const char *arg1, const char *arg2) {
    pid_t pid = fork();

    if (pid == 0) { // If in child process
        if (strcmp(cmd, "RM") == 0) // If remove
        {
            //execlp("rfs", "rfs", "RM", arg1, NULL);
        } else // if write or get
        {
            execlp("rfs", "rfs", cmd, arg1, arg2, NULL);
        }
        perror("execlp failed");
        exit(1);
    }
}

// Main Function:   main
// ---------------------
// Spawns 20 random calls to RFS concurrently
int main()
{
    srand(1);

    for (int j = 0; j < NUM_CYCLES; j++)
    {
        for (int i = 0; i < NUM_OPERATIONS; i++)
        {
            // Pick a random command
            const char *cmd = commands[rand() % 3];

            char filename[128] = {'\0', };

            // And a random file
            int client_index = rand() % 3;
            int server_index = rand() % 3;

            // Pull filenames
            const char *client_file = client_files[client_index];
            const char *server_file = server_files[server_index];

            // Call relevant subdirectory to avoid file access errors by each client
            int client_fd = i + 1;
            sprintf(filename, "/%d/%s", client_fd, client_file);

            // Log operation
            printf("Operation %d: %s ", i + 1, cmd);


            if (strcmp(cmd, "WRITE") == 0)
            {
                printf("%s -> %s\n", filename, server_file);
                spawn_rfs_command("WRITE", filename, server_file);
            }
            else if (strcmp(cmd, "GET") == 0)
            {
                printf("%s -> %s\n", server_file, filename);
                spawn_rfs_command("GET", server_file, filename);
            }
            else
            {
                printf("%s\n", server_file);
                spawn_rfs_command("RM", filename, NULL);
            }

        }
        // Wait between cycles
        sleep(2);
    }


    // Wait for all children to complete
    while (wait(NULL) > 0);

    return 0;
}