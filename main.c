//
// main.c - Blinking LED example for ARM LPC1114
// Written by Ted Burke (based on code by Frank Duignan)
// Last updated 29-11-2013
//

#include "lpc111x.h"
#include <stdint.h>
#include "led.h"
#include "uart.h"

int main()
{
    ledCfg();
    uartCfg();

    int n;
    while(1)
    {
        ledOn();
        uartPuts("led on\n");
        n=1000000; while(--n);
        ledOff();
        uartPuts("led off\n");
        n=1000000; while(--n);
    }
}
