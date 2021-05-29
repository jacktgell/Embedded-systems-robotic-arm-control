#ifndef _SPI_H_
#define _SPI_H_

#include <stm32f4xx.h>
#include "mbed.h"

int read_adc(void);
int read_switches(void);
uint16_t SPI_Read(void);
uint16_t Spi_Flag(uint16_t Flag);
void Reset();
uint16_t Sample();
uint16_t ADC_EN_DIS();
uint16_t Space_Avai_En_Dis();
void Overflow();
uint16_t Avaliable();
void Write_CMD_To_FPGA(uint8_t CMD);
uint16_t Write_ADC(uint16_t ADC_v);

#endif

