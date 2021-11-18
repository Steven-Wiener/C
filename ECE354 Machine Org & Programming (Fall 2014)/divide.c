/* Assignment 3 Divide Program
 * Written by Steven Wiener and Fenzhi Gu, lecture 1
 * October 24, 2014
 *
 * This program prompts the user for two integer numbers, performs a 
 * divide with remainder operation, and exits the program after
 * printing out the number of operations performed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int iterations;

/* Procedure: sigfpe
 * Handles the SIGFPE signal (input of an arithmetic error), printing the 
 * 'signo' error number and exits the program
 *
 * Parameters:
 *  signo: the error number, varying according to the SIGFPE function
 */
void sigfpe(int signo) {
  fprintf(stderr,"Error occured %d\n",signo);
  exit(1);
  return;
}

/* Procedure: sigint
 * Handles the SIGINT signal (input C -c), printing the number of 
 * iterations and exits the program.
 */
void sigint() {
  printf("\nOperations completed: %d\nGracefully exiting the program\n",iterations);
  exit(0);
  return;
}

/* Procedure: main
 * Main procedure of program
 * 
 * Parameters:
 *  buf[]: buffer array for user inputs
 *  firstInt: Integer variable for first user input
 *  secondInt: Integer variable for second user input
 *  sa_sigfpe: sigaction used for SIGFPE
 *  sa_sigint: sigaction used for SIGINT
 *  iterations: integer recording number of operations performed
 */
int main(int argc, char *argv[]) {
  // Initialization of variables
  char buf[100]; // Buffer for user inputs: max size is 100
  int firstInt; // Integer variables for first and second user inputs
  int secondInt;
  struct sigaction sa_sigfpe; // Cooresponds to sigfpe
  struct sigaction sa_sigint; // Cooresponds to sigint

  // Makes sure user has not inputted additional unneeded information
  if (argc != 1) {
    fprintf(stderr,"usage: simplealarm\n");
    exit(1);
  }

  // Initialization of signal actions and handlers
  sa_sigfpe.sa_handler = sigfpe;
  if (sigaction(SIGFPE,&sa_sigfpe,0) != 0) {
    fprintf(stderr,"Error: program closing");
  }

  sa_sigint.sa_handler = sigint;
  if (sigaction(SIGINT,&sa_sigint,0) != 0) {
    fprintf(stderr,"Error: program closing");
  }

  // Resets number of iterations to 0
  iterations = 0;

  while (1) {
    // Asks user for first integer and stores in firstInt variable
    printf("Enter first integer: ");
    if (fgets(buf,sizeof(buf),stdin) != NULL) {
      firstInt = atoi(buf);
    } else {
      fprintf(stderr,"Error: program closing");
      exit(1);
    }
    
    // Asks user for second ineger and stores in secondInt variable
    printf("Enter second integer: ");
    if (fgets(buf,sizeof(buf),stdin) != NULL) {
      secondInt = atoi(buf);
    } else {
      fprintf(stderr,"Error: program closing");
      exit(1);
    }
    // Prints result of calculation to console
    printf("%d / %d is %d with a remainder of %d\n", firstInt, secondInt, firstInt / secondInt, firstInt % secondInt);
    // Iterates iterations
    iterations++;
  }

  return(0);
}
