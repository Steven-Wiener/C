#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../src/boardUtil.h"
#include "eeprom.h"
#include "pc_buffer.h"

#define MCP24LC32AT_DEV_ID    0x50

extern uint16_t score;

extern PC_Buffer UART0_Tx_Buffer;

//*****************************************************************************
// Used to determine if the EEPROM is busy writing the last transaction to 
// non-volatile storage
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
// Returns
// I2C_OK is returned one the EEPROM is ready to write the next byte
//*****************************************************************************
static 
i2c_status_t eeprom_wait_for_write( int32_t  i2c_base){
  
  i2c_status_t status;
  
  if( !i2cVerifyBaseAddr(i2c_base) )
  {
    return  I2C_INVALID_BASE;
  }

  // Set the I2C address to be the EEPROM and in Write Mode
  status = i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_WRITE);

  // Poll while the device is busy.  The  MCP24LC32AT will not ACK
  // writing an address while the write has not finished.
  do 
  {
    // The data we send does not matter.  This has been set to 0x00, but could
    // be set to anything
    i2cSendByte( i2c_base, 0x00, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
    
    // Wait for the address to finish transmitting
    while ( I2CMasterBusy(i2c_base)) {};
    
    // If the address was not ACKed, try again.
  } while (I2CMasterAdrAck(i2c_base) == false);

  //return  I2C_OK;
	return status;
}
  
  
//*****************************************************************************
// Writes a single byte of data out to the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being written.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       Data written to the EEPROM.
//
// Returns
// I2C_OK if the byte was written to the EEPROM.
//*****************************************************************************
i2c_status_t eeprom_byte_write
( 
  uint32_t  i2c_base,
  uint16_t  address,
  uint8_t   data
)
{
	//			send a control byte, two address bytes, and the data written.
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // Set the I2C address to be the EEPROM
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_WRITE);
  
  // If the EEPROM is still writing the last byte written, wait
  eeprom_wait_for_write(i2c_base);
  
  //==============================================================
  // Send the address
  //==============================================================
	i2cSendByte(i2c_base, (address >> 8), I2C_MCS_START | I2C_MCS_RUN);
	i2cSendByte(i2c_base, address, I2C_MCS_RUN);
  
  //==============================================================
  // Send the Byte of data to write
  //==============================================================
	i2cSendByte(i2c_base, data, I2C_MCS_RUN | I2C_MCS_STOP);

  return status;
}

//*****************************************************************************
// Reads a single byte of data from the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being read.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       data read from the EEPROM is returned to a uint8_t pointer.
//
// Returns
// I2C_OK if the byte was read from the EEPROM.
//*****************************************************************************
i2c_status_t eeprom_byte_read
( 
  uint32_t  i2c_base,
  uint16_t  address,
  uint8_t   *data
)
{
	//			send a control byte, two address bytes, a second control byte, and then read the data
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};



  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Write Mode
  //==============================================================
  status = i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_WRITE);
  
  // If the EEPROM is still writing the last byte written, wait
  eeprom_wait_for_write(i2c_base);
    

  //==============================================================
  // Send the address
  //==============================================================
	i2cSendByte(i2c_base, (address >> 8), I2C_MCS_START | I2C_MCS_RUN);
	i2cSendByte(i2c_base, address, I2C_MCS_RUN);

  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Read Mode
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_READ);

  //==============================================================
  // Read the data returned by the EEPROM
  //==============================================================
	i2cGetByte(i2c_base, data, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
  
	return status;
}

// Print EEPROM data
// If write = true, also ask user to enter new values
void print_eeprom(bool write){
	// Default values to load if there is no data in EEPROM	
	uint8_t student1[80] = "Steven Wiener";
	uint8_t student2[80] = "Si Jin Lai";
	uint8_t teamNumber[] = "27";
	
	// Fields for temporary storage
	uint8_t field1[80];
	uint8_t field2[80];
	uint8_t field3[2];
	uint8_t field4;
		
	int i = 0;
	
	// Reads data already in EEPROM to fields
	for (i = 0; i < 80; i++){
		eeprom_byte_read(EEPROM_I2C_BASE, i, &field1[i]); // student1
		eeprom_byte_read(EEPROM_I2C_BASE, i + 80, &field2[i]); // student2
	}
	eeprom_byte_read(EEPROM_I2C_BASE, 160, &field3[0]); // Team Number
	eeprom_byte_read(EEPROM_I2C_BASE, 161, &field3[1]);
	eeprom_byte_read(EEPROM_I2C_BASE, 162, &field4); // High score
	
	printf("\n\rCurrent EEPROM values:");
	
	// Default array values
	if (field1 == 0 && field2 == 0 && field3 == 0){ // If nothing in EEPROM, write defaults
		for (i = 0; i < 80; i++){
			eeprom_byte_write(EEPROM_I2C_BASE, i, student1[i]);
			eeprom_byte_write(EEPROM_I2C_BASE, i + 80, student2[i]);
		}
		for (i = 0; i < 2; i++){
			eeprom_byte_write(EEPROM_I2C_BASE, i + 160, teamNumber[i]);
		}
		eeprom_byte_write(EEPROM_I2C_BASE, 162, 0);
		
		printf("\n\rStudent 1: %s", student1);
		printf("\n\rStudent 2: %s", student2);
		printf("\n\rTeam Number: %s", teamNumber);
		
	}else{ // Else, print what's already in EEPROM
		printf("\n\rStudent 1: %s", field1);
		printf("\n\rStudent 2: %s", field2);
		printf("\n\rTeam Number: %s", field3);
	}
	
	if (field4 == 0 || field4 > 0xF){ // If high score isn't valid, set to 0
		printf("\n\rHigh Score: 0\n");
		eeprom_byte_write(EEPROM_I2C_BASE, 162, 0);
	}
	else // Else, print value already in EEPROM
		printf("\n\rHigh Score: %d\n", field4);
	
	if (write){ // If true, user will enter new values
		printf("\n\rEnter New Values:");
		
		// Prompt user for new data
		printf("\n\rStudent 1: ");
		scanf("%79[^\n]", student1);
		printf("Student 2: ");
		scanf("%79[^\n]", student2);
		printf("Team Number: ");
		scanf("%2[^\n]", teamNumber);
		
		// If the user didn't press enter, save new data
		if (student1[0] != 0)
			for (i = 0; i < 80; i++)
				eeprom_byte_write(EEPROM_I2C_BASE, i, student1[i]);
		
		if (student2[0] != 0)
			for (i = 0; i < 80; i++)
				eeprom_byte_write(EEPROM_I2C_BASE, i + 80, student2[i]);
		
		if (teamNumber[0] != 0){
			eeprom_byte_write(EEPROM_I2C_BASE, 160, teamNumber[0]);
			eeprom_byte_write(EEPROM_I2C_BASE, 161, teamNumber[1]);
		}
		
		// Call save high score function
		save_high_score();
		
		// Reset board
		NVIC_SystemReset();
	}
}

void save_high_score(){
	uint8_t prevScore;
	
	// Reads previous score
	eeprom_byte_read(EEPROM_I2C_BASE, 162, &prevScore);
	
	// If prevScore is valid, and new score is higher, save to EEPROM
	if (score > (prevScore & 0xF))
		eeprom_byte_write(EEPROM_I2C_BASE, 162, score);
}
