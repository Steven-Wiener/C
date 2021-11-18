//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "gpioPort.h"
#include "adc.h"
#include "timer0.h"
#include "boardUtil.h"
#include "interrupts.c"

volatile bool Alert_1ms = false;
volatile bool Alert_Timer0A = false;
volatile bool Alert_Timer0B = true;

extern void serialDebugInit(void);
char  teamNumber[] = "27";
char person1[] = "Steven Wiener";
char person2[] = "Si Jin Lai" ;

// Define the base addresses for the GPIO port and
// the ADC attached to the PS2 joystick.
#define		DAC_BASE			GPIOB_BASE
#define		DIR_BTN_BASE	GPIOF_BASE
#define		PS2_POT_BASE	GPIOE_BASE

// Define the Bitmask for each pin below
#define		PS2_X_DIR_PIN		(1 << 2)
#define		PS2_Y_DIR_PIN		(1 << 3)
#define		DIR_BTN_UP			(1 << 1)
#define   DAC_0						(1 << 0)
#define   DAC_1						(1 << 1)
#define   DAC_2						(1 << 2)
#define   DAC_3						(1 << 3)
#define   DAC_4						(1 << 4)
#define   DAC_5						(1 << 5)
#define		POT							(1 << 1)

// Set the analog channel for each direction
#define		POT_ADC_CHANNEL			2
#define		PS2_X_ADC_CHANNEL		0
#define		PS2_Y_ADC_CHANNEL		1

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	uint16_t	ps2_x_val;
	uint16_t	*ps2_x = &ps2_x_val;
	uint16_t	ps2_y_val;
	uint16_t	*ps2_y = &ps2_y_val;
	uint16_t	pot_val;
	uint16_t	*pot = &pot_val;
	
	int	freq = 2500;
	int dc = 3;
	static int count = 0;
	bool soundOn = true;
  
  serialDebugInit();
  
  printf("\n\r");
  printf("***************************************\n\r");
  printf("ECE353 Spring 2015  - HW3 \n\r");
  printf("\tTeam %s\n\r", teamNumber);
  printf("\t%s\n\r", person1);
  printf("\t%s\n\r", person2);
  printf("****************************************\n\r");
  
	
	// Enable the ports used for the pushbuttons, DAC, joystick, and pot
	gpio_enable_port(DAC_BASE);
	gpio_enable_port(DIR_BTN_BASE);
	gpio_enable_port(PS2_POT_BASE);
	
	// Configure the UP button as digital input, with internal pull-up resistor
	gpio_config_digital_enable(DIR_BTN_BASE, DIR_BTN_UP);
	gpio_config_enable_input(DIR_BTN_BASE, DIR_BTN_UP);
	gpio_config_enable_pullup(DIR_BTN_BASE, DIR_BTN_UP);
	
	// Configuire the DAC pins as digital outputs
	gpio_config_digital_enable(DAC_BASE, DAC_0 | DAC_1 | DAC_2 | DAC_3 | DAC_4 | DAC_5);
	gpio_config_enable_output(DAC_BASE, DAC_0 | DAC_1 | DAC_2 | DAC_3 | DAC_4 | DAC_5);

	// Configure the PS2 joystick GPIO and the rotary potentiometer as analog input pins
	gpio_config_analog_enable(PS2_POT_BASE, POT | PS2_X_DIR_PIN | PS2_Y_DIR_PIN);
	gpio_config_enable_input(PS2_POT_BASE, POT | PS2_X_DIR_PIN | PS2_Y_DIR_PIN);
	
	// Configure ADC0
	configure_adc0();
	
	// Configure Timer0
	configure_timer0();
	
	// Configure the SysTick timer to generate interrupts every 1mS	
	SysTick_Config(50000);

	while(1)
  {
		get_adc_values(ADC0_BASE, ps2_x, ps2_y, pot); // Get ADC Values
		
		if (Alert_1ms){		// Check for Systick Interrupt
			if(!(GPIOF->DATA & DIR_BTN_UP))	// If button is being held, increment count
				count++;
			else
				count = 0;
		}
		
		if (count >= 16){		// If button is held for 16ms, toggle sound (only once)
			if (count == 16)
				soundOn = !soundOn;
			if (GPIOF->DATA & DIR_BTN_UP)
				count = 0;
		}
		
		if (soundOn){ // Only need to execute next if Sound is on
			// Set frequency
			if (ps2_y_val < ((0xFFF * 20) / 100))
				freq = 1000;
			else if (ps2_y_val < ((0xFFF * 30) / 100))
				freq = 1500;
			else if (ps2_y_val < ((0xFFF * 40) / 100))
				freq = 2000;
			else if (ps2_y_val < ((0xFFF * 50) / 100))
				freq = 2500;
			else if (ps2_y_val < ((0xFFF * 60) / 100))
				freq = 3000;
			else if (ps2_y_val < ((0xFFF * 70) / 100))
				freq = 3500;
			else if (ps2_y_val < ((0xFFF * 80) / 100))
				freq = 4000;
			else
				freq = 4500;
			
			// Set DC
			if (ps2_x_val < ((0xFFF * 20) / 100))
				dc = 4; //25%
			else if (ps2_x_val < ((0xFFF * 80) / 100))
				dc = 3; //33%
			else
				dc = 2; //50%
				
				if (Alert_Timer0B){ // If TimerB has expired

					// Set active DAC GPIO Pin
					if (pot_val < ((0xFFF * 15) / 100))
						GPIOB->DATA = DAC_0;
					else if (pot_val < ((0xFFF * 30) / 100))
						GPIOB->DATA = DAC_1;
					else if (pot_val < ((0xFFF * 45) / 100))
						GPIOB->DATA = DAC_2;
					else if (pot_val < ((0xFFF * 60) / 100))
						GPIOB->DATA = DAC_3;
					else if (pot_val < ((0xFFF * 75) / 100))
						GPIOB->DATA = DAC_4;
					else
						GPIOB->DATA = DAC_5;

					start_timer0A((50000000 / freq) / dc); // Start TimerA
				}

				else if (Alert_Timer0A){ // If TimerA has expired
					GPIOB->DATA = 0; // Turn off sound

					start_timer0B(((50000000 / freq) * (dc - 1)) / dc); // Start TimerB
				}
		}else
			GPIOB->DATA = 0; // If sound is not on, turn off sound
		
		Alert_1ms = false; // Reset Interrupt Handlers
		Alert_Timer0A = false;
		Alert_Timer0B = false;
	}
}
