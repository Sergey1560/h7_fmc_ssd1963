#include "fmc.h"

void fmc_init(void){

    fmc_gpio_init();
    //Источник тактирования
    //PLL2R
    RCC->D1CCIPR &= ~(RCC_D1CCIPR_FMCSEL);
    RCC->D1CCIPR |= (FMC_CLK_SRC << RCC_D1CCIPR_FMCSEL_Pos);

    //Тактирование FMC
    RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;

    RCC->AHB3RSTR |= RCC_AHB3RSTR_FMCRST;
    __NOP();
    RCC->AHB3RSTR &= ~RCC_AHB3RSTR_FMCRST;


    FMC_Bank1->BTCR[0] = FMC_BCR1_WFDIS|\
                         FMC_BCR1_MBKEN|\
                         FMC_BCR1_WREN|\
                         FMC_BCR1_FACCEN|\
                         FMC_BCR1_MTYP_1|\
                         FMC_BCR1_MWID_0;

#ifdef FMC_REMAP_BANK     
    FMC_Bank1->BTCR[0] |= FMC_BCR1_BMAP_0;
#endif

    FMC_Bank1->BTCR[1] =  (FSMC_DATA_Lat << FMC_BTR1_DATLAT_Pos)|\
                          (FSMC_CLK_DIV << FMC_BTR1_CLKDIV_Pos)|\
                          (FSMC_DATA_SETUP << FMC_BTR1_DATAST_Pos)|\
                          (FSMC_ADDR_HOLD << FMC_BTR1_ADDHLD_Pos)|\
                          (FSMC_ADDR_SETUP << FMC_BTR1_ADDSET_Pos);
    
    FMC_Bank1->BTCR[0] |= FMC_BCR1_FMCEN;		
}


