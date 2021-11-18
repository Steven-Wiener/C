#ifndef __EEPROM_H__
#define __EEPROM_H__


#include <stdint.h>
#include "i2c.h"





i2c_status_t eeprom_byte_write
( 
  uint32_t  i2c_base,
  uint16_t  eepromDataAddress,
  uint8_t   data
);

i2c_status_t eeprom_byte_read
( 
  uint32_t  i2c_base,
  uint16_t  address,
  uint8_t   *data
);

void write_eeprom(void);

void print_eeprom(bool write);
void save_high_score(void);
//void print_eeprom(uint8_t *field1, uint8_t *field2, uint8_t *field3);

#endif
