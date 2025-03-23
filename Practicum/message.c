/*
 * message.c / Practicum 1
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 3/23/2025
 *
 * Implementation of data structure to store a message
 */

#define MAX_ROW_LENGTH sizeof(Message) + 32 // Include space for commas in CSV and time_t string
#define MAX_FIELD_LENGTH 1028

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
	Message* msg = ( Message* ) malloc (sizeof ( Message ));
	if (msg == NULL)
	{
		fprintf(stderr, "Error allocating memory in message.create_msg\n");
		return NULL;
	}

	// System values
	msg->id = get_id();
	msg->timestamp = time(NULL);

	// Copy user-defined values
	strncpy( msg->sender, sender, sizeof(msg->sender) - 1 );
	strncpy( msg->receiver, receiver, sizeof(msg->receiver) - 1 );
	strncpy( msg->content, content, sizeof(msg->content) - 1 );

	// Cap string values
	msg->sender[ sizeof(msg->sender) - 1 ] = '\0';
	msg->receiver[ sizeof(msg->receiver) - 1 ] = '\0';
	msg->content[ sizeof(msg->content) - 1 ] = '\0';

	// Default flag
	msg->delivered = 0;

	return msg;
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
	if (row == NULL)
	{
		fprintf(stderr, "Null char pointer passed to message.parse_row: %s\n", row);
		return NULL;
	}
	
	Message* new_msg = (Message*)malloc( sizeof(Message) );

	if (new_msg == NULL)
	{
		fprintf(stderr, "Error allocating memory for new Message object in message.new_msg: %s\n", row);
		return NULL;
	}

	char* fields[6]; // 6 Attributes per message
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
	
	new_msg->id = atoi(fields[0]);

	// Parse and store the timestamp
	struct tm tm;

	if (strptime(fields[1], "%a %b %d %H:%M:%S %Y", &tm) == NULL)
	{
		fprintf(stderr, "Error parsing string into tm object in message.parse_row: %s\n", row);
		free(new_msg);
		return NULL;
	}
	
	time_t time_value = mktime(&tm);
	if (time_value == -1)
	{
		fprintf(stderr, "Failed to convert tm to time_t in message.parse_row: %s\n", row);
		free(new_msg);
		return NULL;
	}

	new_msg->timestamp = time_value;

	// Copy string fields	
	strncpy( new_msg->sender, fields[2], sizeof(new_msg->sender) - 1 );
	strncpy( new_msg->receiver, fields[3], sizeof(new_msg->receiver) - 1 );
	strncpy( new_msg->content, fields[4], sizeof(new_msg->content) - 1 );

	// Cap string values
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
	if (msg == NULL)
	{
		fprintf(stderr, "Null Message pointer passed to message.store_msg\n");
		return -1;
	}

	FILE* file;
	file = fopen("messages.csv", "a");

	if (file == NULL)
	{
		fprintf(stderr, "Error opening messages.csv in message.store_msg for message %d\n", msg->id);
		return -1;
	}
	
	char* csv_entry = generate_msg_string(msg);

	if (csv_entry == NULL)
	{
		fprintf(stderr, "Failure to generate CSV entry in message.store_msg for message %d\n", msg->id);
		return -1;
	}
	
	if (fputs(csv_entry, file) == EOF )
	{
		fprintf(stderr, "Failure to write CSV entry to file in message.store_msg for message %d\n", msg->id); 
		fclose(file);
		return -1;
	}
	
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
char* generate_msg_string(Message* msg)
{
	if (msg == NULL)
	{
		fprintf(stderr, "Null Message pointer passed to message.generate_msg_string");
		return NULL;
	}
	
	// Allocate stack space for new row in CSV
	char row[MAX_ROW_LENGTH];
	
	// Generate strings of each field
	char* time_string = ctime( &(msg->timestamp) );
	
	char id_string[32];
	sprintf(id_string, "%d", msg->id);

	char delivered_string[2];
	sprintf(delivered_string, "%d", msg->delivered);

	char sender[ sizeof(msg->sender) ];
	strcpy(sender, msg->sender);

	char receiver[ sizeof(msg->receiver) ];
	strcpy(receiver, msg->receiver);

	char content[ sizeof(msg->content) ];
	strcpy(content, msg->content);

	// Allocate memory for new string and concatenate
	char* csv_string = ( char* )malloc( MAX_ROW_LENGTH * sizeof(char) + 1 );
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
	FILE* file;
	file = fopen("messages.csv", "r");

	if (file == NULL)
	{
		fprintf(stderr, "Error opening messages.csv in message.retrieve_msg for message %d\n", id);
		return NULL;
	}
	
	char result[MAX_ROW_LENGTH];	
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int current_line = 1; // Skip header
	
	// Scan from line to line (ID = Line number)
	while ((read = getline(&line, &len, file)) != -1)
	{
		if (current_line == id)
		{
			strcpy(result, line);
			fclose(file);
			free(line);
		}
		current_line++;
	}

	// Close resources
	free(line);
	fclose(file);

	// ID out of bounds or other indexing error
	if (result == NULL)
	{
		fprintf(stderr, "ID not found in messages.csv by message.retrieve_msg for message %d\n", id);
		return NULL;
	}
		
	Message* new_msg = parse_row(result);
	
	// Error parsing row
	if (new_msg == NULL)
	{
		fprintf(stderr, "Error parsing row data in message.retrieve_msg for message %d\n", id);
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
	FILE* file;
	file = fopen("messages.csv", "r+");

	if (file == NULL)
	{
		fprintf(stderr, "Error opening messages.csv in update_delivered.message for message %d\n", id);
		return -1;
	}

	char line[MAX_ROW_LENGTH];
	long pos = 0; // Relative position of the flag within the file
	int current_id = 1; 

	while (fgets(line, sizeof(line), file))
	{
		if (current_id == id)
		{
			// Locate last field in the entry
			char* last_comma = strrchr(line, ',');
			
			// Error in CSV formatting	
			if (last_comma == NULL)
			{
				fprintf(stderr, "ID %d not found in messages.csv by update_delivered.message\n", id);
				fclose(file);
				return -1;
			}

			// Do nothing if the flag is already raised
			if (*(last_comma + 1) != '0')
			{
				fprintf(stderr, "ID %d already marked delivered, updated_delivered.message\n", id);
				fclose(file);
				return -1;
			}

			// Calculate the absolute position in the file
			pos = ftell(file) - (strlen(line) - (last_comma - line + 1));

			// Seek to position
			fseek(file, pos, SEEK_SET);
			fputc('1', file);

			fclose(file);
			return 0;
		}
		current_id++;
	}

	fprintf(stderr, "ID %d not found in messages.csv, updated_delivered.message\n", id);
	fclose(file);
}

// Function:	get_id
// -------------------
// Helper function retrieves most recent message ID from local storage
//
// returns:	Integer value corresponding with most recent Message.id used
int get_id(void)
{
	FILE* file = fopen("messages.csv", "r");

	if (file == NULL)
	{
		fprintf(stderr, "Error opening messages.csv in message.get_id\n");
		exit(1); // Fatal error, end program to avoid disrupting stored data
	}

	char line[MAX_ROW_LENGTH];
	int id = 0;
	while (fgets (line, MAX_LINE_LENGTH, file))
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
void print_msg(Message* msg)
{
	if (msg == NULL)
	{
		fprintf(stderr, "Null Message pointer passed to message.print_msg\n");
		return;
	}

	// Generate CSV string representation
	char* csv_string = generate_msg_string(msg);
	if (csv_string == NULL)
	{
		fprintf(stderr, "Failed to generate CSV string in message.print_msg\n");
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
