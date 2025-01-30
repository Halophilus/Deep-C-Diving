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
	uint32_t x[5];
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
	uint32_t t = state->x[4];

	// Pull the first seed from the seed array
	uint32_t s = state->x[0];

	// Arbitrarily rotate the seeds 
	state->x[4] = state->x[3]; 
	state->x[3] = state->x[2];
	state->x[2] = state->x[1];
	state->x[1] = s;

	// Bitshift XOR operations to simulate randomness
	t ^= t >> 2; // Perform XOR between t and bitshifted t
	t ^= t << 1; 
	t ^= s ^ (s << 4); // XOR between t and (s and bitshifted s)
	
	// Pass the mutated value back into the array of seeds
	state->x[0] = t;

	// Iterate the RNG counter
	state->counter += 362437;

	// Return a PRNG value
	return t + state->counter;	
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
		new_state->x[i] = *(new_seeds + i);

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

int main(int argc, char* argv [])
{
	global_state = *new_xorwow_state();
	int num_values;
	char *file_mode;
	char filename[MAX_FILENAME_LENGTH];

	if ( argc > 4 )
	{
		syntax_error();
		return(1);
	}

	// If no arguments are passed, print one integer between 0 and UINT32_MAX
	if ( argc == 1 )
	{
		printf("%d\n", genRand( 0, 100 ));
		return (0);
	}
	
	// Set the number of values to be generated
	num_values = atoi(argv[1]);

	// If only one number is provided, print that number of random integers to console
	if ( argc == 2 ) 
	{
		for(int i = 0; i < num_values; i++)
		{
			printf("%d", genRand(0,9));
		}		
		printf("\n");
		return (0);
	}

	// Set file mode to write and pull filename
	file_mode = "w";
	strcpy(filename, argv[2]);
	filename[MAX_FILENAME_LENGTH - 1] = '\0'; // Add null terminator in case of buffer overflow

	// Check to see if 4 arguments have been passed
	if ( argc == 4 )
	{
		// Unless the optional '-a' argument is invoked
		if ( strcmp( argv[3] , "-a" ) == 0 )
		{
			file_mode = "a"; // Then set it to append values
		}
	       	else 
		{
			syntax_error(); // Throw an error if an invalid argument is passed
			return (1);
		}
	}
	
	// Write num_values of random integers to file
	write_rand_to_file(filename, file_mode, num_values);	
	return (0);
}
