#ifndef _MAIN_H_
#define _MAIN_H_

#include <stm32f4xx.h>
#include "mbed.h"
#include "SPI.h"
#include "LCD.h"

#define Buffer_size 256
#define Voltage_per_bit 0.000805664062

static DigitalOut cs(PC_6);            // Chip Select for Basic Outputs to illuminate Onboard FPGA DEO nano LEDs CN7 pin 1
static DigitalOut LCD_cs(PB_15);       // Chip Select for the LCD via FPGA CN7 pin 3
static DigitalOut ADC_cs(PB_9);        // Chip Select for the ADC via FPGA CN7 pin 4
static InterruptIn User_Button(PA_0);
static SPI spi(PA_7, PA_6, PA_5);      // Ordered as: mosi, miso, sclk could use forth parameter ssel
static SPI spi_cmd(PC_2, PC_3, PC_6);  // NB another instance call spi_cmd for 8 bit SPI dataframe see later line 37
static uint16_t Space_Available;

float Calculate_Power(uint16_t Summed_ADC_Values, uint16_t Duty_Cycle, uint16_t N_ADC_Samples);
void float_to_string(float fPower);
int32_t lcd_cls(void);              //LCD Functions here, Clear Screen, Locate and Display String
int32_t lcd_locate(uint8_t line, uint8_t column); //Line Max is 2, Column max is 16
int32_t lcd_display(char* str);     //String str length maximum is 16

#endif
