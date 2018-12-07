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

void backlight_set(void); //calculate duty cycle for backlight
 static volatile uint16_t result2;

 uint8_t storage_location = 0; // used in the interrupt to store new data
 uint8_t read_location = 0; // used in the main application to read valid data that hasn't been read yet
int dutycycle = 0;
int LightCounter = 0;
int wakeup_light = 0;
int RT = 0;
int settime;
int status = 0;
int astatus = 0;
int flag = 1;
int speakerflag = 1;
int alarmflag = 1;
int up = 0;
int setcount = 0;
int down = 0;
int setalarm = 0;
int snooze = 0;
int on = 0;
int length;
int note = 0;       //The note in the music sequence we are on
int breath = 0;     //Take a breath after each note.  This creates seperation
float music_note_sequence[][2] = {  //Twinkle Twinkle
                                 {C4,QUARTER},
                                 {C4,QUARTER},
                                 {G4,QUARTER},
                                 {G4,QUARTER},

                                 {A4,QUARTER},
                                 {A4,QUARTER},
                                 {G4,HALF},

                                 {F4,QUARTER},
                                 {F4,QUARTER},
                                 {E4,QUARTER},
                                 {E4,QUARTER},

                                 {D4,QUARTER},
                                 {D4,QUARTER},
                                 {C4,HALF},

                                 {G4,QUARTER},
                                 {G4,QUARTER},
                                 {F4,QUARTER},
                                 {F4,QUARTER},

                                 {E4,QUARTER},
                                 {E4,QUARTER},
                                 {D4,HALF},

                                 {G4,QUARTER},
                                 {G4,QUARTER},
                                 {F4,QUARTER},
                                 {F4,QUARTER},

                                 {E4,QUARTER},
                                 {E4,QUARTER},
                                 {D4,HALF},

                                 {C4,QUARTER},
                                 {C4,QUARTER},
                                 {G4,QUARTER},
                                 {G4,QUARTER},

                                 {A4,QUARTER},
                                 {A4,QUARTER},
                                 {G4,HALF},

                                 {F4,QUARTER},
                                 {F4,QUARTER},
                                 {E4,QUARTER},
                                 {E4,QUARTER},

                                 {D4,QUARTER},
                                 {D4,QUARTER},
                                 {C4,HALF},

                                 {REST,WHOLE},
                                 {REST,WHOLE},
};

