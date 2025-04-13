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

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1028

// Function:	send_file
// ----------------------
// Opens a file and transmits it byte by byte to the provided socket
//
// filename: string indicating relative filepath
// socket_desc: file descriptor for the socket
//
// returns: 0 on success, -1 on memory allocation failure, 1 for other errors
int send_file(char *filename, int socket_desc)
{

#ifdef DEBUG
	fprintf(stdout, "DEBUG: client.send_file: attempting transfer of %s to socket %d\n", filename, socket_desc);
#endif

	char buffer[BUFFER_SIZE];
	FILE* fp = fopen(filename, "rb");
	if (!fp)
	{
		fprintf(stderr, "client.send_file: error opening file %s\n", filename);
		return -1;
	}

	int bytes_read;
	// Buffering the file one byte at a time
	while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0)
	{
		// Attempt to send contents of buffer
		int bytes_sent = send(socket_desc, buffer, bytes_read, 0);
		if(bytes_sent < 0) // If an error occurred
		{			
			fprintf(stderr, "client.send_file: error transmitting data to socket %d\n", socket_desc);
			fclose(fp);
			return 1;
		} 
		else // If the send was successful 
		{ 
		// No associated action
#ifdef DEBUG
		fprintf(stdout, "DEBUG: client.send_file: %d byte(s) sent to socket %d\n", bytes_sent, socket_desc);
#endif
		}
	}

#ifdef DEBUG
	fprintf(stdout, "DEBUG: client.send_file: file %s successfully sent to socket %d\n", filename, socket_desc);
#endif
	fclose(fp);
	return 0;
}

// Function:	retrieve_file
// -------------------------
// Retrieves a file over TCP and saves it locally
// 
// filename: string file name
// socket_desc: file descriptor for socket
//
// returns: 0 on success
int retrieve_file(char *filename, int socket_desc)
{


#ifdef DEBUG
	fprintf(stdout, "DEBUG: client.retrieve_file: attempting retrieval of %s from socket %d\n", filename, socket_desc);
#endif

	char buffer[BUFFER_SIZE];
	FILE* fp = fopen(filename, "wb");
	if (!fp)
	{
		fprintf(stderr, "client.retrieve_file: error opening file %s\n", filename);
		return -1;
	}

	int bytes_received;
	// Buffering the file one byte at a time
	while ((bytes_read = recv(socket_descbuffer, 1, BUFFER_SIZE, fp)) > 0)
	{
		// Attempt to send contents of buffer
		int bytes_sent = send(socket_desc, buffer, bytes_read, 0);
		if(bytes_sent < 0) // If an error occurred
		{			
			fprintf(stderr, "client.send_file: error transmitting data to socket %d\n", socket_desc);
			fclose(fp);
			return 1;
		} 
		else // If the send was successful 
		{ 
		// No associated action
#ifdef DEBUG
		fprintf(stdout, "DEBUG: client.send_file: %d byte(s) sent to socket %d\n", bytes_sent, socket_desc);
#endif
		}
	}

#ifdef DEBUG
	fprintf(stdout, "DEBUG: client.send_file: file %s successfully sent to socket %d\n", filename, socket_desc);
#endif
	fclose(fp);
	return 0;
}

// Function:	main
// -----------------
// Modified main method to take in clargs
int main(int argc, char *argv[])
{
	// Validate number of arguments
	if (argc < 
	int socket_desc;
	struct sockaddr_in server_addr;
	char server_message[2000], client_message[2000];
	
	// Clean buffers:
	memset(server_message,'\0',sizeof(server_message));
	memset(client_message,'\0',sizeof(client_message));
	
	// Create socket:
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	
	if(socket_desc < 0){
		printf("Unable to create socket\n");
		close(socket_desc);
		return -1;
	}
	
	printf("Socket created successfully\n");
	
	// Set port and IP the same as server-side:
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(2000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	// Send connection request to server:
	if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		printf("Unable to connect\n");
		close(socket_desc);
		return -1;
	}
	printf("Connected with server successfully\n");
	
	// Get input from the user:
	printf("Enter message: ");
	gets(client_message);
	
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
