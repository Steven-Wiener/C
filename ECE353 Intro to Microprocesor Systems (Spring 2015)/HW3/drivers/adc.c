#include "adc.h"

//*****************************************************************************
// Initializes ADC0 to use Sample Sequencer #2, triggered by software, no IRQs
// 
// SS2 should so the following channels are converted in this order
//
//  0 - X direction of PS2 Joystick
//  1 - Y Direction of PS2 Joystick
//  2 - Volume Potentiometer
//*****************************************************************************
void configure_adc0(void){
	// Turn on the ADC0 Clock
  SYSCTL->RCGCADC |= SYSCTL_RCGCADC_R0;
  
  // Wait for ADC0 to become ready
  while((SYSCTL_PRADC_R0 & SYSCTL->PRADC) != SYSCTL_PRADC_R0){}
  
  // Disable sample sequencer #2
	ADC0->ACTSS &= ~ADC_ACTSS_ASEN2;

  // Set the event multiplexer to trigger conversion on a software trigger
  // for sample sequencer #2.
	ADC0->EMUX &= ~ADC_EMUX_EM2_ALWAYS;

  // Set IE2 and END2 in SSCTL2
	ADC0->SSCTL2 = ADC_SSCTL2_IE2 | ADC_SSCTL2_END2;
	
	// Set the channels to sample to be AIN0, AIN1, and AIN2
	ADC0->SSMUX2 = 0x201;
	
	// Enable sample sequencer #2
	ADC0->ACTSS |= ADC_ACTSS_ASEN2;
}

//******************************************************************************
// Returns the values for the PS2 joystic and potentiometer
//*****************************************************************************
bool get_adc_values(uint32_t adc_base, uint16_t *ps2_x, uint16_t *ps2_y, uint16_t *pot){
	ADC0_Type  *myADC;
  myADC = (ADC0_Type *)adc_base; // Typecast ADC
	
	myADC->PSSI = ADC_PSSI_SS2;     // Start SS2
	
  while((myADC->RIS & ADC_RIS_INR2)  == 0){}	// wait
	
	*ps2_x = 0xFFF - (myADC->SSFIFO2 & 0xFFF);    // Read 12-bit data for ps2_x
	*ps2_y = myADC->SSFIFO2 & 0xFFF;    // Read 12-bit data for ps2_y
	*pot = myADC->SSFIFO2 & 0xFFF;    // Read 12-bit data for pot
	  
  myADC->ISC = ADC_ISC_IN2;          // Acknowledge the conversion
	
  return true;
}
