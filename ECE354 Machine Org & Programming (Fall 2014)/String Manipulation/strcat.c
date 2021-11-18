/* 
 * Steven Wiener Sec 001
 * Partner: Fenzhi Gu Sec 001
 * Displays number strings given in inputed file, one number per line
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Procedure: main
 * Main procedure of program
 * Displays number strings given in inputted file, one number per line
 */
int main(int argc, char *argv[]) {
	FILE *fp; // input file to be read
	int d; // data output to be printed to console
	
	/* Checks to ensure user inputted using correct format */
	if (argc != 2) {
		fprintf(stderr, "usage: strcat <infile>\n");
		exit(1);
	}
	
	/* Opens file, then prints number strings to console */
	fp = fopen(argv[1],"r");
	while (fscanf(fp, "%d", &d) != EOF)
		printf("%d\n", d);
	
	/* Checks to ensure file was closed properly */
	if (fclose(fp) != 0) {
		fprintf(stderr, "error closing file -- quitting\n");
		exit(1);
	}
	
	return 0;
}
