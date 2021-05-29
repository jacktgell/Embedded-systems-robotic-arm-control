#ifndef _LCD_H_
#define _LCD_H_
#define LCD_PORT    GPIOD
#define LCD_RS_pin  11
#define LCD_RW_pin  12
#define LCD_E_pin       13
#define RXNEbit         7

#define LCD_D0_pin  0
#define LCD_D4_pin  4


#define LCD_LINE1       0x80
#define LCD_LINE2       0xc0

#define set_LCD_RS()    LCD_PORT->BSRR=(1u<<LCD_RS_pin)
#define clr_LCD_RS()    LCD_PORT->BSRR=(1u<<(LCD_RS_pin+16))
#define set_LCD_RW()    LCD_PORT->BSRR=(1u<<LCD_RW_pin)
#define clr_LCD_RW()    LCD_PORT->BSRR=(1u<<(LCD_RW_pin+16))
#define set_LCD_E()     LCD_PORT->BSRR=(1u<<LCD_E_pin)
#define clr_LCD_E()     LCD_PORT->BSRR=(1u<<(LCD_E_pin+16))
#define Enable_Flag() LCD_PORT->BSRR=(1u<<(LCD_RS_pin+16))|(1u<<(LCD_RW_pin))|(1u<<(LCD_E_pin))

#define LCD_CLR()       cmdLCD(0x01)

#define set_LCD_bus_input()     LCD_PORT->MODER&=~(0xffff<<(2*LCD_D0_pin))
#define set_LCD_bus_output()    LCD_PORT->MODER|=(0x5555<<(2*LCD_D0_pin))
#define Masking()   LCD_PORT->IDR&=(1u<<RXNEbit)

#include <stm32f4xx.h>

uint16_t Find_motor_angle(uint16_t motor);
void Display_Angle();
void lcd_delayus(uint16_t us);
void WaitLcdBusy(void);
void set_LCD_data(uint8_t d);
void LCD_strobe(void);
void putLCD(uint8_t put);
void initLCD(void);
void cmd4LCD(uint8_t cmd);
void set_LCD_dataLSB(uint8_t d);
void set_LCD_dataMSB(uint8_t d);
void delay_milli(uint32_t msec);
void display_power(uint16_t average_adc);

#endif
