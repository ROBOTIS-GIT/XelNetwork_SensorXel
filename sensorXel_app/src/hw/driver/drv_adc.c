/*
 * drv_adc.c
 *
 *  Created on: 2018. 2. 10.
 *      Author: kei
 */

#include "hw.h"
#include "drv_adc.h"


ADC_HandleTypeDef    hADC1;
DMA_HandleTypeDef    hDMA1;

typedef struct
{
  GPIO_TypeDef       *port;
  uint16_t            pin;
} drv_adc_gpio_t;


typedef struct
{
  bool                   is_setup;
  drv_adc_gpio_t         gpio;
  ADC_HandleTypeDef     *p_adc_handle;
  ADC_ChannelConfTypeDef adc_config;
} drv_adc_t;



//-- Internal Variables
//
drv_adc_t drv_adc_tbl[_HW_DEF_ADC_CH_MAX] =
{
    {false, {GPIOA, GPIO_PIN_6}, &hADC1, {ADC_CHANNEL_6, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_96CYCLES}},
    {false, {GPIOA, GPIO_PIN_7}, &hADC1, {ADC_CHANNEL_7, ADC_REGULAR_RANK_2, ADC_SAMPLETIME_96CYCLES}},
    {false, {GPIOB, GPIO_PIN_0}, &hADC1, {ADC_CHANNEL_8, ADC_REGULAR_RANK_3, ADC_SAMPLETIME_96CYCLES}},
    {false, {GPIOB, GPIO_PIN_1}, &hADC1, {ADC_CHANNEL_9, ADC_REGULAR_RANK_4, ADC_SAMPLETIME_96CYCLES}}
};

uint32_t dma_buf[_HW_DEF_ADC_CH_MAX];

//-- External Variables
//

//-- Internal Functions
//
static bool drvAdcInitTriggerTimer(void);
static HAL_StatusTypeDef drvAdcStopLL(uint8_t ch);

//-- External Functions
//

bool drvAdcInit(void)
{
  hADC1.Instance                   = ADC1;
  hADC1.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV4;
  hADC1.Init.Resolution            = ADC_RESOLUTION_12B;
  hADC1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hADC1.Init.ScanConvMode          = ENABLE;
  hADC1.Init.EOCSelection          = ADC_EOC_SEQ_CONV;
  hADC1.Init.LowPowerAutoWait      = ADC_AUTOWAIT_DISABLE;
  hADC1.Init.LowPowerAutoPowerOff  = ADC_AUTOPOWEROFF_DISABLE;
  hADC1.Init.ChannelsBank          = ADC_CHANNELS_BANK_A;
  hADC1.Init.ContinuousConvMode    = DISABLE;
  hADC1.Init.DiscontinuousConvMode = ENABLE;
  hADC1.Init.NbrOfConversion       = _HW_DEF_ADC_CH_MAX;
  hADC1.Init.NbrOfDiscConversion   = 1;
  hADC1.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T4_TRGO;
  hADC1.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hADC1.Init.DMAContinuousRequests = ENABLE;

  if(HAL_ADC_Init(&hADC1) != HAL_OK)
  {
    return false;
  }

  if(drvAdcInitTriggerTimer() == false)
  {
    return false;
  }

  if (HAL_ADC_Start_DMA(&hADC1, (uint32_t *)dma_buf, _HW_DEF_ADC_CH_MAX) != HAL_OK)
  {
    return false;
  }

  return true;
}

bool drvAdcSetup(uint8_t ch)
{
  if(ch >= _HW_DEF_ADC_CH_MAX)
  {
    return false;
  }

  GPIO_InitTypeDef GPIO_InitStruct;

  if (drv_adc_tbl[ch].gpio.port != NULL)
  {
    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Pin   = drv_adc_tbl[ch].gpio.pin;
    HAL_GPIO_Init(drv_adc_tbl[ch].gpio.port, &GPIO_InitStruct);
  }

  if(HAL_ADC_ConfigChannel(drv_adc_tbl[ch].p_adc_handle, &drv_adc_tbl[ch].adc_config) != HAL_OK)
  {
    return false;
  }

  drv_adc_tbl[ch].is_setup = true;

  HAL_ADC_Stop_DMA(&hADC1);

  if (HAL_ADC_Start_DMA(&hADC1, (uint32_t *)dma_buf, _HW_DEF_ADC_CH_MAX) != HAL_OK)
  {
    return false;
  }

  return  true;
}

bool drvAdcStop(uint8_t ch)
{
  if((ch >= _HW_DEF_ADC_CH_MAX)||(drv_adc_tbl[ch].is_setup == false))
  {
    return false;
  }

  GPIO_InitTypeDef GPIO_InitStruct;

  if(drvAdcStopLL(ch) != HAL_OK)
  {
    return false;
  }

  if (drv_adc_tbl[ch].gpio.port != NULL)
  {
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Pin   = drv_adc_tbl[ch].gpio.pin;
    HAL_GPIO_Init(drv_adc_tbl[ch].gpio.port, &GPIO_InitStruct);
  }

  drv_adc_tbl[ch].is_setup = false;

  return true;
}

uint16_t drvAdcRead(uint8_t ch)
{
  uint16_t ret = 0;

  if ((ch >= _HW_DEF_ADC_CH_MAX)||(drv_adc_tbl[ch].is_setup == false))
  {
    return 0;
  }

  ret = dma_buf[ch];

  return ret;
}

uint16_t drvAdcRead8(uint8_t ch)
{
  uint16_t ret = 0;

  ret = drvAdcRead(ch) >> 4;

  return ret;
}

