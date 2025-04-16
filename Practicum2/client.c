/*
 * client.c -- TCP Socket Client / Practicum 2
 * 
 * adapted from: 
 *	 https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 *
 *	 Ben Henshaw / CS5600 / Northeastern University
 *	 Spring 2025 / 4/12/2025
 *
 *	 Custom implementation of client.c from provided template
 */
#include "messenger.h"
#include "tcp.h"

// Function: clean_up
// ------------------
// Frees dynamic resources and closes socket
void clean_up(char *response, int socket_desc)
{
    free(response);
    close(socket_desc);
}

// Function:    handle_error
// -------------------------
// Handles errors in 
// Function:    handle_outbound
// ----------------------------
// Passes a command and a target to a TCP socket and confirms receipt
//
// cmd: string command to be issued
//
// returns 0 on success, -1 on failure
int handle_outbound(char *cmd, char *target, int socket_desc)
{
    // Notify the server
    if(!send_msg(cmd, socket_desc))
    {
        fprintf(stderr, "client.handle_outbound: unable to reach server for %s request\n", cmd);
        return -1;
    }

    // Wait until server is ready
    char *response = receive_msg(socket_desc);
    if(!response)
    {
        fprintf(stderr, "client.handle_outbound: error receiving server's response for %s request\n", cmd);
        return -1;
    }

    if (strcmp(response, "GO") == 0) // Approval is given
    {
        // Free original response
        free(response);
        response = NULL;

        // Send target filename
        if(!send_msg(target, socket_desc))
        {
            fprintf(stderr, "client.handle_outbound: error sending target %s to server\n", target);
            return -1;
        }

        // Get approval from server
        response = receive_msg(socket_desc);
        if(!response)
        {
            fprintf(stderr, "client.handle_outbound: error getting server response about target %s\n", target);
            return -1;
        }

        // If the signal is given to proceed
        if (strcmp(response, "CONTINUE") == 0)
        {
            free(response);
            return 0;
        }
        else
        {
            fprintf(stderr, "client.handle_outbound: session aborted by server before file could be sent\n");
            free(response);
            return -1;
        }
    }
    else
    {
        fprintf(stderr, "client.handle_outbound: session aborted by server before filename could be processed\n");
        free(response);
        return -1;
    }
}

// Function:	main
// -----------------
// Modified main method to take in clargs
int main(int argc, char *argv[])
{
	// Validate number of arguments
	if (argc < 3)
	{
		fprintf(stderr, "client: syntax error\n");
		return -1;
	}

    // Init client
	int socket_desc = client_init();
	
	// Handle different commands
	if (strcmp(argv[1], "WRITE") == 0) //
    {
        if (argc < 4) goto error;
        if (!handle_outbound(argv[1], argv[3], socket_desc))
        {
            // Attempt to send the file
            int sent = send_file(argv[2], socket_desc);
            switch (sent) // Error handling
            {
                case 0:
                    break;
                case 1:
                    fprintf(stderr, "client: lost connection during WRITE\n");
                    close(socket_desc);
                    return -1;
                case -1:
                    fprintf(stderr, "client: error opening file during WRITE\n");
                    close(socket_desc);
                    return -1;
                default:
                    fprintf(stderr, "client: undefined error during WRITE\n");
                    close(socket_desc);
                    return -1;
            }

            // Wait for server response (which contains versioned filename)
            char *response = receive_msg(socket_desc);

            if (!response) {
                fprintf(stderr, "client: error getting server response after WRITE\n");
                close(socket_desc);
                free(response);
                return -1;
            }

            fprintf(stdout, "server: %s\n", response);
            close(socket_desc);
            free(response);
            return 0;
        } else
        {
            fprintf(stderr, "client: WRITE request aborted by server before file could be sent\n");
            close(socket_desc);
            return -1;
        }
    } else if (strcmp(argv[1], "GET") == 0) // Requesting a file from the server
    {
        if (argc < 4) goto error;
        if (!handle_outbound(argv[1], argv[2], socket_desc))
        {
            int received = receive_file(argv[3], socket_desc);
            switch (received)
            {
                case 0:
                    break;
                case 1:
                    fprintf(stderr, "client: lost connection during GET\n");
                    send_msg("File download failed", socket_desc);
                    close(socket_desc);
                    return -1;
                case -1:
                    fprintf(stderr, "client: error saving file during GET\n");
                    send_msg("File download failed", socket_desc);
                    close(socket_desc);
                    return -1;
                default:
                    fprintf(stderr, "client: undefined error during GET\n");
                    send_msg("File download failed", socket_desc);
                    close(socket_desc);
                    return -1;
            }

            if(!send_msg("File download successful", socket_desc)){
                fprintf(stderr, "client: error reaching server to confirm file transfer in GET\n");
                close(socket_desc);
                return -1;
            }
            fprintf(stdout, "client: GET request successful");
            close(socket_desc);
            return 0;
        } else
        {
            fprintf(stderr, "client: GET request aborted by server before file could be sent");
            close(socket_desc);
            return -1;
        }
    } else if (strcmp(argv[1], "RM") == 0)
    {
        if (!handle_outbound(argv[1],argv[2], socket_desc))
        {
            char *response = receive_msg(socket_desc);

            if (!response) {
                fprintf(stderr, "client: error getting server response after RM\n");
                close(socket_desc);
                return -1;
            }

            fprintf(stdout, "server: %s\n", response);
            close(socket_desc);
            free(response);
            return 0;
        } else
        {
            fprintf(stderr, "client: WRITE request aborted by server before file could be sent\n");
            close(socket_desc);
            return -1;
        }
    }


    fprintf(stderr, "client no-op: rfs RM [target path]\n");

	// Syntax error
    error:
    fprintf(stderr, "client no-op: rfs {GET,WRITE} [target path] [destination path]\n");
	close(socket_desc);
	
	return 0;
}
