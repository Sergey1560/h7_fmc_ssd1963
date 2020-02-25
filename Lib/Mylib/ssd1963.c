#include "ssd1963.h"

volatile uint16_t RAM_D1 ALGN4 fill_color;

volatile uint16_t __attribute__ ((aligned (4))) dma_mem[DMA_MEM_SIZE];


void LCD_Draw_Symbol(uint16_t x, uint16_t y, uint16_t color, uint16_t backgroud, char data, FONT_INFO* font){
	uint16_t index=0;
	uint32_t arr_offset=0;
	uint8_t byte_num;
	uint16_t len=0;	

	index = data - font->startChar;
	arr_offset = font->charInfo[index].offset;
	
	SetWindows(x,y,x+font->charInfo[index].widthBits,y+font->heightPixels);
	LCD_WriteCmd(0x2C);

	for(uint8_t h=0;h<font->heightPixels;h++){
			for(uint8_t i=0; i < font->charInfo[index].widthBits;i++){
			  byte_num=i/8;
			  if(font->data[arr_offset+byte_num] & (1 << (7-(i - 8*byte_num)) )){
				 dma_mem[len++]=color;
			  }else{
				 dma_mem[len++]=backgroud;
			  };
			};
			 arr_offset+=byte_num;
			 arr_offset++;
			 byte_num=0;
		};
        dma_mem2mem((uint32_t *)&dma_mem,(uint32_t *)LCD_DATA,len, 1);
	};


uint16_t LCD_Draw_String(uint16_t x, uint16_t y, uint16_t color, uint16_t backgroud, char *data, FONT_INFO* font, uint32_t width){
	uint16_t offset=0;
	uint16_t space_x;
	uint16_t index;

  while (*data != 0) 
  {
		if(*data != 0x20){
			if(*data > 128){ //Символ из UTF-8
				index=lookup_utf(*data,*(data+1));			
				LCD_Draw_Symbol(x+offset,y,color,backgroud,index+font->startChar,font);
				data++;
			}else{ //Обычный символ
				index=*data - font->startChar;
				LCD_Draw_Symbol(x+offset,y,color,backgroud,*data,font);
			}
			
			offset+=font->charInfo[index].widthBits;
			space_x=x;
			space_x+=offset;
			LCD_Draw_space(x+offset,y,backgroud,font);
			offset+=font->spacePixels;
			data++;
		}else{
			LCD_fill_area(x+offset,y,x+offset+font->spacePixels*5,y+font->heightPixels,backgroud,FILL_BY_PIXEL);
			offset+=font->spacePixels*5;
			data++;
		};
	}
	
	if(offset < width){
		LCD_fill_area(x+offset,y,x+width+font->spacePixels,y+font->heightPixels,backgroud,FILL_BY_PIXEL);
	};

	return offset;
}

void LCD_Draw_space(uint16_t x, uint16_t y, uint16_t backgroud, FONT_INFO* font){
	LCD_fill_area(x,y,x+font->spacePixels,y+font->heightPixels,backgroud,FILL_BY_PIXEL);
};



void LCD_fill_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t fill_type){
    uint32_t dx,dy;
    
    SetWindows(x1,y1,x2,y2);
    LCD_WriteCmd ( 0x2C );          // SSD1963_WRITE_MEMORY_START

    if(x2 > x1){
        dx=x2-x1;
    }else{
        dx=x1-x2;
    }

    if(y2 > y1){
        dy=y2-y1;
    }else{
        dy=y1-y2;
    }

    if(fill_type == FILL_BY_DMA){
        fill_color = color; //выровненная переменная, для DMA
        dma_mem2mem((uint32_t *)&fill_color,(uint32_t *)LCD_DATA,dx*dy, 0);
    }else{
        for(uint32_t i=0; i < dx*dy; i++){
            LCD_WriteData(color);
        } 
    }
};

void SetWindows(uint16_t StartX,uint16_t StartY,uint16_t EndX,uint16_t EndY)
{
	uint16_t hi, lo;

	// set_column_address
	LCD_WriteCmd ( 0x2A );							// SSD1963_SET_COLUMN_ADDRESS
	
	hi = StartX >> 8;
	lo = StartX & 0x00ff;
	LCD_WriteData ( hi );					// Hi
	LCD_WriteData ( lo );					// Low
	
	hi = (EndX-1) >> 8;
	lo = (EndX-1) & 0x00ff;
	LCD_WriteData ( hi );					// Hi
	LCD_WriteData ( lo );					// Low

	// set_page_addres
	LCD_WriteCmd ( 0x2B );							// SSD1963_SET_PAGE_ADDRESS
	hi = StartY >> 8;
	lo = StartY & 0x00ff;
	
	LCD_WriteData ( hi );					// Hi
	LCD_WriteData ( lo );					// Low
	
	hi = (EndY-0) >> 8;
	lo = (EndY-0) & 0x00ff;
	LCD_WriteData ( hi );					// Hi
	LCD_WriteData ( lo );					// Low
}

