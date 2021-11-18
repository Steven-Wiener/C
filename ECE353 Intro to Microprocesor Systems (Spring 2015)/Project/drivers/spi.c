#include "spi.h"
#include "sysctrl.h"

/****************************************************************************
 * This routine transmits a character out the SPI1 port.
 ****************************************************************************/
bool spiVerifyBaseAddr(uint32_t base){
  if ( base == SSI0_BASE ||
        base == SSI1_BASE ||
        base == SSI2_BASE ||
        base == SSI3_BASE
  )
  {
    return true;
  }
  else
  {
    return false;
  }
}

//*****************************************************************************
// Configure the given SPI peripheral
//*****************************************************************************
bool initialize_spi( uint32_t base_addr, uint8_t spi_mode, uint32_t CPSDVSR){
    SSI0_Type *mySSI = (SSI0_Type *)base_addr;
  
    // Validate that a correct base address has been passed
    // Turn on the Clock Gating Register
    switch (base_addr) 
    {
      case SSI0_BASE :
      {
          SYSCTL->RCGCSSI |= SYSCTL_RCGCSSI_R0;
          while ((SYSCTL->PRSSI & SYSCTL_PRSSI_R0) == 0){}    /* wait until SSI is ready */
          break;
      }
      case SSI1_BASE :
      {
          SYSCTL->RCGCSSI |= SYSCTL_RCGCSSI_R1;
          while ((SYSCTL->PRSSI & SYSCTL_PRSSI_R1) == 0){}    /* wait until SSI is ready */
          break;
      }
      case SSI2_BASE :
      {
          SYSCTL->RCGCSSI |= SYSCTL_RCGCSSI_R2;
          while ((SYSCTL->PRSSI & SYSCTL_PRSSI_R2) == 0){}    /* wait until SSI is ready */
          break;
      }
      case SSI3_BASE :
      {
          SYSCTL->RCGCSSI |= SYSCTL_RCGCSSI_R3;
          while ((SYSCTL->PRSSI & SYSCTL_PRSSI_R3) == 0){}    /* wait until SSI is ready */
          break;
      }
      default:
      {
          return false;
      }
    }
    
    // Disable the SSI interface (Set entire register to 0).
    mySSI->CR1 =0;

    // Assume that we hvae a 50MHz clock
    // FSSIClk = FSysClk / (CPSDVSR * (1 + SCR))
		mySSI->CPSR = (50000000 / CPSDVSR);
		mySSI->CR0 &= ~SSI_CR0_SCR_M;

		// Configure SPI control0 for freescale format, data width of 8 bits
		mySSI->CR0 = (SSI_CR0_DSS_8 | SSI_CR0_FRF_MOTO);
    
		switch (spi_mode)
    {
      case 0:
      {
        mySSI->CR0 |=  ((SSI_SPO_LOW<<7) | (SSI_SPH_FIRST<<6));
        break;
      }
      case 1:
      {
        mySSI->CR0 |=  ((SSI_SPO_LOW<<6) | (SSI_SPH_SECOND<<7));
        break;
      }
      case 2:
      {
        mySSI->CR0 |=  ((SSI_SPO_HIGH<<6) | (SSI_SPH_FIRST<<7));
        break;
      }
      case 3:
      {
        mySSI->CR0 |=  ((SSI_SPO_HIGH<<6) | (SSI_SPH_SECOND<<7));
        break;
      }
      default:
      {
        mySSI->CR0 |=  ((SSI_SPO_LOW<<6) | (SSI_SPH_FIRST<<7));
        break;
      }
    }
    
    //Enable SSI
    mySSI->CR1 |= SSI_CR1_SSE;

  return true;
}

//*****************************************************************************
// Transmits the array of bytes found at txData to the specified SPI peripheral
// The number of bytes transmitted is determined by numBytes.
//
// The data received by the SPI ternimal is placed in an array of bytes 
// starting at the address found at rxData
//*****************************************************************************
void spiTx(uint32_t base, uint8_t *txData, int numBytes, uint8_t *rxData){
  uint8_t count = 0;
	SSI0_Type *mySSI = (SSI0_Type *) base;

  // Disable the SSI interface
  mySSI->CR1 &= ~SSI_CR1_SSE;
    
  // Fill the Transmit FIFO  
  while((mySSI->SR & SSI_SR_TNF)!= 0 && (count < numBytes) )
  {
    // Send out the first byte
    mySSI->DR = *txData; 
    txData++;
    count++;
  }
                  
  //Enable SSI
  mySSI->CR1 |= SSI_CR1_SSE;
  
  for( count = 0; count < numBytes; count++)
  {
    // Wait until the recieve has finished  
    while((mySSI->SR & SSI_SR_RNE)==0){};// wait until response

    // Store the results  
    *rxData =  mySSI->DR;
    rxData++;
  }
}
