/*
 * learnc.c / Program in C
 *
 * Ben Henshaw / CS5600 / Northeastern University
 * Spring 2025 / 1/13/2025
 *
 */
#include <stdio.h>
#include <stdlib.h>

void clear(){
	while (getchar() != '\n');
}

double mpg2kml(double mpg){
	return mpg*0.425144;
}

double mpg2lphm(double mpg){
	return mpg2kml(mpg)*100.0;
}

double lph2mpg(double lph){
	return lph*2.352;
}

int main(){
	double mpg;
	double lph;

	printf("Enter an MPG value\n");
	while(scanf("%lf", &mpg) != 1 || mpg < 0){
		printf("Invalid input! Try again\n");
		clear();
	}

	printf("MPG to KML: %.2lf\n",mpg2kml(mpg));
	printf("MPG to LPHM: %.2lf\n",mpg2lphm(mpg));
	
	clear();

	printf("Enter an LPH value\n");
	 
	while(scanf("%lf", &lph) != 1 || lph < 0){
		printf("Invalid input! Try again\n");
		clear();
	}


	printf("LPH to MPH: %.2lf\n",lph2mpg(lph));

	return 0;
}
