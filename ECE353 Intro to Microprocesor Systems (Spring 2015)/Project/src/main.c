//*****************************************************************************
// main.c
// Authors: Steven Wiener & Si Jin Lai, Team 27
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "boardUtil.h"
#include "io_expander.h"
#include "lcd.h"
#include "adc.h"
#include "timer0.h"
#include "eeprom.h"

// -----Defines----------------------------------------------------------------
// DIR BTNs, PS2
#define		DIR_BTN_BASE				GPIOF_BASE
#define   PS2_GPIO_BASE    		GPIOE_BASE
#define   PS2_ADC_BASE     		ADC0_BASE

#define		DIR_BTN_UP					(1 << 1)
#define		DIR_BTN_RIGHT				(1 << 2)
#define		DIR_BTN_LEFT				(1 << 3)
#define		DIR_BTN_DOWN				(1 << 4)
#define   PS2_X_DIR_PIN				(1 << 2)
#define   PS2_Y_DIR_PIN				(1 << 3)
#define		PS2_BTN							(1 << 0)

#define   PS2_X_ADC_CHANNEL		1
#define   PS2_Y_ADC_CHANNEL		0

// Variable Declaration
const uint8_t maxX = 12; // Max Width of board: 12 characters with change
const uint8_t maxY = 8;  // Max Length of Board: 8 characters
int8_t headX = 4; // Head of snake starts at (4,5) from top left
int8_t headY = 5;
uint8_t length = 5;
uint8_t score;

uint8_t userDone = 0;
int currGrid[12][8]; // currGrid[maxX][maxY]
int nextGrid[12][8]; // nextGrid[maxX][maxY]
bool win = false;
	
// Wireless data
wireless_com_status_t status;
uint8_t packetsRec = 0;
uint8_t packetsDrop = 0;
uint32_t opponentScore;
	
// Interrupt Alerts
volatile bool Alert_100mS = false;
volatile bool Alert_Timer0A = false;

// For LEDBAR
extern const uint8_t LedNumbers[32][5];
volatile uint8_t Active_Column = 0;

// For LCD screen
extern const uint8_t win_image[];
extern const uint8_t lose_image[];
extern const uint8_t wait_image[];
extern const uint8_t start_image[];


//*****************************************************************************
//*****************************************************************************
void drawGrid(){
	int i,j;
	
	// Clear the board
	dogs102_clear();
	
	// Draw the grid based on integer values
	for (i = 0; i < maxX; i++)	{
		for (j = 0; j < maxY; j++)	{
			if (currGrid[i][j] == -2)
				dogs102_write_snake(j, 18, i); // 'o' ball
			else if (currGrid[i][j] == length - 1)
				dogs102_write_snake(j, 9, i); // 'X' head
			else if (currGrid[i][j] == 0)
				dogs102_write_snake(j, 0, i); // '*' tail
			else if (currGrid[i][j] > 0)
				dogs102_write_snake(j, 27, i); // 'x' body
			// Else, it's blank space: valued '-1'
		}
	}
}

void newBall(){
	// Set ball x and y to random values
	uint8_t ballX = rand() % maxX;
	uint8_t ballY = rand() % maxY;
	// If these random values are NOT on the snake, return
	if (currGrid[ballX][ballY] < 0 || nextGrid[ballX][ballY] < 0)	{
		nextGrid[ballX][ballY] = -2;
		return;
	}else{ // If these random values ARE on the snake, re-generate new random values
		newBall();
	}
}

void copyGrid(){
	int i,j;
	
	// Score to "Win" the game. Can be modified if desired.
	int scoreToWin = 0xF;
	
	// Check for wall collision / body collision
	if ((0 > headX || headX > maxX - 1 || 0 > headY || headY > maxY - 1) || (currGrid[headX][headY] > 0))	{
		userDone = 1;
		return;
	}
	
	// Check for ball collision
	if (currGrid[headX][headY] == -2)	{
		// Increment score and length, generate new ball
		score++;
		length++;
		newBall();
		
		// If score is at max, signal win
		if (score >= scoreToWin){
			win = true;
			userDone = true;
		}
	}
	
	// Copy next grid to current grid
	for (i = 0; i < maxX; i++)	{
		for (j = 0; j < maxY; j++)	{
			currGrid[i][j] = nextGrid[i][j];
		}
	}
}

// Decrements every  positive segment of the grid by 1. This causes the head of the snake
// to become part of the body, and every segment that is part of the body
// or tail to decrement as well. At any given time, each segment of the body
// will be equal to the number of cycles of game time it has left before
// it turns into blank space (represented by -1). The tail will have a value
// of 0 before turning into blank space.
void decGrid(){
	int i,j;
	for (i = 0; i < maxX; i++)	{
		for (j = 0; j < maxY; j++)	{
			if (nextGrid[i][j] > -1){
			nextGrid[i][j] += -1;
			}
		}
	}
}

