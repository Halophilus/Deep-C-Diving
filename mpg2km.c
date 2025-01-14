/*
 * mpg2km.c / Source file for mpg2km header
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 1/13/2025
 *
 * Functions for converting gas consumption rates between imperial and metric
 */

#include <stdio.h>
#include <stdlib.h>

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

