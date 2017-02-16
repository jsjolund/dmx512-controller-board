/**
  ******************************************************************************
  * @file    stm32f4xx_adc.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    08-November-2013
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Analog to Digital Convertor (ADC) peripheral:
  *           + Initialization and Configuration (in addition to ADC multi mode
  *             selection)
  *           + Analog Watchdog configuration
  *           + Temperature Sensor & Vrefint (Voltage Reference internal) & VBAT
  *             management
  *           + Regular Channels Configuration
  *           + Regular Channels DMA Configuration
  *           + Injected channels Configuration
  *           + Interrupts and flags management
  *
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]
    (#) Enable the ADC interface clock using
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADCx, ENABLE);

    (#) ADC pins configuration
         (++) Enable the clock for the ADC GPIOs using the following function:
             RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);
         (++) Configure these ADC pins in analog mode using GPIO_Init();

     (#) Configure the ADC Prescaler, conversion resolution and data
         alignment using the ADC_Init() function.
     (#) Activate the ADC peripheral using ADC_Cmd() function.

     *** Regular channels group configuration ***
     ============================================
     [..]
       (+) To configure the ADC regular channels group features, use
           ADC_Init() and ADC_RegularChannelConfig() functions.
       (+) To activate the continuous mode, use the ADC_continuousModeCmd()
           function.
       (+) To configurate and activate the Discontinuous mode, use the
           ADC_DiscModeChannelCountConfig() and ADC_DiscModeCmd() functions.
       (+) To read the ADC converted values, use the ADC_GetConversionValue()
           function.

     *** Multi mode ADCs Regular channels configuration ***
     ======================================================
     [..]
       (+) Refer to "Regular channels group configuration" description to
           configure the ADC1, ADC2 and ADC3 regular channels.
       (+) Select the Multi mode ADC regular channels features (dual or
           triple mode) using ADC_CommonInit() function and configure
           the DMA mode using ADC_MultiModeDMARequestAfterLastTransferCmd()
           functions.
       (+) Read the ADCs converted values using the
           ADC_GetMultiModeConversionValue() function.

     *** DMA for Regular channels group features configuration ***
     =============================================================
     [..]
       (+) To enable the DMA mode for regular channels group, use the
           ADC_DMACmd() function.
       (+) To enable the generation of DMA requests continuously at the end
           of the last DMA transfer, use the ADC_DMARequestAfterLastTransferCmd()
           function.

     *** Injected channels group configuration ***
     =============================================
     [..]
       (+) To configure the ADC Injected channels group features, use
           ADC_InjectedChannelConfig() and  ADC_InjectedSequencerLengthConfig()
           functions.
       (+) To activate the continuous mode, use the ADC_continuousModeCmd()
           function.
       (+) To activate the Injected Discontinuous mode, use the
           ADC_InjectedDiscModeCmd() function.
       (+) To activate the AutoInjected mode, use the ADC_AutoInjectedConvCmd()
           function.
       (+) To read the ADC converted values, use the ADC_GetInjectedConversionValue()
           function.

    @endverbatim
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
#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"



/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup ADC
  * @brief ADC driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* ADC DISCNUM mask */
#define CR1_DISCNUM_RESET         ((uint32_t)0xFFFF1FFF)

/* ADC AWDCH mask */
#define CR1_AWDCH_RESET           ((uint32_t)0xFFFFFFE0)

/* ADC Analog watchdog enable mode mask */
#define CR1_AWDMode_RESET         ((uint32_t)0xFF3FFDFF)

/* CR1 register Mask */
#define CR1_CLEAR_MASK            ((uint32_t)0xFCFFFEFF)

/* ADC EXTEN mask */
#define CR2_EXTEN_RESET           ((uint32_t)0xCFFFFFFF)

/* ADC JEXTEN mask */
#define CR2_JEXTEN_RESET          ((uint32_t)0xFFCFFFFF)

/* ADC JEXTSEL mask */
#define CR2_JEXTSEL_RESET         ((uint32_t)0xFFF0FFFF)

/* CR2 register Mask */
#define CR2_CLEAR_MASK            ((uint32_t)0xC0FFF7FD)

