#include "LCD.h"
#include "ADC.h"

void lcd_delayus(uint16_t us)       //blocking delay for LCD, argument is approximate number of micro-seconds to delay
{
    uint8_t i;
    while(us--) {
        for(i=0; i<SystemCoreClock/4000000; i++);
    }
}

void WaitLcdBusy(void)
{
    lcd_delayus(3000);      //3ms blocking delay
}

void maskinginterupt(void)
{
    LCD_PORT->MODER&=~(3u<<(2*RXNEbit));
    LCD_PORT->MODER|=(0u<<2*RXNEbit);

    Enable_Flag();  //R/W and E high and RS low
    Masking();

    uint16_t flag=0x00000000;
    flag=Masking();

    clr_LCD_E();        //send E low

    while(flag>0) { //loop till interupt by checking conditions
        set_LCD_E();
        Masking();
        flag=Masking();
        clr_LCD_E();
    }

    LCD_PORT->MODER|=(1u<<(2*RXNEbit));
}

void set_LCD_data(uint8_t d)
{
    LCD_PORT->BSRR=(0xff<<(LCD_D0_pin+16)); //clear data lines
    LCD_PORT->BSRR=(d<<LCD_D0_pin);         //put data on lines
}

void set_LCD_dataLSB(uint8_t d)
{
    d&=0x0f;
    LCD_PORT->BSRR=(0xff<<(LCD_D0_pin+16)); //clear data lines
    LCD_PORT->BSRR=(d<<LCD_D4_pin);                 //put data on lines
}

void set_LCD_dataMSB(uint8_t d)
{
    d&=0xf0;
    LCD_PORT->BSRR=(0xff<<(LCD_D0_pin+16)); //clear data lines
    LCD_PORT->BSRR=(d<<LCD_D0_pin);                 //put data on lines
}


void LCD_strobe(void)       //10us high pulse on LCD enable line
{
    lcd_delayus(10);
    set_LCD_E();
    lcd_delayus(10);
    clr_LCD_E();
}

void cmd4LCD(uint8_t cmd)       //sends a byte to the LCD control register
{
    WaitLcdBusy();              //wait for LCD to be not busy
    clr_LCD_RS();                   //control command
    clr_LCD_RW();                   //write command
    set_LCD_dataMSB(cmd);       //set data on bus
    LCD_strobe();                   //apply command
    set_LCD_dataLSB(cmd);   //set data on bus
    LCD_strobe();                   //apply command
}

void putLCD(uint8_t put)    //sends a char to the LCD display
{
    maskinginterupt();              //wait for LCD to be not busy
    set_LCD_RS();                   //text command
    clr_LCD_RW();                   //write command
    set_LCD_data(put);      //set data on bus
    LCD_strobe();                   //apply command
    set_LCD_dataLSB(put);       //set data on bus
    LCD_strobe();                   //apply command
}

void initLCD(void)
{
    SystemCoreClockUpdate();
    RCC->AHB1ENR|=RCC_AHB1ENR_GPIODEN;  //enable LCD port clock


    //CONFIG LCD GPIO PINS
    LCD_PORT->MODER&=~(                 //clear pin direction settings
                         (3u<<(2*LCD_RS_pin))
                         |(3u<<(2*LCD_RW_pin))
                         |(3u<<(2*LCD_E_pin))
                         |(0xffff<<(2*LCD_D0_pin))
                     );


    LCD_PORT->MODER|=(              //reset pin direction settings to digital outputs
                         (1u<<(2*LCD_RS_pin))
                         |(1u<<(2*LCD_RW_pin))
                         |(1u<<(2*LCD_E_pin))
                         |(0x5500<<(2*LCD_D0_pin))
                     );


    //LCD INIT COMMANDS
    clr_LCD_RS();                   //all lines default low
    clr_LCD_RW();
    clr_LCD_E();


    lcd_delayus(25000);     //25ms startup delay

    set_LCD_data(0x30); //Function set: 1 Line, 8-bit, 5x8 dots 3ms
    LCD_strobe();
    lcd_delayus(50);        //wait 39us

    set_LCD_data(0x20); //Function set: 1 Line, 4-bit, 5x8 dots
    LCD_strobe();
    lcd_delayus(50);    //wait 39us
    LCD_strobe();
    lcd_delayus(50);    //wait 39us


    cmd4LCD(0x28);  //Function set: 1 Line, 4-bit, 5x8 dots
    cmd4LCD(0x0c);  //Display on, Cursor blinking command
    cmd4LCD(0x01);  //Clear LCD
    cmd4LCD(0x06);  //Entry mode, auto increment with no shift

    cmd4LCD(LCD_LINE1);
}

void display_power(uint16_t average_adc)
{

    //48 is ascii for zero add 0 - 9 for ascii equivilent of what was added
    uint16_t column0 = 48;
    uint16_t column1 = 48;
    uint16_t column2 = 48;
    uint16_t column3 = 48;
    uint16_t column4 = 48;

    average_adc *= 1000; //push decimalpoint three place to the right

    //printf("LCD = %u", average_adc);//debug

    while(average_adc>10000) {   //count thousands column and create ascii equivilent
        average_adc -= 10000;
        column0++;
    }

    while(average_adc>1000) {   //count thousands column and create ascii equivilent
        average_adc -= 1000;
        column1++;
    }

    while(average_adc>100) {    //count hundreds column and create ascii equivilent
        average_adc -= 100;
        column2++;
    }

    while(average_adc>10) {     //count tens column and create ascii equivilent
        average_adc -= 10;
        column3++;
    }

    while(average_adc>1) {      //count singles column and create ascii equivilent
        average_adc -= 1;
        column4++;
    }

    cmd4LCD(LCD_LINE1);

    putLCD('P');
    putLCD('o');
    putLCD('w');
    putLCD('e');
    putLCD('r');
    putLCD(' ');
    putLCD('=');
    putLCD(' ');

    //print average power val to 3dp
    putLCD(column0);
    putLCD(column1);
    putLCD(46);     //ascii .
    putLCD(column2);
    putLCD(column3);
    putLCD(column4);

}

uint16_t Find_motor_angle(uint16_t motor)
{

    //convert a value of pulses into degrees
    float angle = motor/1341; //this will find a multiply between 0 and 1 to find the range
    angle *= 160; //find its point between 0 and 160 degress

    return angle;
}

void Display_Angle()
{

    uint16_t angle = Read_motor_angle(); //read current angle of motor

    //48 is ascii for zero add 0 - 9 for ascii equivilent of what was added
    uint16_t column5 = 48;
    uint16_t column6 = 48;
    uint16_t column7 = 48;

    //function to convert pulses to theta
    uint16_t motor = Find_motor_angle(angle);

    while(motor>100) {   //count thousands column and create ascii equivilent
        motor -= 100;
        column5++;
    }

    while(motor>10) {   //count thousands column and create ascii equivilent
        motor -= 10;
        column6++;
    }

    while(motor>1) {    //count hundreds column and create ascii equivilent
        motor -= 1;
        column7++;
    }

    //move cursor to second line of LCD
    cmd4LCD(LCD_LINE2);

    putLCD('A');
    putLCD('n');
    putLCD('g');
    putLCD('l');
    putLCD('e');
    putLCD(' ');
    putLCD('=');
    putLCD(' ');

    //print val
    putLCD(column5);
    putLCD(column6);
    putLCD(column7);
}