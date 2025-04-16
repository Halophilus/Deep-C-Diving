/*
 * tcp.h / Practicum 2
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 4/16/2025
 *
 * Consolidating TCP operations for code legibility
 */
#ifndef TCP_H
#define TCP_H

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

#endif TCP_H
