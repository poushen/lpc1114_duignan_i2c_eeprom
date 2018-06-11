#ifndef _I2C_H
#define _I2C_H

#include <stdint.h>
#include "lpc111x.h"

enum {
	NORMAL_MODE_FAST_MODE,	// 100kHz / 400kHz
	FAST_MODE_PLUS = 2			// 1MHz
};

#define SPEED_MODE FAST_MODE_PLUS


// Global variable type
typedef struct
{
	uint8_t slave_address;	// slave address																-Mandatory
	uint8_t read_write;			// 0: Write, 1: Read														-Mandatory
	uint16_t buffer_len;		// data buffer length														-Mandatory
	uint16_t data_len;			// current untransmit or unreceive data length
	uint8_t *pdata_buffer;	// data buffer pointer													-Mandatory
	uint8_t *pcurrent;			// current pointer to data buffer content
	int8_t result;					// 0: start, 1: successfully finish, -1: fail
	
	uint8_t *psecond_data_buffer; // second data buffer pointer
	uint16_t second_buffer_len;		// second data buffer length
	int8_t second_buffer;					// use second buffer or not (Y: 1, N: 0)
	
	uint8_t do_ack_polling;		// 0: not, 1: do acknowledge polling for testing the end of internal write cycle
	uint8_t do_random_read;		// 0: not, 1: do randon read (write first, then repeat start and read)
} I2C_Data_TypeDef;


// Prototype declaration
void i2cConfig(void);
void i2cConfig_Init(int8_t speed_mode);
void i2cStart(I2C_Data_TypeDef *_pi2c_data);
void i2cStop(void);

#endif
