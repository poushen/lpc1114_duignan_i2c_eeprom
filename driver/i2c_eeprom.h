#ifndef _I2C_EEPROM_H
#define _I2C_EEPROM_H

#include "i2c.h"
#include "lpc111x.h"

#define SLAVE_ADDRESS 0x50					// 7-bit style 
#define PAGE_SIZES 64						// in bytes
#define WRITE_TIME 5						// page write time (ms)

enum {
	ONE_BYTE_ADDRESS = 1,
	TWO_BYTES_ADDRESS,
	THREE_BYTES_ADDRESS
};

// Global Variables
extern int ADDRESS_SIZE;

void i2c_eeprom_write_address(uint8_t slave_address, int address);
void i2c_eeprom_current_read(uint8_t slave_address, uint8_t *data);
void i2c_eeprom_sequential_read(uint8_t slave_address, uint8_t *read_buffer, int buffer_size);
void i2c_eeprom_random_read(uint8_t slave_address, int address, uint8_t *read_buffer, int buffer_size);
void i2c_eeprom_byte_write(uint8_t slave_address, int address, uint8_t data);
void i2c_eeprom_page_write(uint8_t slave_address, int address, uint8_t* data_buffer, uint8_t data_len);
void i2c_eeprom_common_write(uint8_t slave_address, int address, int use_second_buffer, 
	uint8_t* data_buffer, uint8_t data_len, uint8_t ack_polling, uint8_t random_read);
void i2c_eeprom_ack_polling(uint8_t slave_address);

#endif
