/*
 * arger.c / Program in C (Part 2)
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / January 13, 2025
 *
 */
#include <stdio.h>

/*
 * Function:	cap_first
 * -----------------------
 *  Prints a string with the first character capitalized (if alpha) and any
 *  remaining letters in lowercase to console
 *
 *  string: any string
 */
void cap_first(char* string)
{
	if (*string >= 97 && *string <= 122) /* If the first character is lowercase */
		putchar(*string - 32); /* Capitalize it */
       	else
		putchar(*string);

	for (int i = 1; string[i] != '\0'; i++){ /* For the remaining chars */
		if (string[i] >= 65 && string[i] <= 90) /* Lower if a capital letter */
			putchar (* (string + i) + 32);
		else
			putchar (* (string + i));		
	}
}

/*
 * Function:	lower_all
 * -----------------------
 *  Shifts all capital letters to lowercase and prints the result to console
 *
 *  string: any string
 */
void lower_all(char* string)
{
	for (int i = 0; string[i] != '\0'; i++) /* For the chars in this string */
		if (string[i] >= 65 && string[i] <= 90) /* If they are capital letters */
			putchar (* (string + i) + 32); /* Lower them */
		else
			putchar (* (string + i));
}

/*
 * Function:	cap_all
 * -----------------------
 * Shifts all lowercase letters to capitals and prints the result to console
 *
 * string: any string
 */
void cap_all(char* string)
{
	for (int i = 0; string[i] != '\0'; i++) /* For the chars in this string */
		if (string[i] >= 97 && string[i] <= 122) /* If they are lowercase*/
			putchar (* (string + i) - 32); /* Capitalize them */
		else 
			putchar (* (string + i));	
}

/*
 * Function:	parse_clarg
 * -----------------------
 *  Compares a string to a set of legal arguments and returns an integer that
 *  corresponds to that argument
 *
 *  clarg: any string; if legal associated with the following functions
 *  	u: cap_all
 *  	l: lower_all
 *  	cap: cap_first
 *  	The main function calls these methods on the remaining clargs
 *
 *  returns: an integer code associated with the target argument
 *  	u: 1
 *  	l: 2
 *  	cap: 3
 *  	invalid input: -1
 */
int parse_clarg(char* clarg)
{
	if (*clarg == 'u' && *(clarg + 1) == '\0') /* '-u' */
		return 1;

	if (*clarg == 'l' && *(clarg + 1) == '\0') /* '-l' */
		return 2;

	if (*clarg == 'c'
		&& *(clarg + 1) == 'a'
		&& *(clarg + 2) == 'p'
		&& *(clarg + 3) == '\0') /* '-cap' */
		return 3;
	
	return -1;
}

int main(int argc, char* argv [])
{
	int arg_code;
	
	if (argc <= 2)
	{
		printf("Not enough arguments!\n");
		return(-1);
	}

	arg_code = parse_clarg (argv[1] + 1);

	for (int i = 2; i < argc; i++){
		switch (arg_code) {
			case 1:
				cap_all(argv[i]);
				break;
			case 2:
				lower_all(argv[i]);
				break;
			case 3:
				cap_first(argv[i]);
				break;
			default:
				printf("Invalid argument!");
		}
		putchar(' ');
	}

	putchar('\n');
	return 0;
}
