
#ifndef _UART_H_
#define _UART_H_

#include "lpc111x.h"

void uartCfg(void);

int           uartGetRxDataAvail(void); // Detect new data are received
unsigned char uartGetRxData(void);      // Get received data from Uart

unsigned char uartPutc(unsigned char my_ch);    // Uart character output
unsigned char uartGetc(void);           		// Uart character input

void          uartPuts(unsigned char * mytext);  // Uart string output

void uartPutHex(unsigned int din);
void uartPutDec(unsigned int din);

#endif
