/*
 * learnc.c / Program in C
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 1/13/2025
 *
 * For converting gas consumption rates between imperial and metric
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpg2km.h"

#define MPG2KML 0.425144 /* Miles per gal. to km. per liter factor */
#define LPH2MPG 0.02352 /* L. per 100km to MPG */

/* Function: clear
 * -----------------------
 *  Clears stdin input buffer
 */
void clear ()
{
	while (getchar () != '\n');
}

/* Function: valid_double_input
 * -----------------------
 *  Enforces positive double values from stdin
 *
 *  returns: user-inputted valid double value
 */
double valid_double_input(void){
	double value;
	while (scanf ("%lf", &value) != 1 /* While there is no matching input */
		       	|| value < 0) /* Or the input value is negative */ 
	{
		printf ("Invalid input! Try again\n");
		clear (); /* Clear the buffer */
	}

	return value;
}


/* Function: mpg2kml
 * -----------------------
 * Converts MPG to Km/L
 *
 * mpg:	Mileage expressed in miles per gallon
 * 
 * returns: the corresponding value in km/L
 */
double mpg2kml (double mpg)
{
	return mpg * MPG2KML;
}

/* Function: mpg2lphm
 * -----------------------
 * Converts MPG to (100 km)/L
 *
 * mpg: Mileage expressed in miles per gallon
 *
 * returns: the corresponding value in (100 km)/L
 */
double mpg2lphm(double mpg)
{
	return mpg2kml (mpg) * 100.0;
}

/* Function: lph2mpg
 * -----------------------
 * Converts (100 km)/L to MPG
 * 
 * lph: Gas consumption rate in (100 km) / L
 *
 * returns: The corresponding value in MPG
 */
double lph2mpg(double lph)
{
	return lph * LPH2MPG;
}

/* Validates user input for the above functions */
int main()
{
	double mpg;
	double lph;

	printf ("Enter an MPG value\n");
	
	mpg = valid_double_input ();

	printf ("MPG to KML: %.2lf\n", mpg2kml (mpg));
	printf ("MPG to LPHM: %.2lf\n", mpg2lphm (mpg));
	
	clear ();

	printf ("Enter an LPH value\n");
	 
	lph = valid_double_input ();


	printf ("LPH to MPH: %.2lf\n", lph2mpg (lph));

	return 0;
}
