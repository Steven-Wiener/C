/* Assignment 3 Simple Alarm Program
 * Written by Steven Wiener and Fenzhi Gu, lecture 1
 * October 24, 2014
 *
 * This program prints out the current time and date, and counts to a 
 * random multiple of 10000000, displaying a '.' for every multiple counted to
 * until the user inputs (Ctrl -c) to stop the program.
 */

#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>

time_t curtime;

/* Procedure: sigalrm
 * Handles the SIGALRM signal, printing the current time and sending another sig
 * 
 * Parameters:
 *  curtime: handles the current time of the system
 */
void sigalrm(int sig) {
  curtime = time(NULL);
  printf("\ncurrent time is %s",ctime(&curtime));

  alarm(1);
}

/* Procedure: sigint
 * Handles the SIGINT signal (input Ctrl -c), exiting the program
 */
void sigint() {
  printf("\n");
  exit(0);
  return;
}

/* Procedure: main
 * Main procedure of program
 * 
 * Parameters:
 *  sa_sigalrm: Cooresponds to sigalrm
 *  sa_sigint:  Cooresponds to sigint
 *  i: Counter variable
 *  j: Counter variable
 *  randomInt: Random integer
 */
int main(int argc, char *argv[]) {
  // Initialization of variables
  struct sigaction sa_sigalrm;
  struct sigaction sa_sigint;
  int i;
  int j;
  int randomInt;
 
  // Makes sure user has not inputted additional unneeded information
  if (argc != 1) {
    fprintf(stderr,"usage: simplealarm\n");
    exit(1);
  }

  // Initialization of signal actions and handlers
  sa_sigalrm.sa_handler=sigalrm;
  if (sigaction(SIGALRM,&sa_sigalrm,0) != 0)
    fprintf(stderr,"Error: program closing");

  sa_sigint.sa_handler=sigint;
  if (sigaction(SIGINT,&sa_sigint,0) != 0)
    fprintf(stderr,"Error: program closing");

  srandom(time(NULL)); // Initializes random function
  alarm(1); // Initializes alarm function

  printf("Enter ^C to end the program:\n"); // Gives user instructions to exit

  while(1) {
    randomInt = random() % 10; // Generates random number, taking LSB
    for (i = 0; i < randomInt; i++)
      for (j = 0; j < 10000000; j++) { } // Counts to 10000000 <randomInt> times
    printf("."); // Prints a '.' for every iteration
  }

  return(0);
}