void SSD1963_Init (void){
	
	fmc_init();

    LCD_RST_ON;
    Delay_ms(150);
    LCD_RST_OFF;
    Delay_ms(150);

    // Soft reset
    LCD_WriteCmd ( 0x01 );     // software reset
    Delay_ms(100); 

    //LCD_WriteCmd ( 0x28 ); 
    //Set PLL MN
    LCD_WriteCmd ( 0xE2 ); 
    LCD_WriteData( 0x23 );
    LCD_WriteData( 0x02 );
    LCD_WriteData( 0x04 );
            
    //SET PLL
    LCD_WriteCmd ( 0xE0 ); 
    //Enable PLL
    LCD_WriteData( 0x01 );
    Delay_ms(200); 

    LCD_WriteCmd ( 0xE0 ); 
    //Lock PLL
    LCD_WriteData( 0x03 );
    Delay_ms(200); 

    LCD_WriteCmd ( 0x01 );   // software reset
    Delay_ms(200);

    //Set dsip off
    //LCD_WriteCmd ( 0x28 );   
    //Set lshift freq
    LCD_WriteCmd ( 0xE6 );     
    // PLL setting for PCLK, depends on resolution
    LCD_WriteData ( 0x04 );    
    LCD_WriteData ( 0xFF );    
    LCD_WriteData ( 0xFF );

    //Set LCD mode
    LCD_WriteCmd ( 0xB0 );                        
    //P1: A0:A5
    LCD_WriteData ( 0x27 ); 
    LCD_WriteData ( 0x00 ); 
    LCD_WriteData ( (HDP>>8) & 0xFF );  
    LCD_WriteData ( HDP & 0xFF );
    LCD_WriteData ( (VDP>>8) & 0xFF );  
    LCD_WriteData ( VDP & 0xFF );
    LCD_WriteData (0x00); 

    LCD_WriteCmd (0xB4);
    LCD_WriteData ( (HT>>8)& 0xFF );          // Set HT
    LCD_WriteData ( HT & 0xFF );
    LCD_WriteData ( (HPS >> 8) & 0XFF );      // Set HPS
    LCD_WriteData ( HPS & 0xFF );
    LCD_WriteData ( HPW );                    // Set HPW
    LCD_WriteData ( (LPS>>8) & 0XFF );        // Set HPS
    LCD_WriteData ( LPS & 0XFF );
    LCD_WriteData ( 0x00 );

    LCD_WriteCmd ( 0xB6 );                    // Set vertical period
    LCD_WriteData ( (VT>>8) & 0xFF );         // Set VT
    LCD_WriteData ( VT & 0xFF );
    LCD_WriteData ( (VPS>>8) & 0xFF );        // Set VPS
    LCD_WriteData ( VPS & 0xFF );
    LCD_WriteData ( VPW );                    // Set VPW
    LCD_WriteData ( (FPS>>8) & 0xFF );        // Set FPS
    LCD_WriteData ( FPS & 0xFF );

    LCD_WriteCmd ( 0xBA );
    LCD_WriteData( 0x0F );                         // GPIO[3:0] out 1

    LCD_WriteCmd ( 0xB8 );
    LCD_WriteData( 0x07 );                         // 0x07 GPIO3=input, GPIO[2:0]=output
    LCD_WriteData( 0x01 );                         // 0x01 GPIO0 normal

    LCD_WriteCmd ( 0x36 );                         // Set Address mode - rotation
    LCD_WriteData (FlipState|(uint8_t)SSD1963_Hor_DecrDecr);

    LCD_WriteCmd ( 0xBC );        
    LCD_WriteData( 0x40 );      
    LCD_WriteData( 0x90 );
    LCD_WriteData( 0x40 );       
    LCD_WriteData( 0x01 );       

    LCD_WriteCmd ( 0xF0 );                         // pixel data interface
    LCD_WriteData( 0x03 );                         // 03h - RGB565
    //Delay(100);
    LCD_WriteCmd ( 0x29 );                         // display on

    LCD_WriteCmd ( 0xd0 );  
    LCD_WriteData( 0x0d ); 

}
