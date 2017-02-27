/**
 ******************************************************************************
 * File Name          : main.h
 * Description        : This file contains the common defines of the application
 ******************************************************************************
 *
 * COPYRIGHT(c) 2017 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USB_TX_Pin GPIO_PIN_2
#define USB_TX_GPIO_Port GPIOA
#define USB_RX_Pin GPIO_PIN_3
#define USB_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define B0_Pin GPIO_PIN_0
#define B0_GPIO_Port GPIOB
#define EEPROM_SCL_Pin GPIO_PIN_10
#define EEPROM_SCL_GPIO_Port GPIOB
#define LCD_BL_Pin GPIO_PIN_6
#define LCD_BL_GPIO_Port GPIOC
#define B2_Pin GPIO_PIN_7
#define B2_GPIO_Port GPIOC
#define B3_Pin GPIO_PIN_8
#define B3_GPIO_Port GPIOC
#define BUTTONS_SDA_Pin GPIO_PIN_9
#define BUTTONS_SDA_GPIO_Port GPIOC
#define BUTTONS_SCL_Pin GPIO_PIN_8
#define BUTTONS_SCL_GPIO_Port GPIOA
#define DMX_TX_Pin GPIO_PIN_9
#define DMX_TX_GPIO_Port GPIOA
#define DMX_RX_Pin GPIO_PIN_10
#define DMX_RX_GPIO_Port GPIOA
#define DMX_EN_Pin GPIO_PIN_11
#define DMX_EN_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define LCD_SCL_Pin GPIO_PIN_6
#define LCD_SCL_GPIO_Port GPIOB
#define LCD_SDA_Pin GPIO_PIN_7
#define LCD_SDA_GPIO_Port GPIOB
#define EEPROM_SDA_Pin GPIO_PIN_9
#define EEPROM_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define EEPROM_IRQ_EV I2C2_EV_IRQn
#define EEPROM_IRQ_TX DMA1_Stream7_IRQn
#define EEPROM_IRQ_RX DMA1_Stream3_IRQn
#define LCD_IRQ_EV I2C1_EV_IRQn
#define LCD_IRQ_TX DMA1_Stream1_IRQn
#define LCD_IRQ_RX DMA1_Stream0_IRQn
#define LCD_PWM_CHANNEL TIM_CHANNEL_1
#define LCD_TIM_IRQ TIM4_IRQn
#define BUTTONS_IRQ_EV I2C3_EV_IRQn
#define BUTTONS_IRQ_TX DMA1_Stream4_IRQn
#define BUTTONS_IRQ_RX DMA1_Stream2_IRQn
#define USB_USART_IRQ USART2_IRQn
#define USB_USART USART2
#define DMX_USART_IRQ USART1_IRQn
#define DMX_TIM_IRQ TIM2_IRQn
#define DMX_USART USART1
/* USER CODE END Private defines */

/**
 * @}
 */

/**
 * @}
 */

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
