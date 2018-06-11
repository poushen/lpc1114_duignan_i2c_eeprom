
#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include "lpc111x.h"

extern int TickCount;

void SysTickCfg(int reload_value);

void delay_ms(int ms);

#endif
