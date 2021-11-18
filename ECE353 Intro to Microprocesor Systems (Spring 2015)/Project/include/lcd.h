#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>
#include "spi.h"

#define NUM_PAGES   8
#define NUM_COLS    102

extern const uint8_t win_image[];
extern const uint8_t lose_image[];

//*****************************************************************************
// Provided initialization code for the LCD.  Found in the data sheet.
//*****************************************************************************
void dogs102_init(void);

//*****************************************************************************
// Set the page address to page 7-0.  The orientation of the 353 carrier card
// puts page 0 at the top of the display and page 7 at the bottom.
//
// See "Set Page Address" in the EADOGS102W-6 data sheet, pg 5.
//
// Make sure to set the command mode correctly!
//*****************************************************************************
void dogs102_set_page(uint8_t   page);

//*****************************************************************************
// There are 102 columns in the display.  Use this function to set which colum
// data will be written to.
//
// See "Set Column Address LSB and MSB" in the EADOGS102W-6 data sheet, pg 5.
// This will require two different SPI transactions.
//
// Make sure to set the command mode correctly!
//*****************************************************************************
void dogs102_set_column(uint8_t   column);

//*****************************************************************************
// When not in command mode, any data written to the LCD is used to determine
// which pixels are turned ON/OFF for the curretnly active page.  A 1 turns a 
// pixel on and a 0 turns the pixel off.
//*****************************************************************************
void dogs102_write_data( uint8_t   data);
  
//*****************************************************************************
// Function that prints out an image.
//*****************************************************************************
void draw_image(const uint8_t imageToDisplay[]);

//*****************************************************************************
// Used to clear the LCD of all pixels
//*****************************************************************************
void dogs102_clear(void);

//*****************************************************************************
// Draws various snake chars to the LCD
//*****************************************************************************
void dogs102_write_snake(uint8_t row, uint16_t index, uint8_t col);




#endif
