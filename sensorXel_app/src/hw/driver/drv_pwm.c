/*
 * drv_pwm.c
 *
 *  Created on: 2017. 11. 10.
 *      Author: opus
 */

#include "drv_pwm.h"
#include "hw.h"

#define DEFAULT_FREQ   50000 //50kHz

typedef struct
{
  bool          is_setup;
  TIM_HandleTypeDef *hTIM;
  TIM_OC_InitTypeDef ocConfig;
  uint32_t           channel;
} drv_pwm_t;

typedef struct
{
  TIM_TypeDef  *TIMx;
  uint32_t      pwm_ch;
  GPIO_TypeDef *port;
  uint16_t      pin;
  uint8_t       af;
} drv_pwm_assign_t;

TIM_HandleTypeDef hTIM3;

static drv_pwm_t drv_pwm_tbl[_HW_DEF_PWM_CH_MAX];

const drv_pwm_assign_t drv_pwm_assign_tbl[_HW_DEF_PWM_CH_MAX] =
{
  { TIM3, TIM_CHANNEL_1, GPIOA, GPIO_PIN_6, GPIO_AF2_TIM3 },
  { TIM3, TIM_CHANNEL_2, GPIOA, GPIO_PIN_7, GPIO_AF2_TIM3 },
  { TIM3, TIM_CHANNEL_3, GPIOB, GPIO_PIN_0, GPIO_AF2_TIM3 },
  { TIM3, TIM_CHANNEL_4, GPIOB, GPIO_PIN_1, GPIO_AF2_TIM3 },
};

bool drvPwmInit(uint32_t freq)
{
  TIM_TypeDef *p_TIMx;
  TIM_HandleTypeDef* p_hTIM;

  uint32_t tim_clk, period_value;
  uint32_t prescaler_value = 1;
  uint32_t i;


  p_hTIM = &hTIM3;
  p_TIMx = TIM3;

  for(i = 0; i < _HW_DEF_PWM_CH_MAX; i++)
  {
    drv_pwm_tbl[i].is_setup = false;
    drv_pwm_tbl[i].hTIM = p_hTIM;
  }

  tim_clk = SystemCoreClock;

  period_value = (uint32_t) (tim_clk / freq);

  // Adjust 16bit
  prescaler_value = (period_value/0xFFFF) + 1;
  period_value /= prescaler_value;

  p_hTIM->Instance = p_TIMx;
  p_hTIM->Init.Prescaler = prescaler_value - 1;
  p_hTIM->Init.Period = period_value - 1;
  p_hTIM->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  p_hTIM->Init.CounterMode = TIM_COUNTERMODE_UP;

  if (HAL_TIM_PWM_Init(p_hTIM) != HAL_OK)
  {
    return false;
  }

  return true;
}

bool drvPwmSetup(uint8_t channel)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  drv_pwm_tbl[channel].ocConfig.OCMode = TIM_OCMODE_PWM1;
  drv_pwm_tbl[channel].ocConfig.OCFastMode = TIM_OCFAST_DISABLE;
  drv_pwm_tbl[channel].ocConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  drv_pwm_tbl[channel].ocConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
  drv_pwm_tbl[channel].ocConfig.Pulse = 0;
  drv_pwm_tbl[channel].channel = drv_pwm_assign_tbl[channel].pwm_ch;

  if (HAL_TIM_PWM_ConfigChannel(drv_pwm_tbl[channel].hTIM, &drv_pwm_tbl[channel].ocConfig,
      drv_pwm_tbl[channel].channel) != HAL_OK)
  {
    return false;
  }

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = drv_pwm_assign_tbl[channel].af;
  GPIO_InitStruct.Pin = drv_pwm_assign_tbl[channel].pin;
  HAL_GPIO_Init(drv_pwm_assign_tbl[channel].port, &GPIO_InitStruct);

  if(HAL_TIM_PWM_Start(drv_pwm_tbl[channel].hTIM, drv_pwm_tbl[channel].channel) != HAL_OK)
  {
    return false;
  }

  drv_pwm_tbl[channel].is_setup = true;

  return true;
}

bool drvPwmSetDuty(uint8_t channel, uint8_t duty)
{
  TIM_TypeDef *p_TIMx;
  uint32_t pulse;

  if((channel > _HW_DEF_PWM_CH_MAX)||(drv_pwm_tbl[channel].is_setup == false))
  {
    return false;
  }

  p_TIMx = drv_pwm_tbl[channel].hTIM->Instance;
  pulse = ((drv_pwm_tbl[channel].hTIM->Init.Period + 1) * duty) / 100;

  switch (drv_pwm_tbl[channel].channel)
  {
    case TIM_CHANNEL_1:
      p_TIMx->CCR1 = pulse;
      break;

    case TIM_CHANNEL_2:
      p_TIMx->CCR2 = pulse;
      break;

    case TIM_CHANNEL_3:
      p_TIMx->CCR3 = pulse;
      break;

    case TIM_CHANNEL_4:
      p_TIMx->CCR4 = pulse;
      break;

    default:
      break;
  }

  return true;
}

bool drvPwmSetPulse(uint8_t channel, uint32_t res, uint32_t pulse)
{
  TIM_TypeDef *p_TIMx;

  if((channel > _HW_DEF_PWM_CH_MAX)||(drv_pwm_tbl[channel].is_setup == false))
  {
    return false;
  }

  p_TIMx = drv_pwm_tbl[channel].hTIM->Instance;

  pulse = constrain(pulse, 0, (1<<res)-1);
  pulse = map(pulse, 0, (1<<res)-1, 0, drv_pwm_tbl[channel].hTIM->Init.Period + 1);

  switch (drv_pwm_tbl[channel].channel)
  {
    case TIM_CHANNEL_1:
      p_TIMx->CCR1 = pulse;
      break;

    case TIM_CHANNEL_2:
      p_TIMx->CCR2 = pulse;
      break;

    case TIM_CHANNEL_3:
      p_TIMx->CCR3 = pulse;
      break;

    case TIM_CHANNEL_4:
      p_TIMx->CCR4 = pulse;
      break;

    default:
      break;
  }

  return true;
}

uint8_t drvPwmGetDuty(uint8_t channel)
{
  if(drv_pwm_tbl[channel].is_setup == false)
  {
    return 0;
  }

  uint8_t t_duty;

  t_duty = (uint8_t) ((drvPwmGetPulse(channel) * 100)
      / drv_pwm_tbl[channel].hTIM->Init.Period);

  return t_duty;
}

uint32_t drvPwmGetPulse(uint8_t channel)
{
  if(drv_pwm_tbl[channel].is_setup == false)
  {
    return 0;
  }

  uint32_t t_pulse;

  t_pulse = HAL_TIM_ReadCapturedValue(drv_pwm_tbl[channel].hTIM,
      drv_pwm_tbl[channel].channel);

  return t_pulse;
}


void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  if( htim->Instance == TIM3 )
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
}
