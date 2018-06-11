
#include "led.h"

void ledCfg(void)
{
	// Turn on clock for GPIO, IOCON and ADC
  SYSAHBCLKCTRL |= BIT6 + /* BIT13 + */ BIT16;
	IOCON_PIO0_7 = (0x0) + (0<<3) + (0<<5);
  GPIO0DIR = BIT7; // Make PIO0_7 an output
  GPIO0DATA = 0;   // Turn off PIO0 outputs
}

void ledOn(void)
{
	GPIO0DATA = BIT7; // Turn on PIO0_7
}

void ledOff(void)
{
	GPIO0DATA = 0; // Turn off PIO0_7
}
