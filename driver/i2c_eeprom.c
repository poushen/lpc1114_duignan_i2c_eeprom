#include "i2c_eeprom.h"
#include <stdio.h>
//#include "uart.h"

// Global variable
I2C_Data_TypeDef i2c_eeprom_data;
int ADDRESS_SIZE = TWO_BYTES_ADDRESS;

void i2c_eeprom_current_read(uint8_t slave_address, uint8_t *data) {
	uint8_t read_buffer[] = {0};
	i2c_eeprom_sequential_read(slave_address, read_buffer, 1);
	*data = read_buffer[0];
}

void i2c_eeprom_sequential_read(uint8_t slave_address, uint8_t *read_buffer, int buffer_size) {
	i2c_eeprom_data.slave_address = slave_address;
	i2c_eeprom_data.read_write = 0x01;				// read
	i2c_eeprom_data.buffer_len = buffer_size;
	i2c_eeprom_data.pdata_buffer = read_buffer;
	i2c_eeprom_data.second_buffer = 0;				// don't use second buffer
	i2c_eeprom_data.do_ack_polling = 0;
	i2c_eeprom_data.do_random_read = 0;
	
	i2cStart(&i2c_eeprom_data);
	while (i2c_eeprom_data.result == 0) { 
		__asm ("nop");
	};  	// wait for i2c complete
}

void i2c_eeprom_write_address(uint8_t slave_address, int address) {
	i2c_eeprom_common_write(slave_address, address, 0, NULL, 0, 0, 0);
}

void i2c_eeprom_random_read(uint8_t slave_address, int address, 
	uint8_t *read_buffer, int buffer_size) {
	i2c_eeprom_common_write(slave_address, address, 0, read_buffer, buffer_size, 0, 1);
}

void i2c_eeprom_byte_write(uint8_t slave_address, int address, uint8_t data) {
	uint8_t buffer[1] = {0};
	buffer[0] = data;
	i2c_eeprom_page_write(slave_address, address, buffer, 1);
}

void i2c_eeprom_page_write(uint8_t slave_address, int address, 
	uint8_t* data_buffer, uint8_t data_len) {
	i2c_eeprom_common_write(slave_address, address, 1, 
		data_buffer, (data_len <= PAGE_SIZES ? data_len : PAGE_SIZES), 0, 0);
}

void i2c_eeprom_ack_polling(uint8_t slave_address) {
	i2c_eeprom_common_write(slave_address, 0, 0, NULL, 0, 1, 0);
}

void i2c_eeprom_common_write(uint8_t slave_address, int address, int use_second_buffer, 
	uint8_t* data_buffer, uint8_t data_len, uint8_t ack_polling, uint8_t random_read) {
	uint8_t mem_address[] = {0, 0, 0};
	
	i2c_eeprom_data.slave_address = slave_address;
	i2c_eeprom_data.read_write = 0x00;									// write
	i2c_eeprom_data.buffer_len = ADDRESS_SIZE;
	i2c_eeprom_data.pdata_buffer = mem_address;
	i2c_eeprom_data.second_buffer = use_second_buffer;	// use second buffer
	i2c_eeprom_data.psecond_data_buffer = data_buffer;
	i2c_eeprom_data.second_buffer_len = data_len;
	i2c_eeprom_data.do_ack_polling = ack_polling;
	i2c_eeprom_data.do_random_read = random_read;
	
	switch (ADDRESS_SIZE) {
		case ONE_BYTE_ADDRESS:
			mem_address[0] = address & 0xFF;
			break;
		case TWO_BYTES_ADDRESS:
			mem_address[0] = (address >> 8) & 0xFF;
			mem_address[1] = address & 0xFF;
			break;
		case THREE_BYTES_ADDRESS:
			mem_address[0] = (address >> 16) & 0xFF;
			mem_address[1] = (address >> 8) & 0xFF;
			mem_address[2] = address & 0xFF;
			break;
	}
	
	//uartPuts("before i2c start\n");
	i2cStart(&i2c_eeprom_data);
	while (i2c_eeprom_data.result == 0) { 
		__asm ("nop");
	};  	// wait for i2c complete
}