/* ADC SQx mask */
#define SQR3_SQ_SET               ((uint32_t)0x0000001F)
#define SQR2_SQ_SET               ((uint32_t)0x0000001F)
#define SQR1_SQ_SET               ((uint32_t)0x0000001F)

/* ADC L Mask */
#define SQR1_L_RESET              ((uint32_t)0xFF0FFFFF)

/* ADC JSQx mask */
#define JSQR_JSQ_SET              ((uint32_t)0x0000001F)

/* ADC JL mask */
#define JSQR_JL_SET               ((uint32_t)0x00300000)
#define JSQR_JL_RESET             ((uint32_t)0xFFCFFFFF)

/* ADC SMPx mask */
#define SMPR1_SMP_SET             ((uint32_t)0x00000007)
#define SMPR2_SMP_SET             ((uint32_t)0x00000007)

/* ADC JDRx registers offset */
#define JDR_OFFSET                ((uint8_t)0x28)

/* ADC CDR register base address */
#define CDR_ADDRESS               ((uint32_t)0x40012308)

/* ADC CCR register Mask */
#define CR_CLEAR_MASK             ((uint32_t)0xFFFC30E0)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup ADC_Private_Functions
  * @{
  */

/** @defgroup ADC_Group1 Initialization and Configuration functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
              ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the ADC Prescaler
      (+) ADC Conversion Resolution (12bit..6bit)
      (+) Scan Conversion Mode (multichannel or one channel) for regular group
      (+) ADC Continuous Conversion Mode (Continuous or Single conversion) for
          regular group
      (+) External trigger Edge and source of regular group,
      (+) Converted data alignment (left or right)
      (+) The number of ADC conversions that will be done using the sequencer for
          regular channel group
      (+) Multi ADC mode selection
      (+) Direct memory access mode selection for multi ADC mode
      (+) Delay between 2 sampling phases (used in dual or triple interleaved modes)
      (+) Enable or disable the ADC peripheral
@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes all ADCs peripherals registers to their default reset
  *         values.
  * @param  None
  * @retval None
  */
void ADC_DeInit(void)
{
  /* Enable all ADCs reset state */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC, ENABLE);

  /* Release all ADCs from reset state */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC, DISABLE);
}

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters
  *         in the ADC_InitStruct.
  * @note   This function is used to configure the global features of the ADC (
  *         Resolution and Data Alignment), however, the rest of the configuration
  *         parameters are specific to the regular channels group (scan mode
  *         activation, continuous mode activation, External trigger source and
  *         edge, number of conversion in the regular channels group sequencer).
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure that contains
  *         the configuration information for the specified ADC peripheral.
  * @retval None
  */
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct)
{
  uint32_t tmpreg1 = 0;
  uint8_t tmpreg2 = 0;
  /* Check the parameters */


  /*---------------------------- ADCx CR1 Configuration -----------------*/
  /* Get the ADCx CR1 value */
  tmpreg1 = ADCx->CR1;

  /* Clear RES and SCAN bits */
  tmpreg1 &= CR1_CLEAR_MASK;

  /* Configure ADCx: scan conversion mode and resolution */
  /* Set SCAN bit according to ADC_ScanConvMode value */
  /* Set RES bit according to ADC_Resolution value */
  tmpreg1 |= (uint32_t)(((uint32_t)ADC_InitStruct->ADC_ScanConvMode << 8) | \
                                   ADC_InitStruct->ADC_Resolution);
  /* Write to ADCx CR1 */
  ADCx->CR1 = tmpreg1;
  /*---------------------------- ADCx CR2 Configuration -----------------*/
  /* Get the ADCx CR2 value */
  tmpreg1 = ADCx->CR2;

  /* Clear CONT, ALIGN, EXTEN and EXTSEL bits */
  tmpreg1 &= CR2_CLEAR_MASK;

  /* Configure ADCx: external trigger event and edge, data alignment and
     continuous conversion mode */
  /* Set ALIGN bit according to ADC_DataAlign value */
  /* Set EXTEN bits according to ADC_ExternalTrigConvEdge value */
  /* Set EXTSEL bits according to ADC_ExternalTrigConv value */
  /* Set CONT bit according to ADC_ContinuousConvMode value */
  tmpreg1 |= (uint32_t)(ADC_InitStruct->ADC_DataAlign | \
                        ADC_InitStruct->ADC_ExternalTrigConv |
                        ADC_InitStruct->ADC_ExternalTrigConvEdge | \
                        ((uint32_t)ADC_InitStruct->ADC_ContinuousConvMode << 1));

  /* Write to ADCx CR2 */
  ADCx->CR2 = tmpreg1;
  /*---------------------------- ADCx SQR1 Configuration -----------------*/
  /* Get the ADCx SQR1 value */
  tmpreg1 = ADCx->SQR1;

  /* Clear L bits */
  tmpreg1 &= SQR1_L_RESET;

  /* Configure ADCx: regular channel sequence length */
  /* Set L bits according to ADC_NbrOfConversion value */
  tmpreg2 |= (uint8_t)(ADC_InitStruct->ADC_NbrOfConversion - (uint8_t)1);
  tmpreg1 |= ((uint32_t)tmpreg2 << 20);

  /* Write to ADCx SQR1 */
  ADCx->SQR1 = tmpreg1;
}

