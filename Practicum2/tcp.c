/*
 * tcp.c / Practicum 2
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 4/16/2025
 *
 * Consolidating TCP server/client initialization for code legibility
 */


#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

// Function:    server_init
// ------------------------
// Initializes a TCP server
//
// Returns fd associated with socket
int server_init()
{
    socklen_t client_size;
    struct sockaddr_in server_addr, client_addr;

    // Create socket:
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

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

    return socket_desc;
}

// Function:    client_init
// ------------------------
// Initializes a TCP client and attempts to connect to server
//
// Returns fd associated with socket
int client_init()
{
    // Init client
    int socket_desc;
    struct sockaddr_in server_addr;

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
        fprintf(stderr, "client: unable to connect to server\n");
        close(socket_desc);
        return -1;
    }

    return socket_desc;
}