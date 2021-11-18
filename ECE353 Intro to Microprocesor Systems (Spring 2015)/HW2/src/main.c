//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "boardUtil.h"
#include "../include/systick.h"

void UART0_Handler(void);
#define ARRAY_SIZE   128
#define LOOP_COUNT   10

/******************************************************************************
 * External Functions
 *****************************************************************************/
 void bubble_sort(uint32_t *base_addr, int16_t size);
 int32_t insertion_sort(uint32_t *base_addr, int16_t size);
 
/******************************************************************************
 * Global Variables
 *****************************************************************************/
char PERSON1[]="Your Name";
uint32_t   staff_array[ARRAY_SIZE];
uint32_t   student_array[ARRAY_SIZE];

//*****************************************************************************
// Used to verify our sorting algorithm
//*****************************************************************************
__inline void start_clock(void)
{
  SysTick->CTRL     = 0;                  // disable SysTick timer
  SysTick->LOAD     = 0xFFFFFE;           // Reset load value
  SysTick->VAL      = 0;                  // clear the current count
  SysTick->CTRL     = (NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_CLK_SRC);
  
}

//*****************************************************************************
// Used to verify our sorting algorithm
//*****************************************************************************
uint32_t ece353_random(void)
{
  static uint32_t lfsr = 0xA3003456;
  uint32_t bit;

  bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)^ (lfsr >> 10) ^ (lfsr >> 15) ^ (lfsr >> 23) ^ (lfsr >> 29) ) & 1;
  lfsr =  (lfsr >> 1) | (bit << 31);
  
  return lfsr;
  
}

//*****************************************************************************
// Used to verify our sorting algorithm
//*****************************************************************************
__inline uint32_t stop_clock(void)
{
  SysTick->CTRL     = 0;                  // disable SysTick timer
  return (0xFFFFFF - SysTick->VAL);
}


//*****************************************************************************
// Used to initialize arrays
//*****************************************************************************
void initTestVector(uint32_t * staff, uint32_t * student)
{
    int i;
    start_clock();
    for( i=0; i < ARRAY_SIZE; i++)
    {
      staff[i] = ece353_random();
      student[i] = staff[i];
    }
    stop_clock();
}


//*****************************************************************************
//*****************************************************************************
void initializeBoard(void)
{
  DisableInterrupts();
  serialDebugInit();
  EnableInterrupts(); 
}

//*****************************************************************************
// Used to verify our sorting algorithm
//*****************************************************************************
bool verifySort(uint32_t * staff, uint32_t * student)
{
    int i;
    for( i=0; i < ARRAY_SIZE; i++)
    {
        if ( staff[i] != student[i])
            return false;
    }
    
    return true;
}




//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
  uint32_t staff_total=0;
  uint32_t student_total=0;
  uint32_t sort_ticks=0;
  uint32_t i;
  bool pass = false;
  
  initializeBoard();
  
  for (i=0; i < LOOP_COUNT; i++)
  {

    initTestVector(staff_array, student_array);
    
    // Start the timer to count clock cycles
    start_clock();
    
    // Run the staff bubble sort
    bubble_sort(staff_array, ARRAY_SIZE);
    
    // Get the Clock Count
    sort_ticks = stop_clock();

    staff_total += sort_ticks;

    // Start the timer to count clock cycles
    start_clock();
    
    // Run the student sort
    insertion_sort(student_array, ARRAY_SIZE);
    
    // Get the Clock Count
    sort_ticks = stop_clock();
    
    student_total += sort_ticks;

    //  Verify the sort
    pass = verifySort(staff_array,student_array );
    
    // If any of the sorts do not match, stop running tests
    if( pass == false)
    {
      printf("ERROR Sorts did not match\n\r");
      break;
    }
  }
  printf("\n\r");
  printf("**************************************\n\r");
  printf("* ECE353 - Spring 2015 HW2\n\r");
  printf("*      %s\n\r",PERSON1);
  printf("**************************************\n\r");
  if( pass == true)
  {
    printf("\n\rAverage Staff Sort Time for %d runs   : %*d ticks\n\r",LOOP_COUNT, 8,staff_total/LOOP_COUNT);
    printf("Average Student Sort Time for %d runs : %*d ticks\n\r",LOOP_COUNT, 8, student_total/LOOP_COUNT);
    printf("Decrease in clock cycles of %.2f  %%\n\r", 100*((float)((staff_total - student_total))/staff_total));
  }
  
  // Infinite Loop
  while(1){
  
  
  };
}