/**
  * @brief  Fills each ADC_InitStruct member with its default value.
  * @note   This function is used to initialize the global features of the ADC (
  *         Resolution and Data Alignment), however, the rest of the configuration
  *         parameters are specific to the regular channels group (scan mode
  *         activation, continuous mode activation, External trigger source and
  *         edge, number of conversion in the regular channels group sequencer).
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct)
{
  /* Initialize the ADC_Mode member */
  ADC_InitStruct->ADC_Resolution = ADC_Resolution_12b;

  /* initialize the ADC_ScanConvMode member */
  ADC_InitStruct->ADC_ScanConvMode = DISABLE;

  /* Initialize the ADC_ContinuousConvMode member */
  ADC_InitStruct->ADC_ContinuousConvMode = DISABLE;

  /* Initialize the ADC_ExternalTrigConvEdge member */
  ADC_InitStruct->ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;

  /* Initialize the ADC_ExternalTrigConv member */
  ADC_InitStruct->ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;

  /* Initialize the ADC_DataAlign member */
  ADC_InitStruct->ADC_DataAlign = ADC_DataAlign_Right;

  /* Initialize the ADC_NbrOfConversion member */
  ADC_InitStruct->ADC_NbrOfConversion = 1;
}

/**
  * @brief  Initializes the ADCs peripherals according to the specified parameters
  *         in the ADC_CommonInitStruct.
  * @param  ADC_CommonInitStruct: pointer to an ADC_CommonInitTypeDef structure
  *         that contains the configuration information for  All ADCs peripherals.
  * @retval None
  */
void ADC_CommonInit(ADC_CommonInitTypeDef* ADC_CommonInitStruct)
{
  uint32_t tmpreg1 = 0;
  /* Check the parameters */
  assert_param(IS_ADC_MODE(ADC_CommonInitStruct->ADC_Mode));
  assert_param(IS_ADC_PRESCALER(ADC_CommonInitStruct->ADC_Prescaler));
  assert_param(IS_ADC_DMA_ACCESS_MODE(ADC_CommonInitStruct->ADC_DMAAccessMode));
  assert_param(IS_ADC_SAMPLING_DELAY(ADC_CommonInitStruct->ADC_TwoSamplingDelay));
  /*---------------------------- ADC CCR Configuration -----------------*/
  /* Get the ADC CCR value */
  tmpreg1 = ADC->CCR;

  /* Clear MULTI, DELAY, DMA and ADCPRE bits */
  tmpreg1 &= CR_CLEAR_MASK;

  /* Configure ADCx: Multi mode, Delay between two sampling time, ADC prescaler,
     and DMA access mode for multimode */
  /* Set MULTI bits according to ADC_Mode value */
  /* Set ADCPRE bits according to ADC_Prescaler value */
  /* Set DMA bits according to ADC_DMAAccessMode value */
  /* Set DELAY bits according to ADC_TwoSamplingDelay value */
  tmpreg1 |= (uint32_t)(ADC_CommonInitStruct->ADC_Mode |
                        ADC_CommonInitStruct->ADC_Prescaler |
                        ADC_CommonInitStruct->ADC_DMAAccessMode |
                        ADC_CommonInitStruct->ADC_TwoSamplingDelay);

  /* Write to ADC CCR */
  ADC->CCR = tmpreg1;
}

