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

/*
 * This function uses ADC to read the temperature from the LM35
 */

void ADC14_init(void)
{
    ADC14->CTL0 &=~ ADC14_CTL0_ENC;         //Disable ADC converter during initialization
    ADC14->CTL0 |=  0x04200210;             //S/H pulse mode, SMCLK, 16 sample clocks
    ADC14->CTL1  =  0x00000030;             //14-bit resolution
    ADC14->CTL1 |=  0x00000000;             //Convert for mem0 register
    ADC14->MCTL[0] = 0x00000001;            //ADC14INCHx = 0 for mem[0]
    ADC14->CTL0 |=  ADC14_CTL0_ENC;         //Enable ADC14ENC, Starts the ADC after confg.
 }

/*
 * This function initializes the port for temp sensor
 */

void PortADC_init(void)
{
    P5->SEL0 |= BIT4;       //Select ADC14 on P5.5
    P5->SEL1 |= BIT4;
}

/*
 * This is the systic interrupt used to go with the temperature sensor
 */

void SysTick_init_interrupt(void)
{
    SysTick->CTRL=0;                //Sisable SysTick during step
    SysTick->LOAD=1500000;          //Reload value for 0.5 sec interrupts
    SysTick->VAL=0;                 //Any write to current clears it
    SysTick->CTRL=0x00000007;       //Enable SysTick, 3MHz, with interrupts
}

/*
 * Handler for the systick interrupt
 */
void SysTick_Handler(void)
{
    timeout = 1;    //Set flag for timeout of SysTick, rest in main
}
/*-------------------------------------------------------------------------------------------------------------------------------
 *
 * void SetupTimer32s()
 *
 * Configures Timer32_1 as a single shot (runs once) timer that does not interrupt so the value must be monitored.
 * Configures Timer32_2 as a single shot (runs once) timer that does interrupt and will run the interrupt handler 1 second
 * after this function is called (and the master interrupt is enabled).
 *
 *Taken with help from prof Zuidema
-------------------------------------------------------------------------------------------------------------------------------*/

void SetupTimer32s()
{
    TIMER32_2->CONTROL = 0b11100011;                //Sets timer 2 for Enabled, Periodic, With Interrupt, No Prescaler, 32 bit mode, One Shot Mode.  See 589 of the reference manual
    NVIC_EnableIRQ(T32_INT2_IRQn);                  //Enable Timer32_2 interrupt.  Look at msp.h if you want to see what all these are called.
    TIMER32_2->LOAD = 3000000 - 1;                  //Set to a count down of 1 second on 3 MHz clock

    TIMER_A0->CCR[0] = 0;                           // Turn off timerA to start
    TIMER_A0->CCTL[4] = 0b0000000011110100;         // Setup Timer A0_1 Reset/Set, Interrupt, No Output
    TIMER_A0->CCR[4] = 0;                           // Turn off timerA to start
    TIMER_A0->CCR[2] = 0;                           // Turn off timerA to start
    TIMER_A0->CTL = 0b0000001000010100;             // Count Up mode using SMCLK, Clears, Clear Interrupt Flag

    NVIC_EnableIRQ(TA0_N_IRQn);                     // Enable interrupts for CCTL1-6 (if on)

    P2->SEL0 |= BIT7;                               // Setup the P2.4 to be an output for the song.  This should drive a sounder.
    P2->SEL1 &= ~BIT7;
    P2->DIR |= BIT7;

}
void green_LED_pins(void)
{
    P5 -> SEL0 |= BIT6; //sets GPIO
    P5 -> SEL1 &=~ BIT6;
    P5 -> DIR |= BIT6; //output

    TIMER_A2->CCR[0] = (1000 - 1);                  //PWM period
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;     //Reset/set
    TIMER_A2->CCR[1] = 0;                           //Initial duty cycle

    TIMER_A2->CTL = TIMER_A_CTL_TASSEL_2 |          //SMCLK
                    TIMER_A_CTL_MC_1 |              //Up Mode
                    TIMER_A_CTL_CLR;                //Clear TA0R Register
}
void green_LED_pins2(void)
{
    P5 -> SEL0 |= BIT7; //sets GPIO
    P5 -> SEL1 &=~ BIT7;
    P5 -> DIR |= BIT7; //output

    TIMER_A2->CCR[0] = (1000 - 1);                  //PWM period
    TIMER_A2->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7;     //Reset/set
    TIMER_A2->CCR[2] = 0;                           //Initial duty cycle

    TIMER_A2->CTL = TIMER_A_CTL_TASSEL_2 |          //SMCLK
                    TIMER_A_CTL_MC_1 |              //Up Mode
                    TIMER_A_CTL_CLR;                //Clear TA0R Register
}

//Timer A initialization for the LEDs
void TA_init(void)
{
    TIMER_A2->CCR[1] = 0;
    TIMER_A2->CCR[2] = 0;
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; //Reset/set
    TIMER_A2->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7; //Reset/set
    TIMER_A2->CTL  = TIMER_A_CTL_TASSEL_2|//SMCLK
                        TIMER_A_CTL_MC_1|//Up mode count up
                        TIMER_A_CTL_CLR;// clear TAR
}

/*----------------------------------------------------------------
 * void setupSerial()
 * Sets up the serial port EUSCI_A0 as 115200 8E2 (8 bits, Even parity,
 * two stops bit.)  Enables the interrupt so that received data will
 * results in an interrupt.
 * Description:
 * Inputs: None
 * Outputs: None
----------------------------------------------------------------*/
/*----------------------------------------------------------------
 * void setupSerial()
 * Sets up the serial port EUSCI_A0 as 115200 8E2 (8 bits, Even parity,
 * two stops bit.)  Enables the interrupt so that received data will
 * results in an interrupt.
 * Description:
 * Inputs: None
 * Outputs: None
----------------------------------------------------------------*/
void setupSerial()
{
    P1->SEL0 |=  (BIT2 | BIT3); // P1.2 and P1.3 are EUSCI_A0 RX
    P1->SEL1 &= ~(BIT2 | BIT3); // and TX respectively.

    EUSCI_A0->CTLW0  = BIT0; // Disables EUSCI. Default configuration is 8N1
    EUSCI_A0->CTLW0 |= BIT7; // Connects to SMCLK BIT[7:6] = 10
    EUSCI_A0->CTLW0 |= (BIT(15)|BIT(14)|BIT(11));  //BIT15 = Parity, BIT14 = Even, BIT11 = Two Stop Bits
    // Baud Rate Configuration
    // 3000000/(16*115200) = 1.628  (3 MHz at 115200 bps is fast enough to turn on over sampling (UCOS = /16))
    // UCOS16 = 1 (0ver sampling, /16 turned on)
    // UCBR  = 1 (Whole portion of the divide)
    // UCBRF = .628 * 16 = 10 (0x0A) (Remainder of the divide)
    // UCBRS = 3000000/115200 remainder=0.04 -> 0x01 (look up table 22-4)
    EUSCI_A0->BRW = 1;  // UCBR Value from above
    EUSCI_A0->MCTLW = 0x01A1; //UCBRS (Bits 15-8) & UCBRF (Bits 7-4) & UCOS16 (Bit 0)

    EUSCI_A0->CTLW0 &= ~BIT0;  // Enable EUSCI
    EUSCI_A0->IFG &= ~BIT0;    // Clear interrupt
    EUSCI_A0->IE |= BIT0;      // Enable interrupt
    NVIC_EnableIRQ(EUSCIA0_IRQn);
}


