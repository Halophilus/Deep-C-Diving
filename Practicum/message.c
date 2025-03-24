/*
 * message.c / Practicum 1
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 3/23/2025
 *
 * Implementation of data structure to store a message
 */
#include "message.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROW_LENGTH sizeof(Message) + 256 // Include space for commas in CSV and time_t string


// Function:	create_msg
// -----------------------
// Dynamically allocates a Message struct
//
// sender:	String of sender ID
// receiver:	String of receiver ID
// content:	String of message content
//
// returns: Message object from heap memory
Message* create_msg(const char* sender, const char* receiver, const char* content)
{
    // Allocate memory
	Message* msg = ( Message* ) malloc (sizeof ( Message ));
	if (msg == NULL)
	{
		fprintf(stderr, "Message.create_msg: Error allocating memory for new message\n");
		return NULL;
	}

	// System values
    // Generate ID and validate
    int id = get_id();
    if (id < 0)
    {
        fprintf(stderr, "Message.create_msg: can't obtain valid ID.\n");
        free(msg);
        return NULL;
    }
	msg->id = id;

    // Generate timestamp and validate
    time_t timestamp = time(NULL);
    if (!timestamp)
    {
        fprintf(stderr, "Message.create_msg: failed to obtain time_t\n");
        free(msg);
        return NULL;
    }
	msg->timestamp = time(NULL);

	// Copy user-defined values
	strncpy( msg->sender, sender, sizeof(msg->sender) - 1 );
	strncpy( msg->receiver, receiver, sizeof(msg->receiver) - 1 );
	strncpy( msg->content, content, sizeof(msg->content) - 1 );

	// Cap string values in case of overflow
	msg->sender[ sizeof(msg->sender) - 1 ] = '\0';
	msg->receiver[ sizeof(msg->receiver) - 1 ] = '\0';
	msg->content[ sizeof(msg->content) - 1 ] = '\0';

	// Default flag
	msg->delivered = 0;

    // Storage behavior
    int stored = store_msg(msg);
    if (stored < 0)
    {
        fprintf(stderr, "Message.create_msg: Error storing message %d to disk\n", msg->id);
        free(msg);
        return NULL;
    }

	// Cache behavior
	msg->next = NULL;
    int pushed = cache_push(msg);
    if (pushed < 0)
    {
        fprintf(stderr, "Message.create_msg: Message %d push to cache failed.\n", msg->id);
        free(msg);
        return NULL;
    }

	return msg;
}

// Function:	month_to_number
// ----------------------------
// Helper function to convert month name to month number
//
// month:	string subset of months
//
// returns: 	integer month value
int month_to_number(const char* month) {
    // Array of possible month strings
    const char* months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    // Find index of string
    for (int i = 0; i < 12; i++) {
        if (strncmp(month, months[i], 3) == 0) {
            return i;
        }
    }

    fprintf(stderr, "Message.month_to_number: Malformed month entry in time string: %s", month);
    return -1; // Invalid month
}

// Function:	c_strptime
// ---------------------
// Helper function with custom implementation of strptime from time.h
// 
// date_str:	String matching format
// format:	formatting (default "%a %b %d %H:%M:%S %Y")
// tm_out:	struct tm object for storing new time
//
// returns:	0 on success, -1 on failure 
int c_strptime(const char* date_str, const char* format, struct tm* tm_out) {
    if (date_str == NULL || format == NULL || tm_out == NULL) {
        return -1; // Invalid input
    }

    char day_name[4], month_name[4], day_str[3], hour_str[3], minute_str[3], second_str[3], year_str[5];
    int day, hour, minute, second, year, month;

    // Format is: "%a %b %d %H:%M:%S %Y"
    if (sscanf(date_str, "%3s %3s %2s %2s:%2s:%2s %4s", day_name, month_name, day_str, hour_str, minute_str, second_str, year_str) != 7) {
        fprintf(stderr, "Message.c_strptime: malformed timestamp string: %s", date_str);
        return -1; // Error in parsing
    }

    // Parse day of the month
    day = atoi(day_str);

    // Convert month name to month number
    month = month_to_number(month_name);
    if (month == -1) {
        fprintf(stderr, "Message.c_strptime: Error converting month substring to index: %s", month_name);
        return -1; // Invalid month
    }

    // Parse hour, minute, second
    hour = atoi(hour_str);
    minute = atoi(minute_str);
    second = atoi(second_str);

    // Parse year
    year = atoi(year_str);

    // Populate the tm structure
    tm_out->tm_mday = day;
    tm_out->tm_mon = month;   // tm_mon is 0-based, so no need to adjust
    tm_out->tm_hour = hour;
    tm_out->tm_min = minute;
    tm_out->tm_sec = second;
    tm_out->tm_year = year - 1900; // tm_year is years since 1900

    return 0;
}