//*****************************************************************************
// Configure watchdog timer
//*****************************************************************************
void configure_watchdog(void){
	// Turn on the clock for the timer
	SYSCTL->RCGCWD |= SYSCTL_RCGCWD_R0;
	
	// Set the number of clock cycles
	WATCHDOG0->LOAD = 500000000;
	
	// Enable interrupt and reset
	WATCHDOG0->CTL |= 0x3;
	
	// Set priority and enable interrupts
	NVIC_SetPriority (WATCHDOG0_IRQn, 0);
	NVIC_EnableIRQ(WATCHDOG0_IRQn);	
}

void configure_gpios(void){
	// Enable the ports used for the pushbuttons and joystick
	gpio_enable_port(DIR_BTN_BASE);
	gpio_enable_port(PS2_GPIO_BASE);
	
	// Configure the UP button and PS2 button as digital input, with pull-up resistor
	gpio_config_digital_enable(DIR_BTN_BASE, DIR_BTN_UP);
	gpio_config_digital_enable(PS2_GPIO_BASE, PS2_BTN);
	gpio_config_enable_input(DIR_BTN_BASE, DIR_BTN_UP);
	gpio_config_enable_input(PS2_GPIO_BASE, PS2_BTN);
	gpio_config_enable_pullup(DIR_BTN_BASE, DIR_BTN_UP);
	gpio_config_enable_pullup(PS2_GPIO_BASE, PS2_BTN);
	
	// Configure the PS2 joystick GPIO and the rotary potentiometer as analog input pins
	gpio_config_analog_enable(PS2_GPIO_BASE, PS2_X_DIR_PIN | PS2_Y_DIR_PIN);
	gpio_config_enable_input(PS2_GPIO_BASE, PS2_X_DIR_PIN | PS2_Y_DIR_PIN);
	
	
	// Set up Interrupts for GPIOE Pin 0 (PS2)	
	GPIOE->IM = (1 << 0); // Pin 0
	
	// Set priority and enable interrupts
	NVIC_SetPriority (GPIOE_IRQn, 2);
	NVIC_EnableIRQ(GPIOE_IRQn);	
}

// Configure the board for various capabilities
void configureBoard(void){
  DisableInterrupts();
  serialDebugInit();
  i2cInit();
	lcd_init();
	dogs102_init();
	rfInit();
	ioExpanderInit(IO_EXPANDER_I2C_BASE);
	configure_gpios();
	configure_adc0();
	configure_timer0();
	SysTick_Config(5000000); // 100mS
  EnableInterrupts();
}

//*****************************************************************************
//*****************************************************************************

