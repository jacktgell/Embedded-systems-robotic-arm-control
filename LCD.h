#ifndef _LCD_H_
#define _LCD_H_

#include <stm32f4xx.h>
#include "mbed.h"
#include "Motor_Control.h"
#include "SPI.h"
#include "main.h"

#define DD 25                   //Display Delay us
#define CD 2000                 //Command Delay us
#define resistance 1    

float Calculate_Power(uint16_t ADC_DATA, int16_t Power_Time);
void Display_Power(uint16_t ADC_DATA, int16_t Power_Time);
void float_to_string(float watts);
int32_t lcd_cls(void);              //LCD Functions here, Clear Screen, Locate and Display String
int32_t lcd_locate(uint8_t line, uint8_t column); //Line Max is 2, Column max is 16
int32_t lcd_display(char* str);     //String str length maximum is 16

#endif