void fmc_gpio_init(void){
    GPIO_TypeDef *PORT;

    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN|RCC_AHB4ENR_GPIOEEN|RCC_AHB4ENR_GPIOCEN;

    /****************************************** FMC GPIOD **************************************/
    /* 0,1,4,5,8,9,10,11,14,15 */
    PORT=GPIOD;
    
    PORT->MODER &= ~(GPIO_MODER_MODER0|GPIO_MODER_MODER1|GPIO_MODER_MODER4|GPIO_MODER_MODER5|GPIO_MODER_MODER8|GPIO_MODER_MODER9|GPIO_MODER_MODER10|GPIO_MODER_MODER11|GPIO_MODER_MODER14|GPIO_MODER_MODER15);
    PORT->MODER |= GPIO_MODER_MODER0_1|\
                   GPIO_MODER_MODER1_1|\
                   GPIO_MODER_MODER4_1|\
                   GPIO_MODER_MODER5_1|\
                   GPIO_MODER_MODER8_1|\
                   GPIO_MODER_MODER9_1|\
                   GPIO_MODER_MODER10_1|\
                   GPIO_MODER_MODER11_1|\
                   GPIO_MODER_MODER14_1|\
                   GPIO_MODER_MODER15_1;
                   
    //Push-pull
    PORT->OTYPER &= ~(GPIO_OTYPER_OT_0|\
                       GPIO_OTYPER_OT_1|\
                       GPIO_OTYPER_OT_4|\
                       GPIO_OTYPER_OT_5|\
                       GPIO_OTYPER_OT_8|\
                       GPIO_OTYPER_OT_9|\
                       GPIO_OTYPER_OT_10|\
                       GPIO_OTYPER_OT_11|\
                       GPIO_OTYPER_OT_14|\
                       GPIO_OTYPER_OT_15);

    //AF12 0,1,4,5
    PORT->AFR[0] &= ~(GPIO_AFRL_AFRL0|GPIO_AFRL_AFRL1|GPIO_AFRL_AFRL4|GPIO_AFRL_AFRL5);
    PORT->AFR[0] |= (12 << GPIO_AFRL_AFRL0_Pos)|\
                    (12 << GPIO_AFRL_AFRL1_Pos)|\
                    (12 << GPIO_AFRL_AFRL4_Pos)|
                    (12 << GPIO_AFRL_AFRL5_Pos);


    //AF12 8,9,10,11,14,15
    PORT->AFR[1] &= ~(GPIO_AFRH_AFRH8|GPIO_AFRH_AFRH9|GPIO_AFRH_AFRH10|GPIO_AFRH_AFRH11|GPIO_AFRH_AFRH14|GPIO_AFRH_AFRH15);
    PORT->AFR[1] |= (12 << GPIO_AFRH_AFRH8_Pos)|\
                    (12 << GPIO_AFRH_AFRH9_Pos)|\
                    (12 << GPIO_AFRH_AFRH10_Pos)|\
                    (12 << GPIO_AFRH_AFRH11_Pos)|\
                    (12 << GPIO_AFRH_AFRH14_Pos)|\
                    (12 << GPIO_AFRH_AFRH15_Pos);

    //Hi speed
    PORT->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0|GPIO_OSPEEDER_OSPEEDR1|GPIO_OSPEEDER_OSPEEDR4|GPIO_OSPEEDER_OSPEEDR5|GPIO_OSPEEDER_OSPEEDR8|GPIO_OSPEEDER_OSPEEDR9|GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR11|GPIO_OSPEEDER_OSPEEDR14|GPIO_OSPEEDER_OSPEEDR15);
    PORT->OSPEEDR |= (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR0_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR1_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR4_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR5_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR8_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR9_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR10_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR11_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR14_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR15_Pos);

    //No pull PUPDR=0
    PORT->PUPDR &= ~(GPIO_PUPDR_PUPDR0|GPIO_PUPDR_PUPDR1|GPIO_PUPDR_PUPDR4|GPIO_PUPDR_PUPDR5|GPIO_PUPDR_PUPDR8|GPIO_PUPDR_PUPDR9|GPIO_PUPDR_PUPDR10|GPIO_PUPDR_PUPDR11|GPIO_PUPDR_PUPDR14|GPIO_PUPDR_PUPDR15);



    //*****************************************FMC GPIOE*************************************************************
    //PE 7,8,9,10,11,12,13,14,15
    PORT=GPIOE;

    PORT->MODER &= ~(GPIO_MODER_MODER7|GPIO_MODER_MODER8|GPIO_MODER_MODER9|GPIO_MODER_MODER10|GPIO_MODER_MODER11|GPIO_MODER_MODER12|GPIO_MODER_MODER13|GPIO_MODER_MODER14|GPIO_MODER_MODER15);
    PORT->MODER |= GPIO_MODER_MODER7_1|\
                   GPIO_MODER_MODER8_1|\
                   GPIO_MODER_MODER9_1|\
                   GPIO_MODER_MODER10_1|\
                   GPIO_MODER_MODER11_1|\
                   GPIO_MODER_MODER12_1|\
                   GPIO_MODER_MODER13_1|\
                   GPIO_MODER_MODER14_1|\
                   GPIO_MODER_MODER15_1;
    //Push-pull
    PORT->OTYPER &= ~(GPIO_OTYPER_OT_7|\
                     GPIO_OTYPER_OT_8|\
                     GPIO_OTYPER_OT_9|\
                     GPIO_OTYPER_OT_10|\
                     GPIO_OTYPER_OT_11|\
                     GPIO_OTYPER_OT_12|\
                     GPIO_OTYPER_OT_13|\
                     GPIO_OTYPER_OT_14|\
                     GPIO_OTYPER_OT_15);

    //AF12 7
    PORT->AFR[0] &= ~(GPIO_AFRL_AFRL7);
    PORT->AFR[0] |= (12 << GPIO_AFRL_AFRL7_Pos);
    //AF 8,9,10,11,12,13,14,15
    PORT->AFR[1] &= ~(GPIO_AFRH_AFRH8|GPIO_AFRH_AFRH9|GPIO_AFRH_AFRH10|GPIO_AFRH_AFRH11|GPIO_AFRH_AFRH12|GPIO_AFRH_AFRH13|GPIO_AFRH_AFRH14|GPIO_AFRH_AFRH15);
    PORT->AFR[1] |= (12 << GPIO_AFRH_AFRH8_Pos)|\
                    (12 << GPIO_AFRH_AFRH9_Pos)|\
                    (12 << GPIO_AFRH_AFRH10_Pos)|\
                    (12 << GPIO_AFRH_AFRH11_Pos)|\
                    (12 << GPIO_AFRH_AFRH12_Pos)|\
                    (12 << GPIO_AFRH_AFRH13_Pos)|\
                    (12 << GPIO_AFRH_AFRH14_Pos)|\
                    (12 << GPIO_AFRH_AFRH15_Pos);
    
    PORT->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR7|GPIO_OSPEEDER_OSPEEDR8|GPIO_OSPEEDER_OSPEEDR9|GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR11|GPIO_OSPEEDER_OSPEEDR12|GPIO_OSPEEDER_OSPEEDR13|GPIO_OSPEEDER_OSPEEDR14|GPIO_OSPEEDER_OSPEEDR15);
    PORT->OSPEEDR |= (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR7_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR8_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR9_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR10_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR11_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR12_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR13_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR14_Pos)|\
                     (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR15_Pos);

    //No pull 
    PORT->PUPDR &= ~(GPIO_PUPDR_PUPDR7|GPIO_PUPDR_PUPDR8|GPIO_PUPDR_PUPDR9|GPIO_PUPDR_PUPDR10|GPIO_PUPDR_PUPDR11|GPIO_PUPDR_PUPDR12|GPIO_PUPDR_PUPDR13|GPIO_PUPDR_PUPDR14|GPIO_PUPDR_PUPDR15);

    //*****************************************FMC GPIOC*************************************************************    
    //PC 7
    PORT=GPIOC;

    PORT->MODER &= ~(GPIO_MODER_MODER7);
    PORT->MODER |= GPIO_MODER_MODER7_1;
    
    //Push-pull
    PORT->OTYPER &= ~(GPIO_OTYPER_OT_7);
    
    //AF12 7
    PORT->AFR[0] &= ~(GPIO_AFRL_AFRL7);
    PORT->AFR[0] |= (9 << GPIO_AFRL_AFRL7_Pos);

    PORT->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR7);
    PORT->OSPEEDR |= (FSMC_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR7_Pos);

    //No pull 
    PORT->PUPDR &= ~(GPIO_PUPDR_PUPDR7);

    //****************************************Misc

    //** LCD Reset PD12
    PORT=GPIOD;
    PORT->MODER &= ~GPIO_MODER_MODER12;
    PORT->MODER |= GPIO_MODER_MODER12_0;  //Out
    PORT->PUPDR &= ~GPIO_PUPDR_PUPDR12;
    LCD_RST_ON;

}