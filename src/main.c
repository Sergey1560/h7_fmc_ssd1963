#include "main.h"

uint16_t colors[]={YELLOW,BLUE,RED,GREEN};
char *colors_desc[]={"Yellow","Blue","Red","Green"};
volatile uint8_t fps_test_run=0;

/*
Функция не учитывает все множество вариантов тактирования.
Предполагается, что тактируется от HSE, а тактирование FMC от PLL2R
*/
uint32_t get_fmc_clock(void){
	uint32_t div_m2,div_n2,div_r2;
	uint32_t fmc_clock;

	div_m2 = ( (RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM2_Msk) >> RCC_PLLCKSELR_DIVM2_Pos);
	div_n2 = ( (RCC->PLL2DIVR & RCC_PLL2DIVR_N2_Msk) >> RCC_PLL2DIVR_N2_Pos) + 1;
	div_r2 = ( (RCC->PLL2DIVR & RCC_PLL2DIVR_R2_Msk) >> RCC_PLL2DIVR_R2_Pos) + 1;

	fmc_clock = ( (RCC_HSE / div_m2) * div_n2 ) / div_r2;
	return fmc_clock;
}


int main(void){
	uint32_t fps;
	uint32_t color_index;

#ifndef FMC_REMAP_BANK
	MPU_Config();
#endif	

	RCC_init();
	dwt_init(); 
	Delay_Init();
	timer2_init();
	
	SSD1963_Init();
	
	INFO("FMC Clock: %ld",get_fmc_clock());

	//Заполнение всего экрана с паузой, вывод через одиночную запись в экран
	INFO("Test fill screen by pixel:");
	for(uint32_t i=0; i< sizeof(colors)/sizeof(colors[0]); i++){
		LCD_fill_area(0,0,DISP_WIDTH,DISP_HEIGHT,colors[i],FILL_BY_PIXEL);
		INFO("Color: %s",colors_desc[i]);
		Delay_ms(1000);
	}

	//Заполнение всего экрана с паузой, вывод через DMA
	INFO("Test fill screen by dma");
	for(uint32_t i=0; i < sizeof(colors)/sizeof(colors[0]); i++){
		LCD_fill_area(0,0,DISP_WIDTH,DISP_HEIGHT,colors[i],FILL_BY_DMA);
		INFO("Color: %s",colors_desc[i]);
		Delay_ms(1000);
	}

	//Замер FPS
	INFO("Test FPS, fill by pixel:");
	fps=0;
	color_index=0;
	
	timer2_init(); // 5 sec timer
	fps_test_run=1;
	TIM2->CR1 |= TIM_CR1_CEN;
	while(fps_test_run > 0){
		LCD_fill_area(0,0,DISP_WIDTH,DISP_HEIGHT,colors[color_index++],FILL_BY_PIXEL);
		if(color_index == sizeof(colors)/sizeof(colors[0])){
			color_index=0;
		}
		fps++;
	}
	INFO("%d/sec",fps/5);

	//Замер FPS
	INFO("Test FPS, fill by DMA:");
	fps=0;
	color_index=0;

	timer2_init(); // 5 sec timer
	fps_test_run=1;
	__DSB();
	TIM2->CR1 |= TIM_CR1_CEN;
	while(fps_test_run > 0){
		LCD_fill_area(0,0,DISP_WIDTH,DISP_HEIGHT,colors[color_index++],FILL_BY_DMA);
		if(color_index == sizeof(colors)/sizeof(colors[0])){
			color_index=0;
		}
		fps++;
	}
	INFO("%d/sec",fps/5);

	//Очистка экрана
	LCD_fill_area(0,0,DISP_WIDTH,DISP_HEIGHT,MAGENTA,FILL_BY_DMA);

	//Вывод текста, с русскими символами
	LCD_Draw_String(20,100,BLACK,MAGENTA,"Тест экрана SSD1963",(FONT_INFO *)&arialNarrow_20ptFontInfo,0);
	
	//В шрифте только цифры
	LCD_Draw_String(20,200,BLACK,MAGENTA,"1963",(FONT_INFO *)&arial_48ptFontInfo,0);

	while(1){
	};
}

void timer2_init(void){
	RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
	TIM2->PSC = (uint16_t)(TIM2_CLK/10000)-1;
	TIM2->ARR = 50000 - 1; 	
	TIM2->DIER = 0;
	TIM2->EGR |= TIM_EGR_UG;
	__DSB();
	TIM2->SR &= ~(TIM_SR_UIF);
	TIM2->DIER = TIM_DIER_UIE;
	

	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority (TIM2_IRQn, 0);
}


void TIM2_IRQHandler(void)
{
	if(TIM2->SR & TIM_SR_UIF)	TIM2->SR &= ~TIM_SR_UIF; 
	DEBUG("TIM2");
	fps_test_run=0;
	TIM2->CR1 &= ~TIM_CR1_CEN;	
}


