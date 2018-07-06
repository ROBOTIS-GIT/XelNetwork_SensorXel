// drv_mux.c



#include <stdarg.h>
#include <stdbool.h>

#include "hw.h"
#include "drv_led.h"





//-- Internal Variables
//




//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//

#define LOW 0
#define HIGH 1
#define IS_MUX_ENABLED(__state__) (LOW == (__state__))

typedef struct
{
  GPIO_PinState       pin_state;
  GPIO_TypeDef       *port;
  uint16_t            pin;
}drv_mux_ch_t;

typedef struct
{
	drv_mux_ch_t				gpio[4]; // {Id0,Id1,Id2,EN}
}drv_mux_id_t;

drv_mux_id_t drv_mux_tbl[_HW_DEF_MUX_CH_MAX];



bool drvMuxInit(void)
{
	uint32_t i, j;
	GPIO_InitTypeDef  GPIO_InitStruct;



	drv_mux_tbl[0].gpio[0].port = GPIOB;
	drv_mux_tbl[0].gpio[0].pin  = GPIO_PIN_15;
	drv_mux_tbl[0].gpio[1].port = GPIOA;
	drv_mux_tbl[0].gpio[1].pin  = GPIO_PIN_11;
	drv_mux_tbl[0].gpio[2].port = GPIOA;
	drv_mux_tbl[0].gpio[2].pin  = GPIO_PIN_12;
  drv_mux_tbl[0].gpio[3].port = GPIOC;
  drv_mux_tbl[0].gpio[3].pin  = GPIO_PIN_13;


	drv_mux_tbl[1].gpio[0].port = GPIOA;
	drv_mux_tbl[1].gpio[0].pin  = GPIO_PIN_15;
	drv_mux_tbl[1].gpio[1].port = GPIOB;
	drv_mux_tbl[1].gpio[1].pin  = GPIO_PIN_6;
	drv_mux_tbl[1].gpio[2].port = GPIOB;
	drv_mux_tbl[1].gpio[2].pin  = GPIO_PIN_7;
  drv_mux_tbl[1].gpio[3].port = GPIOB;
  drv_mux_tbl[1].gpio[3].pin  = GPIO_PIN_10;


	drv_mux_tbl[2].gpio[0].port = GPIOB;
	drv_mux_tbl[2].gpio[0].pin  = GPIO_PIN_12;
	drv_mux_tbl[2].gpio[1].port = GPIOB;
	drv_mux_tbl[2].gpio[1].pin  = GPIO_PIN_13;
	drv_mux_tbl[2].gpio[2].port = GPIOB;
	drv_mux_tbl[2].gpio[2].pin  = GPIO_PIN_14;
  drv_mux_tbl[2].gpio[3].port = GPIOB;
  drv_mux_tbl[2].gpio[3].pin  = GPIO_PIN_11;


	for (i=0; i<_HW_DEF_MUX_CH_MAX; i++)
	{
		for(j=0; j<4; j++)
		{
			if (drv_mux_tbl[i].gpio[j].port != NULL)
			{
				GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
				GPIO_InitStruct.Pull  = GPIO_PULLUP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
				GPIO_InitStruct.Pin   = drv_mux_tbl[i].gpio[j].pin;
				HAL_GPIO_Init(drv_mux_tbl[i].gpio[j].port, &GPIO_InitStruct);
			}
		}
	}

  return true;
}

//LOW ACTIVE
void drvMuxEnableId(uint8_t id, bool state)
{
  drv_mux_ch_t *p_ch_gpio;

  p_ch_gpio = &drv_mux_tbl[id].gpio[3];

	if(state)
	{
	  HAL_GPIO_WritePin(p_ch_gpio->port, p_ch_gpio->pin, GPIO_PIN_RESET);
	}
	else
	{
	  HAL_GPIO_WritePin(p_ch_gpio->port, p_ch_gpio->pin, GPIO_PIN_SET);
	}

	p_ch_gpio->pin_state = HAL_GPIO_ReadPin(p_ch_gpio->port, p_ch_gpio->pin);
}

bool drvMuxSetChannel(uint8_t id, uint8_t ch)
{
	uint8_t i;
	uint8_t ret;
	uint8_t mux_state;

	mux_state = drv_mux_tbl[id].gpio[3].pin_state;

	if(IS_MUX_ENABLED(mux_state))
	{
		if(ch<8)
		{
			for(i = 0; i < 3; i++)
			{
				ret = (ch >> i) & 0x01;
				if(ret)
				{
				  HAL_GPIO_WritePin(drv_mux_tbl[id].gpio[i].port, drv_mux_tbl[id].gpio[i].pin, GPIO_PIN_SET);
				}
				else
				{
				  HAL_GPIO_WritePin(drv_mux_tbl[id].gpio[i].port, drv_mux_tbl[id].gpio[i].pin, GPIO_PIN_RESET);
			  }
			}
		}
	}
	else
	{
	  return false; // over the max num of ch or didn't turn on the mux before setting id&ch value.
	}

	return true;
}

uint8_t drvMUxGetChannel(uint8_t id)
{
	uint8_t ret_ch = 0;
	uint8_t i;
	uint8_t mux_state;

	mux_state = drv_mux_tbl[id].gpio[3].pin_state;

	if(IS_MUX_ENABLED(mux_state))
	{
		for(i = 0; i < 3; i++)
		{
			ret_ch += (HAL_GPIO_ReadPin(drv_mux_tbl[id].gpio[i].port, drv_mux_tbl[id].gpio[i].pin) << i);
		}
		return ret_ch;
	}
	else
	{
	  return 255; //didn't turn on the mux before getting id&ch value.
	}
}
