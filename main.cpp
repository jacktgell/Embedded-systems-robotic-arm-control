#include "mbed.h"
#include "LCD.h"
#include "ADC.h"

//NBB the following line for F429ZI !!!!
DigitalIn DO_NOT_USE(PB_12);    // MAKE PB_12 (D19) an INPUT do NOT make an OUTPUT under any circumstances !!!!! ************* !!!!!!!!!!!
// This Pin is connected to the 5VDC from the FPGA card and an INPUT is 5V Tolerant

DigitalIn sw1(USER_BUTTON); //blue user button

int main()
{

    //set timers
    //Timer t;    //debug timer
    Timer Debounce; //debounce user button
    Timer LCD;  //set refresh frequeny of LCD screen
    Timer adc_sampling; //time between samples

    //declare variable with main scope
    uint16_t average_time = 0;   //interger that stores the average of the summed timer values
    uint16_t average_adc = 0;   //interger that stores the average of the summed adc values
    uint16_t adc_array[99];         //array to hold all samples in one dimension
    uint16_t time_array[99];        //array to hold the time taken for each sample in one dimension
    uint16_t adc_sample = 0;        //contains a single adc value before passing into array
    uint16_t sample_n = 0;          //adc_array's pointer
    uint16_t sample_time = 0;       //time_array's pointer
    uint16_t space_availiable = 0; //free space inside the buffer
    uint16_t power = 0;

    bool Fifo_state = false;

    for(uint8_t i = 0; i<100; i++) {   //set all array element to zero
        adc_array[i] = 0;
    }

    for(uint8_t i = 0; i<100; i++) {   //set all array element to zero
        time_array[i] = 0;
    }

    bool debounce = true;   //bool to be able to tell if the userbutton has been debounced

    //initialisation section before while loop
    init_spi();
    initLCD();
    init_motor();

    //inits have delays so starting timer right before going into main while loop
    adc_sampling.start();
    //t.start();  //start debug timer
    LCD.start();    //start LCD refresh timer

    while(true) {
        //debugging section
        //t.reset();  //debug
        //printf(" The time taken was %f seconds", t.read()); //debug
        //printf("ADC SAMPLE = %u", adc_sample);//debug
        //printf(" ADC AVE = %u", average_adc);//debug
        //printf(" TIME AVE = %u", average_time);//debug
        //printf(" POWER = %u", power);//debug
        //printf(" SPACE AVAILABLE = %u\n\r", space_availiable);//debug

        space_availiable = space_avai();    //check free space is the buffer
//printf(" SPACE AVAILABLE = %u\n\r", space_availiable);//debug

        if(space_availiable>0) {

printf(" 4 = %\n\r");//debug
            adc_sample = ADC_Sample();    //takes samples from the buffer and returns a average
printf(" 5 = %\n\r");//debug
            adc_array[sample_n] = adc_sample; //sets a element of the array to the value of a single sample
            sample_n++; //point to the next element of the array for the next time this loops

printf(" 6 = %\n\r");//debug
            if(sample_n==100)//when the array reaches its last element loop round and start overwriting the..
                sample_n = 0;//oldest samples with the newest samples

            time_array[sample_time] = adc_sampling.read_us();//stores length of time taken per loop (roughly time between adc samples)
            adc_sampling.reset();//reset timer to zero for next loop
            sample_time++;//point to the next element of the array of time values

            if(sample_time==100)//when the array reaches its last element loop round and start overwriting the..
                sample_time = 0;//oldest times with the newest samples

            average_adc = adc_average(adc_array);//calculate the average of the array and return to a uint16_t
            average_time = time_average(time_array);//calculate the average of the array and return to a uint16_t
            power = average_adc*average_time;//multiply by one another to find the area underneath the signal as if was a graph
        }
printf(" 1 = %u\n\r", space_availiable);//debug

        Motor_Control(average_adc);     //generates a value for fpga to count towards on the optical encoder

printf(" 2 = %u\n\r", space_availiable);//debug
        if(LCD.read_ms()>50) { //LCD does not refresh more than 20Hz
            display_power(power);  //takes the average adc and pushing to the LCD screen
            Display_Angle();//push theta to LCD
            LCD.reset(); //reset timer for screen refresh rate
        }

        Fifo_state = Fifo_config();
printf(" 3 = %u\n\r", space_availiable);//debug

        if(Fifo_state==true) {

            while(sw1==false) {
                ;
            }

            if(debounce==true) { //if user button is pressed and that it is not bouncing

                Reset();
                Start_Sampling();   //tell fpga to initiate sampling

                debounce = false;   //set to false for a period of time to debounce
                Debounce.start();   //start debounce timer
            }
        }

        if(Debounce.read_ms()>=50) {   //when debounce timer hits 50ms condition becomes true

            debounce = true;    //becomes true atleast 50ms after the userbutton has been pressed
            Debounce.stop();    //stop timer till user button is pressed again
            Debounce.reset();   //return timer to zero
        }
    }
}
