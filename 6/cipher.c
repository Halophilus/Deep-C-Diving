/*
 * cipher.c / Multiprocessing in C
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 2/23/2025
 *
 * Refactoring cipher.c to take in any string from stdin and return an
 * an encoded string to stdout
 */
#include "polybius.h"

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		// Use fprintf to avoid sending error message to stdout
		fprintf(stderr, "Usage: an array of strings", argv[0]);
		exit(1);
	}
	
	// Generate a Caesar cipher table with offset 5
	struct table *new_table = generate_table(5);
	char buffer[MAX_STRING];

	// Iterate through the provided list of strings, encode, print to stdout
	for( int i = 1; i < argc; i++)
	{
		char *string = argv[i];
		char *encrypted_string = pbEncode(string, new_table);

		printf("%s\n", encrypted_string);
		free(encrypted_string);
	}
	return 0;
}

