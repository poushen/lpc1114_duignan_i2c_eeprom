#include "i2c.h"
#include "irq.h"

// Global variable
I2C_Data_TypeDef *pi2c_data;

void i2cConfig(void) {
	i2cConfig_Init(SPEED_MODE);
}

void i2cConfig_Init(int8_t speed_mode) {
	// sec 15.2
	IOCON_PIO0_4 &= ~(0x303);								//clear "FUNC" and "I2CMODE" bits (sec 7.4.11)
	IOCON_PIO0_4 = (speed_mode << 8) | 0x01;	//select I2C function SCL (sec 7.4.11)
	IOCON_PIO0_5 &= ~(0x303);								//clear "FUNC" and "I2CMODE" bits (sec 7.4.12)
	IOCON_PIO0_5 = (speed_mode << 8) | 0x01;	//select I2C function SDA (sec 7.4.12)
	
	SYSAHBCLKCTRL |= (1<<5);	//enable clock to I2C block (sec 3.5.14)
	PRESETCTRL    |= (0x2);		//disable reset to I2C unit (sec 3.5.2)
	
	// sec 15.11
	NVIC_ClearPendingIRQ(I2C_IRQn);
	NVIC_SetPriority(I2C_IRQn, 0);
	NVIC_EnableIRQ(I2C_IRQn);	// Enable I2C Interrupt
	
	// sec 15.8
	I2C0CONSET = (1<<6);	//put I2C unit in master transmit mode (sec 15.8.1 and 15.7.1)
	I2C0CONCLR = 0x2C;		//clear STA, SI, AA to 0
	I2C0SCLH	= 0x16;		//set clk dividers (sec 15.7.5)
	I2C0SCLL	= 0x16;		//set clk dividers (sec 15.7.5)
}

void i2cStart(I2C_Data_TypeDef *_pi2c_data) {
	pi2c_data = _pi2c_data;
	pi2c_data->result = 0;		// i2c transmission start
	I2C0CONSET = (1<<5); //set STA bit to initiate transmission (sec 15.7.1)
}

void i2cStop(void) {
	I2C0CONSET = 0x14;	// set STO, AA
	I2C0CONCLR = 0x08; // clear SI
	
	NVIC_DisableIRQ(I2C_IRQn);	// Disable I2C Interrupt
}

void I2C_IRQHandler(void) {
	unsigned int status = 0;
	
	status = I2C0STAT & 0xF8;
	switch (status) {
		// -------------------
		// Non mode specific
		// -------------------
		case 0x00:	// bus error
			I2C0CONSET = 0x14;	// set STO, AA
			I2C0CONCLR = 0x08; // clear SI
			pi2c_data->result = -1;	// i2c transmission fail
			break;
		
		case 0x08:	// START condition has been transmitted
			I2C0DAT = ((pi2c_data->slave_address << 1) | (pi2c_data->read_write & 0x01));
			I2C0CONSET = 0x04;	// set AA
			I2C0CONCLR = 0x28;	// clear SI, STA (sec 15.7.6, 15.8.1)
			// setup data buffer
			pi2c_data->data_len = pi2c_data->buffer_len;
			pi2c_data->pcurrent = pi2c_data->pdata_buffer;
			break;
		
		case 0x10:	// Repeated START condition has been transmitted 
			I2C0DAT = ((pi2c_data->slave_address << 1) | (pi2c_data->read_write & 0x01));
			I2C0CONSET = 0x04;	// set AA
			I2C0CONCLR = 0x28;	// clear SI, STA (sec 15.7.6, 15.8.1)
			// setup data buffer
			pi2c_data->pcurrent = pi2c_data->psecond_data_buffer;
			pi2c_data->data_len = pi2c_data->second_buffer_len;
			break;
		// ------------------
		// Master Transmitter
		// ------------------	
		case 0x18:
			if (pi2c_data->do_ack_polling) {
				I2C0CONSET = 0x14;	// set STO, AA
				pi2c_data->result = 1;	// internal write cycle stop
			} else {
				I2C0DAT = *pi2c_data->pcurrent;
				I2C0CONSET = 0x04;	// set AA
				pi2c_data->pcurrent++;
			}
			I2C0CONCLR = 0x08;	// clear SI
			break;
		
		case 0x20:
			if (pi2c_data->do_ack_polling) {
				I2C0CONSET = 0x24;	// set STA, AA
			} else {
				I2C0CONSET = 0x14;	// set STO, AA
				pi2c_data->result = -1;	// i2c transmission fail
			}
			I2C0CONCLR = 0x08;	// clear SI		
			break;
		
		case 0x28:
			pi2c_data->data_len--;
			if (pi2c_data->data_len == 0) {
				if (pi2c_data->do_random_read) {
					pi2c_data->read_write = 1;	// read
					I2C0CONSET = 0x24;			// set STA, AA
				} else {
				if (pi2c_data->second_buffer) {
					pi2c_data->pcurrent = pi2c_data->psecond_data_buffer;
					pi2c_data->data_len = pi2c_data->second_buffer_len;
					pi2c_data->second_buffer = 0;
					
					I2C0DAT = *pi2c_data->pcurrent;
					I2C0CONSET = 0x04;	// set AA
					pi2c_data->pcurrent++;
				} else {
					I2C0CONSET = 0x14;	// set STO, AA
					// The end of data transmit
					pi2c_data->result = 1;	// i2c successfully finish
				} }
			} else {
				I2C0DAT = *pi2c_data->pcurrent;
				I2C0CONSET = 0x04;	// set AA
				pi2c_data->pcurrent++;
			}
			I2C0CONCLR = 0x08;	// clear SI
			break;
			
		case 0x30:
			I2C0CONSET = 0x14;	// set STO, AA
			I2C0CONCLR = 0x08;	// clear SI		
			pi2c_data->result = -1;	// i2c transmission fail
			break;
		
		case 0x38:
			I2C0CONSET = 0x24;	// set STA, AA  ??
			I2C0CONCLR = 0x08;	// clear SI
			break;
		// --------------
		// Master Receive 
		// --------------
		case 0x40:
			if (pi2c_data->data_len == 1){
				I2C0CONCLR = 0x0C;	// clear SI, AA
			} else {
				I2C0CONSET = 0x04; // set AA
				I2C0CONCLR = 0x08; // clear SI
			}
			break;
		
		case 0x48:
			I2C0CONSET = 0x14; // set STO, AA
			I2C0CONCLR = 0x08;	// clear SI
			pi2c_data->result = -1;	// i2c transmission fail
			break;
		
		case 0x50:
			*pi2c_data->pcurrent = (int8_t)I2C0DAT;
			pi2c_data->data_len--;
			if (pi2c_data->data_len == 1) {	
				I2C0CONCLR = 0x0C;	// clear SI, AA
			} else {
				I2C0CONSET = 0x04;	// set AA
				I2C0CONCLR = 0x08;	// clear SI
			}
			pi2c_data->pcurrent++;
			break;
			
		case 0x58:
			*pi2c_data->pcurrent = (int8_t)I2C0DAT;
			I2C0CONSET = 0x14;	// set STO, AA
			I2C0CONCLR = 0x08;	// clear SI
			// The end of data receive  
			pi2c_data->result = 1;	// i2c successfully finish
			break;
	}
	
	return;
}
