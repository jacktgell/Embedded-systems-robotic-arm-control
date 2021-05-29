#include "mbed.h"
#include "ADC.h"

static SPI spi(PA_7, PA_6, PA_5);      // Ordered as: mosi, miso, sclk could use forth parameter ssel
static DigitalOut cs(PC_6);            // Chip Select for Basic Outputs to illuminate Onboard FPGA DEO nano LEDs CN7 pin 1

uint16_t sum_samples()
{

    uint16_t buffer = 0;
    buffer = space_avai();

    printf(" SPACE AVAILABLE = %u\n\r", buffer);

    uint32_t summed_sample = 0;
    uint16_t samples = 0;
    uint16_t adc_sample = 0;

    buffer = 9600 - buffer;

    uint16_t sample_batch = 0;

    //do{
    if(buffer>20) {
        for(int i = 0; i<20; i++) {
            adc_sample = ADC_Sample();

            if(adc_sample!=0) {
                summed_sample += adc_sample;
                samples++;
                //printf(" Average sample = %u\n\r ", adc_sample);
                sample_batch++;
            }
        }
    }
    //}while(samples<buffer&&adc_sample!=0);

    printf(" Average sample = %u ", adc_sample);
    printf(" samples per loop = %u ", sample_batch);

    summed_sample /= samples;

    uint16_t average_sample = summed_sample;
    return average_sample;
}

uint16_t space_avai()
{

    uint16_t reg = 0xc000;
    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4008);
    cs = 1;             //De-Select the device by seting chip select HIGH

    while (reg == 0xc000) {
        cs = 0;
        reg = spi.write(0x4008) & 0xffff;               //Send the command
        cs = 1;             //De-Select the device by seting chip select HIGH
    }

    return reg;
}

uint16_t ADC_Sample()
{

    uint16_t reg = 0xc000;

    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4004);
    cs = 1;             //De-Select the device by seting chip select HIGH
    reg = 0xc000;

    while (reg == 0xc000) {
        cs = 0;
printf(" im stuck = %\n\r");//debug
        reg = spi.write(0x4004) & 0xffff;               //Send the command
        cs = 1;             //De-Select the device by seting chip select HIGH
    }

    reg = (reg & 0x0fff);

    return reg;
}

void init_spi()
{

    cs = 1;                     // Chip must be deselected, Chip Select is active LOW
    spi.format(16,3);            // Setup the DATA frame SPI for 16 bit wide word, Clock Polarity 0 and Clock Phase 0 (0)
    spi.frequency(3000000);     // 1MHz clock rate

    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4001);              //Send the command
    cs = 1;             //De-Select the device by seting chip select HIGH
    wait_ms(100);

    Start_Sampling();

    wait_ms(100);
}

void Motor_Control(uint16_t average_adc)
{

    float steps_per_bit = 0.3274;    //break down the adc value for one encoder pulse
    uint16_t position_target = steps_per_bit*average_adc; //multiply by adc val to get accurate number of pulses

printf(" 3 = %\n\r");//debug
    //printf(" Target position = %u/3141 ", position_target); //debug
    uint16_t Current_angle = Read_motor_angle();    //check where the motor is at this time

printf(" 4 = %\n\r");//debugdebug
    if(Current_angle!=position_target)  //will only send the position to the FPGA if there is a diffrence.
        Write_Motor_Angle(position_target);

printf(" 5 = %\n\r");//debugebug
}

void Write_Motor_Angle(float position_target)
{

    uint16_t Motor_address = 0x7000; //register address
    Motor_address += position_target; //adding register address as signature

    cs = 0;             //Select the device by seting chip select LOW
    spi.write(Motor_address);   //send angle with address
    cs = 1;             //De-Select the device by seting chip select HIGH
}

void Start_Sampling()
{

    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4002);
    cs = 1;             //De-Select the device by seting chip select HIGH
}

uint16_t adc_average(uint16_t adc_array[])
{

    uint16_t avg, sum = 0;

    for (uint8_t i = 0; i < 100; ++i) {
        sum += adc_array[i];
    }

    avg = (sum / 100);

    return avg;
}

uint16_t time_average(uint16_t time_array[])
{

    uint16_t avg, sum = 0;

    for (uint8_t i = 0; i < 100; ++i) {
        sum += time_array[i];
    }

    avg = (sum / 100);

    return avg;
}

void init_motor()
{

    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x6000);
    cs = 1;             //De-Select the device by seting chip select HIGH
}

uint16_t Read_motor_angle()
{

    uint16_t reg = 0xc000;

    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x5000);
    cs = 1;             //De-Select the device by seting chip select HIGH
    reg = 0xc000;
printf(" 4 = %\n\r");//debug
    while (reg == 0xc000) {
        printf(" 5 = %\n\r");//debug
        cs = 0;
        reg = spi.write(0x5000) & 0xffff;               //Send the command
        cs = 1;             //De-Select the device by seting chip select HIGH
    }
printf(" 6 = %\n\r");//debug
    reg = (reg & 0x0fff);

    return reg;
}

uint16_t Check_overflow()
{

    uint16_t Fifo_state;
    Fifo_state = Fifo_config();

    Fifo_state = (Fifo_state<<9u);
    Fifo_state = (Fifo_state>>15u);

    if(Fifo_state==1)
        return 1;
    else
        return 0;
}

void Reset()
{

    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4001);
    cs = 1;             //De-Select the device by seting chip select HIGH
}

uint16_t Fifo_config()
{

    uint16_t reg = 0xc000;
    cs = 0;             //Select the device by seting chip select LOW
    spi.write(0x4004);
    cs = 1;             //De-Select the device by seting chip select HIGH

    while (reg == 0xc000) {
        cs = 0;
        reg = spi.write(0x4004) & 0xffff;               //Send the command
        cs = 1;             //De-Select the device by seting chip select HIGH
    }

    return reg;
}

