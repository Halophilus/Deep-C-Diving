#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_LINE 512

// Struct:	dyn_block
// --------------------------
// Stores an array of integers of arbitrary size
//
// int_array: an array of integers
// size: the size of the array
typedef struct dyn_block
{
	int *int_array;
	int size;
} dyn_block_t;

// Function:	print_array
// -------------------------
// Prints an array of integers
//
// array: array of integers
// size: size of array
void print_array(int *array, int size)
{
	for (int i = 0; i < size; i++)
		printf("%d ", array[i]);
	printf("\n");
}

// Function:	print_dyn_block
// -----------------------------
// Prints the contents of a dyn_block to the console
//
// dyn_block: pointer to any dyn_block_t  instance
void print_dyn_block(dyn_block_t* dyn_block)
{
	print_array(dyn_block->int_array, dyn_block->size);
	printf("\n");
}

// Function:	alloc_dyn_block
// -----------------------------
// Allocates memory on the heap for a dyn_block instance
//
// returns: pointer to allocated memory
dyn_block_t *alloc_dyn_block(int size)
{
	// Declare variables
	dyn_block_t *dyn_block_ptr;
	
	// Allocate memory to the heap
	dyn_block_ptr = (dyn_block_t*)malloc(sizeof(dyn_block_t));
	dyn_block_ptr->int_array = (int *)calloc(size, size * sizeof(int));

	// Determine memory allocation 
	if (dyn_block_ptr == NULL)
	{
		printf("Memory allocation failed in loadmem.alloc_dyn_block\n");
		return NULL;
	}
	
	// Assign struct fields
	dyn_block_ptr->size = size;

	// Return pointer to allocated memory
	return dyn_block_ptr;
}

// Function:	store_mem_blk
// --------------------------
// Stores an array of integers in a previously allocated block
// overwriting old values and replacing the remainder with 0
//
// dyn_block: pointer to previously allocated dyn_block_t
// array: an input array of integers
// array_size: number of elements in the input array
void store_mem_blk(dyn_block_t *dyn_block, int *array)
{
	for (int i = 0; i < dyn_block->size; i++)
	{
		dyn_block->int_array[i] = array[i];
	}
}

// Function:	count_lines
// -------------------------
// Counts the number of lines in a file
//
// fp: file pointer
//
// returns: number of lines in a file
int count_lines(FILE* fp)
{
	int count = 0;
	char c;
	// Iterate through the file character by character
	for (c = getc(fp); c != EOF; c = getc(fp))
        	if (c == '\n') // Increment count if newline
            		count++;
	return count;
}

// Function:	count_ints
// ------------------------
// Counts the number of integers in a string of integers and whitespace
//
// string: any string containing integers and whitespace
//
// returns: number of integers in string
int count_ints(char *string)
{
	// strtok modifies the original string, so a copy needs to be made
	char *string_copy = (char*)malloc(MAX_LINE);	
	strcpy(string_copy, string);

	int count = 0;

	char *token = strtok(string_copy, " ");
	while (token != NULL)
       	{
		count++;
		token = strtok(NULL, " ");
	}
	// Tokenize the string one at a time
	printf("%d\n", count);
	return count;	
}

// Function:	string_to_array
// -----------------------------
// Converts a string of integers between whitespace into an array of integers
//
// string: any string
//
// returns: pointer to array of integers
int *string_to_array(char *string)
{
	char *string_copy = (char *)malloc(MAX_LINE);
	int count = count_ints(string);
	int *int_array = (int *)malloc(count * sizeof(int));
	int *value;

	strcpy(string_copy, string);
	char *token = strtok(string_copy, " ");
	int i = 0;

	while (token != NULL)
	{
		sscanf(token, "%d", value);
		int_array[i] = *value;
		token = strtok(NULL, " ");
		i++;
	}

	return int_array;
}

// Function:	string_to_dyn_block
// --------------------------------
// Converts a string into a dyn_block_t instance and returns a pointer
//
// string: any string
//
// returns: dyn_block_t pointer
dyn_block_t *string_to_dyn_block(char *string)
{

	int new_size = count_ints(string);
	
	printf("new_size: %d\n", new_size);
	int *new_int_array = string_to_array(string);
	


	dyn_block_t *new_dyn_block = alloc_dyn_block(new_size);
	store_mem_blk(new_dyn_block, new_int_array);
	
	return new_dyn_block;
}
// Function:	parse_to_dyn_block
// -------------------------------
// Parses a file containing integer values into a corresponding dyn_block
//
// filename: the name of the file
// dyn_block: dyn_block_t instance
//
// returns: an array of dyn_block_t pointers
dyn_block_t** parse_to_dyn_block(char *filename)
{
	FILE *fp = fopen(filename, "r");
	char line_buffer[MAX_LINE];
	int lines_count = count_lines(fp);
	int i = 0;

	dyn_block_t** block_list = (dyn_block_t**)malloc(lines_count * sizeof (dyn_block_t *));
	dyn_block_t* current_block = (dyn_block_t*)malloc(sizeof (dyn_block_t));

	while(fgets(line_buffer, MAX_LINE, fp) != NULL)
	{
		
		printf("%s\n",line_buffer);
		//current_block = string_to_dyn_block(line_buffer);
		//current_block = string_to_dyn_block(line_buffer);
		//block_list[i++] = current_block;
	}
	
	fclose(fp);
	return block_list;
}

int main(void)
{
	char *test_string = "1 2 3 4 5 6 7 8";
	int count = count_ints(test_string);
	int *int_array = string_to_array(test_string);

	print_array(int_array, count);
	
	dyn_block_t *new_dyn_block = string_to_dyn_block(test_string);
	print_dyn_block(new_dyn_block);

	
	parse_to_dyn_block("blocks.data");
}
