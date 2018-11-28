/*
 * Authors: David Brinkmeier & Danny Weller
 * Class: EGR 226 - 908
 * Date of code beginning: 11/16/2018
 * Due date of Project: 12/7/2018
 * Professor Zuidema
 * Description: Source file for the Alarm Project
 */

//libraries for source file
#include "msp.h"
#include "string.h"
#include "Alarm_Header.h"

/*
 * This function initializes Systick timer for use in the code
 * Created with the help of Dr. Kandalaft's lecture slides 7
 */
void SysTick_Initialize(void)
{
    //Initialize systic timer
    SysTick -> CTRL = 0;
    SysTick -> LOAD = 0xBB8;
    SysTick -> VAL = 0;
    SysTick -> CTRL = 0x00000005;
}


/*
 * This function delays for one millisecond
 * Created with the help of Dr. Kandalaft's Lecture slides 7
 */
void SysTick_delay(uint16_t delay)
{
    //systick delay function
    SysTick -> LOAD = ((delay * 3000) - 1);
    SysTick -> VAL = 0;
    while ((SysTick -> CTRL & 0x00010000) == 0);
}

/*
 * This function is used to enable a button used as an interrupt for the button used to set clock to run 1:1 second real time
 * For the 1 second real time equals one minute clock time
 * For the Set time feature
 * For the Set Alarm feature
 * For ON/OFF/UP
 * These buttons are in PIN 6.0, 6.1, 6.4, 6.5, 6.6
 */
void BUTTONseconds(void)
{
    P6->SEL0 &=~ (BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);      //Sets Port 6 pins 0,1,4,5,6 as GPIO
    P6->SEL1 &=~ (BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);
    P6->DIR |= (BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);        //Sets Port 6 Pins 0,1,4,5,6 as output
    P6->REN = (BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);         //Enables pull resistors on Port 6 pins 0,1,4,5,6
    P6->OUT = (BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);         //Sets pull-up on Port 6 pins 0,1,4,5,6
    P6->IES = (BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);         //Set pin interrupt to trigger when it goes from high to low
    P6->IE = (BIT0|BIT1|BIT4|BIT5|BIT6|BIT7);          //Enable interrupts for Port 6 pins 0,1,4,5,6
    P6->IFG = 0;            //Clear all P6 interrupt flags
}

/*
 * This is the Interrupt handler for the Port 6 interrupts
 * The pins for interrupt on port 6 are 0, 1, 4, 5, 6
 */
void PORT6_IRQHandler(void)
{

    int status;               //Read status of Port 6

    SysTick_delay(1);

    //Small delay to help debounce
    status = P6->IFG;
    if(status & BIT0)              //Button press for 1:1 real time to second
    {

    }
    else if(status & BIT1)              //Button press for 1 second real: 1 minute clock
    {

    }
    else if(status & BIT4)              //Button for set time
    {

    }
    else if(status & BIT5)              //Button for set alarm
    {

    }
    else if(status & BIT6)              //Button for ON/OFF/Up
    {

    }
    else if(status & BIT6)              //Button for SNOOZE/DOWN
    {

    }

    P6->IFG = 0;                        //Clear interrupt flags
}

/*
 * This function is usedd to initialize the LCD screen
 * LCD is in port 4
 */

void LCD_init(void)
{
    P4->DIR = 0xFF;     /* make P4 pins output for data and controls */
    SysTick_delay(30);                /* initialization sequence */
    LCD_nibble_write(0x30, 0);
    SysTick_delay(10);
    LCD_nibble_write(0x30, 0);
    SysTick_delay(1);
    LCD_nibble_write(0x30, 0);
    SysTick_delay(1);
    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    SysTick_delay(1);
    LCD_command(0x28);      /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x01);      /* clear screen, move cursor to home */
    LCD_command(0x0F);      /* turn on display, cursor blinking */
}

/*
 * This function is used to write nibbles to the LCD screen in port 4
 */

void LCD_nibble_write(unsigned char data, unsigned char control)
{
    data &= 0xF0;           /* clear lower nibble for control */
    control &= 0x0F;        /* clear upper nibble for data */
    P4->OUT = data | control;       /* RS = 0, R/W = 0 */
    P4->OUT = data | control | EN;  /* pulse E */
    SysTick_delay(10);
    P4->OUT = data;                 /* clear E */
    P4->OUT = 0;
}

/*
 * This function is used to go to a specific in the LCD screen in Port 4
 */

void LCD_command(unsigned char command)
{
    LCD_nibble_write(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write(command << 4, 0);      /* then lower nibble */
    if (command < 4)
        SysTick_delay(4);         /* commands 1 and 2 need up to 1.64ms */
    else
        SysTick_delay(1);         /* all others 40 us */
}

/*
 * This function is used to send data to the LCD screen
 */

void LCD_data(unsigned char data)
{
    LCD_nibble_write(data & 0xF0, RS);    /* upper nibble first */
    LCD_nibble_write(data << 4, RS);      /* then lower nibble  */
    SysTick_delay(1);
}