/**
  * @brief  Fills each ADC_CommonInitStruct member with its default value.
  * @param  ADC_CommonInitStruct: pointer to an ADC_CommonInitTypeDef structure
  *         which will be initialized.
  * @retval None
  */
void ADC_CommonStructInit(ADC_CommonInitTypeDef* ADC_CommonInitStruct)
{
  /* Initialize the ADC_Mode member */
  ADC_CommonInitStruct->ADC_Mode = ADC_Mode_Independent;

  /* initialize the ADC_Prescaler member */
  ADC_CommonInitStruct->ADC_Prescaler = ADC_Prescaler_Div2;

  /* Initialize the ADC_DMAAccessMode member */
  ADC_CommonInitStruct->ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;

  /* Initialize the ADC_TwoSamplingDelay member */
  ADC_CommonInitStruct->ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
}

/**
  * @brief  Enables or disables the specified ADC peripheral.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the ADCx peripheral.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */

  if (NewState != DISABLE)
  {
    /* Set the ADON bit to wake up the ADC from power down mode */
    ADCx->CR2 |= (uint32_t)ADC_CR2_ADON;
  }
  else
  {
    /* Disable the selected ADC peripheral */
    ADCx->CR2 &= (uint32_t)(~ADC_CR2_ADON);
  }
}
/**
  * @}
  */

/** @defgroup ADC_Group2 Analog Watchdog configuration functions
 *  @brief    Analog Watchdog configuration functions
 *
@verbatim
 ===============================================================================
             ##### Analog Watchdog configuration functions #####
 ===============================================================================
    [..] This section provides functions allowing to configure the Analog Watchdog
         (AWD) feature in the ADC.

    [..] A typical configuration Analog Watchdog is done following these steps :
      (#) the ADC guarded channel(s) is (are) selected using the
          ADC_AnalogWatchdogSingleChannelConfig() function.
      (#) The Analog watchdog lower and higher threshold are configured using the
          ADC_AnalogWatchdogThresholdsConfig() function.
      (#) The Analog watchdog is enabled and configured to enable the check, on one
          or more channels, using the  ADC_AnalogWatchdogCmd() function.
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the analog watchdog on single/all regular or
  *         injected channels
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_AnalogWatchdog: the ADC analog watchdog configuration.
  *         This parameter can be one of the following values:
  *            @arg ADC_AnalogWatchdog_SingleRegEnable: Analog watchdog on a single regular channel
  *            @arg ADC_AnalogWatchdog_SingleInjecEnable: Analog watchdog on a single injected channel
  *            @arg ADC_AnalogWatchdog_SingleRegOrInjecEnable: Analog watchdog on a single regular or injected channel
  *            @arg ADC_AnalogWatchdog_AllRegEnable: Analog watchdog on all regular channel
  *            @arg ADC_AnalogWatchdog_AllInjecEnable: Analog watchdog on all injected channel
  *            @arg ADC_AnalogWatchdog_AllRegAllInjecEnable: Analog watchdog on all regular and injected channels
  *            @arg ADC_AnalogWatchdog_None: No channel guarded by the analog watchdog
  * @retval None
  */
void ADC_AnalogWatchdogCmd(ADC_TypeDef* ADCx, uint32_t ADC_AnalogWatchdog)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_ANALOG_WATCHDOG(ADC_AnalogWatchdog));

  /* Get the old register value */
  tmpreg = ADCx->CR1;

  /* Clear AWDEN, JAWDEN and AWDSGL bits */
  tmpreg &= CR1_AWDMode_RESET;

  /* Set the analog watchdog enable mode */
  tmpreg |= ADC_AnalogWatchdog;

  /* Store the new register value */
  ADCx->CR1 = tmpreg;
}



