#include "interrupts.h"
#include "boardUtil.h"

extern volatile bool Alert_1ms;
//*****************************************************************************
// Used to debounce the pushbutton and check the potentiometer.
//*****************************************************************************
void SysTick_Handler(void)
{
  uint32_t val;
  
  Alert_1ms = true;
  
  // Clear the interrupt
  val = SysTick->VAL;
}

extern volatile bool Alert_Timer0A;
//*****************************************************************************
// Timer0A Interrupt Handler
// Used to time the "ON" portion of the waveform
//*****************************************************************************
void TIMER0A_Handler(void)
{
  Alert_Timer0A = true;
  TIMER0->ICR |= TIMER_ICR_TATOCINT;
}

extern volatile bool Alert_Timer0B;
//*****************************************************************************
// Timer0B Interrupt Handler
// Used to time the "OFF" portion of the waveform
//*****************************************************************************
void TIMER0B_Handler(void)
{
  Alert_Timer0B = true;
  TIMER0->ICR |= TIMER_ICR_TBTOCINT;
}
