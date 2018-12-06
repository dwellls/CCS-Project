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
 *
 * Update 3: David and Danny 11/30/2018
 * temp sensor setup
 * lcd attached
 * clock coding
 * speaker
 *
 * Update 4: David and Danny 12/2/2018
 * start bringing it all together
 */

//Libraries included
#include "msp.h"
#include "string.h"
#include "Alarm_Header.h"
#include "stdio.h"

int RT = 0;
int settime;
int up = 0;
int setcount = 0;
int down = 0;
int setalarm = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//initialization function calls
    BUTTONseconds();
    SysTick_Initialize();
    ADC14_init();
    PortADC_init();
    SysTick_init_interrupt();
    LCD_init();

	//Enabling interupts and the vectors
    NVIC->ISER[1] = 1 <<((PORT6_IRQn) & 31);    //Enable Port 6 interrupt on the NVIC
    __enable_interrupt ( );
    __enable_irq();

    //local variables
    int numsec = 0;
    int nummin =0;
    int numhour = 12;
    int anumsec = 0;
    int anummin = 0;
    int anumhour = 12;
    int AP = 1;
    int aAP = 1;
    TIMER32_1->LOAD=3000000-1;
    TIMER32_1->CONTROL=0xC2;

    sprintf(alarmtime, "12:00:00 AM");

    while(1)
    {
            if(settime == 1)
            {
                if(setcount == 1)
                {
                    if(up == 1)
                    {
                        numhour += 1;
                        up = 0;
                        if(numhour == 13)
                        {
                            numhour = 1;
                            if(AP == 1)
                            {
                                AP = 0;
                            }
                            else
                            {
                                AP= 1;
                            }
                        }
                    }
                    else if(down == 1)
                    {
                        numhour -= 1;
                        down = 0;
                        if(numhour <= 0)
                        {
                            numhour = 12;
                            if(AP == 1)
                            {
                                AP = 0;
                            }
                            else
                            {
                                AP= 1;
                            }
                        }
                    }
                }
                if(setcount == 2)
                {
                    if(up == 1)
                    {
                        nummin += 1;
                        up = 0;
                    }
                    else if(down == 1)
                    {
                        nummin -= 1;
                        down = 0;
                        if(nummin < 0)
                        {
                            nummin = 59;
                            if(numhour == 1)
                            {
                                numhour = 12;
                            }
                            else
                            {
                            numhour -= 1;
                            }
                        }
                    }
                }
                if(setcount == 3)
                {
                    if(up == 1)
                    {
                        numsec += 1;
                        up = 0;
                    }
                    else if(down == 1)
                    {
                        numsec -= 1;
                        down = 0;
                        if(numsec <= 0)
                        {
                            numsec = 59;
                            nummin -= 1;
                            if(nummin < 0)
                            {
                                nummin = 59;
                                numhour -= 1;
                                if(numhour < 1)
                                {
                                    numhour = 12;
                                    if(AP == 1)
                                    {
                                        AP == 0;
                                    }
                                    else if (AP == 0)
                                    {
                                        AP == 1;
                                    }
                                }
                            }
                        }
                    }
                }
                if( setcount == 4)
                {
                    settime = 0;
                    setcount = 0;
                }


                if(nummin >= 60)
                {
                    nummin = 0;
                    numhour += 1; //update hours
                }
                if(numsec > 60)
                {
                    numsec = 0;
                    nummin += 1;
                }
                if(numhour < 12)
                {
                    if(AP == 0)
                    {
                        if(nummin < 10 && numhour < 10)
                        {
                            if(numsec < 10)
                            {
                                sprintf(time,"0%d:0%d:0%d AM", numhour, nummin, numsec);
                            }
                            else
                            {
                                sprintf(time,"0%d:0%d:%d AM", numhour, nummin, numsec);
                            }
                        }
                        else if(numhour < 10)
                        {
                            if(numsec < 10)
                            {
                                sprintf(time,"0%d:%d:0%d AM", numhour, nummin, numsec);
                            }
                            else
                            {
                                sprintf(time,"0%d:%d:%d AM", numhour, nummin, numsec);
                            }
                        }
                        else if(numhour > 9 && nummin < 10)
                        {
                            if(numsec < 10)
                            {
                                sprintf(time,"%d:0%d:0%d AM", numhour, nummin, numsec);
                            }
                            else
                            {
                                sprintf(time,"%d:0%d:%d AM", numhour, nummin, numsec);
                            }
                        }
                        else if(numhour > 9)
                        {
                            if(numsec < 10)
                            {
                                sprintf(time,"%d:%d:0%d AM", numhour, nummin, numsec);
                            }
                            else
                            {
                                sprintf(time,"%d:%d:%d AM", numhour, nummin, numsec);
                            }
                        }
                    }
                    else if(AP == 1)
                    {
                        if(nummin < 10 && numhour < 10)
                        {
                            if(numsec < 10)
                            {
                                sprintf(time,"0%d:0%d:0%d PM", numhour, nummin, numsec);
                            }
                            else
                            {
                                sprintf(time,"0%d:0%d:%d PM", numhour, nummin, numsec);
                            }
                        }
                        else if(numhour < 10)
                        {
                            if(numsec < 10)
                            {
                                sprintf(time,"0%d:%d:0%d PM", numhour, nummin, numsec);
                            }
                            else
                            {
                                sprintf(time,"0%d:%d:%d PM", numhour, nummin, numsec);
                            }
                        }
                        else if(numhour > 9 && nummin < 10)
                        {
                            if(numsec < 10)
                            {
                                sprintf(time,"%d:0%d:0%d PM", numhour, nummin, numsec);
                            }
                            else
                            {
                                sprintf(time,"%d:0%d:%d PM", numhour, nummin, numsec);
                            }
                        }
                        else if(numhour > 9)
                        {
                            if(numsec < 10)
                            {
                                sprintf(time,"%d:%d:0%d PM", numhour, nummin, numsec);
                            }
                            else
                            {
                                sprintf(time,"%d:%d:%d PM", numhour, nummin, numsec);
                            }
                        }
                    }
                }
                else if(numhour == 12 && AP == 1)
                {
                    if(nummin < 10)
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:0%d:0%d AM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:0%d:%d AM", numhour, nummin, numsec);
                        }
                    }
                    else
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:%d:0%d AM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:%d:%d AM", numhour, nummin, numsec);
                        }
                    }
                }
                else if(numhour == 12 && AP == 0)
                {
                    if(nummin < 10)
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:0%d:0%d PM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:0%d:%d PM", numhour, nummin, numsec);
                        }
                    }
                    else
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:%d:0%d PM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:%d:%d PM", numhour, nummin, numsec);
                        }
                    }
                }


            }
            if(setalarm == 1)
            {
                if(setcount == 1)
                {
                    if(up == 1)
                    {
                        anumhour += 1;
                        up = 0;
                        if(anumhour == 13)
                        {
                            anumhour = 1;
                            if(aAP == 1)
                            {
                                aAP = 0;
                            }
                            else
                            {
                                aAP= 1;
                            }
                        }
                    }
                    else if(down == 1)
                    {
                        anumhour -= 1;
                        down = 0;
                        if(anumhour <= 0)
                        {
                            anumhour = 12;
                            if(aAP == 1)
                            {
                                aAP = 0;
                            }
                            else
                            {
                                aAP= 1;
                            }
                        }
                    }
                }
                if(setcount == 2)
                {
                    if(up == 1)
                    {
                        anummin += 1;
                        up = 0;
                    }
                    else if(down == 1)
                    {
                        anummin -= 1;
                        down = 0;
                        if(anummin < 0)
                        {
                            anummin = 59;
                            if(anumhour == 1)
                            {
                                anumhour = 12;
                            }
                            else
                            {
                            anumhour -= 1;
                            }
                        }
                    }
                }
                if(setcount == 3)
                {
                    if(up == 1)
                    {
                        anumsec += 1;
                        up = 0;
                    }
                    else if(down == 1)
                    {
                        anumsec -= 1;
                        down = 0;
                        if(anumsec <= 0)
                        {
                            anumsec = 59;
                            anummin -= 1;
                            if(anummin < 0)
                            {
                                anummin = 59;
                                anumhour -= 1;
                                if(anumhour < 1)
                                {
                                    anumhour = 12;
                                    if(aAP == 1)
                                    {
                                        aAP == 0;
                                    }
                                    else if (aAP == 0)
                                    {
                                        aAP == 1;
                                    }
                                }
                            }
                        }
                    }
                }
                if( setcount == 4)
                {
                    setalarm = 0;
                    setcount = 0;
                }


                if(anummin >= 60)
                {
                    anummin = 0;
                    anumhour += 1; //update hours
                }
                if(anumsec > 60)
                {
                    anumsec = 0;
                    anummin += 1;
                }
                if(anumhour < 12)
                {
                    if(aAP == 0)
                    {
                        if(anummin < 10 && anumhour < 10)
                        {
                            if(anumsec < 10)
                            {
                                sprintf(alarmtime,"0%d:0%d:0%d AM", anumhour, anummin, anumsec);
                            }
                            else
                            {
                                sprintf(alarmtime,"0%d:0%d:%d AM", anumhour, anummin, anumsec);
                            }
                        }
                        else if(anumhour < 10)
                        {
                            if(anumsec < 10)
                            {
                                sprintf(alarmtime,"0%d:%d:0%d AM", anumhour, anummin, anumsec);
                            }
                            else
                            {
                                sprintf(alarmtime,"0%d:%d:%d AM", anumhour, anummin, anumsec);
                            }
                        }
                        else if(anumhour > 9 && anummin < 10)
                        {
                            if(anumsec < 10)
                            {
                                sprintf(alarmtime,"%d:0%d:0%d AM", anumhour, anummin, anumsec);
                            }
                            else
                            {
                                sprintf(alarmtime,"%d:0%d:%d AM", anumhour, anummin, anumsec);
                            }
                        }
                        else if(anumhour > 9)
                        {
                            if(anumsec < 10)
                            {
                                sprintf(alarmtime,"%d:%d:0%d AM", anumhour, anummin, anumsec);
                            }
                            else
                            {
                                sprintf(alarmtime,"%d:%d:%d AM", anumhour, anummin, anumsec);
                            }
                        }
                    }
                    else if(aAP == 1)
                    {
                        if(anummin < 10 && anumhour < 10)
                        {
                            if(anumsec < 10)
                            {
                                sprintf(alarmtime,"0%d:0%d:0%d PM", anumhour, anummin, anumsec);
                            }
                            else
                            {
                                sprintf(alarmtime,"0%d:0%d:%d PM", anumhour, anummin, anumsec);
                            }
                        }
                        else if(anumhour < 10)
                        {
                            if(anumsec < 10)
                            {
                                sprintf(alarmtime,"0%d:%d:0%d PM", anumhour, anummin, anumsec);
                            }
                            else
                            {
                                sprintf(alarmtime,"0%d:%d:%d PM", anumhour, anummin, anumsec);
                            }
                        }
                        else if(anumhour > 9 && anummin < 10)
                        {
                            if(anumsec < 10)
                            {
                                sprintf(alarmtime,"%d:0%d:0%d PM", anumhour, anummin, anumsec);
                            }
                            else
                            {
                                sprintf(alarmtime,"%d:0%d:%d PM", anumhour, anummin, anumsec);
                            }
                        }
                        else if(anumhour > 9)
                        {
                            if(anumsec < 10)
                            {
                                sprintf(alarmtime,"%d:%d:0%d PM", anumhour, anummin, anumsec);
                            }
                            else
                            {
                                sprintf(alarmtime,"%d:%d:%d PM", anumhour, anummin, anumsec);
                            }
                        }
                    }
                }
                else if(anumhour == 12 && aAP == 1)
                {
                    if(anummin < 10)
                    {
                        if(anumsec < 10)
                        {
                            sprintf(alarmtime,"%d:0%d:0%d AM", anumhour, anummin, anumsec);
                        }
                        else
                        {
                            sprintf(alarmtime,"%d:0%d:%d AM", anumhour, anummin, anumsec);
                        }
                    }
                    else
                    {
                        if(anumsec < 10)
                        {
                            sprintf(alarmtime,"%d:%d:0%d AM", anumhour, anummin, anumsec);
                        }
                        else
                        {
                            sprintf(alarmtime,"%d:%d:%d AM", anumhour, anummin, anumsec);
                        }
                    }
                }
                else if(anumhour == 12 && aAP == 0)
                {
                    if(anummin < 10)
                    {
                        if(anumsec < 10)
                        {
                            sprintf(alarmtime,"%d:0%d:0%d PM", anumhour, anummin, anumsec);
                        }
                        else
                        {
                            sprintf(alarmtime,"%d:0%d:%d PM", anumhour, anummin, anumsec);
                        }
                    }
                    else
                    {
                        if(anumsec < 10)
                        {
                            sprintf(alarmtime,"%d:%d:0%d PM", anumhour, anummin, anumsec);
                        }
                        else
                        {
                            sprintf(alarmtime,"%d:%d:%d PM", anumhour, anummin, anumsec);
                        }
                    }
                }


            }
            else
            {
                if(nummin >= 60)
                {
                    nummin = 0;
                    numhour += 1; //update hours
                }
                if(numsec > 60)
                {
                    numsec = 0;
                    nummin += 1;
                }
                if(numhour == 25)
                {
                    numhour = 1;
                    AP =  1;
                }
                if(numhour == 13 && AP == 1)
                {
                    numhour = 1;

                }

                if(numhour < 12)
                {
                    if(nummin < 10)
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"0%d:0%d:0%d AM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"0%d:0%d:%d AM", numhour, nummin, numsec);
                        }
                    }
                    else
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:%d:0%d AM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:%d:%d AM", numhour, nummin, numsec);
                        }
                    }
                }
                else if(numhour > 12)
                {
                    if(nummin < 10)
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"0%d:0%d:0%d PM", (numhour - 12), nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"0%d:0%d:%d PM", (numhour - 12), nummin, numsec);
                        }
                    }
                    else
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:%d:0%d PM", (numhour - 12), nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:%d:%d PM", (numhour - 12), nummin, numsec);
                        }
                    }
                }
                else if(numhour == 12 && AP == 1)
                {
                    if(nummin < 10)
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:0%d:0%d AM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:0%d:%d AM", numhour, nummin, numsec);
                        }
                    }
                    else
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:%d:0%d AM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:%d:%d AM", numhour, nummin, numsec);
                        }
                    }
                }
                else if(numhour == 12 && AP == 0)
                {
                    if(nummin < 10)
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:0%d:0%d PM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:0%d:%d PM", numhour, nummin, numsec);
                        }
                    }
                    else
                    {
                        if(numsec < 10)
                        {
                            sprintf(time,"%d:%d:0%d PM", numhour, nummin, numsec);
                        }
                        else
                        {
                            sprintf(time,"%d:%d:%d PM", numhour, nummin, numsec);
                        }
                    }
                }

            }

            LCD_command(0x80);
            for(i = 0; i < 11; i++)
            {
                LCD_data(time[i]);
            }

            sprintf(alarm, "Alarm: OFF   ");

            LCD_command(0xC0);
            for(i = 0; i < 13; i++)
            {
                LCD_data(alarm[i]);
            }

            LCD_command(0x90);
            for(i = 0; i < 11; i++)
            {
                LCD_data(alarmtime[i]);
            }
            if(timeout)
            {
                ADC14->CTL0 |= ADC14_CTL0_SC;       //Start conversation
                while(!ADC14->IFGR0 & BIT0);        //Wait for conversation to complete
                result = ADC14->MEM[0];             //Get the value from the ADC
                nADC = ((result * 3.3) / 16384);

                temp_c = nADC*100;
                temp_f = ((temp_c * 9 / 5) + 32);

                sprintf(temp,"%.1f F",temp_f);

                LCD_command(0xD0);

                for(i = 0; i<6; i++)
                {
                    LCD_data(temp[i]);
                }

            }
            if(settime == 0)
            {
            if (RT == 0)
            {
                numsec += 1;
            }
            else if (RT == 1)
            {
                nummin += 1;
            }
            while((TIMER32_1->RIS & 1) == 0); /* wait until the RAW_IFG is set */
            TIMER32_1->INTCLR = 0;  /* clear raw interrupt flag */
            }
    }
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
        RT = 0;
    }
    else if(status & BIT1)              //Button press for 1 second real: 1 minute clock
    {
        RT = 1;
    }
    else if(status & BIT4)              //Button for set time
    {
        settime = 1;
        setcount += 1;
    }
    else if(status & BIT5)              //Button for set alarm
    {
        setalarm = 1;
        setcount += 1;
    }
    else if(status & BIT6)              //Button for ON/OFF/Up
    {
        up = 1;
    }
    else if(status & BIT7)              //Button for SNOOZE/DOWN
    {
        down = 1;
    }

    P6->IFG = 0;                        //Clear interrupt flags
}