/** @defgroup ADC_Group4 Regular Channels Configuration functions
 *  @brief   Regular Channels Configuration functions
 *
@verbatim
 ===============================================================================
             ##### Regular Channels Configuration functions #####
 ===============================================================================
    [..] This section provides functions allowing to manage the ADC's regular channels,
         it is composed of 2 sub sections :

      (#) Configuration and management functions for regular channels: This subsection
          provides functions allowing to configure the ADC regular channels :
         (++) Configure the rank in the regular group sequencer for each channel
         (++) Configure the sampling time for each channel
         (++) select the conversion Trigger for regular channels
         (++) select the desired EOC event behavior configuration
         (++) Activate the continuous Mode  (*)
         (++) Activate the Discontinuous Mode
         -@@- Please Note that the following features for regular channels
             are configurated using the ADC_Init() function :
           (+@@) scan mode activation
           (+@@) continuous mode activation (**)
           (+@@) External trigger source
           (+@@) External trigger edge
           (+@@) number of conversion in the regular channels group sequencer.

         -@@- (*) and (**) are performing the same configuration

      (#) Get the conversion data: This subsection provides an important function in
          the ADC peripheral since it returns the converted data of the current
          regular channel. When the Conversion value is read, the EOC Flag is
          automatically cleared.

          -@- For multi ADC mode, the last ADC1, ADC2 and ADC3 regular conversions
              results data (in the selected multi mode) can be returned in the same
              time using ADC_GetMultiModeConversionValue() function.

@endverbatim
  * @{
  */
/**
  * @brief  Configures for the selected ADC regular channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure.
  *          This parameter can be one of the following values:
  *            @arg ADC_Channel_0: ADC Channel0 selected
  *            @arg ADC_Channel_1: ADC Channel1 selected
  *            @arg ADC_Channel_2: ADC Channel2 selected
  *            @arg ADC_Channel_3: ADC Channel3 selected
  *            @arg ADC_Channel_4: ADC Channel4 selected
  *            @arg ADC_Channel_5: ADC Channel5 selected
  *            @arg ADC_Channel_6: ADC Channel6 selected
  *            @arg ADC_Channel_7: ADC Channel7 selected
  *            @arg ADC_Channel_8: ADC Channel8 selected
  *            @arg ADC_Channel_9: ADC Channel9 selected
  *            @arg ADC_Channel_10: ADC Channel10 selected
  *            @arg ADC_Channel_11: ADC Channel11 selected
  *            @arg ADC_Channel_12: ADC Channel12 selected
  *            @arg ADC_Channel_13: ADC Channel13 selected
  *            @arg ADC_Channel_14: ADC Channel14 selected
  *            @arg ADC_Channel_15: ADC Channel15 selected
  *            @arg ADC_Channel_16: ADC Channel16 selected
  *            @arg ADC_Channel_17: ADC Channel17 selected
  *            @arg ADC_Channel_18: ADC Channel18 selected
  * @param  Rank: The rank in the regular group sequencer.
  *          This parameter must be between 1 to 16.
  * @param  ADC_SampleTime: The sample time value to be set for the selected channel.
  *          This parameter can be one of the following values:
  *            @arg ADC_SampleTime_3Cycles: Sample time equal to 3 cycles
  *            @arg ADC_SampleTime_15Cycles: Sample time equal to 15 cycles
  *            @arg ADC_SampleTime_28Cycles: Sample time equal to 28 cycles
  *            @arg ADC_SampleTime_56Cycles: Sample time equal to 56 cycles
  *            @arg ADC_SampleTime_84Cycles: Sample time equal to 84 cycles
  *            @arg ADC_SampleTime_112Cycles: Sample time equal to 112 cycles
  *            @arg ADC_SampleTime_144Cycles: Sample time equal to 144 cycles
  *            @arg ADC_SampleTime_480Cycles: Sample time equal to 480 cycles
  * @retval None
  */
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
  uint32_t tmpreg1 = 0, tmpreg2 = 0;
  /* Check the parameters */

  /* if ADC_Channel_10 ... ADC_Channel_18 is selected */
  if (ADC_Channel > ADC_Channel_9)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SMPR1;

    /* Calculate the mask to clear */
    tmpreg2 = SMPR1_SMP_SET << (3 * (ADC_Channel - 10));

    /* Clear the old sample time */
    tmpreg1 &= ~tmpreg2;

    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * (ADC_Channel - 10));

    /* Set the new sample time */
    tmpreg1 |= tmpreg2;

    /* Store the new register value */
    ADCx->SMPR1 = tmpreg1;
  }
  else /* ADC_Channel include in ADC_Channel_[0..9] */
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SMPR2;

    /* Calculate the mask to clear */
    tmpreg2 = SMPR2_SMP_SET << (3 * ADC_Channel);

    /* Clear the old sample time */
    tmpreg1 &= ~tmpreg2;

    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);

    /* Set the new sample time */
    tmpreg1 |= tmpreg2;

    /* Store the new register value */
    ADCx->SMPR2 = tmpreg1;
  }
  /* For Rank 1 to 6 */
  if (Rank < 7)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SQR3;

    /* Calculate the mask to clear */
    tmpreg2 = SQR3_SQ_SET << (5 * (Rank - 1));

    /* Clear the old SQx bits for the selected rank */
    tmpreg1 &= ~tmpreg2;

    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 1));

    /* Set the SQx bits for the selected rank */
    tmpreg1 |= tmpreg2;

    /* Store the new register value */
    ADCx->SQR3 = tmpreg1;
  }
  /* For Rank 7 to 12 */
  else if (Rank < 13)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SQR2;

    /* Calculate the mask to clear */
    tmpreg2 = SQR2_SQ_SET << (5 * (Rank - 7));

    /* Clear the old SQx bits for the selected rank */
    tmpreg1 &= ~tmpreg2;

    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 7));

    /* Set the SQx bits for the selected rank */
    tmpreg1 |= tmpreg2;

    /* Store the new register value */
    ADCx->SQR2 = tmpreg1;
  }
  /* For Rank 13 to 16 */
  else
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SQR1;

    /* Calculate the mask to clear */
    tmpreg2 = SQR1_SQ_SET << (5 * (Rank - 13));

    /* Clear the old SQx bits for the selected rank */
    tmpreg1 &= ~tmpreg2;

    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 13));

    /* Set the SQx bits for the selected rank */
    tmpreg1 |= tmpreg2;

    /* Store the new register value */
    ADCx->SQR1 = tmpreg1;
  }
}



