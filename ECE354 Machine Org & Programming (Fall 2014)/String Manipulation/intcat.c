/* 
 * Steven Wiener Sec 001
 * Partner: Fenzhi Gu Sec 001
 * Displays binary contents given in inputed file in readable ASCII characters
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
 * Displays binary contents given in inputted file in readable ASCII characters
 */
int main(int argc, char *argv[]) {
	int fp; // input file to be opened
	int fd; // value from binary file
	
	/* Checks to ensure user inputted using correct format */
	if (argc != 2) {
		fprintf(stderr, "usage: intcat <infile>\n");
		exit(1);
	}	
	
	/* Opens file, then prints ASCII characters to console */
	fp = open(argv[1], O_RDONLY);
	while (read(fp, &fd, sizeof(int)) != 0)
		printf("%d\n", fd);
	
	/* Checks to ensure file was closed properly */
	if (close(fp) == -1) {
		fprintf(stderr, "error closing file -- quitting\n");
		exit(1);
	}
	
	return 0;
}
