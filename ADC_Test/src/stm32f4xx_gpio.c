/**
  ******************************************************************************
  * @file    stm32f4xx_gpio.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    08-November-2013
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the GPIO peripheral:
  *           + Initialization and Configuration
  *           + GPIO Read and Write
  *           + GPIO Alternate functions configuration
  *
@verbatim
 ===============================================================================
                      ##### How to use this driver #####
 ===============================================================================
 [..]
   (#) Enable the GPIO AHB clock using the following function
       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

   (#) Configure the GPIO pin(s) using GPIO_Init()
       Four possible configuration are available for each pin:
       (++) Input: Floating, Pull-up, Pull-down.
       (++) Output: Push-Pull (Pull-up, Pull-down or no Pull)
            Open Drain (Pull-up, Pull-down or no Pull). In output mode, the speed
            is configurable: 2 MHz, 25 MHz, 50 MHz or 100 MHz.
       (++) Alternate Function: Push-Pull (Pull-up, Pull-down or no Pull) Open
            Drain (Pull-up, Pull-down or no Pull).
       (++) Analog: required mode when a pin is to be used as ADC channel or DAC
            output.

   (#) Peripherals alternate function:
       (++) For ADC and DAC, configure the desired pin in analog mode using
            GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AN;
            (+++) For other peripherals (TIM, USART...):
            (+++) Connect the pin to the desired peripherals' Alternate
                     Function (AF) using GPIO_PinAFConfig() function
            (+++) Configure the desired pin in alternate function mode using
                     GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
            (+++) Select the type, pull-up/pull-down and output speed via
                     GPIO_PuPd, GPIO_OType and GPIO_Speed members
            (+++) Call GPIO_Init() function

   (#) To get the level of a pin configured in input mode use GPIO_ReadInputDataBit()

   (#) To set/reset the level of a pin configured in output mode use
       GPIO_SetBits()/GPIO_ResetBits()

   (#) During and just after reset, the alternate functions are not
       active and the GPIO pins are configured in input floating mode (except JTAG
       pins).

   (#) The LSE oscillator pins OSC32_IN and OSC32_OUT can be used as general purpose
       (PC14 and PC15, respectively) when the LSE oscillator is off. The LSE has
       priority over the GPIO function.

   (#) The HSE oscillator pins OSC_IN/OSC_OUT can be used as
       general purpose PH0 and PH1, respectively, when the HSE oscillator is off.
       The HSE has priority over the GPIO function.

@endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/**
  * @brief  Initializes the GPIOx peripheral according to the specified parameters in the GPIO_InitStruct.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that contains
  *         the configuration information for the specified GPIO peripheral.
  * @retval None
  */


/**
  * @brief  De-initializes the GPIOx peripheral registers to their default reset values.
  * @note   By default, The GPIO pins are configured in input floating mode (except JTAG pins).
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices.
  * @retval None
  */
void GPIO_DeInit(GPIO_TypeDef* GPIOx)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  if (GPIOx == GPIOA)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, DISABLE);
  }
  else if (GPIOx == GPIOB)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, DISABLE);
  }
  else if (GPIOx == GPIOC)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOC, DISABLE);
  }
  else if (GPIOx == GPIOD)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOD, DISABLE);
  }
  else if (GPIOx == GPIOE)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOE, DISABLE);
  }
#ifdef GPIOF
  else if (GPIOx == GPIOF)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOF, DISABLE);
  }
#endif
#ifdef GPIOG
  else if (GPIOx == GPIOG)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOG, DISABLE);
  }
#endif
  else if (GPIOx == GPIOH)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOH, DISABLE);
  }
#ifdef GPIOI
  else if (GPIOx == GPIOI)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOI, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOI, DISABLE);
  }
#endif
#ifdef GPIOJ
  else if (GPIOx == GPIOJ)
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOJ, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOJ, DISABLE);
  }
#endif
  else
  {
#ifdef GPIOK
    if (GPIOx == GPIOK)
    {
      RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOK, ENABLE);
      RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOK, DISABLE);
    }
#endif
  }
}

/**
  * @brief  Initializes the GPIOx peripheral according to the specified parameters in the GPIO_InitStruct.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that contains
  *         the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
  uint32_t pinpos = 0x00, pos = 0x00 , currentpin = 0x00;

  /* Check the parameters */

  /* ------------------------- Configure the port pins ---------------- */
  /*-- GPIO Mode Configuration --*/
  for (pinpos = 0x00; pinpos < 0x10; pinpos++)
  {
    pos = ((uint32_t)0x01) << pinpos;
    /* Get the port pins position */
    currentpin = (GPIO_InitStruct->GPIO_Pin) & pos;

    if (currentpin == pos)
    {
      GPIOx->MODER  &= ~(GPIO_MODER_MODER0 << (pinpos * 2));
      GPIOx->MODER |= (((uint32_t)GPIO_InitStruct->GPIO_Mode) << (pinpos * 2));

      if ((GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) || (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_AF))
      {
        /* Check Speed mode parameters */


        /* Speed mode configuration */
        GPIOx->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinpos * 2));
        GPIOx->OSPEEDR |= ((uint32_t)(GPIO_InitStruct->GPIO_Speed) << (pinpos * 2));

        /* Check Output mode parameters */


        /* Output mode configuration*/
        GPIOx->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pinpos)) ;
        GPIOx->OTYPER |= (uint16_t)(((uint16_t)GPIO_InitStruct->GPIO_OType) << ((uint16_t)pinpos));
      }

      /* Pull-up Pull down resistor configuration*/
      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pinpos * 2));
      GPIOx->PUPDR |= (((uint32_t)GPIO_InitStruct->GPIO_PuPd) << (pinpos * 2));
    }
  }
}

/**
  * @brief  Fills each GPIO_InitStruct member with its default value.
  * @param  GPIO_InitStruct : pointer to a GPIO_InitTypeDef structure which will be initialized.
  * @retval None
  */
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
{
  /* Reset GPIO init structure parameters values */
  GPIO_InitStruct->GPIO_Pin  = GPIO_Pin_All;
  GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct->GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct->GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct->GPIO_PuPd = GPIO_PuPd_NOPULL;
}
