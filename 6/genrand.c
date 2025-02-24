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

struct xorwow_state global_state;
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
uint32_t xorwow(struct xorwow_state *state)
{
	// Pull the final seed from the seed array
	uint32_t initial_first_seed = state->seeds[4];

	// Pull the first seed from the seed array
	uint32_t initial_final_seed = state->seeds[0];

	// Arbitrarily rotate the seeds 
	state->seeds[4] = state->seeds[3]; 
	state->seeds[3] = state->seeds[2];
	state->seeds[2] = state->seeds[1];
	state->seeds[1] = initial_final_seed;

	// Bitshift XOR operations to simulate randomness
	 initial_first_seed ^= initial_first_seed >> 2; // Perform XOR between initial_first_seed and bitshifted t
	 initial_first_seed ^= initial_first_seed << 1; 
	 initial_first_seed ^= initial_final_seed ^ (initial_final_seed << 4); // XOR between initial_first_seed and (s and bitshifted s)
	
	// Pass the mutated value back into the array of seeds
	state->seeds[0] = initial_first_seed;

	// Iterate the RNG counter
	state->counter += 362437;

	// Return a PRNG value
	return initial_first_seed + state->counter;	
}


/*
 * Function:	generate_seeds
 * --------------------------------
 *  Generates an array of 5 seed values based on time data for use in
 *  generating a xorwow state
 *
 *  returns: 5 member array of uint32_t values
 */
uint32_t* generate_seeds(void)
{
	time_t seconds; // Seconds as a 64-bit integer
	struct tm *local_time; // Struct containing time data
	uint32_t *seeds; // Seeds for xorwow state
	int offset; // Determines the order in which the seeds are stored

	// Allocate heap memory for storing seed values
	seeds = (uint32_t*)malloc(5 * sizeof(uint32_t));
	
	if (seeds == NULL)
	{
		printf("Error allocating memory for genRand.generate_seeds()\n");
		exit(1);
	}

	// Pull random values from the environment
	time(&seconds);
	local_time = localtime(&seconds);

	// Extract hour, minute, second data from tm
	int hour = local_time->tm_hour;
	int minute = local_time->tm_min;
	int second = local_time->tm_sec;

	// Generate seeds using time data and method for generating Marsenne primes
	uint32_t seed1 = seconds; // Truncates to uint32
	uint32_t seed2 = (uint32_t)(pow(2, hour) - 1);
	uint32_t seed3 = (uint32_t)pow(2, minute) - 1;
	uint32_t seed4 = (uint32_t)pow(2, second) -1;
	
	// Seed that is a random permutation of the above data 
	int pseudorandom_permutation = seconds % (minute + second + hour);
	if (pseudorandom_permutation > 32)
		pseudorandom_permutation = pseudorandom_permutation >> 1;
	uint32_t seed5 = pow(2, pseudorandom_permutation) - 1;
	
	// Bitshift seed values to maximize size of seeds
	seed2 = seed2 << (int)(31 - floor(log2(seed2) - 1) - 1);
	seed3 = seed3 << (int)(31 - floor(log2(seed3) - 1) - 1);
	seed4 = seed4 << (int)(31 - floor(log2(seed4) - 1) - 1);
	seed5 = seed5 << (int)(31 - floor(log2(seed5) - 1) - 1);

	// Create an offset to determine seed order
	offset = seconds % 5;
	// Insert them into an array 
	seeds[offset] = seed1;
	seeds[++offset%5] = seed2;
	seeds[++offset%5] = seed3;
	seeds[++offset%5] = seed4;
	seeds[++offset%5] = seed5;
	
	return seeds;
}

/*
 * Function:	new_xorwow_state
 * -----------------------------
 * Generates a new xorwow_state
 *
 * returns: struct xorwow_state
 */
struct xorwow_state* new_xorwow_state(void)
{
	struct xorwow_state *new_state;

	new_state = (struct xorwow_state*) malloc(sizeof(struct xorwow_state));
	
	uint32_t *new_seeds = generate_seeds();

	// Add seeds incrementally
	for(int i = 0; i < 5; i++)
		new_state->seeds[i] = *(new_seeds + i);

	new_state->counter = 0;

	return new_state;
}

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
uint32_t generate_value(int max_value, struct xorwow_state *current_state)
{
	return xorwow(current_state) % (max_value + 1); 
}

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
int genRand(int lower, int upper)
{
	return lower + (int)(generate_value(upper - lower, &global_state)); 
}

/*
 * Function: syntax_error
 * --------------------------
 *  Exits the program and prints an error message upon syntax error
 */
void syntax_error(void)
{
	printf("Syntax error! genRand [NUMBER] [FILENAME] [-a]");
}

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
void write_rand_to_file(char *filename, char *mode, int num_val)
{
	FILE *fp;

	fp = fopen(filename, mode);

	if (fp == NULL) 
	{
		printf("Failure opening file in genRand.write_rand_to_file");
		exit(1);
	}

	for ( int i = 0; i < num_val; i++ )
	{
		fputc( 48 + genRand(0,9), fp ); 
	}

	if ( fclose(fp) == EOF )
	{
		printf("Failure closing file in genRand.write_rand_to_file");
		exit(1);
	}
}

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
char gen_char(void)
{
	return (char) genRand(65, 90);
}

/*
 * Function: gen_word
 * ------------------
 * Generates a random string based on characters produced by gen_char until
 * a space is reached, then returns a pointer to the string
 *
 * returns: a random string
 */
char *gen_word(void)
{
	char* buffer = (char*) malloc( sizeof(char) * 512 ); // Allocate space for return string
	int i = 0;
	char current_char = '[';

	while (current_char == '[') // Ensure first character isn't a space
		current_char = gen_char();
	
	while (current_char != '[' && i < 511) 
	{
		buffer[i] = current_char;
		i++;
		current_char = gen_char();	
	}

	buffer[i] = '\0';

	return buffer;
}

/*
 * Function: append_strings
 * ------------------------
 *  Appends a number of strings to a provided filename
 */
void append_strings(char* filename, int num_strings)
{
	FILE *fp;
	char *buffer;

	fp = fopen(filename, "a");

	if (fp == NULL) 
	{
		printf("Failure opening file in genRand.append_strings");
		exit(0);
	}

	for (int i = 0; i < num_strings; i++)
	{
		buffer = gen_word();
		fprintf(fp, "%s\n", buffer);
		free(buffer);
	}

	if ( fclose(fp) == EOF )
	{
		printf("Failure closing file in genRand.append_strings");
		exit(1);
	}
}
