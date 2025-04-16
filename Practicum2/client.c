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
// Handles errors in the client
//
// response:    server response message
// socket_desc: fd for client socket
// msg:         error message
// server_msg:  optional message sent to server
int handle_error(char *response, int socket_desc, char *msg, char *server_msg)
{
    fprintf(stderr, "\n%s\n", msg);
    if (server_msg)
        send_msg(server_msg, socket_desc);
    clean_up(response, socket_desc);
    return -1;
}

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

// Function:    handle_write
// -------------------------
// Handling outbound write requests
//
// target:      target filename
// socket_desc: client socket fd
//
// returns 0 on success, 1 on file error, -1 on connection error
int handle_write(char *target, int socket_desc)
{
    
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
                    return handle_error(NULL, socket_desc,
                                 "client: lost connection during WRITE\n",
                                 NULL);
                case -1:
                    return handle_error(NULL, socket_desc,
                                        "client: error opening file during WRITE\n",
                                        NULL);
                default:
                    return handle_error(NULL, socket_desc,
                                        "client: undefined error during WRITE\n",
                                        NULL);
            }

            // Wait for server response (which contains versioned filename)
            char *response = receive_msg(socket_desc);

            if (!response) {
                return handle_error(response, socket_desc,
                                    "client: error getting server response after WRITE\n",
                                    NULL);
            }

            fprintf(stdout, "server: %s\n", response);
            clean_up(response, socket_desc);
            return 0;
        } else
        {
            return handle_error(NULL, socket_desc,
                                "client: WRITE request aborted by server before file could be sent\n",
                                NULL);
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
                    return handle_error(NULL, socket_desc,
                                        "client: lost connection during GET\n",
                                        "File download failed");
                case -1:
                    return handle_error(NULL, socket_desc,
                                        "client: error saving file during GET\n",
                                        "File download failed");
                default:
                    return handle_error(NULL, socket_desc,
                                        "client: undefined error during GET\n",
                                        "File download failed");
            }

            if(!send_msg("File download successful", socket_desc)){
                return handle_error(NULL, socket_desc,
                                    "client: error reaching server to confirm file transfer in GET\n",
                                    NULL);

            }
            fprintf(stdout, "client: GET request successful");
            clean_up(NULL, socket_desc);
            return 0;
        } else
        {
            return handle_error(NULL, socket_desc,
                                "client: GET request aborted by server before file could be sent\n",
                                NULL);
        }

    } else if (strcmp(argv[1], "RM") == 0)
    {
        if (!handle_outbound(argv[1],argv[2], socket_desc))
        {
            char *response = receive_msg(socket_desc);

            if (!response) {
                return handle_error(NULL, socket_desc, "client: error getting server response after RM\n", NULL);
            }

            fprintf(stdout, "server: %s\n", response);
            clean_up(response, socket_desc);
            return 0;
        } else
        {
            return handle_error(NULL, socket_desc, "client: RM request aborted by server\n", NULL);
        }
    }


    fprintf(stderr, "client no-op: rfs RM [target path]\n");

	// Syntax error
    error:
    fprintf(stderr, "client no-op: rfs {GET,WRITE} [target path] [destination path]\n");
	clean_up(NULL, socket_desc);
	
	return -1;
}
