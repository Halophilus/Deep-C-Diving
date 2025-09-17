/*
 * cipher.c / Random Number Generation and Encryption
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 1/29/2025
 *
 * Implementing polybius.h in cipher.c
 */
#include "polybius.h"

int main(int argc, char* argv [])
{
	struct table *new_table = generate_table(5);

	// Verify correct argument count
	if (argc != 3)
	{
		printf("Syntax error! ./cipher [-e, -d] [STRING]\n");
		return (1);
	}	

	if ( strcmp( argv[1], "-e" ) == 0 )
	{
		printf("%s\n", pbEncode(argv[2], new_table)); 
	}

	if ( strcmp( argv[1], "-d" ) == 0 )
	{
		printf("%s\n", pbDecode(argv[2], new_table)); 
	}
}

