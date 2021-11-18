/* 
 * Steven Wiener Sec 001
 * Partner: Fenzhi Gu Sec 001
 * Writes integers to new file in reverse order of which they are inputted
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	int data; // Data held by node
	struct node *pNext; // Node following particular node
} linknode; //Corresponds to typedef, defining linknode

typedef linknode *linklist;

/*
 * Procedure: pop
 * Pops a node from the top of the stack and returns it
 * Returns: 0 if stack is empty, pointer to old top of stack if stack is not empty
 */
int pop(linklist *pHead) {
    int data; // Data held by node
	linklist tempnode; // Temporary node to be used as filler

	if ((*pHead) == NULL)
		return 0;
	
	/* tempnode becomes pHead, pHead points to pNext, returns data of previous pHead */
	tempnode = (*pHead);
	data = tempnode->data;
	(*pHead) = tempnode->pNext;
	free(tempnode);
	return data;
}

/*
 * Procedure: push
 * Pushes a node onto the top of the stack
 * Returns: pointer to new top of stack
 */
linklist push(linklist pHead, int d) {
	linklist pNew; // New node created to be at the head
	/* Allocates memory for new node, assigns data to new node (which becomes pHead), points pHead to next node, and returns pHead */
	pNew = (linklist)malloc(sizeof(linknode));
	pNew->data = d;
	pNew->pNext = pHead;
	pHead = pNew;
	return pHead;
}

/*
 * Procedure: main
 * Main procedure of program
 * Writes integers to new file in reverse order of which they are inputted
 */
int main(int argc, char *argv[]) {
  FILE *fp; // file pointer
  int d; // data from file
  linklist pHead = NULL; // Head of stack
	
	/* Checks to ensure user inputted using correct format */
	if (argc != 3) {
		fprintf(stderr, "usage: strrev <infile> <outfile>\n");
		exit(1);
	}
	
	/* Opens input file and pushes integers to stack */
	fp = fopen(argv[1], "r");
	while (fscanf(fp, "%d", &d) != EOF)
	 	pHead = push(pHead, d);
	
	/* Checks to ensure input file was closed properly */
	if (fclose(fp) != 0) {
		fprintf(stderr, "error closing input file -- quitting\n");
		exit(1);
	}
	
	/* Writes to output file, popping from stack and returning integers in reverse order */
	fp = fopen(argv[2], "w+");
	while (pHead != NULL)
		fprintf(fp,"%d\n", pop(&pHead));
	
	/* Checks to ensure output file was closed properly */
	if (fclose(fp) != 0) {
		fprintf(stderr, "error closing output file -- quitting\n");
		exit(1);
	}
	
	return 0;
}
