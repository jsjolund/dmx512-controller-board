/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Environment : Atollic TrueSTUDIO(R)
**                STMicroelectronics STM32F4xx Standard Peripherals Library
**
**  Distribution: The file is distributed "as is", without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. This file may only be built (assembled or compiled and linked)
**  using the Atollic TrueSTUDIO(R) product. The use of this file together
**  with other tools than Atollic TrueSTUDIO(R) is not permitted.
**
*****************************************************************************
*/

/*
#include "stm32f4xx.h"

#include "stm32f4xx_adc.h"



volatile uint32_t uhADC1ConvertedValue;

uint32_t GetADCVal(){
    return uhADC1ConvertedValue;
}

void ADCInitWithDMA(void)
{

	// GPIO pins

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef      GPIO_InitStructure;


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;					// APB2 Peripheral clock enable ADC1 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;					// AHB1 peripheral clock enable DMA 2 clock

    //configure ADC
    ADC1->CR2 = ADC_CR2_ADON |                          // ADC on
                ADC_CR2_EXTEN_0 |                       // enable Trigger detection on the rising edge
                ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_0 |   // enable Timer 4 CC4 event
                ADC_CR2_DDS |                           // continious DMA requests are issued
                ADC_CR2_DMA;                            // DMA mode enabled

    // enable scan mode
    // NOTE: in scan mode inuts selected through ADC_SQRx or ADC_JSQRx registers are converted
    ADC1->CR1 = ADC_CR1_SCAN; //| ADC_CR1_EOCIE;

    // sample time register
    // NOTE: These bits are written by software to select the sampling time individually for each channel.
    //		 During sampling cycles, the channel selection bits must remain unchanged.
    // 3 + 12  ADC == 42MHz  ~1Ms
    ADC1->SMPR1 = 0;//ADC_SMPR1_SMP11_1 | ADC_SMPR1_SMP12_1;

    // regular sequence register
    ADC1->SQR1 = (1)<<20;		// sequence lenght set to 2 conversions

    // 2nd conversion in regular sequence
    ADC1->SQR3 = 10<<5;

    // configure DMA
    DMA2_Stream0->NDTR = 1;                 //
    DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)&uhADC1ConvertedValue;

    DMA2_Stream0->CR =  DMA_SxCR_PL_1 |     // priority high
                        DMA_SxCR_MSIZE_0 |  // memory size 16bit
                        DMA_SxCR_PSIZE_0 |  // pheriperial size 16bit
                        DMA_SxCR_MINC |     //
                        DMA_SxCR_CIRC |     //
                        DMA_SxCR_EN;        //

    //
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4->CR1 = 0;                          //
    TIM4->PSC = 0;                          //prescaller na 0
    TIM4->ARR = 167;                        //reload register na 83
    TIM4->CCR4 = 167;                       //
    TIM4->CCMR2 |= TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1; //
    TIM4->CCER |= TIM_CCER_CC4E;            //

    //NVIC
    NVIC_EnableIRQ(ADC_IRQn);               //

    //
    ADC1->CR2 |= ADC_CR2_ADON;


    TIM4->CR1 = TIM_CR1_CEN;

}

int main(void)
{
	int i = 0;


	//LED ON/OFF
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= (1<<5*2);
	GPIOA->ODR |= (1<<5);


    uint32_t rev;


    //ADC2_Init();

    ADCInit();


    while (1) {


  	i++;
    }
}
*/
#include "stm32f4xx.h"

#include "stm32f4xx_adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

int ConvertedValue = 0; //Converted value readed from ADC


void adc_configure(){
 ADC_InitTypeDef ADC_init_structure; //Structure for adc confguration
 GPIO_InitTypeDef GPIO_initStructre; //Structure for analog input pin
 //Clock configuration

 RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;					// APB2 Peripheral clock enable ADC1 clock

 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;					//


 //Analog pin configuration
 GPIO_initStructre.GPIO_Pin = GPIO_Pin_0;			//The channel 10 is connected to PC0
 GPIO_initStructre.GPIO_Mode = GPIO_Mode_AN; 		//The PC0 pin is configured in analog mode
 GPIO_initStructre.GPIO_PuPd = GPIO_PuPd_NOPULL; 	//We don't need any pull up or pull down
 GPIO_Init(GPIOC,&GPIO_initStructre);				//Affecting the port with the initialization structure configuration


 //ADC structure configuration
 RCC->APB2RSTR |= RCC_APB2Periph_ADC;
 // Release all ADCs from reset state
 RCC->APB2RSTR &= ~RCC_APB2Periph_ADC;


 ADC_init_structure.ADC_DataAlign = ADC_DataAlign_Right;						//data converted will be shifted to right
 ADC_init_structure.ADC_Resolution = ADC_Resolution_12b;						//Input voltage is converted into a 12bit number giving a maximum value of 4096
 ADC_init_structure.ADC_ContinuousConvMode = ENABLE; 							//the conversion is continuous, the input data is converted more than once
 ADC_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;			// conversion is synchronous with TIM1 and CC1 (actually I'm not sure about this one :/)
 ADC_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//no trigger for conversion
 ADC_init_structure.ADC_NbrOfConversion = 1;									//I think this one is clear :p
 ADC_init_structure.ADC_ScanConvMode = DISABLE;									//The scan is configured in one channel
 ADC_Init(ADC1,&ADC_init_structure);											//Initialize ADC with the previous configuration
 //Enable ADC conversion
 ADC_Cmd(ADC1,ENABLE);
 //Select the channel to be read from
 ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_144Cycles);
}
int adc_convert(){
 ADC_SoftwareStartConv(ADC1);													//Start the conversion
 while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));									//Processing the conversion
 return ADC_GetConversionValue(ADC1); //Return the converted data
}
int main(void){
 adc_configure();//Start configuration
    while(1){//loop while the board is working
     ConvertedValue = adc_convert();//Read the ADC converted value
    }
}


