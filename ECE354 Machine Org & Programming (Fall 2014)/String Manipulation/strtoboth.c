/* 
 * Steven Wiener Sec 001
 * Partner: Fenzhi Gu Sec 001
 * Reads input from file in ASCII format and writes out integer version of the same, as well as a modified version in ASCII format
 * 
 * Question: Could we use strcat to look at the .str output? No, because the strcat only reads integers, and will crash at the ";"
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Procedure: main
 * Main procedure of program
 * Reads input from file in ASCII format and writes out integer version of the same, as well as a modified version in ASCII format
 */
int main(int argc, char *argv[]) {
	FILE *fp1, *fp2; // file pointers for input file (fp1) and output files (fp2)
	int d; // Data from file
	
	/* Checks to ensure user inputted using correct format */
	if (argc != 4) {
		fprintf(stderr, "usage: strtoboth <infile.str> <outfile1.int> <outfile2.str2>\n");
		exit(1);
	}
	
	/* Opens input to read and first output file to write */
	fp1 = fopen(argv[1], "r");
	fp2 = fopen(argv[2], "wb+");
	while (fscanf(fp1, "%d", &d) != EOF)
		fwrite(&d, sizeof(int), 1, fp2);
	
	/* Checks to ensure input file was closed properly */
	if (fclose(fp1) != 0) {
		fprintf(stderr, "error closing <infile.str> file -- quitting\n");
		exit(1);
	}
	
	/* Checks to ensure first output file was closed properly */
	if (fclose(fp2) != 0) {
		fprintf(stderr, "error closing <outfile1.int> file -- quitting\n");
		exit(1);
	}
	
	/* Reopens input and opens second output to write to */
	fp1 = fopen(argv[1], "r");
	fp2 = fopen(argv[3], "wt+");
	
	/* Goes through file and appends ";" to positive integers, and "- <integer>;" to negative integers */
	while (fscanf(fp1, "%d", &d) != EOF) {
		if (d >= 0) {
		  fprintf(fp2, "%d;\n", d);
		} else {
			d = d * (-1);
			fprintf(fp2, "- %d;\n", d);
		}
	}	
	/* Checks to ensure input file was closed properly */
	if (fclose(fp1) != 0) {
		fprintf(stderr, "error closing <infile.str> file -- quitting\n");
		exit(1);
	}
	
	/* Checks to ensure second output file was closed properly */
	if (fclose(fp2) != 0) {
		fprintf(stderr, "error closing <outfile2.str2> file -- quitting\n");
		exit(1);
	}
	
	return 0;
}
