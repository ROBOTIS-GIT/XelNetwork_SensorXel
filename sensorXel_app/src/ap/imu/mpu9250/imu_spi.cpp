#include "hw.h"
#include "spi.h"
#include "imu_spi.h"
#include "MPU9250_REGS.h"
#include "driver/drv_spi.h"


#define MPU_SPI_CH          _DEF_SPI1



void imu_spi_init(void)
{
  spiBegin(MPU_SPI_CH);
  spiSetDataMode(MPU_SPI_CH, SPI_MODE3 );
  spiSetBitOrder(MPU_SPI_CH, SPI_MSB_FIRST );
  spiSetClockDivider(MPU_SPI_CH, SPI_CLOCK_DIV_16 );      // 32Mhz/2/16 = 1MHz  //MAX 1Mhz for MPU9250
  spiSetCS(MPU_SPI_CH, _DEF_HIGH);

  delay(100);
}

void imu_spi_initFast(void)
{
  spiBegin(MPU_SPI_CH);
  spiSetDataMode(MPU_SPI_CH, SPI_MODE3 );
  spiSetBitOrder(MPU_SPI_CH, SPI_MSB_FIRST );
  spiSetClockDivider(MPU_SPI_CH, SPI_CLOCK_DIV_4 );
  spiSetCS(MPU_SPI_CH, _DEF_HIGH);
  delay(200);
}

int  imu_spi_writes(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t *data)
{
  uint32_t i;

  spiSetCS(MPU_SPI_CH, _DEF_LOW);
  drvSpiWrite8(MPU_SPI_CH, reg_addr );

  for( i=0; i<length; i++ )
  {
    spiWrite8(MPU_SPI_CH, data[i] );
  }
  spiSetCS(MPU_SPI_CH, _DEF_HIGH);
  delay(1);
	return 0;
}

int imu_spi_reads(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t *data)
{
  uint32_t i;

  spiSetCS(MPU_SPI_CH, _DEF_LOW);
  drvSpiWrite8(MPU_SPI_CH, reg_addr | 0x80 );  // reg | 0x80 to denote read
  for( i=0; i<length; i++ )
  {
    data[i] = spiTransfer8(MPU_SPI_CH, 0xFF );
  }
  spiSetCS(MPU_SPI_CH, _DEF_HIGH);
	return 0;
}

int imu_spi_ak8963_reads(uint8_t akm_addr, uint8_t reg_addr, uint8_t len, uint8_t *data)
{
	uint8_t index = 0;
	uint8_t status = 0;
	uint32_t timeout = 0;
	uint8_t tmp = 0;

	tmp = akm_addr | MPU9250_I2C_READ;
	imu_spi_writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_ADDR, 1, &tmp);
	delay(1);
	while(index < len){
		tmp = reg_addr + index;
		imu_spi_writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_REG, 1, &tmp);
		delay(1);
		tmp = MPU9250_I2C_SLV4_EN;
		imu_spi_writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_CTRL, 1, &tmp);
		delay(1);

		do {
			if (timeout++ > 50){
				return -2;
			}
			imu_spi_reads(MPU9250_SPIx_ADDR, MPU9250_I2C_MST_STATUS, 1, &status);
			delay(2);
		} while ((status & MPU9250_I2C_SLV4_DONE) == 0);
		imu_spi_reads(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_DI, 1, data + index);
		delay(1);
		index++;
	}
	return 0;
}

int imu_spi_ak8963_writes(uint8_t akm_addr, uint8_t reg_addr, uint8_t len, uint8_t *data)
{
  uint32_t timeout = 0;
	uint8_t status = 0;
	uint8_t tmp = 0;
	uint8_t index = 0;

	tmp = akm_addr;
	imu_spi_writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_ADDR, 1, &tmp);
	delay(2);

	while(index < len){
		tmp = reg_addr + index;
		imu_spi_writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_REG, 1, &tmp);
		delay(2);
		imu_spi_writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_DO, 1, data + index);
		delay(2);
		tmp = MPU9250_I2C_SLV4_EN;
		imu_spi_writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_CTRL, 1, &tmp);
		delay(2);

		do {
			if (timeout++ > 50)
				return -2;
			imu_spi_reads(MPU9250_SPIx_ADDR, MPU9250_I2C_MST_STATUS, 1, &status);
			delay(2);
		} while ((status & MPU9250_I2C_SLV4_DONE) == 0);
		if (status & MPU9250_I2C_SLV4_NACK)
			return -3;
		index++;
	}
	return 0;
}

int imu_spi_write(uint8_t addr, uint8_t reg_addr, uint8_t data)
{
  spiSetCS(MPU_SPI_CH, _DEF_LOW);
	spiTransfer8(MPU_SPI_CH, reg_addr);
	spiTransfer8(MPU_SPI_CH, data);
	spiSetCS(MPU_SPI_CH, _DEF_HIGH);
	return 0;
}

uint8_t imu_spi_read(uint8_t addr, uint8_t reg_addr)
{
	uint8_t dummy = 0;
	uint8_t data = 0;

	spiSetCS(MPU_SPI_CH, _DEF_LOW);
	spiTransfer8(MPU_SPI_CH, 0x80 | reg_addr);
	data = spiTransfer8(MPU_SPI_CH, dummy);
	spiSetCS(MPU_SPI_CH, _DEF_HIGH);
	return data;
}

int imu_spi_ak8963_write(uint8_t akm_addr, uint8_t reg_addr, uint8_t data)
{
  uint8_t param[1];

  param[0] = data;

  return imu_spi_ak8963_writes(akm_addr,reg_addr, 1, param);
}
