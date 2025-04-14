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
	int socket_desc;
	struct sockaddr_in server_addr;
	char server_message[2000], client_message[2000];
	
	// Clean buffers:
	memset(server_message,'\0',sizeof(server_message));
	memset(client_message,'\0',sizeof(client_message));
	
	// Create socket:
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc < 0){
		fprintf(stderr, "client: unable to create socket\n");
		close(socket_desc);
		return -1;
	}	
	
	// Set port and IP the same as server-side:
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(2000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	// Send connection request to server:
	if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		fprintf("client: unable to connect to server\n");
		close(socket_desc);
		return -1;
	}
	
	// Handle different commands
	if (strcmp(argv[1], "WRITE") == 0)
	{
		// Notify the server
		char *msg = "WRITE";
		if(send(socket_desc, msg, strlen(msg), 0) < 0)
		{
			fprintf(stderr, "client: unable to reach server for WRITE request\n");
			close(socket_desc);
			return -1;
		}
		
		// Wait until server is ready
		if(recv(socket_desc, server_message, sizeof(server_message), 0 < 0))
		{
			fprintf(stderr, "client: error receiving server's response for WRITE request\n");
			close(socket_desc);
			return -1;
		}

		if (strcmp(server_message, "GO") == 0)
		{
			// Clear buffers
			memset(server_message,'\0',sizeof(server_message));
			memset(client_message,'\0',sizeof(client_message));
			
			// Send destination filename
			if(send(socket_desc, argv[3], BUFFER_SIZE, 0) < 0)
			{
				fprintf(stderr, "client: error sending destination filename to server\n");
				close(socket_desc);
				return -1;
			}

			if(recv(socket_desc
			send_file(argv[2], socket_desc);
	
	}

	// Send the message to server:
	if(send(socket_desc, client_message, strlen(client_message), 0) < 0){
		printf("Unable to send message\n");
		close(socket_desc);
		return -1;
	}
	
	// Receive the server's response:
	if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
		printf("Error while receiving server's msg\n");
		close(socket_desc);
		return -1;
	}
	
	printf("Server's response: %s\n",server_message);
	
	// Close the socket:
	close(socket_desc);
	
	return 0;
}
