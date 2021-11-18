#ifndef __ECE353_INTERRUPTS_H__
#define __ECE353_INTERRUPTS_H__

#include <stdint.h>
#include <stdbool.h>
#include "TM4C123.h"

void SysTick_Handler(void);
void TIMER0A_Handler(void);
void TIMER0B_Handler(void);

#endif
