/*
 * polybius.h / Random Number Generation and Encryption
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 1/29/2025
 *
 * Header for Polybius cipher as basic encryption
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
struct map;

/* Struct:	table
 * --------------------------
 *  An array of character maps corresponding to a letter cipher
 */
struct table;

/* Function:	find_value
 * --------------------------
 *  Find the value corresponding to a given key in a map using linear search
 *
 *  target_value: value to be located
 *  current_table: table containing coded alphabet
 *
 *  returns: value character 
 */
char find_value(char target_value, struct table *current_table);

/* Function:	find_key
 * --------------------------
 *  Find the value corresponding to a given key in a map using linear search
 *
 *  target_key: key to be located
 *  current_table: table containing coded alphabet
 *
 *  returns: value character 
 */
char find_key(char target_key, struct table *current_table);

/*Function:	generate_table
 * --------------------------
 *  Generates a table based on a numerical offset that wraps around the
 *  set of uppercase letters in ASCII to produce a Caesar cipher
 *
 *  offset: integer value for displacement of ASCII values
 *
 *  returns: pointer to newly generated table
 */
struct table* generate_table(int offset);

/*Function:	print_table
 * --------------------------
 *  Prints the key to value pairs of the cipher associated with a table
 *
 *  current_table: any table
 */
void print_table(struct table *current_table);

/*Function:	filter_chars
 * --------------------------
 *  Takes a character, and converts it to upper case if lowercase, or to an
 *  empty char if another ASCII character
 *
 *  current_char: character to be converted
 *
 *  returns: modified char
 */
char filter_char(char current_char);

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
char decode_char(int row, int col);

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
char* encode_char(char character);

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
char* pbEncode(const char *plaintext, struct table *custom_table);

/* Function:	pbDecode
 * --------------------------
 *  Decodes a string that has been encoded using a table and the Polybius
 *  cipher
 *
 *  plaintext: encoded string
 *  used_table: the table originally used to map the characters
 *
 *  returns: the decoded string
 */
char* pbDecode(const char *plaintext, struct table *used_table);

