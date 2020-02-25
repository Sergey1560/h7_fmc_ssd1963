#ifndef FMC_H
#define FMC_H
#include "common_defs.h"
#include "clocks.h"

/* 
Подключение
STM32 Pin  AF_name          Lcd Pin	    Type*	    Description
GND                         GND	        G	        Ground
VDD                         VCC	        P	        3.3V Power Supply pin
NC                          NC	        -	        No connection
PD11           FSMC_A16     RS	        I       	Data/Command selection
PD5            FSMC_NWE     WR	        I       	Write signal enable, low active
PD4            FSMC_NOE     RD	        I       	Read signal enable, low active
PE11                        DB8	        I       	Data Bus
PE12                        DB9	        I       	Data Bus
PE13                        DB10    	I	        Data Bus
PE14                        DB11	    I	        Data Bus
PE15                        DB12    	I	        Data Bus
PD8                         DB13	    I	        Data Bus
PD9                         DB14	    I	        Data Bus
PD10                        DB15	    I	        Data Bus
PC7(PD7)       FSMC_NE1     CS	        I	        Chip Selection，Low level active
NC                          NC	        -	        No connection
PD12                        RST	        I	        Reset Pin
NC                          NC	        -	        No connection
LCDVDD                      LED	        P	        Backlight(5V)
NC                          NC	        -	        No connection

PD14                        DB0	        I       	Data Bus
PD15                        DB1	        I       	Data Bus
PD0                         DB2	        I       	Data Bus
PD1                         DB3	        I       	Data Bus
PE7                         DB4	        I       	Data Bus
PE8                         DB5	        I       	Data Bus
PE9                         DB6	        I       	Data Bus
PE10                        DB7	        I       	Data Bus
PB10                        T_CLK	    I	        Touch clock
PB11                        T_CS	    I	        Touch chip selection
PB15                        T_DIN	    I	        Touch data input
NC                          T_BUSY	    O	        Touch status
PB14                        T_DOUT	    O	        Touch data output
PD13                        T_IRQ	    O	        Touch interrupt
NC                          SD_SO	    O	        SD MISO
NC                          SD_SCK	    I	        SD SCK
NC                          SD_SI	    I	        SD MOSI
NC                          SD_NSS	    I	        SD NSS
NC                          NC	        -       	No connection
NC                          NC	        -       	No connection
*/      



#define FSMC_DATA_SPEED     S_VH
#define LCD_RST_ON          GPIOD->BSRR = GPIO_BSRR_BR_12
#define LCD_RST_OFF         GPIOD->BSRR = GPIO_BSRR_BS_12

//#define FMC_REMAP_BANK 

#ifdef FMC_REMAP_BANK 
#define LCD_DATA    		    ((uint32_t)0xC0020000)    
#define LCD_REG   		  	    ((uint32_t)0xC0000000)	  
#else
#define LCD_DATA    		    ((uint32_t)0x60020000)    
#define LCD_REG   		  	    ((uint32_t)0x60000000)	  
#endif

/*
Таймиги
*/
#define FSMC_DATA_SETUP 2
#define FSMC_ADDR_SETUP 14

/*
Эти тайминги в данной конфигурации не работают
Нужны только для установки корректного значения в регистре
*/
#define FSMC_DATA_Lat   1
#define FSMC_ADDR_HOLD  1
#define FSMC_CLK_DIV    15

#define DISP_WIDTH  800
#define DISP_HEIGHT 480


void fmc_init(void);
void fmc_gpio_init(void);

#endif