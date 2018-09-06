/*
 * mux_init.cpp
 *
 *  Created on: 2018. 9. 5.
 *      Author: D.ggavy
 */

#include "mux_init.h"




void mux_initSet(mux_default_set_t mux_comm_type)
{
  muxInit();
	switch(mux_comm_type)
	{
		case MUX_UART:
			muxSetChannel(MUX_ID_1, MUX_UART_RX);
			muxSetChannel(MUX_ID_2, MUX_UART_TX);
			break;

		case MUX_I2C:
			muxSetChannel(MUX_ID_1, MUX_I2C_SCL);
			muxSetChannel(MUX_ID_2, MUX_I2C_SDA);
			break;

		case MUX_SPI:
			muxSetChannel(MUX_ID_1, MUX_SPI_SCK);
			muxSetChannel(MUX_ID_2, MUX_SPI_MOSI);
			muxSetChannel(MUX_ID_3, MUX_SPI_MISO);
			break;
	}
}
