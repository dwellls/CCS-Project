/*Author(s): Daivd Brinkmeier & Daniel Weller
 * Description: Print the room temperature using a temp. sensor to the LCD screen
 * Date: 08/25/2018
 * Lab Instructor: Prof. Zuidema
 * EGR-226
 */


#include "msp.h"
#include <stdio.h>

#define RS BIT1     /* P4.3 mask */
#define RW BIT2     /* P4.1 mask */
#define EN BIT3     /* P4.2 mask */

void SysTick_init(void);                //SysTick initialization
void SysTick_init_interrupt(void);      //SysTick interrupt initialization
void SysTick_Handler(void);             //SysTick Handler
void delay_ms(uint16_t ms);             //SysTick timer in milliseconds
void PortADC_init(void);                //ADC port initialization
void ADC14_init(void);                  //ADC initialization
void LCD_nibble_write(unsigned char data, unsigned char control); //sends bits to LCD screen
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void); //initialize LCD

volatile uint32_t timeout;

int main(void)
{
    static volatile uint16_t result; //raw values for ADC
    float nADC, temp_c, temp_f; //initialize variables for temperature & LCD
    char first[] = "Current Temp is:"; //saying saved as array to send to LCD
    char temp[60]; //temp array
    int i; //increment variable

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     //Stop watchdog timer

    SysTick_init();             //Function call to initialize SysTick timer
    SysTick_init_interrupt();   //Function call to initialize SysTicker timer with interrupt
    PortADC_init();             //Function call to initialize ADC port
    ADC14_init();               //Function call to initialize ADC
    LCD_init();

    __enable_irq();             //Enable global interrupts

    while(1)
    {
        if(timeout)
        {
            ADC14->CTL0 |= ADC14_CTL0_SC;       //Start conversation
            while(!ADC14->IFGR0 & BIT0);        //Wait for conversation to complete
            result = ADC14->MEM[0];             //Get the value from the ADC
            nADC = ((result * 3.3) / 16384);

            temp_c = (((nADC * 1000) - 500) / 10); //temperature conversion using ADC to degrees C
            temp_f = ((temp_c * 9 / 5) + 32); //temperature conversion from degrees C to degrees F

            sprintf(temp,"%.1f", temp_c); //prints temp

            LCD_command(0x80); //prints "Current Temp. is to the LCD"
            for (i=0; i<16; i++)
            {
                LCD_data(first[i]);
            }
            LCD_command(0x90); //prints temperature to the LCD screen
            for (i=0; i<5; i++)
            {
                 LCD_data(temp[i]);
            }
            LCD_data(0xDF); //prints a degree symbol to the LCD screen
            LCD_data('C'); //degrees C
        }
    }
}

//SysTick initialization
void SysTick_init(void)
{
    SysTick->CTRL=0;                //Disable SysTick during step
    SysTick->LOAD=0x00FFFFFF;       //Max reload value
    SysTick->VAL=0;                 //Resets
    SysTick->CTRL=0x00000005;       //Enable SysTick, 3MHz, no interrupts
}

//SysTick interrupt initialization
void SysTick_init_interrupt(void)
{
    SysTick->CTRL=0;                //Sisable SysTick during step
    SysTick->LOAD=1500000;          //Reload value for 0.5 sec interrupts
    SysTick->VAL=0;                 //Any write to current clears it
    SysTick->CTRL=0x00000007;       //Enable SysTick, 3MHz, with interrupts
}

//Systick Handler
void SysTick_Handler(void)
{
    timeout = 1;    //Set flag for timeout of SysTick, rest in main
}

//SysTick timer in milliseconds
void delay_ms(uint16_t ms)
{
    SysTick->LOAD=((ms*3000)-1);                //Delay for 1 millisecond per delay value
    SysTick->VAL=0;                             //Resets
    while((SysTick->CTRL & 0x00010000) == 0);     //Wait for flag to be set
}

//ADC port initialization
void PortADC_init(void)
{
    P5->SEL0 |= BIT5;       //Select ADC14 on P5.5
    P5->SEL1 |= BIT5;
}

//ADC initialization
void ADC14_init(void)
{
    ADC14->CTL0 &=~ ADC14_CTL0_ENC;         //Disable ADC converter during initialization
    ADC14->CTL0 |=  0x04200210;             //S/H pulse mode, SMCLK, 16 sample clocks
    ADC14->CTL1  =  0x00000030;             //14-bit resolution
    ADC14->CTL1 |=  0x00000000;             //Convert for mem0 register
    ADC14->MCTL[0] = 0x00000000;            //ADC14INCHx = 0 for mem[0]
    ADC14->CTL0 |=  ADC14_CTL0_ENC;         //Enable ADC14ENC, Starts the ADC after confg.
 }
void LCD_init(void) {
    P4->DIR = 0xFF;     /* make P4 pins output for data and controls */
   delay_ms(30);                /* initialization sequence */
    LCD_nibble_write(0x30, 0);
   delay_ms(10);
    LCD_nibble_write(0x30, 0);
   delay_ms(1);
    LCD_nibble_write(0x30, 0);
   delay_ms(1);
    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
   delay_ms(1);

    LCD_command(0x28);      /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x01);      /* clear screen, move cursor to home */
    LCD_command(0x0F);      /* turn on display, cursor blinking */
}

/* With 4-bit mode, each command or data is sent twice with upper
 * nibble first then lower nibble.
 */
void LCD_nibble_write(unsigned char data, unsigned char control) {
    data &= 0xF0;           /* clear lower nibble for control */
    control &= 0x0F;        /* clear upper nibble for data */
    P4->OUT = data | control;       /* RS = 0, R/W = 0 */
    P4->OUT = data | control | EN;  /* pulse E */
   delay_ms(10);
    P4->OUT = data;                 /* clear E */
    P4->OUT = 0;
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write(command << 4, 0);      /* then lower nibble */

    if (command < 4)
       delay_ms(4);         /* commands 1 and 2 need up to 1.64ms */
    else
       delay_ms(1);         /* all others 40 us */
}

void LCD_data(unsigned char data) {
    LCD_nibble_write(data & 0xF0, RS);    /* upper nibble first */
    LCD_nibble_write(data << 4, RS);      /* then lower nibble  */

   delay_ms(1);
}

