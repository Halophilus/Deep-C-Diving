/*
 * arger.c / Program in C (Part 2)
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / January 13, 2025
 *
 */
#include <stdio.h>

void cap_first(char* string)
{
	if (*string >= 97 && *string <= 122)
		putchar(*string - 32);
       	else
		putchar(*string);

	for (int i = 1; string[i] != '\0'; i++)
		putchar (* (string + i));
}

void cap_all(char* string)
{
	for (int i = 0; string[i] != '\0'; i++)
		if (string[i] >= 97 && string[i] <= 122)
			putchar (*(string + i) - 32);
		else 
			putchar (*(string + i));	
}

int parse_clarg(char* clarg)
{

}

int main(int argc, char* argv [])
{
	if (argc <= 2)
	{
		printf("Not enough arguments!\n");
		return -1;
	}

	printf("%s\n", argv[1]+2); /* The string following the first char of the first argument */
	cap_all(argv[1]);

	return 0;
}
