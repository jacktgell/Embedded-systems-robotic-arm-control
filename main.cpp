#include <stm32f4xx.h>
#include "mbed_config.h"
#include "mbed.h"
#include "LCD.h"
#include "Motor_Control.h"
#include "SPI.h"
#include "main.h"

#define ADC_High 1
#define ADC_Low 1
#define ADC_Range 0xFFF
#define steps_for_160_deg 1341

Timer switch_bounce;
Timer power;
Timer LCD_Refresh;

//global variables
uint16_t CMD = 0;
uint16_t ADC_DATA = 0;
uint16_t saved_data = 0;
uint8_t FIFO_data = 0;
uint16_t data = 0;
uint16_t ADC_Sample = 0;
uint16_t Address = 0;
uint16_t Steps_To_Take();

//Function Prototypes, will just output a string to display, see later
void init();
void ADC_Sample_Start();

//NBB the following line for F429ZI !!!!
DigitalIn DO_NOT_USE(PB_12);    // MAKE PB_12 (D19) an INPUT do NOT make an OUTPUT under any circumstances !!!!! ************* !!!!!!!!!!!
                                // This Pin is connected to the 5VDC from the FPGA card and an INPUT is 5V Tolerant

int main() {
    
    cs = 1;
    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4001);              //Send the command
    cs = 1;             //De-Select the device by seting chip select HIGH
    wait_ms(100);
    
    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4002);              //Send the command
    cs = 1;             //De-Select the device by seting chip select HIGH
    wait_ms(100);
    
    int16_t Power_Time = 0;
    int val = 0;
    init();
    
    while(true){
        uint16_t ste = Steps_To_Take();
        printf("%u \n\r" ,ste);
    }
      
  while(true)                 
  { 
        data = SPI_Read();
        
        Address = data>>12; //move MSNibble to LSNibble and clear all otherbits for testing
        data = data<<4; //clear MSNibble
        data = data>>4; //place all other bits back into their original position
        
        if(Address==12){    //using the MSB of what came over spi we can direct the data to a relevant function
            FIFO_data = data&0x00FF;
            
            switch(FIFO_data){
                case(1):
                    Reset();
                    break;
                case(2):
                    //Sample();
                    break;
                case(4):
                    ADC_EN_DIS();
                    break;
                case(8):
                    Space_Avai_En_Dis();
                    break;
                case(64):
                    Overflow();
                    break;
                case(128):
                    Avaliable();
                    break;
            }
            
            FIFO_data = 0;
        }   
        
        if(Address==13){    Space_Available = data; data= 0;}
        if(Address==14){    ADC_Sample = data; data= 0;}

// 1 = RESET        2 =     SAMPLE      4 = Enable read ADC samples         8 = Enable read space avaliable

        Write_CMD_To_FPGA(CMD); 

        if(ADC_DATA!=saved_data&&LCD_Refresh.read_ms()>=50){
            
            LCD_Refresh.reset();
            Power_Time = power.read_ms();
            Display_Power(ADC_DATA, Power_Time);
        }   
        
        if(switch_bounce.read_ms()>=50){
            __enable_irq();
            switch_bounce.stop();
            switch_bounce.reset();
        }
    }
}

void init(){
    
    cs = 1;                     // Chip must be deselected, Chip Select is active LOW
    LCD_cs = 1;                 // Chip must be deselected, Chip Select is active LOW
    ADC_cs = 1;                 // Chip must be deselected, Chip Select is active LOW
    spi.format(16,3);           // Setup the DATA frame SPI for 16 bit wide word, Clock Polarity 0 and Clock Phase 0 (0)
    //spi_cmd.format(16,3);        // Setup the COMMAND SPI as 8 Bit wide word, Clock Polarity 0 and Clock Phase 0 (0)
    spi.frequency(1000000);     // 1MHz clock rate
    //spi_cmd.frequency(1000000); // 1MHz clock rate
    LCD_Refresh.start();                // Timer so LCD fresh rate is capped at 20Hz
    Reset();
    power.start();       
    ADC_Sample_Start();                 // Timer to convert a joules to watts 
    //User_Button.rise(&ADC_Sample_Start);

}

//interupt function
void ADC_Sample_Start(){
    
    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4001);              //Send the command
    cs = 1;             //De-Select the device by seting chip select HIGH
    wait_ms(100);
    
    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4002);              //Send the command
    cs = 1;             //De-Select the device by seting chip select HIGH
    wait_ms(100);

}

uint16_t Steps_To_Take(){
    
    uint16_t Samples = 0; 
    uint32_t ADC_Sum = 0;
    uint16_t Buffer = Space_Avai_En_Dis();
    uint16_t adc_samp = 0;
    
    Buffer = 256-Buffer; //number of samples in buffer at this time
    
    while(Samples<Buffer){ //empty the buffer
        adc_samp = Sample();
        ADC_Sum += adc_samp;
        Samples++;
    }
    
    ADC_Sum /= Samples;  //take average of the adc
    
    ADC_Sum -= ADC_Low; //relate sum to range of operation
    uint16_t steps = float(ADC_Sum/ADC_Range)*steps_for_160_deg; //divide by adc range to make a value between 0 and 1 then divide by total steps to give refrence in range of steps
    return steps;
}
