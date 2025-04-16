/*
 * messenger.h / Practicum 2
 *
 * Ben Henshaw / CS5600 / Northeastern Univeristy
 * Spring 2025 / 4/13/2025
 *
 * Header file for symmetric TCP operations
 */
#ifndef MESSENGER_H
#define MESSENGER_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <malloc.h>

#define BUFFER_SIZE 1028
#define DEBUG
#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 2000

// Function:    server_init
// ------------------------
// Initializes a TCP server
//
// Returns fd associated with socket
int server_init();

// Function:    client_init
// ------------------------
// Initializes a TCP client and attempts to connect to server
//
// Returns fd associated with socket
int client_init();

// Function:	send_file
// ----------------------
// Opens a file and transmits it byte by byte to the provided socket
//
// filename: string indicating relative filepath
// socket_desc: file descriptor for the socket
//
// returns: 0 on success, -1 on memory allocation failure, 1 for other errors
int send_file(char *filename, int socket_desc);

// Function:	receive_file
// -------------------------
// Receives a file over TCP and saves it locally, automatically handles versioning and directory existence checks
// 
// filename: string file name
// socket_desc: file descriptor for socket
//
// returns: 0 on success, -1 on 
int receive_file(char *filename, int socket_desc);

// Function:	send_msg
// ---------------------
// Sends a simple message over TCP
//
// msg: string containing message
// socket_desc: file descriptor for destination socket
//
// returns 0 on success, -1 on failure
int send_msg(char *msg, int socket_desc);

// Function:	receive_msg
// ------------------------
// Receives a simple message over TCP
//
// msg: string containing message
// socket_desc: file descriptor for origin socket
//
// returns 0 on success, -1 on failure
char* receive_msg(int socket_desc);

#endif