// Function:	parse_row
// --------------------------
// Converts a row in a CSV into a message object and returns a Message pointer
//
// row:		String representing row in CSV
//
// returns:	Message* object pointer
Message* parse_row(char* row)
{
    // Null string
	if (row == NULL)
	{
		fprintf(stderr, "Message.parse_row: Null argument exception\n");
		return NULL;
	}

    Message *new_msg;
    new_msg = (Message *) malloc(sizeof(Message));

    // Allocation error
	if (new_msg == NULL)
	{
		fprintf(stderr, "Message.parse_row: Error allocating memory for new Message object: %s\n", row);
		return NULL;
	}

    // 6 defined attributes not related to cache
	char* fields[6];
	char* token;
	int field_count = 0;

	// Create a local version of the row to tokenize
	char row_buffer[strlen(row) + 1];
	strcpy(row_buffer, row);

	// Tokenize the string
	token = strtok(row_buffer, ",");
	while (token != NULL && field_count < 6)
	{
		fields[field_count] = token;
		field_count++;
		token = strtok(NULL, ",");
	}

    // Convert id string to int
	new_msg->id = atoi(fields[0]);

	// Parse and store the timestamp
	struct tm tm;
	if (c_strptime(fields[1], "%a %b %d %H:%M:%S %Y", &tm) == -1)
	{
		fprintf(stderr, "Message.parse_row: Error parsing string into tm object: %s\n", row);
		free(new_msg);
		return NULL;
	}

    // Convert tm to time_t
	time_t time_value = mktime(&tm);
	if (time_value == -1)
	{
		fprintf(stderr, "Message.parse_row: Failed to convert tm to time_t: %s\n", row);
		free(new_msg);
		return NULL;
	}

    // Assign time_t
	new_msg->timestamp = time_value;

	// Copy string fields	
	strncpy( new_msg->sender, fields[2], sizeof(new_msg->sender) - 1 );
	strncpy( new_msg->receiver, fields[3], sizeof(new_msg->receiver) - 1 );
	strncpy( new_msg->content, fields[4], sizeof(new_msg->content) - 1 );

	// Cap string values in case of overflow
	new_msg->sender[ sizeof(new_msg->sender) - 1 ] = '\0';
	new_msg->receiver[ sizeof(new_msg->receiver) - 1 ] = '\0';
	new_msg->content[ sizeof(new_msg->content) - 1 ] = '\0';

	// Received flag
	new_msg->delivered = atoi(fields[5]);
	
	return new_msg;
}

// Function:	store_message
// --------------------------
// Stores copy of generated message object in a local text file
//
// msg:		Message object pointer
//
// returns: 0 on success, -1 on failure
int store_msg(const Message* msg)
{
    // Null message
	if (msg == NULL)
	{
		fprintf(stderr, "Message.store_msg: Null pointer argument exception\n");
		return -1;
	}

	FILE* file;
	file = fopen("messages.csv", "a");

    // File opening error
	if (file == NULL)
	{
		fprintf(stderr, "Message.store_msg: Error opening messages.csv for message %d\n", msg->id);
		return -1;
	}

    // Generate a new entry
	char* csv_entry = generate_msg_string(msg);

    // Row generation failure
	if (csv_entry == NULL)
	{
		fprintf(stderr, "Message.store_msg: Failure to generate CSV entry for message %d\n", msg->id);
		fclose(file);
        return -1;
	}

    // Write failure
	if (fputs(csv_entry, file) == EOF )
	{
		fprintf(stderr, "Message.store_msg: Failure to write CSV entry to file for message %d\n", msg->id);
		fclose(file);
		return -1;
	}

    fflush(file); // Ensure
    // Close resources
	fclose(file);
	return 0;
}

// Function:	generate_msg_string
// --------------------------------
// Converts a message object into a string to be stored in csv
//
// msg:		Message* object pointer
//
// returns:	String reflecting CSV entry
char* generate_msg_string(const Message* msg)
{
	if (msg == NULL)
	{
		fprintf(stderr, "Message.generate_msg_string: Null Message pointer argument\n");
		return NULL;
	}
	
	// Generate strings of each field
	char* time_string = ctime( &(msg->timestamp) );
	time_string[strlen(time_string) - 1] = '\0'; // Replace newline

    // ID
	char id_string[32];
	sprintf(id_string, "%d", msg->id);

    // Delivered
	char delivered_string[2];
	sprintf(delivered_string, "%d", msg->delivered);

    // Sender
	char sender[ sizeof(msg->sender) ];
	strcpy(sender, msg->sender);

    // Receiver
	char receiver[ sizeof(msg->receiver) ];
	strcpy(receiver, msg->receiver);

    // Content
	char content[ sizeof(msg->content) ];
	strcpy(content, msg->content);

	// Allocate memory for new string and concatenate
	char* csv_string = ( char* )malloc( MAX_ROW_LENGTH * sizeof(char) + 1 );
	if (csv_string == NULL)
    {
        fprintf(stderr, "Message.generate_msg_string: Memory allocation error for new string\n");
        return NULL;
    }

    sprintf(csv_string, "%s,%s,%s,%s,%s,%s\n",
			id_string, time_string, sender, receiver, content, delivered_string);

	return csv_string;
}

