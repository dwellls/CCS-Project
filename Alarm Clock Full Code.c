/*
 * Authors: David Brinkmeier & Danny Weller
 * Class: EGR 226 - 908
 * Date of code beginning: 11/16/2018 
 * Due date of Project: 12/7/2018
 * Professor Zuidema
 * Description: This is the full code for the Alarm Clock final semester project
 *
 *
 * Update 1: David 11/21/2018
 * -> Added all six buttons and tested to make sure they work
 *
 * Update 2: David 11/23/2018
 * -> Begin the state machine for the functionality and LCD display
 */

//Libraries included
#include "msp.h"
#include "string.h"
#include "Alarm_Header.h"



void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//initialization function calls
	BUTTONseconds();
	SysTick_Initialize();

	//Enabling interupts and the vectors
    NVIC->ISER[1] = 1 <<((PORT6_IRQn) & 31);    //Enable Port 6 interrupt on the NVIC
    __enable_interrupt ( );
    __enable_irq();

    //local variables
    enum states state;
    state = INITIAL;

    while(1)
    {
        switch(state)
        {
        case INITIAL:
        {

        }
        case TIMEDISPLAY:
        {

        }
        case SETTIME:
        {

        }
        case SETALARM:
        {

        }
        }
    }
}

