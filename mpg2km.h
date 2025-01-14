/*
 * mpg2km.h / C Header file
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 1/13/2025
 *
 * Function signatures for mileage conversion
 */

/* Function: clear
 * -----------------------
 *  Clears stdin input buffer
 */
void clear ();

/* Function: valid_double_input
 * -----------------------
 *  Enforces positive double values from stdin
 *
 *  returns: user-inputted valid double value
 */
double valid_double_input(void);

/* Function: mpg2kml
 * -----------------------
 * Converts MPG to Km/L
 *
 * mpg:	Mileage expressed in miles per gallon
 * 
 * returns: the corresponding value in km/L
 */
double mpg2kml (double mpg);

/* Function: mpg2lphm
 * -----------------------
 * Converts MPG to (100 km)/L
 *
 * mpg: Mileage expressed in miles per gallon
 *
 * returns: the corresponding value in (100 km)/L
 */
double mpg2lphm(double mpg);


/* Function: lph2mpg
 * -----------------------
 * Converts (100 km)/L to MPG
 * 
 * lph: Gas consumption rate in (100 km) / L
 *
 * returns: The corresponding value in MPG
 */
double lph2mpg(double lph);