// Function:	retrieve_msg
// -----------------------------
// Retrieves a message object from messages.csv
//
// id:		integer ID associated with the message
//
// returns:	Message* object pointer to associated message
Message* retrieve_msg(int id)
{
    // Search cache first
    Message* new_msg = cache_fetch(id);
    if (new_msg) return new_msg;

    // Open file
	FILE* file;
	file = fopen("messages.csv", "r");

    // Opening error
	if (file == NULL)
	{
		fprintf(stderr, "Message.retrieve_msg: Error opening messages.csv for message %d\n", id);
		return NULL;
	}
	
	char result[MAX_ROW_LENGTH];	
	int current_line = 0;
	
	// Scan from line to line (ID = Line number)
	while (fgets(result, MAX_ROW_LENGTH, file))
	{
		if (current_line == id) break;
		current_line++;
	}

	// Close resources
	fclose(file);

	// ID out of bounds or other indexing error
	if (result[0] == '\0')
	{
		fprintf(stderr, "Message.retrieve_msg: ID not found in messages.csv for message %d\n", id);
		return NULL;
	}

    // Generate a new message from the row
	new_msg = parse_row(result);
	if (new_msg == NULL)
	{
		fprintf(stderr, "Message.retrieve_msg: Error parsing row data in for message %d\n", id);
		return NULL;	
	}

    // Push to cache
    int pushed = cache_push(new_msg);
    if (pushed < 0)
    {
        fprintf(stderr, "Message.retrieve_msg: Error pushing to cache for message %d\n", id);
        return NULL;
    }

	return new_msg;
}

// Function:	update_delivered
// -----------------------------
// Finds local record for a stored message and raises its delivered flag
//
// id:		Integer message ID, corresponds to line in file
//
// returns: 	0 if success, -1 if failed
int update_delivered(int id)
{
    // Open disk storage
	FILE* file;
	file = fopen("messages.csv", "r+");
	if (file == NULL)
	{
		fprintf(stderr, "Message.update_delivered: Error opening messages.csv in for message %d\n", id);
		return -1;
	}

	char line[MAX_ROW_LENGTH];
	long pos = 0; // Relative position of the flag within the file
	int current_id = 0; 

    // Scan through each line
	while (fgets(line, sizeof(line), file))
	{
        // Found match
		if (current_id == id)
		{
			// Locate last field in the entry
			char* last_comma = strrchr(line, ',');
			
			// CSV formatting error
			if (last_comma == NULL)
			{
				fprintf(stderr, "Message.update_delivered: ID %d not found in messages.csv\n", id);
				fclose(file);
				return -1;
			}

			// Do nothing if the flag is already raised
			if (*(last_comma + 1) != '0')
			{
				fprintf(stderr, "Message.update_delivered: ID %d already marked delivered\n", id);
				fclose(file);
				return -1;
			}

			// Calculate the absolute position in the file
			pos = ftell(file) - (strlen(line) - (last_comma - line + 1));

			// Seek to position
			fseek(file, pos, SEEK_SET);

            // Update char at location
            fputc('1', file);

			fclose(file);

            // Update message in cache
            Message* msg = cache_fetch(id);

            if (!msg)
            {
                msg = retrieve_msg(id);
                if (!msg)
                {
                    fprintf(stderr, "Message.update_delivered: message not found in messages.csv, cache update failed", id);
                    return -1;
                }
                msg->delivered = 1;
            }
			return 0;
		}

		current_id++;
	}

	fprintf(stderr, "Message.update_delivered: ID %d not found in messages.csv\n", id);
	fclose(file);
	return -1;
}

// Function:	get_id
// -------------------
// Helper function retrieves most recent message ID from local storage
//
// returns:	Integer value corresponding with most recent Message.id used
int get_id(void)
{
    // Open messages.csv
	FILE* file = fopen("messages.csv", "r");
	if (file == NULL)
	{
		fprintf(stderr, "Message.get_id: Error opening messages.csv\n");
		return -1;
	}

	char line[MAX_ROW_LENGTH];
	int id = 0;

    // Scan through messages until line number = id
	while (fgets (line, MAX_ROW_LENGTH, file))
	{
		id++;
	}

	return id;
}

// Function:	print_msg
// ----------------------
// Prints the contents of a message object 
//
// msg:		Message* object pointer
void print_msg(const Message* msg)
{
	if (msg == NULL)
	{
		fprintf(stderr, "Message.print_msg: Null pointer argument exception\n");
		return;
	}

	// Generate CSV string representation
	char* csv_string = generate_msg_string(msg);
	if (csv_string == NULL)
	{
		fprintf(stderr, "Message.print_msg: Failed to generate CSV string\n");
		return;
	}

	// Tokenize and print each field
	char* token;
	char csv_copy[MAX_ROW_LENGTH];
	strcpy(csv_copy, csv_string);

	const char* labels[] = {"ID", "Timestamp", "Sender", "Receiver", "Content", "Delivered" };
	int i = 0;

	token = strtok(csv_copy, ",");
	while (token != NULL && i < 6)
	{
		printf("%s: %s\n", labels[i], token);
		token = strtok(NULL, ",");
		i++;
	}

	free(csv_string);
}
