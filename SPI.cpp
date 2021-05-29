#include <stm32f4xx.h>
#include "main.h"
#include "mbed.h"
#include "SPI.h"
#include "LCD.h"
#include "Motor_Control.h"

int read_adc(void){
    int adval_d;
    float adval_f;
    ADC_cs = 0;
    adval_d = spi.write(0x0000);
    ADC_cs =1 ;
    adval_f = 3.3f*((float)adval_d/4095);
    printf("%d %.3fV\r\n",adval_d,adval_f);
    return adval_d;    
}


int read_switches(void){
    int sw_val;
    cs = 0;
    spi_cmd.write(0);
    sw_val = spi.write(0x00)&0x0F; // Just want lower 4bit nibble
    cs = 1 ;
    if (sw_val&(1<<0)){printf("Switch 0 :");}
    if (sw_val&(1<<1)){printf("Switch 1 :");}
    if (sw_val&(1<<2)){printf("Switch 2 :");}
    if (sw_val&(1<<3)){printf("Switch 3 :");}
    if (sw_val>0){printf("\r\n");}
    return sw_val;    
}


uint16_t SPI_Read(void){

    uint16_t data; //16 bit value to be received from the fpga
    
    cs = 0;
    spi_cmd.write(0);
    data = spi.write(0x0000)&0xFFFF; // taking 16 bits
    cs = 1 ;

    return data;
}

//spi functions

uint16_t Spi_Flag(uint16_t nag){
    
    uint16_t Flag = 0xC000;
    
    while(Flag==0xC000){
        cs = 0;
        Flag = spi.write(nag)&0xFFFF; 
        cs = 1;
    }
    
    Flag &= 0x0fff;
    
    return Flag;
}

void Write_CMD_To_FPGA(uint8_t CMD){
//possible values for CMD
// 1 = RESET        2 =     SAMPLE      4 = Enable read ADC samples         8 = Enable read space avaliable

    uint16_t Send_CMD = 0x4000+CMD;
    spi_cmd.write(Send_CMD);
}

uint16_t Write_ADC(uint16_t ADC_v){
    
    uint16_t ADC_val = 0;
    cs = 0;
    spi.write(0x8004);
    cs =1 ;
    
    return ADC_val;
}

void Reset(){//resets the adc sampling, no return data
    
    cs = 0;
    spi.write(0x4001);
    cs =1 ;
    
}

uint16_t Sample(){//reads adc sample
    
    cs = 0;
    spi.write(0x4004);
    cs =1 ;
    uint16_t avai = Spi_Flag(0x4004);
    
    return avai;
    
}

uint16_t ADC_EN_DIS(){//enable sampling
    
    cs = 0;
    spi.write(0x4002);
    cs =1 ;
    int16_t avai = Spi_Flag(0x4002);
    return avai;
    
}
uint16_t Space_Avai_En_Dis(){//space left in the fifo buffer
    
    cs = 0;
    spi.write(0x4008);
    cs =1 ;
    uint16_t avai = Spi_Flag(0x4008);
    return avai;
    
}
void Overflow(){}

uint16_t fifo_config(){//contains 
    
    cs = 0;
    spi.write(0x4010);
    cs =1 ;
    uint16_t config = Spi_Flag(0x4010);
    return config;
    
}

uint16_t Avaliable(){
 return 0;   
}


    
