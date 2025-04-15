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
int socket_desc, client_sock;

// Function:    handle_inbound
// ---------------------------
// Handles commands from a client, parsing command and target identity to perform some operation
//
// Commands:
// GET: fetches a file from the server and transfers it to client
//
int handle_inbound(int client_socket)
{
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
        if (!send_msg("GO", client_socket)) {
            fprintf(stderr, "\nserver.handle_inbound: handshake initiation aborted at GO\n");
            free(cmd);
            return -1;
        }

        // Take in target name
        char *target = receive_msg(client_socket);
        if (!target) {
            fprintf(stderr, "\nserver.handle_inbound: error receiving target ID from client\n");
            free(cmd);
            return -1;
        }

        // Prompt client to fulfill request
        if (!send_msg("CONTINUE", client_socket)) {
            fprintf(stderr, "\nserver.handle_inbound: handshake initiation aborted at CONTINUE\n");
            free(cmd);
            free(target);
            return -1;
        }

        // Behavior controlled by cmd
        if (!strcmp(cmd, "WRITE")) // Write request
        {
            // Attempt file receipt
            int received = receive_file(target, client_socket);
            switch (received) {
                case 0:
                    break;
                case 1:
                    fprintf(stderr, "\nserver.handle_inbound: lost connection during WRITE\n");
                    send_msg("File write failed", client_socket);
                    close(client_socket);
                    free(cmd);
                    free(target);
                    return -1;
                    break;
                case -1:
                    fprintf(stderr, "\nserver.handle_inbound: error saving file during WRITE\n");
                    send_msg("File write failed", client_socket);
                    close(client_socket);
                    free(cmd);
                    free(target);
                    return -1;
                    break;
                default:
                    fprintf(stderr, "\nserver.handle_inbound: undefined error during WRITE\n");
                    send_msg("File write failed", client_socket);
                    close(client_socket);
                    free(cmd);
                    free(target);
                    return -1;
                    break;
            }

            // Get update from client
            if (!send_msg("\nFile written successfully", client_socket)) {
                fprintf(stderr, "server.handle_inbound: file transfer success message aborted\n");
                close(client_socket);
                free(cmd);
                free(target);
                return -1;
            }
        } else if (!strcmp(cmd, "GET")) // Get request
        {
            int sent = send_file(target, client_socket);
            switch (sent) // Error handling
            {
                case 0:
                    break;
                case 1:
                    fprintf(stderr, "\nserver.handle_inbound: lost connection during GET\n");
                    free(cmd);
                    free(target);
                    close(client_socket);
                    return -1;
                    break;
                case -1:
                    fprintf(stderr, "\nserver.handle_inbound: error opening file during GET\n");
                    free(cmd);
                    free(target);
                    close(client_socket);
                    return -1;
                    break;
                default:
                    fprintf(stderr, "\nserver.handle_inbound: undefined error during GET\n");
                    free(cmd);
                    free(target);
                    close(client_socket);
                    return -1;
                    break;
            }

            // Confirm receipt with client
            char *response = receive_msg(client_socket);
            if (!response) {
                fprintf(stderr, "\nserver.handle_inbound: error getting server response about target %s\n", target);
                free(cmd);
                free(target);
                close(client_socket);
                return -1;
            }

            fprintf(stdout, "\nclient: %s\n", response);
            free(cmd);
            free(target);
            close(client_socket);
            return 0;
        } else if (!strcmp(cmd, "RM")) // File delete request
        {
            if (!unlink(target)) {
                fprintf(stdout, "\nserver: %s deleted", target);
                send_msg("target deleted successfully", client_socket);
                free(cmd);
                free(target);
                close(client_socket);
                return 0;
            } else
            {
                fprintf(stderr, "\nserver: %s could not be deleted", target);
                send_msg("target could not be deleted", client_socket);
                free(cmd);
                free(target);
                close(client_socket);
                return -1;
            }

        }
    } else
    {
	fprintf(stderr, "\nserver: client request did not issue valid command\n");
    	free(cmd);
	close(client_socket);
	return -1;
    }

}

// Function:    handle_sigint
// --------------------------
// Closes client and server socket upon keyboard interrupt
void handle_sigint(int sig)
{
    fprintf(stdout, "\nserver: shutting down\n");
    close(socket_desc);
    close(client_sock);
    exit(0);
}

// Function:    main
// -----------------
// Modified main function that keeps the server online until a keyboard interrupt is invoked
int main(void)
{
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[8196], client_message[8196];

  // Register signature handler
  signal(SIGINT, handle_sigint);

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));
  
  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  
  if(socket_desc < 0){
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");
  
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  // Bind to the set port and IP:
  if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("Couldn't bind to the port\n");
    return -1;
  }
  printf("Done with binding\n");
  
  // Listen for clients:
  if(listen(socket_desc, 1) < 0){
    printf("Error while listening\n");
    close(socket_desc);
    return -1;
  }
  printf("\nListening for incoming connections.....\n");
  
  // Accept incoming connections on loop:
  while(1)
  {
      client_size = sizeof(client_addr);
      client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);

      // Check success
      if (client_sock < 0){
          printf("Can't accept\n");
          close(socket_desc);
          close(client_sock);
          return -1;
      }

      // Tell console
      printf("Client connected at IP: %s and port: %i\n",
             inet_ntoa(client_addr.sin_addr),
             ntohs(client_addr.sin_port));

      // Parse command
      handle_inbound(client_sock);
  }
  
  // Closing the socket:
  close(client_sock);
  close(socket_desc);
  
  return 0;
}