/**
  * @brief  Enables the selected ADC software start conversion of the regular channels.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval None
  */
void ADC_SoftwareStartConv(ADC_TypeDef* ADCx)
{
  /* Check the parameters */


  /* Enable the selected ADC conversion for regular group */
  ADCx->CR2 |= (uint32_t)ADC_CR2_SWSTART;
}

/**
  * @brief  Gets the selected ADC Software start regular conversion Status.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval The new state of ADC software start conversion (SET or RESET).
  */
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef* ADCx)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));

  /* Check the status of SWSTART bit */
  if ((ADCx->CR2 & ADC_CR2_SWSTART) != (uint32_t)RESET)
  {
    /* SWSTART bit is set */
    bitstatus = SET;
  }
  else
  {
    /* SWSTART bit is reset */
    bitstatus = RESET;
  }

  /* Return the SWSTART bit status */
  return  bitstatus;
}


/**
  * @brief  Checks whether the specified ADC flag is set or not.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg ADC_FLAG_AWD: Analog watchdog flag
  *            @arg ADC_FLAG_EOC: End of conversion flag
  *            @arg ADC_FLAG_JEOC: End of injected group conversion flag
  *            @arg ADC_FLAG_JSTRT: Start of injected group conversion flag
  *            @arg ADC_FLAG_STRT: Start of regular group conversion flag
  *            @arg ADC_FLAG_OVR: Overrun flag
  * @retval The new state of ADC_FLAG (SET or RESET).
  */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */

  /* Check the status of the specified ADC flag */
  if ((ADCx->SR & ADC_FLAG) != (uint8_t)RESET)
  {
    /* ADC_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* ADC_FLAG is reset */
    bitstatus = RESET;
  }
  /* Return the ADC_FLAG status */
  return  bitstatus;
}


/**
  * @brief  Returns the last ADCx conversion result data for regular channel.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval The Data conversion value.
  */
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx)
{
  /* Check the parameters */


  /* Return the selected ADC conversion value */
  return (uint16_t) ADCx->DR;
}
