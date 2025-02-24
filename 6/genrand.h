/*
 * genRand.c / Random Number Generation and Encryption (Part 1)
 *
 * Ben Henshaw / C5600 / Northeastern University
 * Spring 2025 / January 27, 2025
 * 
 * This program implements a version of xorshift
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#define MAX_FILENAME_LENGTH 32

/*
 * Struct:	xorwow_state
 * -------------------------
 *  Xorwow state used to generate random numbers
 *
 *  x: an array of seed values that are rotated per call
 *  counter: value used to directly generate random numbers
 */ 
struct xorwow_state
{
	uint32_t seeds[5];
	uint32_t counter;
};

/*
 * Variable:	global_state
 * -------------------------
 * Defines the xorwow state for the current session
 */
extern struct xorwow_state global_state;

/*
 * Function: xorwow
 * -----------------------
 *  Generates a pseudorandom value using the algorithm outlined in G. Marsaglia
 *  Xorshift RNGs paper
 *
 *  state: pointer to xorwow state
 *
 *  Returns: pseudorandom uint32_t value
 */
uint32_t xorwow(struct xorwow_state *state);

/*
 * Function:	generate_seeds
 * --------------------------------
 *  Generates an array of 5 seed values based on time data for use in
 *  generating a xorwow state
 *
 *  returns: 5 member array of uint32_t values
 */
uint32_t* generate_seeds(void);

/*
 * Function:	new_xorwow_state
 * -----------------------------
 * Generates a new xorwow_state
 *
 * returns: struct xorwow_state
 */
struct xorwow_state* new_xorwow_state(void);

/*
 * Function:	generate_value
 * ---------------------------
 *  Generates a value within a provided range
 *
 *  max_value: any integer value up to 362457
 *  current_state: pointer to a xorwow_state
 *
 *  returns: pseudorandom uint32 within the range set by max_value
 */
uint32_t generate_value(int max_value, struct xorwow_state *current_state);

/*
 * Function:	genRand
 * --------------------------
 *  Generates a random integer between two bounds
 *
 *  lower: the lower bound of generation
 *  upper: the upper bound of generation
 *
 *  returns: int
 */
int genRand(int lower, int upper);

/*
 * Function: syntax_error
 * --------------------------
 *  Exits the program and prints an error message upon syntax error
 */
void syntax_error(void);

/*
 * Function:	write_rand_to_file
 * -------------------------------
 * Writes a select amount of random integers to file.
 *
 * filename: string of filename to write 
 * mode: string of file mode
 * 	w: (default) new file or overwrite of existing file
 * 	a: append to existing file
 * num_val: integer value of quantity of numbers to be generated
 */
void write_rand_to_file(char *filename, char *mode, int num_val);

/*
 * BEGINNING OF MULTIPROCESSING IN C ASSIGNMENT
 */

/*
 * Function: gen_char
 * ------------------
 * Generates a random character between 65 and 91, capital alphabetic characters and [
 * [ indicates the end of a string
 *
 * returns: a random char
 */
char gen_char(void);

/*
 * Function: gen_word
 * ------------------
 * Generates a random string based on characters produced by gen_char until
 * a space is reached, then returns a pointer to the string
 *
 * returns: a random string
 */
char *gen_word(void);

/*
 * Function: append_strings
 * ------------------------
 *  Appends a number of strings to a provided filename
 */
void append_strings(char* filename, int num_strings);

