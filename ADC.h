#ifndef _ADC_H_
#define _ADC_H_

#include "mbed.h"

void Reset();
void init_motor();
uint16_t Read_motor_angle();
uint16_t Fifo_config();
void init_spi();
uint16_t space_avai();
uint16_t sum_samples();
uint16_t ADC_Sample();
void Motor_Control(uint16_t average_adc);
void Write_Motor_Angle(float position_target);
void Start_Sampling();
uint16_t adc_average(uint16_t adc_array[]);
uint16_t time_average(uint16_t time_array[]);

#endif


