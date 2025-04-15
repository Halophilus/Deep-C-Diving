/*
 * messenger.c / Practicum 2
 *
 * Ben Henshaw / CS5600 / Northeastern Univeristy
 * Spring 2025 / 4/13/2025
 *
 * Source file for symmetric TCP operations
 */

#include "messenger.h"

// Helper Function:    data_per_column
// -----------------------------------
// Find the amount of data in the file proportional to a single column in stdout
//
// file_size: size of file in bytes
//
// returns transfer volume per column in a progress bar
double data_per_column(const uint32_t file_size)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int window_width = w.ws_col;
    return(double)file_size / (double)window_width;
}

// Helper Function:    print_progress_bar
// -------------------------------
// Prints '#'s proportionate to download/upload progress
//
// previous_progress: represents amount of progress since progress bar last updated
// column_volume: write volume proportional to 1*'#'
void print_progress_bar(double *previous_progress, const double column_volume)
{
    while (*previous_progress > column_volume)
    {
        fprintf(stdout, "#");
        *previous_progress -= column_volume;
    }
}

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
		fprintf(stderr, "send_file: error opening file %s\n", filename);
		return -1;
	}

	// Get and send file size
	fseek(fp, 0, SEEK_END);
	uint32_t file_size = (uint32_t) ftell(fp);
	rewind(fp);

    file_size = htonl(file_size);
	if (send(socket_desc, &file_size, sizeof(file_size), 0) == -1)
	{
		fprintf(stderr, "send_file: error sending file size to socket %d\n", socket_desc);
		fclose(fp);
		return 1;
	}

	// File transfer progress display information
	size_t bytes_read;
	size_t bytes_transferred = 0;
    double previous_progress = 0;
    double column_volume = data_per_column(file_size);

    // Indent for progress bar
    fprintf(stdout, "\n");

	// Buffered iteration through the file
	while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0)
	{
		// Mechanism to handle when all bytes aren't sent at once
		int bytes_sent = 0;
		while (bytes_sent < bytes_read)
		{
			// Attempt to send the buffer
			int result = send(socket_desc, buffer + bytes_sent, bytes_read - bytes_sent, 0);
			if (result == -1) // If sending failed
			{
				fprintf(stderr, "\nsend_file: Error sending data from file %s to socket %d\n", filename, socket_desc);
				fclose(fp);
				return 1;
			}

			// Increment the amount of bytes sent
			bytes_sent += result;		
		}

		// Handle progress bar logic
		bytes_transferred += bytes_read;
		previous_progress += (double)bytes_read;

        print_progress_bar(&previous_progress, column_volume);
	}

    // Add newline after progress bar terminates
    fprintf(stdout, "\n");

#ifdef DEBUG
	fprintf(stdout, "DEBUG: client.send_file: file %s successfully sent to socket %d\n", filename, socket_desc);
#endif
	fclose(fp);
	return 0;
}

// Function:	receive_file
// -------------------------
// Receives a file over TCP and saves it locally,
// 
// filename: string file name
// socket_desc: file descriptor for socket
//
// returns: 0 on success, -1 on 
int receive_file(char *filename, int socket_desc)
{

#ifdef DEBUG
	fprintf(stdout, "receive_file: attempting retrieval of %s from socket %d\n", filename, socket_desc);
#endif

	// Find outer directory
	char directory_name[BUFFER_SIZE];
	char *last_slash = strrchr(filename, '/');
	
	// If there is a specified relative path
	if (last_slash)
	{
		// Truncate outer directory
		int last_slash_index = last_slash - filename;
		strcpy(directory_name, filename);
		directory_name[last_slash_index] = '\0';

#ifdef DEBUG
		fprintf(stdout, "DEBUG receive_file: outer directory provided: %s\n", directory_name);
#endif

		// Test if outer directory exists
		struct stat info;
		if (stat(directory_name, &info) != 0)
		{
			perror("stat"); // stat related error
			return -1;
		}
		
		// If the provided path isn't a directory, abort
		if (!S_ISDIR(info.st_mode))
		{
			fprintf(stderr, "receive_file: directory queried at socket %d doesn't exist\n", socket_desc);
			return -1;
		}
	}

	// Test if the file exists
	char filename_buffer[BUFFER_SIZE];
	strcpy(filename_buffer, filename);
	int version = 0;

	// Iterate through version numbers until an unused filename is discovered
    snprintf(filename_buffer, BUFFER_SIZE, "%s", filename);
    while (access(filename_buffer, F_OK) == 0) {
        snprintf(filename_buffer, BUFFER_SIZE, "%s%d", filename, version++);
    }

#ifdef DEBUG
	fprintf(stdout, "DEBUG receive_file: proceeding to download to %s at socket %d\n", filename_buffer, socket_desc);
#endif

    // Open file
	FILE* fp = fopen(filename_buffer, "wb");
	if (!fp)
	{
		fprintf(stderr, "receive_file: error opening file %s\n", filename);
		return -1;
	}

	// Receive file_size
	int file_size;
	if (recv(socket_desc, &file_size, sizeof(file_size), 0) == -1)
	{
		fprintf(stderr, "receive_file: error receiving file size of %s\n", filename);
		fclose(fp);
		return 1;
	}
    file_size = ntohl(file_size);

#ifdef DEBUG
	fprintf(stdout, "DEBUG receive_file: file size of %d", file_size);
#endif

	// Receive file data
	char buffer[BUFFER_SIZE];
	int total_bytes_received = 0;
    double column_volume = data_per_column(file_size);
    double previous_progress = 0;

    // Indent for progress bar
    fprintf(stdout, "\n");

    while (total_bytes_received < file_size)
	{
		int bytes_received = recv(socket_desc, buffer, BUFFER_SIZE, 0);
	       	if (bytes_received < 0)
		{
			fprintf(stderr, "\nreceive_file: error occurred receiving data\n");
			fclose(fp);
			return 1;
		}
		if (bytes_received == 0)
		{
			fprintf(stderr, "\nreceive_file: sender disconnected mid-stream\n");
			fclose(fp);
			return 1;
		}

		// Write the received data to file
		fwrite(buffer, 1, bytes_received, fp);
		total_bytes_received += bytes_received;
        previous_progress += (double)bytes_received;

        print_progress_bar(&previous_progress, column_volume);

#ifdef DEBUG
		fprintf(stdout, "DEBUG: receive_file: %d bytes received of %d total bytes\n", bytes_received, total_bytes_received);
#endif

	}
#ifdef DEBUG
	fprintf(stdout, "DEBUG: client.send_file: file %s successfully sent to socket %d\n", filename_buffer, socket_desc);
#endif
	fclose(fp);
	return 0;
}

// Function:	send_msg
// ---------------------
// Sends a simple message over TCP
//
// msg: string containing message
// socket_desc: file descriptor for destination socket
//
// returns 0 on success, -1 on failure
int send_msg(char *msg, int socket_desc)
{
	if (send(socket_desc, msg, BUFFER_SIZE, 0) < 0)
		return -1;
	else
		return 0;
}

// Function:	receive_msg
// ------------------------
// Receives a simple message over TCP and returns a dynamically allocated string
//
// msg: string containing message
// socket_desc: file descriptor for origin socket
//
// returns 0 on success, -1 on failure
char* receive_msg(int socket_desc)
{
	char *msg = (char *)malloc(BUFFER_SIZE);
	
	if (recv(socket_desc, msg, BUFFER_SIZE, 0) < 0)
    {
        free(msg);
        return NULL;
    }
	else
		return msg;
}
