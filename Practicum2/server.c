/*
 * server.c -- TCP Socket Server
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include "messenger.h"

// Function:    handle_inbound
// ---------------------------
// Handles commands from a client
int handle_inbound(int socket_desc)
{
    // Receive command
    char *cmd = receive_msg(socket_desc);
    if(!cmd)
    {
        fprintf(stderr, "server.handle_inbound: error receiving command from client\n");
        return -1;
    }

    // Parse command
    if (!strcmp(cmd, "WRITE") && !strcmp(cmd, "GET") && !strcmp(cmd, "RM"))
    {
        // Initiate handshake
        if(!send_msg("GO", socket_desc))
        {
            fprintf(stderr, "server.handle_inbound: handshake initiation aborted\n", cmd);
            free(cmd);
            return -1;
        }
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
int main(void)
{
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[8196], client_message[8196];
  
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
  
  // Accept an incoming connection:
  client_size = sizeof(client_addr);
  client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
  
  if (client_sock < 0){
    printf("Can't accept\n");
    close(socket_desc);
    close(client_sock);
    return -1;
  }
  printf("Client connected at IP: %s and port: %i\n", 
         inet_ntoa(client_addr.sin_addr), 
         ntohs(client_addr.sin_port));
  
  // Receive client's message:
  if (recv(client_sock, client_message, 
           sizeof(client_message), 0) < 0){
    printf("Couldn't receive\n");
    close(socket_desc);
    close(client_sock);
    return -1;
  }
  printf("Msg from client: %s\n", client_message);
  
  // Respond to client:
  strcpy(server_message, "This is the server's response message.");
  
  if (send(client_sock, server_message, strlen(server_message), 0) < 0){
    printf("Can't send\n");
    close(socket_desc);
    close(client_sock);
    return -1;
  }
  
  // Closing the socket:
  close(client_sock);
  close(socket_desc);
  
  return 0;
}
