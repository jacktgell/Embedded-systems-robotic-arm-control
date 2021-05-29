#include <stm32f4xx.h>
#include "main.h"
#include "mbed.h"
#include "LCD.h"
#include "Motor_Control.h"
#include "SPI.h"

//calculation for power following the formula for V^2/R = power
//the ADC being out Power out put and the duty cycle scaling it down. (eg if we have 10 Powers with a 60% duty we could
//therefor call the Power value 6 Powers)
float Calculate_Power(uint16_t ADC_DATA, int16_t Power_Time){
    
    //when the buffer unloads i need to find the average power over the time that the buffer has been filling
    int16_t Free_buf_avai = Avaliable();
    int16_t N_ADC_Samples = Buffer_size - Free_buf_avai;
    int16_t average_ADC = ADC_DATA/N_ADC_Samples;
    int16_t Voltage = Voltage_per_bit*average_ADC;
    int16_t joules = (Voltage*Voltage)/resistance;
    float watts = joules*Power_Time*10^-3;
    
    return watts;
}

void float_to_string(float watts){
    
    uint16_t newpower = watts * 1000;
    uint8_t column_1 = 48;
    uint8_t column_2 = 48;
    uint8_t column_3 = 48;
    uint8_t column_4 = 48;
        
    while (newpower >= 1000){column_1++;    newpower = newpower - 1000;}
    LCD_cs = 0;
  spi_cmd.write(0);
  spi.write((1u<<8)+column_1);
  LCD_cs = 1;
    wait_us(DD);
    
    LCD_cs = 0;
  spi_cmd.write(0);
  spi.write((1u<<8)+46); // 46 = "." in ascii
  LCD_cs = 1;
    wait_us(DD);
    
    while (newpower >= 100){column_2++; newpower = newpower - 100;}
    LCD_cs = 0;
  spi_cmd.write(0);
  spi.write((1u<<8)+column_2);
  LCD_cs = 1;
    wait_us(DD);
    
    while (newpower >= 10){column_3++;  newpower = newpower - 10;}  
    LCD_cs = 0;
  spi_cmd.write(0);
  spi.write((1u<<8)+column_3);
  LCD_cs = 1;
    wait_us(DD);
    
    while (newpower >= 1){column_4++;   newpower = newpower - 1;}
    LCD_cs = 0;
  spi_cmd.write(0);
  spi.write((1u<<8)+column_4);
  LCD_cs = 1;
}

int lcd_cls(void){
    LCD_cs = 0;spi_cmd.write(0);spi.write(0x0001);LCD_cs = 1;wait_us(CD);    //Clear Screen
    return 0;
}

int lcd_locate(uint8_t line, uint8_t column){
    uint8_t line_addr;
    uint8_t column_addr;
    switch(line){
        case 1: line_addr=0x80; break;
        case 2: line_addr=0xC0; break;
        default: return -1; //return code !=0 is error
    }
        
    if(column<16){column_addr=column;}
    else{return -1;}
    LCD_cs = 0;
    spi_cmd.write(0);
    spi.write(line_addr+column_addr);
    LCD_cs = 1;
    wait_us(CD); //DDRAM location Second line is 0x00C0 first line starts at 0x0080
    return 0;
}

void Display_Power(uint16_t ADC_DATA,int16_t Power_Time){
    
    lcd_cls();
    lcd_locate(1,1);
    float watts = Calculate_Power(ADC_DATA, Power_Time);
    float_to_string(watts);
    
}



