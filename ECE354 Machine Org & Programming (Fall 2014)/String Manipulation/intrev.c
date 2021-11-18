/* 
 * Steven Wiener Sec 001
 * Partner: Fenzhi Gu Sec 001
 * Writes integers to new file in reverse order of which they are inputted
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * Procedure: main
 * Main procedure of program
 * Writes integers to new file in reverse order of which they are inputted
 */
int main(int argc, char *argv[]) {
	int d; // Data from file
	int fp; // File pointer
	int *array; // Array of integers to be reversed
	int i; // Counter of characters in file
	struct stat buffer;
	
	/* Checks to ensure user inputted using correct format */
	if (argc != 3) {
		fprintf(stderr, "usage: intrev <infile> <outfile>\n");
		exit(1);
	}
	
	/* Open file and make sure file is valid */
	fp = open(argv[1], O_RDONLY);	
	if (fstat(fp, &buffer) < 0) {
		fprintf(stderr, "error using fstat")
		exit(1);
	}
	
	/* Allocate array, initialize counter, and read through file */
	array = (int *)malloc(buffer.st_size);
	i = 0;

	while (read(fp, &d, sizeof(int)) != 0) {
		*(array + i) = d;
		i++;
	}
	i--;
	
	/* Checks to ensure input file was closed properly */
	if (close(fp) == -1) {
		fprintf(stderr, "error closing input file -- quitting\n");
		exit(1);
	}
	
	/* 
	 * Open (or create a new) file and write to it 
	 * with data from input reversed
	 */
	fp = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	while (i >= 0) {
		write(fp, (array + i), sizeof(int));
		i--;
	}
	
	/* Checks to ensure output file was closed properly */
	if (close(fp) == -1) {
		fprintf(stderr, "Error closing output file -- quitting\n");
		exit(1);
	}
	
	return 0;
}
