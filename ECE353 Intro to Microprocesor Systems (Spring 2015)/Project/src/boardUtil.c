#include "TM4C123GH6PM.h"
#include "boardUtil.h"
#include "../include/sysctrl.h"

void DisableInterrupts(void){
  __asm {
         CPSID  I
  }
}
void EnableInterrupts(void){
  __asm {
    CPSIE  I
  }
}


//*****************************************************************************
// Configure PA0 and PA1 to be UART pins
//*****************************************************************************
void uart0_config_gpio(void){
   gpio_enable_port(GPIOA_BASE);
   gpio_config_digital_enable( GPIOA_BASE, PA0 | PA1);
   gpio_config_alternate_function( GPIOA_BASE, PA0 | PA1);
   gpio_config_port_control( GPIOA_BASE, GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX);
}

//*****************************************************************************
//*****************************************************************************
void serialDebugInit(void){
	uart0_config_gpio();
	uart0_init_115K();
}

//*****************************************************************************
// Initialize the I2C peripheral
//*****************************************************************************
void i2cInit(void){
  // Configure SCL
	gpio_enable_port(IO_EXPANDER_GPIO_BASE);
  gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN);
  gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN);
  gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN_PCTL);

  
  // Configure SDA 
  gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN);
  gpio_config_open_drain(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN);
  gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN);
  gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN_PCTL);
  
  //  Initialize the I2C peripheral
  initializeI2CMaster(IO_EXPANDER_I2C_BASE);
  
}

//*****************************************************************************
// Initialize the LCD peripheral
//*****************************************************************************
void lcd_init(void){
  // Configure SPI CLK
  gpio_enable_port(LCD_GPIO_BASE);
  gpio_config_digital_enable(LCD_GPIO_BASE, LCD_CLK_PIN);
  gpio_config_alternate_function(LCD_GPIO_BASE, LCD_CLK_PIN);
  gpio_config_port_control(LCD_GPIO_BASE, LCD_CLK_PIN_PCTL);
    
  // Configure SPI CS
  gpio_config_digital_enable(LCD_GPIO_BASE, LCD_CS_PIN);
  gpio_config_alternate_function(LCD_GPIO_BASE, LCD_CS_PIN);
  gpio_config_port_control(LCD_GPIO_BASE, LCD_CS_PIN_PCTL);

  // Configure SPI MOSI
  gpio_config_digital_enable(LCD_GPIO_BASE, LCD_MOSI_PIN);
  gpio_config_alternate_function(LCD_GPIO_BASE, LCD_MOSI_PIN);
  gpio_config_port_control(LCD_GPIO_BASE, LCD_MOSI_PIN_PCTL);
  
  // Configure CD
  gpio_enable_port(GPIO_LCD_CD_BASE);
  gpio_config_digital_enable(GPIO_LCD_CD_BASE,LCD_CD_PIN);
  gpio_config_enable_output(GPIO_LCD_CD_BASE,LCD_CD_PIN);
  
  // Configure RST_N
  gpio_enable_port(GPIO_LCD_RST_N_BASE);
  gpio_config_digital_enable(GPIO_LCD_RST_N_BASE, LCD_RST_N_PIN);
  gpio_config_enable_output(GPIO_LCD_RST_N_BASE, LCD_RST_N_PIN);
 
  // Configure the SPI interface for Mode 3, 25MHz.
  initialize_spi( LCD_SPI_BASE, 3, 25000000);
}

//*****************************************************************************
//*****************************************************************************
void rfInit(void){  
  wireless_set_pin_config(
    RF_SPI_BASE,
    RF_PAYLOAD_SIZE,
    RF_CHANNEL,
    RF_CS_BASE,
    RF_CS_PIN,
    RF_CE_GPIO_BASE,
    RF_CE_PIN
  );
  
  gpio_enable_port(RF_GPIO_BASE);
  
  // Configure SPI CLK
  gpio_config_digital_enable(  RF_GPIO_BASE, RF_CLK_PIN);
  gpio_config_alternate_function(    RF_GPIO_BASE, RF_CLK_PIN);
  gpio_config_port_control(     RF_GPIO_BASE, RF_CLK_PIN_PCTL);
  
  // Configure SPI MISO
  gpio_config_digital_enable(  RF_GPIO_BASE, RF_MISO_PIN);
  gpio_config_alternate_function(    RF_GPIO_BASE, RF_MISO_PIN);
  gpio_config_port_control(     RF_GPIO_BASE, RF_MISO_PIN_PCTL);
  
  // Configure SPI MOSI
  gpio_config_digital_enable(  RF_GPIO_BASE, RF_MOSI_PIN);
  gpio_config_alternate_function(    RF_GPIO_BASE, RF_MOSI_PIN);
  gpio_config_port_control(     RF_GPIO_BASE, RF_MOSI_PIN_PCTL);
  
  // Configure CS to be a normal GPIO pin that is controlled 
  // explicitly by software
  gpio_enable_port(RF_CS_BASE);
  gpio_config_digital_enable(  RF_CS_BASE,RF_CS_PIN);
  gpio_config_enable_output(    RF_CS_BASE,RF_CS_PIN);
  
  // Configure CE Pin as an output
  gpio_enable_port(RF_CE_GPIO_BASE);
  gpio_config_digital_enable(  RF_CE_GPIO_BASE,RF_CE_PIN);
  gpio_config_enable_output(    RF_CE_GPIO_BASE,RF_CE_PIN);

	// Set up Interrupts for GPIOD Pin 7 (Rx from Nordic)
	gpio_enable_port(RF_IRQ_GPIO_BASE);
	gpio_config_digital_enable(  RF_IRQ_GPIO_BASE,RF_IRQ_PIN);
	gpio_config_enable_input(    RF_IRQ_GPIO_BASE,RF_IRQ_PIN);
	
	// Configure interrupt for GPIOD pin 7 (Rx from Nordic)
	GPIOD->IM = (1 << 7); // Pin 7
	NVIC_SetPriority (GPIOD_IRQn, 2);
	NVIC_EnableIRQ(GPIOD_IRQn);	

	// Configure the SPI interface for Mode 0, 5MHz.
  initialize_spi( RF_SPI_BASE, 0, 5000000);
	
  RF_CE_PERIH->DATA |= (1 << 1);
}
