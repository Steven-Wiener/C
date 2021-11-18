#ifndef __BOARD_UTIL_H__
#define __BOARD_UTIL_H__

#include <stdint.h>
#include "TM4C123.h"
#include "gpioPort.h"
#include "timer0.h"
#include "adc.h"

void DisableInterrupts(void);
void EnableInterrupts(void) ;

#define   PUSH_BUTTON_GPIO_BASE   GPIOF_BASE
#define   PUSH_BUTTON_GPIO_PERIPH GPIOF
#define   PUSH_BUTTON_UP_PIN      (1 << 1)
#define   PUSH_BUTTON_DOWN_PIN    (1 << 4)

#define   ADC_GPIO_BASE           GPIOE_BASE
#define   ADC_GPIO_X_PIN           (1 << 2)
#define   ADC_GPIO_Y_PIN           (1 << 3)
#define   ADC_GPIO_POT_PIN         (1 << 1)

#define   DAC_GPIO_BASE   GPIOB_BASE
#define   DAC_GPIO_PERIPH GPIOB
#define   DAC_GPIO_0      (1 << 0)
#define   DAC_GPIO_1      (1 << 1)
#define   DAC_GPIO_2      (1 << 2)
#define   DAC_GPIO_3      (1 << 3)
#define   DAC_GPIO_4      (1 << 4)
#define   DAC_GPIO_5      (1 << 5)

typedef struct Freq_Gen Freq_Gen;

struct Freq_Gen 
{
    uint16_t frequency;
    uint16_t count_25_on;
    uint16_t count_25_off;
    uint16_t count_33_on;
    uint16_t count_33_off;
    uint16_t count_50_on;
    uint16_t count_50_off;
};


void peripheral_init(void);

#endif
