#include <stm32f4xx.h>
#include "main.h"
#include "mbed.h"
#include "LCD.h"
#include "Motor_Control.h"
#include "SPI.h"

void Motor_control(uint16_t ADC_Data, uint16_t Old_Motor_Data, uint16_t arm_Position){
    
    uint16_t Pwm_raise_time;
    uint16_t Pwm_low_time;
    uint16_t Motor_Position;
    uint16_t Target_position;
    uint16_t Upper_parameter;   //to be defined as a constant
    uint16_t Lower_parameter; //to be defined as a constant
    int16_t disance_to_move;
    uint32_t target_and_current_positions;
    
    target_and_current_positions = Read_Encoder_Position();
    
    Target_position = (target_and_current_positions&0x0000FFFF);
    Motor_Position = (target_and_current_positions>>16);
    
    disance_to_move = Motor_Position - Target_position;
    
    if(disance_to_move<0){
        disance_to_move*=-1;
        Reverse();
    }
    
    if(Target_position>=Upper_parameter){
        Target_position=Upper_parameter;
    }
        
    if(Target_position<=Lower_parameter){
        Target_position=Lower_parameter;
    }
    
    //velocity control
    Pwm_raise_time = ADC_Data/0xFFFF;       //a whole pwm cycle will always have be the same length of time
    Pwm_low_time = 1 - Pwm_raise_time;  //by minusing the desired up time offsetof the full cycle potential we can_filter range the pwm more predictably
    
    Write_Move_CMD(Target_position);    
    Write_Velocity(Pwm_low_time, Pwm_raise_time);
}

void Write_Move_CMD(int16_t disance_to_move){
    
}

uint32_t Read_Encoder_Position(){
    
    uint16_t Motor_Position;
    uint16_t Target_position;
    uint32_t return_target_and_current;
    
  cs = 0;
  spi_cmd.write(0);
  Motor_Position = spi.write(0x0000)&0x0FFF; // taking 12 bits
  cs = 1;
    
    cs = 0;
  spi_cmd.write(0);
  Target_position = spi.write(0x0000)&0x0FFF; // taking 12 bits
  cs = 1 ;
    
    return_target_and_current = (Motor_Position<<16)+Target_position;
    
    return return_target_and_current;
}

void Reverse(){
    
    cs = 0;
  spi_cmd.write(0);
  spi.write(0x0000); //command to change direction of current which goes through the h bridge
  cs = 1;
    
}

void Forward(){
    
    cs = 0;
  spi_cmd.write(0);
  spi.write(0x0000); //command to change direction of current which goes through the h bridge
  cs = 1;
    
}

void Write_Velocity(uint16_t Pwm_low_time, uint16_t Pwm_raise_time){
    
    cs = 0;
  spi_cmd.write(0);
  spi.write(Pwm_raise_time);
  cs = 1;
    
    cs = 0;
  spi_cmd.write(0);
  spi.write(Pwm_low_time);
  cs = 1;
    
}
