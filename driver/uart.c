#include "uart.h"

void uartCfg(void)
{
	// UART interface are : PIO1_7 (TXD) and PIO1_6 (RXD)
	// Other UART signals (DTR, DSR, CTS, RTS, RI) are not used

	// Enable clock to IO configuration block 
	// (bit[16] of AHBCLOCK Control register)
	SYSAHBCLKCTRL |= BIT16;

	// PIO1_7 IO output config
	//  bit[5]   - Hysteresis (0=disable, 1 =enable)
	//  bit[4:3] - MODE(0=inactive, 1 =pulldown, 2=pullup, 3=repeater)
	//  bit[2:0] - Function (0 = IO, 1=TXD, 2=CT32B0_MAT1)
	IOCON_PIO1_7 = (0x1) + (0<<3) + (0<<5);	// TXD, no pullup no pulldown, hysteresis disable
	
	// PIO1_6 IO input config
	//  bit[5]   - Hysteresis (0=disable, 1 =enable)
	//  bit[4:3] - MODE(0=inactive, 1 =pulldown, 2=pullup, 3=repeater)
	//  bit[2:0] - Function (0 = IO, 1=RXD, 2=CT32B0_MAT0)
	IOCON_PIO1_6 = (0x1) + (2<<3) + (1<<5);	// RXD, pullup, hysteresis enable

	// Enable clock to UART (bit[12] of AHBCLOCK Control register
	SYSAHBCLKCTRL |= BIT12;
	// UART_PCLK divide ratio = 1
	UARTCLKDIV = 1;

	// UART_PCLK = 48MHz, Baudrate = 38400, divide ratio = 1250
	// Line Control Register
	U0LCR = (1<<7) |   // Enable access to Divisor Latches
			(0<<6) |   // Disable Break Control
			(0<<4) |   // Bit[5:4] parity select (odd, even, sticky-1, sticky-0)
			(0<<3) |   // parity disabled
			(0<<2) |   // 1 stop bit
			(3<<0);    // 8-bit data
	U0DLL = 78;  	// Divisor Latch Least Significant Byte 
					// 48MHz/38400/16 = 78.125
	U0DLM = 0;   	// Divisor Latch Most Significant Byte  : 0
	U0LCR = (0<<7) |   // Disable access to Divisor Latches
			(0<<6) |   // Disable Break Control
			(0<<4) |   // Bit[5:4] parity select (odd, even, sticky-1, sticky-0)
			(0<<3) |   // parity disabled
			(0<<2) |   // 1 stop bit
			(3<<0);    // 8-bit data

	U0FCR = 1; // Enable FIFO
}

// Get received data
__inline unsigned char uartGetRxData(void)
{
  return ((char)U0RBR);
}

// Detect if new received data are available
__inline int  uartGetRxDataAvail(void){
  return (U0LSR & 0x1);
}

// Output a character, with additional formatting for HyperTerminal
unsigned char uartPutc(unsigned char my_ch)
{
  if (my_ch == '\n') {
    while ((U0LSR & (1<<5))==0); 
	  // Wait if Transmit Holding register is not empty
    U0THR = 13;  
	  // Output carriage return (for Windows Hyperterminal)
    }
  while ((U0LSR & (1<<5))==0); 
      // Wait if Transmit Holding register is not empty
  U0THR = my_ch; // write to transmit holding register

  if (my_ch == 13) {
    while ((U0LSR & (1<<5))==0); 
	   // Wait if Transmit Holding register is not empty
    U0THR = 10;  
	   // Output new line (for Windows Hyperterminal)
    }
  return (my_ch);
}

// Get a character from UART, if no data available then wait
unsigned char uartGetc(void)
{
  while (uartGetRxDataAvail()==0); // wait if receive buffer empty
  return ((char)U0RBR);
}

// Uart string output
void uartPuts(unsigned char * mytext)
{
  unsigned char CurrChar;
  CurrChar = *mytext;
  while (CurrChar != (char) 0x0){
    uartPutc(CurrChar);  // Normal data
    mytext++;
    CurrChar = *mytext;
    }
  return;
}

void uartPutHex(unsigned int din)
{
//unsigned int nmask = 0xF0000000U;
unsigned int nmask = 0xF0U;
unsigned int nshift = 4;
unsigned short int data4bit;
  do {
   data4bit = (din & nmask) >> nshift;
   data4bit = data4bit+48; // convert data to ASCII
   if (data4bit>57)	data4bit = data4bit+7;
   uartPutc((char) data4bit);
   nshift = nshift - 4;
   nmask = nmask >> 4;
  } while (nmask!=0);
  return;

}

void uartPutDec(unsigned int din)
{
const unsigned int DecTable[10] = {
  1000000000,100000000,10000000,1000000, 
  100000, 10000, 1000, 100, 10, 1};

int count=0;// digital count
int n;      // calculation for each digital
// Remove preceding zeros
  while ((din < DecTable[count]) ) {count++;}

  while (count<10) {
    n=0;
    while (din >= DecTable[count]) {
      din = din - DecTable[count];
      n++;
      }
    n = n + 48; // convert to ascii 0 to 9
    uartPutc((char) n);
    count++;
  };
  return;
}
