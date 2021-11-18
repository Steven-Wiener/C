//*****************************************************************************
// TITLE:			THE GAME OF SNAKE
// 
// NAME:		    Steven Wiener, Jon Butler, Simon Josephson
// LAB SECTION:		301
// LAB DAY:			Tuesday
//
// CLASS:			ECE 210
// DATE: 			FALL 2012
// 
// DESCRIPTION:		Our project simulates the game of snake. The user presses PB3 to start
// 			the game, then once the game begins, the user can hold PB2 to rotate the snake 
// 			CCW, or PB1 to rotate the snake CW. On a hit with the ball, the snake will lengthen,
// 			the LED will blink with a custom color controlled by the Potentiometers (P0 for R,
// 			P1 for G, and P2 for B) and the score will update and output on the LEDBAR. 
// 			On a collision with the wall or
// 			another part of the snake, the screen will display a crash and the user will be able
// 			to broadcast the score to another board that has played the game, by changing the 
// 			destination address on the switches, pressing PB1, changing the source address, 
// 			and pressing PB2.
//*****************************************************************************
#include <math.h>

/* Initialization Functions */
void	LEDBARInit(void);
void 	DIPSWInit(void);
void	PBSwInit(void);	
void 	RGB_LEDInit(void);
void 	sysTickInit(void);
void	drawGrid();
void	copyGrid();
void	decGrid();
int		rand();
void	newBall();
void	PWM_Init(void);
void	srand(unsigned int seed);

char* 	convert(int baudotCode);
void	setDutyCycle(char color, int duration);
void	potentiometersInit(void);
int		readPotentiometer0(void);
int		readPotentiometer1(void);
int		readPotentiometer2(void);
/* Inputs, outputs, and wait timers */	
void    RIT128x96x4Clear(void); 
void	RIT128x96x4Init(int freq);
void    RIT128x96x4StringDraw(const char* letter, int xx, int yy, int intensity);
int 	read_PBSwitchNum(int SwitchNumber);
int		read_Switches(void);
void    LEDBAROutput(int value);
void    sysTickWait1mS(int waitTime);
/* Xbee Functions */
void 	Xbee_ConfigureAddresses(int destination, int ownAddr);
void 	Xbee_Send(int message);
int		Xbee_Receive(void);
//*****************************************************************************
//
// Main Program:
//
//*****************************************************************************
int maxX = 21;
int maxY = 12;
int length = 5;
int score;
int xDir = 1;
int yDir = 0;
int headX = 4;
int headY = 5;
int ballX;
int ballY;
int isDone = 0;
int currGrid[21][12];
int nextGrid[21][12];
int baudotCodes[12];
int win = 0;

