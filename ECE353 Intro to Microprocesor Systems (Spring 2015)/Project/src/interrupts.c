#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "TM4C123.h"
#include "boardUtil.h"
#include "timer0.h"
#include "eeprom.h"
#include "io_expander.h"

extern volatile uint8_t Active_Column;
extern volatile bool Alert_100mS;
extern volatile bool Alert_Timer0A;

extern wireless_com_status_t status;
extern uint32_t opponentScore;
extern uint8_t packetsRec;
extern uint8_t packetsDrop;

extern PC_Buffer UART0_Rx_Buffer;
extern PC_Buffer UART0_Tx_Buffer;

void SysTick_Handler(void){
  // Clear the interrupt
  uint32_t val = SysTick->VAL;
  
	// Used to trigger several functions
  Alert_100mS = true;
}

//*****************************************************************************
// Timer0A Interrupt Handler
// Used to time the "ON" portion of the waveform
//*****************************************************************************
void TIMER0A_Handler(void){
	Alert_Timer0A = true;
	TIMER0->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// GPIOE Interrupt Handler (PS2 Z Direction)
// Used to trigger EEPROM
//*****************************************************************************
void GPIOE_Handler(void){
	// Clear the interrupt
	GPIOD->ICR |= GPIO_ICR_GPIO_M;
	
	// Stop Transmit/Receive from wireless
	GPIOD->IM = 0;
	
	// Clear LED matrix
	ledMatrixClear(IO_EXPANDER_I2C_BASE);
	
	// Stop the Watchdog Timer
	SYSCTL->RCGCWD &= ~SYSCTL_RCGCWD_R0;
	
	// Turn the timers off
	TIMER0->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
	
	// Print eeprom data
	print_eeprom(true);
}

//*****************************************************************************
// GPIOD Interrupt Handler
// Used to signal Rx from Nordic
//*****************************************************************************
void GPIOD_Handler(void){
	// Clear the interrupt
	GPIOD->ICR |= GPIO_ICR_GPIO_M;
	
	status = wireless_get_32(false, &opponentScore);
	opponentScore = opponentScore & 0xFF;
		if (status == NRF24L01_RX_SUCCESS){
			// Increment Packets Received
			packetsRec++;
			
			// Reload Watchdog timer
			WATCHDOG0->LOAD = 500000000;
		}
		else
			packetsDrop++;
}

void WDT0_Handler(void){
	while(1){}
}

//*****************************************************************************
// Rx Portion of the UART ISR Handler
//*****************************************************************************
__INLINE static void UART0_Rx_Flow(PC_Buffer *rx_buffer){
  // Remove entries from the RX FIFO until the HW FIFO is empty.
  // Data should be placed in the rx_buffer.  
	while (!(UART0->FR & UART_FR_RXFE)){
		pc_buffer_add(rx_buffer, UART0->DR);
	}

  // Clear the RX interrupts so it can trigger again when the hardware 
  // FIFO becomes full
	UART0->MIS &= ~UART_MIS_RXMIS | ~UART_MIS_RTMIS;
}

//*****************************************************************************
// Tx Portion of the UART ISR Handler
//*****************************************************************************
__INLINE static void UART0_Tx_Flow(PC_Buffer *tx_buffer){
	char c;

	// Check to see if we have any data in the circular queue
	if (!pc_buffer_empty(tx_buffer)){
		// Move data from the circular queue to the hardware FIFO
		// until the hardware FIFO is full or the circular buffer
		// is empty.
		do {
			pc_buffer_remove(tx_buffer, (char *)&c);
			UART0->DR = c;
		// Any data in the hardware FIFO will continue to be transmitted
		}while (!((UART0->FR & UART_FR_TXFF) | pc_buffer_empty(tx_buffer)));
	}
	// Clear the TX interrupt so it can trigger again when the hardware
	// FIFO is empty
	UART0->ICR |= UART_ICR_TXIC;
}
  
//*****************************************************************************
// UART0 Interrupt Service handler
//*****************************************************************************
void UART0_Handler(void){
    uint32_t  status;

    // Check to see if RXMIS or RTMIS is active
    status = UART0->MIS;

    if ( status & (UART_MIS_RXMIS | UART_MIS_RTMIS ) )
    {
       UART0_Rx_Flow(&UART0_Rx_Buffer);
    }

    // Check the TX interrupts
    if ( status & UART_MIS_TXMIS )
    {
      UART0_Tx_Flow(&UART0_Tx_Buffer);
    }
    return;
}
