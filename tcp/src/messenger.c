/*
 * messenger.c / Practicum 2
 *
 * Ben Henshaw / CS5600 / Northeastern Univeristy
 * Spring 2025 / 4/13/2025
 *
 * Consolidation of TCP operations
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
    int window_width = w.ws_col; // Number of columns in the window
	double column_volume = (double)file_size / (double)window_width; // data volume per column
#ifdef DEBUG
	fprintf(stdout, "\nmessenger.data_per_column: %d columns in this window, data per column: %lf\n", window_width, column_volume);
#endif
    return column_volume;
}

// Helper Function:    print_progress_bar
// -------------------------------
// Prints '#'s proportionate to download/upload progress
// Works by iteratively subtracting a volume of data proportional to the amount of
// download progress since the last time it was called and printing an '#' to
// represent the fraction of the total file downloaded so far
//
// previous_progress: represents amount of progress since progress bar last updated
// column_volume: write volume proportional to 1*'#'
void print_progress_bar(double *previous_progress, const double column_volume)
{
#ifdef DEBUG
    fprintf(stdout, "\nDEBUG messenger.print_progress_bar: previous_progress %lf\n", *previous_progress);
#endif

    while (*previous_progress > column_volume)
    {
        fprintf(stdout, "#");
        *previous_progress -= column_volume;
        fflush(stdout);
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
	fprintf(stdout, "DEBUG: messenger.send_file: attempting transfer of %s to socket %d\n", filename, socket_desc);
#endif

    // Open target file
	char buffer[BUFFER_SIZE];
	FILE* fp = fopen(filename, "rb");
	if (!fp)
	{
		fprintf(stderr, "messenger.send_file: error opening file %s\n", filename);
		return -1;
	}

	// Get and send file size
	fseek(fp, 0, SEEK_END);
	uint32_t file_size = (uint32_t) ftell(fp);
	rewind(fp);

    // Discovering column volume
    double column_volume = data_per_column(file_size);

    // Query information about the requested directory
    int directory_confirmation;
    if (recv(socket_desc, &directory_confirmation, sizeof(int), 0) == -1)
    {
        fprintf(stderr, "messenger.send_file: error confirming filepath validity for %s\n", filename);
        fclose(fp);
        return 1;
    }

    // If the directory is malformed
    if (!directory_confirmation)
    {
        fprintf(stderr, "messenger.send_file: invalid destination filepath %s\n", filename);
        fclose(fp);
        return 1;
    }

    // Send file size so the host can track transfer continuity
    file_size = htonl(file_size);
	if (send(socket_desc, &file_size, sizeof(file_size), 0) == -1)
	{
		fprintf(stderr, "messenger.send_file: error sending file size to socket %d\n", socket_desc);
		fclose(fp);
		return 1;
	}

	// File transfer progress display information
	int bytes_read;
	int total_bytes_transferred = 0;
	double previous_progress = 0;
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
				fprintf(stderr, "\nmessenger.send_file: Error sending data from file %s to socket %d\n", filename, socket_desc);
				fclose(fp);
				return 1;
			}
			// Increment the amount of bytes sent
			bytes_sent += result;		
		}

		// Handle progress bar logic
		total_bytes_transferred += bytes_read;
		previous_progress += (double)bytes_read;
        print_progress_bar(&previous_progress, column_volume);
	}

    // Add newline after progress bar terminates
    fprintf(stdout, "\n");

#ifdef DEBUG
	fprintf(stdout, "DEBUG: messenger.send_file: file %s successfully sent to socket %d\n", filename, socket_desc);
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
	fprintf(stdout, "messenger.receive_file: attempting retrieval of %s from socket %d\n", filename, socket_desc);
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
		fprintf(stdout, "DEBUG messenger.receive_file: outer directory provided: %s\n", directory_name);
#endif

		// Test if outer directory exists
		struct stat info;
		if (stat(directory_name, &info) != 0)
		{
            fprintf(stderr, "messenger.receive_file: directory queried at socket %d doesn't exist\n", socket_desc); // stat related error

            int message = 0;
            if (send(socket_desc, &message, sizeof(message), 0) == -1)
                fprintf(stderr, "messenger.receive_file: error notifying client of directory discovery failure\n");
            return -1;
		}
		
		// If the provided path isn't a directory, abort
		if (!S_ISDIR(info.st_mode))
		{
			fprintf(stderr, "receive_file: directory queried at socket %d is not a path\n", socket_desc);
            int message = 0;
            if (send(socket_desc, &message, sizeof(message), 0) == -1)
                fprintf(stderr, "messenger.receive_file: error notifying client of directory discovery failure\n");
            return -1;
		}
	}

#ifdef DEBUG
	fprintf(stdout, "DEBUG receive_file: proceeding to download to %s at socket %d\n", filename, socket_desc);
#endif

    // Signal that the directory exists.
    int message = 1;
    if (send(socket_desc, &message, sizeof(message), 0) == -1)
    {
        fprintf(stderr, "messenger.receive_file: error notifying client of directory discovery failure\n");
    }

    // Open file
	FILE* fp = fopen(filename, "wb");
	if (!fp)
	{
		fprintf(stderr, "receive_file: error opening file %s\n", filename);
		return -1;
	}

	// Receive file volume
	int file_size;
	if (recv(socket_desc, &file_size, sizeof(file_size), 0) == -1)
	{
		fprintf(stderr, "receive_file: error receiving file size of %s\n", filename);
		fclose(fp);
		return 1;
	}
    file_size = ntohl(file_size);

#ifdef DEBUG
	fprintf(stdout, "DEBUG receive_file: file size of %d\n", file_size);
#endif

	// Use variables to keep track of file completion status
	char buffer[BUFFER_SIZE];
	int total_bytes_received = 0;
    double column_volume = data_per_column(file_size);
    double previous_progress = 0;

    // Newline to start progress bar
    fprintf(stdout, "\n");

    // While there is unreceived file volume
    while (total_bytes_received < file_size)
	{
        // Attempt to buffer file
		int bytes_received = recv(socket_desc, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0)
		{
			fprintf(stderr, "\nreceive_file: error occurred receiving data\n");
			fclose(fp);
			return 1;
		}

        // If the stream is interrupted
		if (bytes_received == 0)
		{
			fprintf(stderr, "\nreceive_file: sender disconnected mid-stream\n");
			fclose(fp);
			return 1;
		}

		// Write the received data to file
		fwrite(buffer, sizeof(char), bytes_received, fp);

        // Handle progress bar logic
		total_bytes_received += bytes_received;
        previous_progress += (double)bytes_received;

        print_progress_bar(&previous_progress, column_volume);
	}

    // Print last section of bar
    fprintf(stdout, "\n");

#ifdef DEBUG
	fprintf(stdout, "DEBUG: client.send_file: file %s successfully sent to socket %d\n", filename, socket_desc);
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
// returns 1 on success, 0 on failure
int send_msg(char *msg, int socket_desc)
{
	if (send(socket_desc, msg, BUFFER_SIZE, 0) < 0)
		return 0;
	else
		return 1;
}

// Function:	receive_msg
// ------------------------
// Receives a simple message over TCP and returns a dynamically allocated string
//
// msg: string containing message
// socket_desc: file descriptor for origin socket
//
// returns msg
char* receive_msg(int socket_desc)
{
	char *msg = (char *)malloc(BUFFER_SIZE);
	
	if (recv(socket_desc, msg, BUFFER_SIZE, 0) < 0)
        free(msg);
	else
        return msg;

    return NULL;
}

// Function:    server_init
// ------------------------
// Initializes a TCP server
//
// Returns fd associated with socket
int server_init()
{
    struct sockaddr_in server_addr;

    // Create socket:
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    server_addr.sin_addr.s_addr = inet_addr(DEFAULT_ADDRESS);

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
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

    // Return outbound socket fd
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
    server_addr.sin_port = htons(DEFAULT_PORT);
    server_addr.sin_addr.s_addr = inet_addr(DEFAULT_ADDRESS);

    // Send connection request to server:
    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        fprintf(stderr, "client: unable to connect to server\n");
        close(socket_desc);
        return -1;
    }

    // Return server's socket fd
    return socket_desc;
}