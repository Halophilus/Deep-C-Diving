/*
 * server.c -- TCP Socket Server
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 *
 *   Ben Henshaw / CS5600 / Northeastern University
 *   Spring 2025 / 4/14/2025
 *
 *   Custom implementation of server.c from provided template
 */

#include <stdlib.h>
#include <signal.h>
#include "messenger.h"
#include "waitingroom.h"

int socket_desc, client_sock;

// Function:    clean_up
// ---------------------
// Frees allocated memory for inbound requests
//
// cmd:         command issued
// target:      target filename
// client:      socket file descriptor
void clean_up(char *cmd, char *target, int client)
{
    free(cmd);
    free(target);
    close(client);
}

// Function:    handle_error
// -------------------------
// Error message and cleanup for processes following handshake
//
// cmd:         command issued
// target:      target filename
// client:      socket file descriptor
// msg:         error message
// client_msg:  message sent to client
//
// returns -1 to indicate error state
int handle_error(char *cmd, char *target, int client, char *msg, char *client_msg)
{
    if (msg)
        fprintf(stderr, "%s\n", msg);
    if (client_msg)
        send_msg(client_msg, client);

    clean_up(cmd, target, client);
    return -1;
}

// Function:    handle_write
// -------------------------
// Server process handling write request
//
// client_socket:   socket fd
// target:          target filename
//
// returns 0 on success, 1 on lost connection, -1 for file saving errors
int handle_write(int client_socket, char *target)
{
    int received = receive_file(target, client_socket);
    switch (received) {
        case 0:
            break;
        case 1:
            return handle_error(NULL, target, client_socket,
                                "\nserver.handle_inbound: lost connection during WRITE\n",
                                "File write failed");
        case -1:
            return handle_error(NULL, target, client_socket,
                                "\nserver.handle_inbound: error saving file during WRITE\n",
                                "File write failed");
        default:
            return handle_error(NULL, target, client_socket,
                                "\nserver.handle_inbound: undefined error during WRITE\n",
                                "File write failed");
    }

    // Get update from client
    if (!send_msg("File written successfully", client_socket)) {
        return handle_error(NULL, target, client_socket,
                            "server.handle_inbound: file transfer success message aborted\n",
                            NULL);
    }
    return 0;
}

// Function:    handle_get
// -----------------------
// Server process handling get request
//
// client_socket:   socket fd
// target:          target filename
//
// returns 0 on success, 1 on lost connection, -1 for file saving errors
int handle_get(int client_socket, char *target)
{
    int sent = send_file(target, client_socket);
    switch (sent) // Error handling
    {
        case 0:
            break;
        case 1:
            return handle_error(NULL, target, client_socket,
                         "\nserver.handle_inbound: lost connection during GET\n",
                        NULL);
        case -1:
            return handle_error(NULL, target, client_socket,
                                "\nserver.handle_inbound: error opening file during GET\n",
                                NULL);
        default:
            return handle_error(NULL, target, client_socket,
                                "\nserver.handle_inbound: undefined error during GET\n",
                                NULL);
    }

    // Confirm receipt with client
    char *response = receive_msg(client_socket);
    if (!response) {
        return handle_error(NULL, target, client_socket,
                            "\nserver.handle_inbound: error getting server response about target %s\n",
                            NULL);
    }

    fprintf(stdout, "\nclient: %s\n", response);
    clean_up(NULL, target, client_socket);
    return 0;
}

// Function:    handle_rm
// ----------------------
// Server process to handle removal command
//
// client_socket:   socket fd
// target:          target filename
//
// returns 0 on success, 1 on lost connection, -1 for file saving errors
int handle_rm(int client_socket, char *target)
{
    if (!unlink(target)) // Attempt delete
    { // Upon success
        fprintf(stdout, "\nserver: %s deleted", target);
        send_msg("target deleted successfully", client_socket);
        clean_up(NULL, target, client_socket);
        return 0;
    } else // Failure
    {
        return handle_error(NULL, target, client_socket,
                            "\nserver: %s could not be deleted",
                            "target could not be deleted");
    }
}


// Function:    handle_inbound
// ---------------------------
// Handles commands from a client, parsing command and target identity to perform some operation
//
// Commands:
// GET: fetches a file from the server and transfers it to client
//
int handle_inbound(int client_socket)
{
    char *target; // target file
    int result = -1; // status flag
    // Receive command
    char *cmd = receive_msg(client_socket);
    if(!cmd)
    {
        fprintf(stderr, "\nserver.handle_inbound: error receiving command from client\n");
        return -1;
    }

#ifdef DEBUG
    fprintf(stdout, "\nDEBUG server.handle_inbound: CMD = %s", cmd);
#endif

    // Parse command
    if (!strcmp(cmd, "WRITE") || !strcmp(cmd, "GET") || !strcmp(cmd, "RM")) {
        // Initiate handshake
        if (!send_msg("GO", client_socket))
            return handle_error(cmd, NULL, client_socket,
                                "\nserver.handle_inbound: handshake initiation aborted at GO\n",
                                NULL);
        // Take in target name
        target = receive_msg(client_socket);
        if (!target)
            return handle_error(cmd, NULL, client_socket,
                                "\nserver.handle_inbound: error receiving target ID from client\n",
                                NULL);

        // Prompt client to fulfill request
        if (!send_msg("CONTINUE", client_socket))
            return handle_error(cmd, target, client_socket,
                                "\nserver.handle_inbound: handshake initiation aborted at CONTINUE\n",
                                NULL);


        // Behavior controlled by cmd
        if (!strcmp(cmd, "WRITE")) // Write request
        {
            result = handle_write(client_socket, target);
        } else if (!strcmp(cmd, "GET")) // Get request
        {
            result = handle_get(client_socket, target);
        } else if (!strcmp(cmd, "RM")) // File delete request
        {
            result = handle_rm(client_socket, target);
        }
    }
    else // If the second command is invalid
    	return handle_error(cmd, NULL, client_socket,
                            "\nserver: client request did not issue valid command\n",
                            NULL);

    clean_up(cmd, target, client_socket);
    return result;
}

// Function:    handle_sigint
// --------------------------
// Closes client and server socket upon keyboard interrupt
void handle_sigint(int sig)
{
    fprintf(stdout, "\nserver: shutting down\n");
    cleanup_waiting_room();
    close(socket_desc);
    exit(sig);
}

// Function:    main
// -----------------
// Modified main function that keeps the server online until a keyboard interrupt is invoked
int main(void)
{
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;

  // Register signature handler
  signal(SIGINT, handle_sigint);
  
  // Initialize server, open inbound socket
  socket_desc = server_init();

  // Initialize waiting room / file map
  waiting_room_init();

  // Accept incoming connections on loop:
  while(1)
  {
      client_size = sizeof(client_addr);
      client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);

      // Check success
      if (client_sock < 0){
          printf("Can't accept\n");
          close(client_sock);
          handle_error(NULL, socket_desc, "client: error getting server response after RM\n", NULL);
          handle_sigint(-1);
      }

      // Tell console
      printf("Client connected at IP: %s and port: %i\n",
             inet_ntoa(client_addr.sin_addr),
             ntohs(client_addr.sin_port));

      // Get the target filename
      char *filename = receive_msg(client_sock);

      if (!filename) {
          close(client_sock);
          handle_error(NULL, socket_desc, "client: error getting server response after RM\n", NULL);
          handle_sigint(-1);
      }

      // Pass request to the waiting room
      make_request(filename, client_sock, handle_inbound);
  }

}