int main(void) {
	// Variable Initilization
	uint8_t myID[]      = { '3', '5', '3', 'E', 'B'};
	uint8_t remoteID[]  = { '3', '5', '3', 'B', 'E'};
	
	int8_t xDir = 1; // 1 if Right, 0 if Up/Down, -1 if Left
	int8_t yDir = 0; // 1 if Down, 0 if Left/Right, -1 if Up
	
	uint8_t packetsSent = 0;
	int i,j; // Count variables
	int systick = 0;
	bool tick_1mS = false;
	int tick_5Sec = 0;
	
	// PS2 data values
	uint16_t	ps2_x;
	uint16_t	ps2_y;
	
	// Configure various board functions
	configureBoard();
	
	// Print current values of the eeprom
	print_eeprom(false);

	// Initialize board with blank spaces
	for (i = 0; i < maxX; i++)	{
		for (j = 0; j < maxY; j++)	{
			currGrid[i][j] = -1;
			nextGrid[i][j] = -1;
		}
	}
	
	//	Initialize body/tail
	for (i = 0; i < 5; i++)	{
		currGrid[i][5] = i;
		nextGrid[i][5] = i - 1;
	}
	
	//	Initialize head
	currGrid[headX][headY] = length - 1;
	
	//	Initialize ball
	newBall();
	
	// Draw start image
	draw_image(start_image);
	
	// Wait for UP to be pressed for more than 500mS to start game
	while(systick < 5){
		if (Alert_100mS){
			Alert_100mS = false;
			if(!(GPIOF->DATA & DIR_BTN_UP))	// If button is being held, increment count
				systick++;
			else
				systick = 0;
		}
	}

	// Start Timer0A to trigger every .5 mS
	start_timer0A(25000);
	
	// Get ADC Values for the PS2 Joystick
	get_adc_values(ADC0_BASE, &ps2_x, &ps2_y);
	
	// Configure Wireless device
	wireless_configure_device(myID, remoteID);
	
	systick = 0; // Reset count
	
	// Configure Watchdog Timer
	configure_watchdog();
	
//*****************************************************************************
// Game loop
//*****************************************************************************
	while(userDone == 0){
		// If heading right/left, only check up/down of joystick for new direction
		if (xDir != 0){
			if (ps2_y < ((0xFFF * 25) / 100)){
				yDir = 1;		// Go Down
				xDir = 0;
			}else if (ps2_y > ((0xFFF * 75) / 100)){
				yDir = -1;	// Go Up
				xDir = 0;
			}
		}else{ // Else, snake is heading up/down, check left/right of joystick
			if (ps2_x < ((0xFFF * 25) / 100)){
				yDir = 0;
				xDir = -1;	// Go Left
			}else if (ps2_x > ((0xFFF * 75) / 100)){
				yDir = 0;
				xDir = 1;		// Go Right
			}
		}
				
		// Move snake
		headX += xDir;
		headY += yDir;
		nextGrid[headX][headY] = length - 1;
		
		// Copy and decrement all numbers of grid
		copyGrid();
		decGrid();
		
		// If game isn't over, redraw the grid
		if (userDone == 0)
			drawGrid();
		
		// wait for 500mS
		while(systick < 5){
			if (Alert_Timer0A){ // Every .5mS
				Alert_Timer0A = false;
				
				// Get ADC Values for the PS2 Joystick
				get_adc_values(ADC0_BASE, &ps2_x, &ps2_y);
				
				tick_1mS = !tick_1mS;
				tick_5Sec = (tick_5Sec + 1) % 10000;
				if (tick_5Sec == 0){
					
					// Print wireless packet stats every 5Sec
					printf("Total Packets Sent: %d\n\r", packetsSent);
					printf("Total Packets Received: %d\n\r", packetsRec);
					printf("Total Packets Dropped: %d\n\r", packetsDrop);
				}
				if (tick_1mS){
					Active_Column = (Active_Column + 1) % 5;
					ledMatrixWriteData(IO_EXPANDER_I2C_BASE, Active_Column, LedNumbers[opponentScore & 0xF][Active_Column]);
				}
			}
			if (Alert_100mS){ // Every 100mS
				Alert_100mS = false;
				systick++;
			}
		}
		systick = 0;
		
		// Send Score and whether or not you have finished the game. If opponent
		// receives 0x1X, they know you have finished the game, where X is your score
		wireless_send_32(false, false, ((userDone << 4) | score));
		
		// Increment Packets Sent
		packetsSent++;
	}
	
//*****************************************************************************
// Game loop done, Game over
//*****************************************************************************
	// Save the high score to the EEPROM
	save_high_score();
	
	status = wireless_get_32(false, &opponentScore);
		if (status == NRF24L01_RX_SUCCESS){
			// Increment Packets Received
			packetsRec++;
			
			// Reload Watchdog timer
			WATCHDOG0->LOAD = 500000000;
		}
		else
			packetsDrop++;
	
	// Draw wait image if opponent still playing
	if ((opponentScore & (1 << 4)) == 0)
		draw_image(wait_image);		// Waiting for opponent to finish
	
	// wait for 1Sec
	while(systick < 10){
		if (Alert_100mS){
			Alert_100mS = false;
			systick++;
		}
	}
	systick = 0;
	
	// While opponent is still playing
	while((opponentScore & (1 << 4)) == 0){
		// Send Score and whether or not you have finished the game. If opponent
		// receives 0x1X, they know you have finished the game, where X is your score
		wireless_send_32(false, false, ((userDone << 4) | score));
		
		// Increment Packets Sent
		packetsSent++;
		
		// Wait 500mS and continue to output opponent score on LEDBAR
		while(systick < 1000){
			if (Alert_Timer0A){ // Occurs every .5mS
				Alert_Timer0A = false;
				tick_5Sec = (tick_5Sec + 1) % 10000;
				if (tick_5Sec == 0){ // Occurs every 5 Sec
					// Print wireless packet stats every 5Sec
					printf("Total Packets Sent: %d\n\r", packetsSent);
					printf("Total Packets Received: %d\n\r", packetsRec);
					printf("Total Packets Dropped: %d\n\r", packetsDrop);
				}
				tick_1mS = !tick_1mS;
				if (tick_1mS){ // Occurs every 1mS
					// Update LEDBAR data with opponent score
					Active_Column = (Active_Column + 1) % 5;
					ledMatrixWriteData(IO_EXPANDER_I2C_BASE, Active_Column, LedNumbers[opponentScore & 0xF][Active_Column]);
				}
				systick++;
			}
		}
		systick = 0;
	}
	
	// Turn off the LEDBAR
	ledMatrixClear(IO_EXPANDER_I2C_BASE);
	
	if(!win){ // If you have not reached the max score..
		if (score > (opponentScore & 0xF))	// ..but you have beat your opponent, you win
			draw_image(win_image);		// Congratulations
		else
			draw_image(lose_image);		// Otherwise, you lose. (Draws count as a lose)
	}
	
	// wait for 3Sec
	while(systick < 30){
		if (Alert_100mS){
			Alert_100mS = false;
			systick++;
		}
	}
	
	// Clear the lcd
	dogs102_clear();
}
