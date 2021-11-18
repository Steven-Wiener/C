//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "boardUtil.h"

void UART0_Handler(void);
extern void hw1(void);

/******************************************************************************
 * Global Variables
 *****************************************************************************/
 extern char TRANSACTIONS_TOTAL[];
 extern char TRANSACTIONS[1000][8];
 extern char PERSON1[];
 
//*****************************************************************************
// Initialize the serial debug interface
//*****************************************************************************
void initializeBoard(void)
{
  
  DisableInterrupts();
  serialDebugInit();
  EnableInterrupts();
   
}


//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
  
  initializeBoard();
  
  printf("\n\r");
  printf("**************************************\n\r");
  printf("* ECE353 - Spring 2015 HW1\n\r");
  printf("*      %s\n\r",PERSON1);
  printf("**************************************\n\r");
  
  // Initialize transactions
  strcpy( TRANSACTIONS[0],"-190.90");
  strcpy( TRANSACTIONS[1],"-900.10"); //-109100
  strcpy( TRANSACTIONS[2],"-015.05"); //-100605
  strcpy( TRANSACTIONS[3],"-100.01"); //-120606
  strcpy( TRANSACTIONS[4],"+912.19"); //- 29387
  // Indicate last transaction
  TRANSACTIONS[5][0] = 0;
  
  hw1();

  // Check the calculated total
  if( strcmp( TRANSACTIONS_TOTAL, "-$00,293.87") != 0 )
  {
		printf(TRANSACTIONS_TOTAL);
    printf("\nTest 1 \n\r");
  }
  else
  {
    printf("Passed Test 1: %s\n\r",TRANSACTIONS_TOTAL);
  }

  // Initialize transactions
  strcpy( TRANSACTIONS[0],"+901.90");
  strcpy( TRANSACTIONS[1],"+902.91");
  strcpy( TRANSACTIONS[2],"-103.92");
  strcpy( TRANSACTIONS[3],"+904.93");
  strcpy( TRANSACTIONS[4],"+905.94");
  // Indicate last transaction
  TRANSACTIONS[5][0] = 0;
  
  hw1();
  
  // Check the calculated total
  if( strcmp( TRANSACTIONS_TOTAL, "+$03,511.76") != 0 )
  {
    printf(TRANSACTIONS_TOTAL);
    printf("\nTest 2 failed\n\r");
  }
  else
  {
    printf("Passed Test 2: %s\n\r",TRANSACTIONS_TOTAL);
  }

  // Initialize transactions
  strcpy( TRANSACTIONS[0],"+001.90");
  strcpy( TRANSACTIONS[1],"0002.91");
  strcpy( TRANSACTIONS[2],"-003.92");
  strcpy( TRANSACTIONS[3],"+004.93");
  strcpy( TRANSACTIONS[4],"+005.94");
  // Indicate last transaction
  TRANSACTIONS[5][0] = 0;
  
  hw1();

  // Check the calculated total
  if( strcmp( TRANSACTIONS_TOTAL, "CALC ERROR") != 0 )
  {
    printf(TRANSACTIONS_TOTAL);
    printf("\nTest 3 failed\n\r");
  }
  else
  {
    printf("Passed Test 3: %s\n\r",TRANSACTIONS_TOTAL);
  }
  
  
  // Infinite Loop
  while(1){
  
  
  };
}
