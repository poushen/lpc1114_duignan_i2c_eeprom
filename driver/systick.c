#include "systick.h"

int TickCount = 0;

void SysTick_Handler()
{
	TickCount++;
}

void SysTickCfg(int reload_value)
{
	SYST_CSR = 0;				// disable SysTick
	SYST_RVR = reload_value - 1; 	// reload value
	SYST_CVR = 0;				// set current value = 0
	SYST_CSR = 0x07;			// enable SysTick, interrupt, using mcu clock
}

void delay_ms(int ms)
{
	int temp = TickCount + ms;
	while (temp > TickCount) {
		__asm ("wfi");
	};
}
