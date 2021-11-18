#include "timer0.h"

//*****************************************************************************
// Configure Timer 0 to be two 16-bit, periodic, count down timers.
// Do not set the values for TAILR or TBILR and DO NOT enable the timer.
//*****************************************************************************
void configure_timer0(void){
	// Turn on the clock for the timer
	SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R0;
	
	// Wait for the timer to turn on
  while((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R0) == 0) {};
	
	// Turn the timers off
	TIMER0->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
	
	// Set the timer to be in 16-bit mode
	TIMER0->CFG = TIMER_CFG_16_BIT;
	
	// Set the Timer A & B Mode Registers to be in periodic, count down
	TIMER0->TAMR = TIMER_TAMR_TAMR_PERIOD;
	TIMER0->TBMR = TIMER_TBMR_TBMR_PERIOD;
	
	// Clear the Timer Interrupt Clear Registers for Timer0 A & B
	TIMER0->ICR |= (TIMER_ICR_TATOCINT | TIMER_ICR_TBTOCINT);
		
	TIMER0->IMR = TIMER_IMR_TATOIM | TIMER_IMR_TBTOIM;
	NVIC_SetPriority (TIMER0A_IRQn, 0);
	NVIC_SetPriority (TIMER0B_IRQn, 0);
	NVIC_EnableIRQ(TIMER0A_IRQn);
	NVIC_EnableIRQ(TIMER0B_IRQn);
}

//*****************************************************************************
// Turns on Timer0A and Turns Off Timer0B.  The TAILR is set to load_value
//*****************************************************************************
void start_timer0A(uint16_t load_value){
	// Turn off Timer0B
	stop_timer0B();
	
	// Turn on Timer0A
	// Set the number of clock cycles in the Timer A Interval Load Register
	TIMER0->TAILR = load_value;
	
	// Clear Timer 0B's ICR
	TIMER0->ICR |= TIMER_ICR_TATOCINT;
	
	// Enable Timer A using the Timer Control Register
	TIMER0->CTL |= TIMER_CTL_TAEN;
}

//*****************************************************************************
// Turns off Timer0A.  This function does not alter the load value.
//*****************************************************************************
void stop_timer0A(void){
	// Turn Timer0A off
	TIMER0->CTL &= ~TIMER_CTL_TAEN;
}

//*****************************************************************************
// Turns on Timer0B and Turns Off Timer0A.  The TBILR is set to load_value
//*****************************************************************************
void start_timer0B(uint16_t load_value){
	// Turn off Timer0A
	stop_timer0A();
	
	// Turn on Timer0B
	// Set the number of clock cycles in the Timer B Interval Load Register
	TIMER0->TBILR = load_value;
	
	// Clear Timer 0B's ICR
	TIMER0->ICR |= TIMER_ICR_TBTOCINT;
	
	// Enable Timer B using the Timer Control Register
	TIMER0->CTL |= TIMER_CTL_TBEN;
}

//*****************************************************************************
// Turns off Timer0B.  This function does not alter the load value.
//*****************************************************************************
void stop_timer0B(void){
	// Turn Timer0B off
	TIMER0->CTL &= ~TIMER_CTL_TBEN;	
}