int
main(void)
{
	//Initializing the variables used in the lab.
	int PB1,PB2,PB3;			//Pushbutton variables								
	int	destAddress;			//Destination and source addresses for the Xbee
	int ownAddress;
	int messageToSend = 0x0;	//Variables for containing the messages sent 
	int receivedMessage;		//and recieved
	int i;
	int j;
	int x;

	//Initializing the LEDBAR, RGB LED, DIPSwitches and Pushbuttons, and a wait timer
	LEDBARInit();
	DIPSWInit();
	PBSwInit();
	potentiometersInit();
	RGB_LEDInit();
	sysTickInit();
	RIT128x96x4Init(1000000);
	
	/* Main Part of the Program starts here */
	baudotCodes[0] = 1; // o
	baudotCodes[1] = 2; // x
	baudotCodes[2] = 3; // X
	baudotCodes[3] = 4; // *
	baudotCodes[4] = 5; // ' '
	baudotCodes[5] = 6; // Y
	baudotCodes[6] = 7; // o
	baudotCodes[7] = 8; // u
	baudotCodes[8] = 9; // W
	baudotCodes[9] = 10; // i
	baudotCodes[10] = 11; // n
	baudotCodes[11] = 12; // !
	
	//seed the RNG
	srand(readPotentiometer0()<< 11 + readPotentiometer1()<< 6 + readPotentiometer2());
		
	// Initialize the LED and blink twice with potentiometer values
	PWM_Init();
	for (i = 0; i < 2; i++) {
		setDutyCycle('R', ((readPotentiometer0() * 100) / 1024));
		setDutyCycle('G', ((readPotentiometer1() * 100) / 1024));
		setDutyCycle('B', ((readPotentiometer2() * 100) / 1024));
		sysTickWait1mS(150);
		setDutyCycle('R',0);
		setDutyCycle('G',0);
		setDutyCycle('B',0);
		sysTickWait1mS(150);
	}
	
	
	//	Initialize board with blanks
	
	for (i = 0; i < maxX; i++) {
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
	
	//	Draw Grid
	drawGrid();
	
	//	Wait for user to hit PB3 to start game
	PB3 = read_PBSwitchNum(3);
	while (PB3 != 0x0)
		PB3 = read_PBSwitchNum(3);
		
	//	MAIN LOOP
	while(isDone == 0) {
		// Initialize LEDs and read Pushbuttons
		PWM_Init();
		sysTickWait1mS(150);
		PB1 = read_PBSwitchNum(1);
		PB2 = read_PBSwitchNum(2);
		setDutyCycle('R', 0);
		setDutyCycle('G', 0);
		setDutyCycle('B', 0);
		
		//rotate snake clockwise if PB1 is pressed
		if (PB1 != 0x1){
			PB2 = 0x1;
			if (xDir == 1) {
				xDir = 0;
				yDir = 1;
			} else if (xDir == -1) {
				xDir = 0;
				yDir = -1;
			} else if (yDir == 1) {
				yDir = 0;
				xDir = -1;
			} else if (yDir == -1) {
				yDir = 0;
				xDir = 1;
			}
			PB1 = 0x1;
		}

		//rotate snake ccw if PB2 pressed	
		if (PB2 != 0x1) {
			if (xDir == 1){
				xDir = 0;
				yDir = -1;
			} else if (xDir == -1){
				xDir = 0;
				yDir = 1;
			} else if (yDir == 1) {
				yDir = 0;
				xDir = 1;
			} else if (yDir == -1){
				yDir = 0;
				xDir = -1;
			}
			PB2 = 0x1;
		}
		
		// Move snake
		headX += xDir;
		headY += yDir;
		nextGrid[headX][headY] = length - 1;
		
		// Copy and decriment all numbers of grid
		copyGrid();
		decGrid();
		// Output the score
		LEDBAROutput(score);
		// If game isn't over, redraw the grid
		if (isDone == 0)
			drawGrid();
	}
	sysTickWait1mS(500);
	
	// If the user has achieved the max score, blink the LED four times
	if (win == 1) {
		for (i = 0; i < 4; i++) {
			setDutyCycle('R', ((readPotentiometer0() * 100) / 1024));
			setDutyCycle('G', ((readPotentiometer1() * 100) / 1024));
			setDutyCycle('B', ((readPotentiometer2() * 100) / 1024));
			sysTickWait1mS(150);
			setDutyCycle('R',0);
			setDutyCycle('G',0);
			setDutyCycle('B',0);
			sysTickWait1mS(150);
		}
		setDutyCycle('R',0);
		setDutyCycle('G',0);
		setDutyCycle('B',0);
		// Clear the board bit by bit
		for (i = 0; i < maxX; i++) {
			for (j = 0; j < maxY; j++) {
				RIT128x96x4StringDraw(convert(baudotCodes[4]), i*6, j*8, 15);
				sysTickWait1mS(5);
			}
		}
		// Draw "You Win!" on the board
		RIT128x96x4StringDraw(convert(baudotCodes[5]), 3*6, 5*8, 15);
		RIT128x96x4StringDraw(convert(baudotCodes[6]), 4*6, 5*8, 15);
		RIT128x96x4StringDraw(convert(baudotCodes[7]), 5*6, 5*8, 15);
		RIT128x96x4StringDraw(convert(baudotCodes[4]), 6*6, 5*8, 15);
		RIT128x96x4StringDraw(convert(baudotCodes[8]), 7*6, 5*8, 15);
		RIT128x96x4StringDraw(convert(baudotCodes[9]), 8*6, 5*8, 15);
		RIT128x96x4StringDraw(convert(baudotCodes[10]), 9*6, 5*8, 15);
		RIT128x96x4StringDraw(convert(baudotCodes[11]), 10*6, 5*8, 15);
		sysTickWait1mS(5000);
	} else { // On a lose, clear the grid after filling with random characters
		for (i = 0; i < maxX; i++) {
			for (j = 0; j < maxY; j++)	{
				x = rand() % 5;
				RIT128x96x4StringDraw(convert(baudotCodes[x]), i*6, j*8, 15);
				sysTickWait1mS(5);
			}
		}
	}
	
	sysTickWait1mS(500);
	
	// Clear the grid and the red LED
	RIT128x96x4Clear();
	setDutyCycle('R',0);
		
	// XBEE Stuff
	
	//First the	destination address should be set.
	PB1 = read_PBSwitchNum(1);
	while (PB1 != 0x0)
		PB1 = read_PBSwitchNum(1); //Waits until PB1 has been pressed.
	destAddress = read_Switches();

	//LED Sequence lets us know we have recieved the destination properly.
	setDutyCycle('G',0);
	setDutyCycle('R',100);
	sysTickWait1mS(150);
	setDutyCycle('R',0);

	//Next, the source (for this Xbee) address should be set.
	PB2 = read_PBSwitchNum(2);
	while (PB2 != 0x0)
		PB2 = read_PBSwitchNum(2); //Waits until PB2 has been pressed.
	ownAddress = read_Switches();

	//LED Sequence lets us know we have recieved the source properly.
	setDutyCycle('B',100);
	sysTickWait1mS(50);
	setDutyCycle('B',0);

	//Time to set the Xbee up.
	Xbee_ConfigureAddresses(destAddress, ownAddress);

   	//LED Sequence lets us know we have configured the Xbee 
	setDutyCycle('G',100);
	sysTickWait1mS(150);
	setDutyCycle('G',0);
	sysTickWait1mS(150);
	setDutyCycle('G',100);
	sysTickWait1mS(150);
	setDutyCycle('G',0);
	sysTickWait1mS(150);

	/* Main Loop */
	while(1) {
		// Set the message to send
		messageToSend = score;

		//Xbee sends, then recieves data.
		Xbee_Send(messageToSend);
		sysTickWait1mS(10);
		receivedMessage = Xbee_Receive();
		
		//We check that data is valid (0x5A5 is	a data invalid flag), and output
		while(receivedMessage == 0x5A5)
			receivedMessage = Xbee_Receive();
		LEDBAROutput(receivedMessage);		
	}
}

void drawGrid() {
	// Draw the grid based on integer values
		int i, j;
		for (i = 0; i < maxX; i++)
			for (j = 0; j < maxY; j++)
				if (currGrid[i][j] == -2)
					RIT128x96x4StringDraw(convert(baudotCodes[0]), i*6, j*8, 15); // o
				else if (currGrid[i][j] == length - 1){
					RIT128x96x4StringDraw(convert(baudotCodes[2]), i*6, j*8, 15); // X
				else if (currGrid[i][j] == 0){
					RIT128x96x4StringDraw(convert(baudotCodes[3]), i*6, j*8, 15); // *
				else if (currGrid[i][j] > 0){
					RIT128x96x4StringDraw(convert(baudotCodes[1]), i*6, j*8, 15); // x
				else if (currGrid[i][j] == -1) {
					RIT128x96x4StringDraw(convert(baudotCodes[4]), i*6, j*8, 15); // ' '
}

void newBall() {
	// Set ball x and y to random values
	ballX = rand() % maxX;
	ballY = rand() % maxY;
	// If these random values are on the snake, re-generate new random values
	if (currGrid[ballX][ballY] < 0 || nextGrid[ballX][ballY] < 0) {
		nextGrid[ballX][ballY] = -2;
		return;
	} else {
		newBall();
	}
}

void copyGrid()
{
	int i, j;
	// Check for wall collision / body collision
	if ((0 > headX || headX > maxX - 1 || 0 > headY || headY > maxY - 1) || (currGrid[headX][headY] > 0)) {
		isDone = 1;
		setDutyCycle('R',100);
		return;
	}
	
	// Check for ball collision
	if (currGrid[headX][headY] == -2) {
		PWM_Init();
		// Increment score and length, generate new ball
		score++;
		length++;
		newBall();
		// If score is at max, signal win
		if (score > 30)	{
			win = 1;
			isDone = 1;
		}
		// Blink LED once signalling score
		setDutyCycle('R',((readPotentiometer0()*100)/1024));
		setDutyCycle('G',((readPotentiometer1()*100)/1024));
		setDutyCycle('B',((readPotentiometer2()*100)/1024));
	}
	
	// Copy next grid to current grid
	for (i = 0; i < maxX; i++)
		for (j = 0; j < maxY; j++)
			currGrid[i][j] = nextGrid[i][j];
}

// decrements every segment of snake by 1
void decGrid() {
	int i, j;
	for (i = 0; i < maxX; i++)
		for (j = 0; j < maxY; j++)
			if (nextGrid[i][j] > -1)
				nextGrid[i][j] += -1;
}

char* convert(int baudotCode) {
	switch(baudotCode) {
		case(1): return "o";
		case(2): return "x";
		case(3): return "X";
		case(4): return "*";
		case(5): return " ";
		case(6): return "Y";
		case(7): return "o";
		case(8): return "u";
		case(9): return "W";
		case(10): return "i";
		case(11): return "n";
		case(12): return "!";
	}
	return " ";
}