uint16_t drvAdcRead10(uint8_t ch)
{
  uint16_t ret = 0;


  ret = drvAdcRead(ch) >> 2;

  return ret;
}

uint16_t drvAdcRead12(uint8_t ch)
{
  uint16_t ret = 0;

  ret = drvAdcRead(ch);

  return ret;
}

uint16_t drvAdcRead16(uint8_t ch)
{
  uint16_t ret = 0;


  ret = drvAdcRead(ch) << 4;

  return ret;
}

uint16_t drvAdcReadVoltage(uint8_t ch)
{
  uint16_t ret;

  if ((ch >= _HW_DEF_ADC_CH_MAX)||(drv_adc_tbl[ch].is_setup == false))
  {
    return 0;
  }

  ret = drvAdcConvVoltage(ch, drvAdcRead(ch));

  return ret;
}

uint16_t drvAdcConvVoltage(uint8_t ch, uint32_t adc_value)
{
  uint16_t ret = 0;

  switch(ch)
  {
    case _DEF_ADC1:
    case _DEF_ADC2:
    case _DEF_ADC3:
    case _DEF_ADC4:
      ret = (uint16_t)((adc_value * 330 * 100 / 100) / 4095);
      break;
      break;
  }

  return ret;
}

uint8_t  drvAdcGetRes(uint8_t ch)
{
  return 12;
}


void ADC1_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&hADC1);
}

void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hADC1.DMA_Handle);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hADC1)
{
  RCC_OscInitTypeDef        RCC_OscInitStructure;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_ADC1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  HAL_RCC_GetOscConfig(&RCC_OscInitStructure);
  RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStructure.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStructure.HSIState = RCC_HSI_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStructure);

  hDMA1.Instance                 = DMA1_Channel1;
  hDMA1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hDMA1.Init.PeriphInc           = DMA_PINC_DISABLE;
  hDMA1.Init.MemInc              = DMA_MINC_ENABLE;
  hDMA1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hDMA1.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hDMA1.Init.Mode                = DMA_CIRCULAR;
  hDMA1.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_DeInit(&hDMA1);
  HAL_DMA_Init(&hDMA1);

  __HAL_LINKDMA(hADC1, DMA_Handle, hDMA1);

  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

  HAL_NVIC_SetPriority(ADC1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(ADC1_IRQn);
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hADC1)
{
  uint32_t i;

  __HAL_RCC_ADC1_FORCE_RESET();
  __HAL_RCC_ADC1_RELEASE_RESET();

  for (i=0; i<_HW_DEF_ADC_CH_MAX; i++)
  {
    if (drv_adc_tbl[i].gpio.port != NULL)
    {
      HAL_GPIO_DeInit(drv_adc_tbl[i].gpio.port, drv_adc_tbl[i].gpio.pin);
    }
  }

  if(hADC1->DMA_Handle != NULL)
  {
    HAL_DMA_DeInit(hADC1->DMA_Handle);
  }

  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
  HAL_NVIC_DisableIRQ(ADC1_IRQn);
}

static bool drvAdcInitTriggerTimer(void)
{
  static TIM_HandleTypeDef    hTIM4;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef   sConfig;

  __HAL_RCC_TIM4_CLK_ENABLE();

  hTIM4.Instance = TIM4;
  hTIM4.Init.Period = 10 - 1;
  hTIM4.Init.Prescaler = ((SystemCoreClock / 1000) - 1);
  hTIM4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  hTIM4.Init.CounterMode = TIM_COUNTERMODE_UP;

  if (HAL_TIM_Base_Init(&hTIM4) != HAL_OK)
  {
    return false;
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&hTIM4, &sMasterConfig) != HAL_OK)
  {
    return false;
  }

  if (HAL_TIM_Base_Start(&hTIM4) != HAL_OK)
  {
    return false;
  }

  return true;
}

static HAL_StatusTypeDef drvAdcStopLL(uint8_t ch)
{
  ADC_TypeDef *ADCx;
  ADC_ChannelConfTypeDef *ADCcfg;

  ADCx = drv_adc_tbl[ch].p_adc_handle->Instance;
  ADCcfg = &drv_adc_tbl[ch].adc_config;

  __HAL_LOCK(drv_adc_tbl[ch].p_adc_handle);

  if (ADCcfg->Rank < 7)
  {
    MODIFY_REG(ADCx->SQR5,
               ADC_SQR5_RK(ADC_SQR5_SQ1, ADCcfg->Rank),
               ADC_SQR5_RK(0, ADCcfg->Rank) );
  }
  else if (ADCcfg->Rank < 13)
  {
    MODIFY_REG(ADCx->SQR4,
               ADC_SQR4_RK(ADC_SQR4_SQ7, ADCcfg->Rank),
               ADC_SQR4_RK(0, ADCcfg->Rank) );
  }
  else if (ADCcfg->Rank < 19)
  {
    MODIFY_REG(ADCx->SQR3,
               ADC_SQR3_RK(ADC_SQR3_SQ13, ADCcfg->Rank),
               ADC_SQR3_RK(0, ADCcfg->Rank) );
  }
  else if (ADCcfg->Rank < 25)
  {
    MODIFY_REG(ADCx->SQR2,
               ADC_SQR2_RK(ADC_SQR2_SQ19, ADCcfg->Rank),
               ADC_SQR2_RK(0, ADCcfg->Rank) );
  }
  else
  {
    MODIFY_REG(ADCx->SQR1,
               ADC_SQR1_RK(ADC_SQR1_SQ25, ADCcfg->Rank),
               ADC_SQR1_RK(0, ADCcfg->Rank) );
  }

  __HAL_UNLOCK(drv_adc_tbl[ch].p_adc_handle);

  return HAL_OK;
}
