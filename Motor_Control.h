#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <stm32f4xx.h>
#include "mbed.h"

uint32_t Read_Encoder_Position();
void Break();
void Reverse();
void Forward();
void Write_Velocity(uint16_t Pwm_low_time, uint16_t Pwm_raise_time);
void Motor_control(uint16_t ADC_Data, uint16_t Old_Motor_Data, uint16_t arm_Position);
void Write_Move_CMD(int16_t disance_to_move);   

#endif
