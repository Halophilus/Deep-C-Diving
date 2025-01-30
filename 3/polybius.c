/*
 * polybius.c / Random Number Generation and Encryption
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 1/29/2025
 *
 * Implementation of a Polybius cipher as basic encryption
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define MAX_STRING 1024

/* Struct:	map
 * --------------------------
 *  A key-value pair linking an input char (key) to an output char (value)
 *  	key -> value : encoding
 *  	value -> key : decoding
 */
struct map
{
	char key;
	char value;
};

/* Struct:	table
 * --------------------------
 *  An array of character maps corresponding to a letter cipher
 */
struct table
{
	struct map coded_alphabet[26];
};

/* Function:	find_value
 * --------------------------
 *  Find the 

/*Function:	generate_table
 * --------------------------
 *  Generates a table based on a numerical offset that wraps around the
 *  set of uppercase letters in ASCII to produce a Caesar cipher
 *
 *  offset: integer value for displacement of ASCII values
 *
 *  returns: pointer to newly generated table
 */
struct table* generate_table(int offset)
{
	struct table* new_table;

	new_table = (struct table*) malloc( sizeof( struct table ));

	for (int i = 0; i < 26; i++)
	{
		struct map new_map;
		new_map.key = i + 65;
		new_map.value = ((i + offset) % 26) + 65;
		new_table->coded_alphabet[i] = new_map;
	}

	return new_table;
}

/*Function:	print_table
 * --------------------------
 *  Prints the key to value pairs of the cipher associated with a table
 *
 *  current_table: any table
 */
void print_table(struct table *current_table)
{
	char current_key;
	char current_value;

	for(int i = 0; i < 26; i++)
	{
		current_key = (*(current_table->coded_alphabet + i)).key;
		current_value = (*(current_table->coded_alphabet + i)).value;
		printf("%c to %c\n", current_key, current_value);
	}
}

/*Function:	decode_char
 * --------------------------
 * Takes a 1-indexed row and column value and returns the corresponding char
 * Each row corresponds to a certain ASCII value, and each column is an offset
 *
 * row: row index of Polybius square
 * col: col index ibid.
 *
 * returns: an uppercase character, or a space if the value is undefined
 */
char decode_char(int row, int col)
{
	// Test if value falls within defined range of the table
	if (row > 5 || col > 5)
	{
		printf("Value pair out of bounds of defined square in polybius.decode_char\n");
		printf("Row: %d, Col: %d", row, col);
		return (32);
	}

	// Correct for zero indexing
	col--;

	// Filter by row then add col offset to ASCII map
	switch (row)
	{
		case 1: // A-E
			return ( 'A' + col );
		
		case 2: // F-K
			if ( col == 4 )
				return 'K'; // Special case to omit J from the square
			return ( 'F' + col );
		
		case 3: // L-P
			return( 'L' + col );

		case 4: // Q-U
			return ( 'Q' + col );

		case 5: // V - Z
			return ( 'V' + col );

		default: // Returns "Space"
			return ( 32 );
	}
}

/* Function:	encode_char
 * -------------------------
 *  Creates a string with the row/column pair corresponding to a provided char
 *
 *  character: any character A-B, upper or lower case
 *
 *  returns: a 3-member character array
 *  	If alpha, then these are two digits corresponding to row and column
 *  	If other, then it returns a single space
 */
char* encode_char(char character)
{
	
	char* coded; // String containing encoded char
	
	int offset; // Char offset from 65 on ASCII table

	// Polybius square coordinates (1-indexed)
	int row;
	int col;

	// Make the string available on the heap
	coded = ( char* )malloc( 3 * sizeof( char ) );

	// Null pointer check
	if (coded == NULL) 
	{
		printf("Failed to allocate memory for string in polybius.encode_char");
		exit(1);
	}

	// Filter input to only uppercase letters
	if (character >= 'a' && 'z' >= character) // If character is lowercase
	{
		character -= 32; // Convert it to upper case
	} else if (character >= 'A' && 'Z' >= character) // character is upper
	{
		// Do nothing, creates a condition to catch OoB chars
	} else
	{
		strcpy(coded, " ");
		return coded;// Return a space if not alpha
	}
	
	// Roll 'J' chars into 'I'
	if ( character == 'J' )
		character--;

	// Catch 'K' so that offset can be resumed at a clean value
	if ( character == 'K' )
	{
		strcpy(coded, "25");
		return coded;
	}

	// Find initial offset to determine row value
	offset = character - 'A';

	// Find row, adjusting for i/j discrepancy where Z would return 6
	row = offset / 5 + 1;
	
	// Adjust offset for values past 'K'
	if (character > 'K')
	{
		offset = character - 'L';
		row = (( offset / 5 ) + 1) + 2;// Offset rows by 2
	}

	// Use mod to determine column value
	col = (offset % 5) + 1;

	// Assign row and col to ptr, adjust for offset of numeric chars
	*coded = row + 48;
       	*(coded + 1) = col + 48;
	*(coded + 2) = '\0';

	return coded;	
}

/* Function:	pbEncode
 * --------------------------
 *  Encodes an input string based on a key-value map of characters and a
 *  Polybius cipher
 *
 *  plaintext: a string to be encoded
 *  custom_table: an array of maps of characters to their corresponding cipher
 *
 *  returns: the encoded string
 */
char* pbEncode(const char *plaintext, struct table *custom_table)
{
	char* encoded_string;
	int i

	encoded_string = (char*) malloc( MAX_STRING * sizeof( char ));

	while (*(plaintext + i
}

int main(int argc, char* argv)
{
	struct table new_table = *generate_table(3);
	print_table(&new_table);
	return (0);

}
