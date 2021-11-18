#include "timer0.h"

//*****************************************************************************
// Configure Timer 0 to be one 32-bit, periodic, count down timer.
//*****************************************************************************
void configure_timer0(void){
	// Turn on the clock for the timer
	SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R0;
	
	// Wait for the timer to turn on
  while((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R0) == 0) {};
	
	// Turn the timers off
	TIMER0->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
	
	// Set the timer to be in 32-bit mode
	TIMER0->CFG = TIMER_CFG_32_BIT_TIMER;
	
	// Set the Timer0A to be in periodic, count down
	TIMER0->TAMR = TIMER_TAMR_TAMR_PERIOD;
	
	// Clear the Timer Interrupt Clear Registers for Timer0A
	TIMER0->ICR |= TIMER_ICR_TATOCINT;
	TIMER0->IMR = TIMER_IMR_TATOIM;
	
	NVIC_SetPriority (TIMER0A_IRQn, 1);
	NVIC_EnableIRQ(TIMER0A_IRQn);
}

//*****************************************************************************
// Turns on Timer0A.  The TAILR is set to load_value
//*****************************************************************************
void start_timer0A(uint16_t load_value){
	// Set the number of clock cycles in the Timer A Interval Load Register
	TIMER0->TAILR = load_value;
		
	// Enable Timer A using the Timer Control Register
	TIMER0->CTL |= TIMER_CTL_TAEN;
}