void main(void)
 {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    //initialization function calls
    BUTTONseconds();
    SysTick_Initialize();
    ADC14_init();
    PortADC_init();
    SysTick_init_interrupt();
    LCD_init();
    green_LED_pins();
    green_LED_pins2();
    TA_init();
    setupSerial();

    //Enabling interupts and the vectors
    NVIC->ISER[1] = 1 <<((PORT6_IRQn) & 31);    //Enable Port 6 interrupt on the NVIC
    __enable_interrupt ( );
    __enable_irq();

    //local variables
    char string[100];
    char sec[4];
    char min[4];
    char hour[5];
    int numsec = 0;
    int nummin =0;
    int numhour = 12;
    int anumsec = 0;
    int anummin = 0;
    int anumhour = 12;
    char sendtime[34];
    int AP = 1;
    int LEDcount = 0;
    int aAP = 1;
    P5->SEL0 |= BIT0; //set GPIO for backlight
    P5->SEL1 |= BIT0;

     //LCD backlight
      P2->SEL0 |= BIT6;       //P2.6 selected TA0
      P2->SEL1 &=~ BIT6;      //P2.6 selected TA0
      P2->DIR |= BIT6;        //P2.6 set as output
    TIMER32_1->LOAD=3000000-1;
    TIMER32_1->CONTROL=0xC2;

    sprintf(alarmtime, "12:00:00 AM");

    while(1)
    {
        if(flag == 1)
            {readInput(string);
        if(string[0] != '\0')
        {
            if(string[0] == 'S' && string[1] == 'E' && string[2] == 'T' && string[3] == 'T')
            {
                hour[0] = string[8];
                hour[1] = string[9];
                min[0] = string[11];
                min[1] = string[12];
                sec[0] = string[14];
                sec[1] = string[5];

                numhour = atoi(hour);
                nummin = atoi(min);
                numsec = atoi(sec);

                if(numhour > 12)
                {
                    numhour = numhour - 12;
                    AP = 1;
                }
            }

            /*else if(string[0] == 'S' && string[1] == 'E' && string[2] == 'T' && string[3] == 'A')
            {
                hour[0] = string[8];
                hour[1] = string[9];
                min[0] = string[11];
                min[1] = string[12];
                sec[0] = string[14];
                sec[1] = string[5];

                anumhour = atoi(hour);
                anummin = atoi(min);
                anumsec = atoi(sec);

                if(anumhour > 12)
                {
                    anumhour = anumhour - 12;
                    aAP = 1;
                }

                if(aAP == 0)
                {
                //sprintf(alarmtime,"%d:%d:%d AM", anumhour, anummin, anumsec);
                }
                else
                {
                    //sprintf(alarmtime,"%d:%d:%d PM", anumhour, anummin, anumsec);
                }
            }

            else if(string[0] == 'R' && string[4] == 'T')
            {
                if(AP == 1) //Check if clock is PM
                {
                    if (numhour > 12) //current hour = 12
                    {
                    sprintf(sendtime, "%d:%02d:%02d", (numhour + 12), nummin, numsec);
                    writeOutput(sendtime);
                    writeOutput("\n");
                    }
                }
                else
                {
                    sprintf(sendtime,"%d:%0d:0%d", numhour, nummin,numsec);
                    writeOutput(sendtime);
                    writeOutput(sendtime);
                }
            }

            else if(string[0] == 'R' && string[4] == 'A')
            {
                if(aAP == 1) //Check if clock is PM
                {
                    if (anumhour > 12) //current hour = 12
                    {
                    sprintf(sendtime, "%d:%02d:%02d", (anumhour + 12), anummin, anumsec);
                    writeOutput(sendtime);
                    writeOutput("\n");
                    }
                }
                else
                {
                    sprintf(sendtime,"%02d:%02d:%02d", anumhour, anummin, anumsec);
                    writeOutput(sendtime);
                    writeOutput(sendtime);
                }
            }*/

            flag = 0;
        }



            if(settime == 1)
            {


                if(setcount == 1)
                {
                    LCD_command(0x80);
                    LCD_data(" ");
                    LCD_data(" ");
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
                    LCD_command(0x83);
                    LCD_data(" ");
                    LCD_data(" ");

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
                    LCD_command(0x86);
                    LCD_data(" ");
                    LCD_data(" ");

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
                                        AP = 0;
                                    }
                                    else if (AP == 0)
                                    {
                                        AP = 1;
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
            else
            {
                if(setalarm == 1)
                {
                    if(setcount == 1)
                    {
                        LCD_command(0x90);
                        LCD_data(" ");
                        LCD_data(" ");

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
                        LCD_command(0x93);
                        LCD_data(" ");
                        LCD_data(" ");
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
                        LCD_command(0x96);
                        LCD_data(" ");
                        LCD_data(" ");
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
                                            aAP = 0;
                                        }
                                        else if (aAP == 0)
                                        {
                                            aAP = 1;
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
                if(numsec >= 60)
                {
                    numsec = 0;
                    nummin += 1;
                }
                if(nummin >= 60)
                {
                    nummin = 0;
                    numhour += 1; //update hours
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

            LCD_command(0x80);
            for(i = 0; i < 11; i++)
            {
                LCD_data(time[i]);
            }

            if(on == 1 && settime == 0 && setalarm == 0)
            {
                up = 0;
                astatus = 1;
                sprintf(alarm,"Alarm ON    ");
                length = strlen(alarm);
                if(aAP == AP)
                {
                    if(numhour == anumhour)
                    {
                        if((anummin - nummin) == 5)
                        {
                            wakeup_light = 1;
                            speakerflag = 0;
                            SetupTimer32s();//Initializes Timer32_1 as a non-interrupt timer and Timer32_2 as a interrupt timers.  Also initializes TimerA and P2.4 for music generation.
                        }
                    }
                }
                else
                {
                    if(abs(anumhour - numhour) == 1)
                    {
                        if(abs(anummin - nummin) == 55)
                        {
                            wakeup_light = 1;
                            speakerflag = 0;
                            SetupTimer32s();//Initializes Timer32_1 as a non-interrupt timer and Timer32_2 as a interrupt timers.  Also initializes TimerA and P2.4 for music generation.
                        }
                    }
                }
                if(!(strcmp(time, alarmtime)))
                {
                    snooze = 0;
                    speakerflag = 0;
                    NVIC_DisableIRQ(T32_INT2_IRQn);
                    TIMER_A0->CCR[0] = 0;                                   //Set output of TimerA to 0
                    TIMER_A0->CCR[4] = 0;
                    TIMER_A0->CCR[2] = 0;
                    TIMER32_2->LOAD = BREATH_TIME;
                    TIMER32_2->LOAD = music_note_sequence[note][1] - 1;
                }

            }
            else if(on == 0)
            {
                snooze = 0;
                sprintf(alarm, "Alarm OFF   ");
                length = strlen(alarm);
                astatus = 0;
                alarmflag = 0;

                TIMER_A2->CCR[1] = 0;      //Initial duty cycle
                TIMER_A2->CCR[2] = 0;     //Initial duty cycle
                speakerflag = 1;

                TIMER_A0->CCR[0] = 0;                                   //Set output of TimerA to 0
                TIMER_A0->CCR[4] = 0;
                TIMER_A0->CCR[2] = 0;
                TIMER32_2->LOAD = BREATH_TIME;



            }

            if( snooze == 1)
            {
                anummin += 10;
                sprintf(alarm, "Alarm SNOOZE");
                length = strlen(alarm);


            }


            LCD_command(0xC0);
            for(i = 0; i < length; i++)
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
            LEDcount += 1;

            if (wakeup_light==1) //call wakeup light when timer is 5 minutes to alarm
                {
            LightCounter += 1;

            if (LightCounter == 3 && dutycycle <= 1000)
            {
             dutycycle += 10;
             TIMER_A2->CCR[1] = dutycycle;      //Initial duty cycle
             TIMER_A2->CCR[2] = dutycycle;     //Initial duty cycle

             LightCounter = 0;
             wakeup_light = 0;
            }


            //insert flag off button when pressed then the LEDs turn off and the sound off
          }
        }


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
        if(settime == 1 || setalarm == 1)
        {
        up = 1;
        }
        else{
            if(astatus == 1)
            {
                on = 0;
            }
            else
            {
                on = 1;
            }
        }
    }
    else if(status & BIT7)              //Button for SNOOZE/DOWN
    {
        if(settime == 1 || setalarm == 1)
        {
        down = 1;
        }
        else
        {
            snooze = 1;
        }
    }

    P6->IFG = 0;                        //Clear interrupt flags
}
/*-------------------------------------------------------------------------------------------------------------------------------
 *
 * void T32_INT2_IRQHandler()
 *
 * Interrupt Handler for Timer 2.  The name of this function is set in startup_msp432p401r_ccs.c
 *
 * This handler clears the status of the interrupt for Timer32_2
 *
 * Sets up the next note to play in sequence and loads it into TimerA for play back at that frequency.
 * Enables a new Timer32 value to interrupt after the note is complete.
 *
-------------------------------------------------------------------------------------------------------------------------------*/

void T32_INT2_IRQHandler()
{
    if(speakerflag == 0){
    TIMER32_2->INTCLR = 1;                                      //Clear interrupt flag so it does not interrupt again immediately.
    if(breath) {                                                //Provides separation between notes
        TIMER_A0->CCR[0] = 0;                                   //Set output of TimerA to 0
        TIMER_A0->CCR[4] = 0;
        TIMER_A0->CCR[2] = 0;
        TIMER32_2->LOAD = BREATH_TIME;                          //Load in breath time to interrupt again
        breath = 0;                                             //Next Timer32 interrupt is no longer a breath, but is a note
    }
    else {                                                      //If not a breath (a note)
        TIMER32_2->LOAD = music_note_sequence[note][1] - 1;     //Load into interrupt count down the length of this note
        if(music_note_sequence[note][0] == REST) {              //If note is actually a rest, load in nothing to TimerA
            TIMER_A0->CCR[0] = 0;
            TIMER_A0->CCR[4] = 0;
            TIMER_A0->CCR[2] = 0;
        }
        else {
            TIMER_A0->CCR[0] = 3000000 / music_note_sequence[note][0];  //Math in an interrupt is bad behavior, but shows how things are happening.  This takes our clock and divides by the frequency of this note to get the period.
            TIMER_A0->CCR[4] = 1500000 / music_note_sequence[note][0];  //50% duty cycle
            TIMER_A0->CCR[2] = TIMER_A0->CCR[0];                        //Had this in here for fun with interrupts.  Not used right now
        }
        note = note + 1;                                                //Next note
        if(note >= MAX_NOTE) {                                          //Go back to the beginning if at the end
            note = 0;
        }
        breath = 1;                                             //Next time through should be a breath for separation.
    }}
    else
    {
        TIMER_A0->CCR[0] = 0;                                   //Set output of TimerA to 0
        TIMER_A0->CCR[4] = 0;
        TIMER_A0->CCR[2] = 0;
        TIMER32_2->LOAD = BREATH_TIME;                          //Load in breath time to interrupt again
        breath = 0;
    }
}
//Calculate the duty cycle for the LCD backlight
void backlight_set(void)
{
    //Declare variables
    static volatile uint16_t result;
    float nADC, backlight;

    ADC14->CTL0 |= 1;       //Start conversation
    while(!ADC14->IFGR0);        //Wait for conversation to complete
    result = ADC14->MEM[2];             //Get the value from the ADC
    nADC = ((result * 5.0) / 16384);    //Calculate voltage

    backlight = ((nADC * 1315) - 5574.8685);
    TIMER_A0->CCR[1] = backlight;

    ADC14->CTL0 &=~ 2;
    ADC14->CTL0 |= 2;
}
void TA0_N_IRQHandler()
{
    TIMER_A0-> CCTL[4] &=~ TIMER_A_CCTLN_CCIFG;
}

/*----------------------------------------------------------------
 * void EUSCIA0_IRQHandler(void)
 *
 * Description: Interrupt handler for serial communication on EUSCIA0.
 * Stores the data in the RXBUF into the INPUT_BUFFER global character
 * array for reading in the main application
 * Inputs: None (Interrupt)
 * Outputs: Data stored in the global INPUT_BUFFER. storage_location
 * in the INPUT_BUFFER updated.
----------------------------------------------------------------*/
void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & BIT0)  // Interrupt on the receive line
    {
        INPUT_BUFFER[storage_location] = EUSCI_A0->RXBUF; // store the new piece of data at the present location in the buffer
        EUSCI_A0->IFG &= ~BIT0; // Clear the interrupt flag right away in case new data is ready
        storage_location++; // update to the next position in the buffer
        if(storage_location == 100) // if the end of the buffer was reached, loop back to the start
            storage_location = 0;

        if(EUSCI_A0 -> RXBUF == '\0')
        {
            flag = 1;
        }
        else
        {
            flag = 0;
        }
    }
}
/*----------------------------------------------------------------
 * void readInput(char *string)
 *
 * Description:  This is a function similar to most serial port
 * functions like ReadLine.  Written as a demonstration and not
 * production worthy due to limitations.
 * One of the limitations is that it is BLOCKING which means
 * it will wait in this function until there is a \n on the
 * serial input.
 * Another limitation is poor memory management.
 * Inputs: Pointer to a string that will have information stored
 * in it.
 * Outputs: Places the serial data in the string that was passed
 * to it.  Updates the global variables of locations in the
 * INPUT_BUFFER that have been already read.
----------------------------------------------------------------*/
void readInput(char *string)
{
    int i = 0;  // Location in the char array "string" that is being written to

    // One of the few do/while loops I've written, but need to read a character before checking to see if a \n has been read
    do
    {
        // If a new line hasn't been found yet, but we are caught up to what has been received, wait here for new data
        while(read_location == storage_location && INPUT_BUFFER[read_location] != '\n');
        string[i] = INPUT_BUFFER[read_location];  // Manual copy of valid character into "string"
        INPUT_BUFFER[read_location] = '\0';
        i++; // Increment the location in "string" for next piece of data
        read_location++; // Increment location in INPUT_BUFFER that has been read
        if(read_location == 100)  // If the end of INPUT_BUFFER has been reached, loop back to 0
            read_location = 0;
    }
    while(string[i-1] != '\n'); // If a \n was just read, break out of the while loop

    string[i-1] = '\0'; // Replace the \n with a \0 to end the string when returning this function
}
/*----------------------------------------------------------------
 * void writeOutput(char *string)
 *
 * Description:  This is a function similar to most serial port
 * functions like printf.  Written as a demonstration and not
 * production worthy due to limitations.
 * One limitation is poor memory management.
 * Inputs: Pointer to a string that has a string to send to the serial.
 * Outputs: Places the data on the serial output.
----------------------------------------------------------------*/
void writeOutput(char *string)
{
    int i = 0;  // Location in the char array "string" that is being written to

    while(string[i] != '\0') {
        EUSCI_A0->TXBUF = string[i];
        i++;
        while(!(EUSCI_A0->IFG & BIT1));
    }
}

