#include "msp.h"
#include <stdio.h>
#define RS BIT0 //5.0 pin
#define RW BIT1 //5.1 pin
#define EN BIT2 //5.2 pin
//prototypes for all used functions
/*
 * Sequence the enable pin and turns R and RW to 1
 * Clears lower and upper nibbles
 */
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_nibble_write2(int data, int control);
/*
 * command writes one byte of command
 *
 */
void LCD_command(unsigned char command);
void LCD_command2(int command);
/*
 * data writes one byte of data with paramters
 *
 */
void LCD_data(unsigned char data);
void LCD_data2(int data);
/*
 * Intiazies the LCD Screen sequence
 */
void LCD_init(void);
void LCD_init2(void);
/*
 * Intializes the systimic timer
 */
void SysTick_Init(void);
/*
 * A millisecond systimic timer function.
 */
void SysTick_delayMS(uint16_t delay); //
/*
*Declare the strings that will be put in the data pins
* The strings will be printed on the LCD screen in the middle of the 4 lines
*/
char Str1[] = "Time"; //The string for Connor
char second[60];
char minute[60];
char hour[60];
int numsec = 0;
int nummin =0;
int numhour = 1;
int isec = 0;
int imin = 0;
int ihour = 0;
//char Str3[] = "EGR";
//char Str4[] = "226";
int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    SysTick_Init(); //function to initialize systic time
    LCD_init();
    LCD_init2();
    int j,i; // this j is used for the increments of the for loops to print the strings
    TIMER32_1->LOAD = 3000000-1;    /* set the reload value */
    /* no prescaler, periodic wrapping mode, disable interrupt, 32-bit timer. */
    TIMER32_1->CONTROL = 0xC2;
    while(1)
    {
        LCD_command(0x85);  /* set cursor at beginning of first line */
         for(j=0; j<4;j++)
         {
              LCD_data(Str1[j]);      /* write the word "Connor" */
           }



        //LCD_command(1);     /* clear display */
              //SysTick_delayMS(500);
        if(numsec>=60)
        {
            numsec = 0;
            second[0] = '0';
            second[1] = '0';
            isec=0;
            nummin = nummin + 1; //update mintues
        }
        if(numsec>=10)
        {
            isec=2;
        }
        if(nummin>=60)
        {
            nummin = 0;
            minute[0] = '0';
            minute[1] = '0';
            imin=0;
            numhour = numhour + 1; //update hours
        }
        if(nummin>=10)
        {
            imin=2; //So the zero is not placed
        }
        if(numhour > 12)
        {
            numhour=1;
            ihour = 0;
            //change PM to AM
        }
        if(numhour>=10)
        {
            ihour = 2;
        }



               LCD_command(0xC0);
               sprintf(hour,"%d", numhour); //copy the value of the temp into the string
               if(ihour==0)
               {
                   LCD_data(hour[0]);
               }
               else if(ihour ==2)
               {
                   for(i=0; i<2 ; i++)
                   {
                       LCD_data(hour[i]);
                       printf("hour");
                   }
               }
               LCD_command(0xC2);
               LCD_data(':');
               LCD_command(0xC3);
               sprintf(minute,"%d", nummin); //copy the value of the temp into the string
               if(imin==0)
               {
                   LCD_data('0');
                   LCD_command(0xC4);
                   LCD_data(minute[0]);
               }
               else if(imin==2)
               {
                   for(i=0; i<2 ; i++)
                   {
                       LCD_data(minute[i]);
                       printf("min");
                   }
               }




                 LCD_command(0xC6);  /* set cursor at beginning of 2nd line */
                 LCD_data(':'); //Displays secound
                 LCD_command(0xC7);
                 sprintf(second,"%d", numsec); //copy the value of the temp into the string
                 if(isec==0)
                 {
                     LCD_data('0');
                     LCD_command(0xC8);
                     LCD_data(second[0]);
                 }
                 else if(isec==2)
                 {
                     for(i=0; i<2 ; i++)
                     {
                         LCD_data(second[i]);
                         //printf("2 digits\n");
                     }
                 }
                 numsec= numsec + 1;

                 while((TIMER32_1->RIS & 1) == 0); /* wait until the RAW_IFG is set */
                        TIMER32_1->INTCLR = 0;  /* clear raw interrupt flag */
                        //printf("delay\n");
                        printf("%d\n", numsec);

    }
}
void LCD_init(void)
{
    P5->DIR = 0xFF;     /* make P4 pins output for data and controls */
    SysTick_delayMS(30);                /* initialization sequence */
    LCD_nibble_write(0x30, 0);
    SysTick_delayMS(10);
    LCD_nibble_write(0x30, 0);
    SysTick_delayMS(1);
    LCD_nibble_write(0x30, 0);
    SysTick_delayMS(1);
    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    SysTick_delayMS(1);
    LCD_command(0x28);      /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x01);      /* clear screen, move cursor to home */
    LCD_command(0x0F);      /* turn on display, cursor blinking */
}
/* With 4-bit mode, each command or data is sent twice with upper
 * nibble first then lower nibble.
 */
void LCD_nibble_write(unsigned char data, unsigned char control)
 {
    data &= 0xF0;           /* clear lower nibble for control */
    control &= 0x0F;        /* clear upper nibble for data */
    P5->OUT = data | control;       /* RS = 0, R/W = 0 */
    P5->OUT = data | control | EN;  /* pulse E */
    SysTick_delayMS(10);
    P5->OUT = data;                 /* clear E */
    P5->OUT = 0;
}
void LCD_command(unsigned char command)
{
    LCD_nibble_write(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write(command << 4, 0);      /* then lower nibble */
    if (command < 4)
        SysTick_delayMS(4);         /* commands 1 and 2 need up to 1.64ms */
    else
        SysTick_delayMS(1);         /* all others 40 us */
}
void LCD_data(unsigned char data)
{
    LCD_nibble_write(data & 0xF0, RS);    /* upper nibble first */
    LCD_nibble_write(data << 4, RS);      /* then lower nibble  */
    SysTick_delayMS(1);
}




void LCD_init2(void)
{
    P5->DIR = 0xFF;     /* make P4 pins output for data and controls */
    SysTick_delayMS(30);                /* initialization sequence */
    LCD_nibble_write2(0x30, 0);
    SysTick_delayMS(10);
    LCD_nibble_write2(0x30, 0);
    SysTick_delayMS(1);
    LCD_nibble_write2(0x30, 0);
    SysTick_delayMS(1);
    LCD_nibble_write2(0x20, 0);  /* use 4-bit data mode */
    SysTick_delayMS(1);
    LCD_command2(0x28);      /* set 4-bit data, 2-line, 5x7 font */
    LCD_command2(0x01);      /* clear screen, move cursor to home */
    LCD_command2(0x0F);      /* turn on display, cursor blinking */
}
/* With 4-bit mode, each command or data is sent twice with upper
 * nibble first then lower nibble.
 */
void LCD_nibble_write2(int data, int control)
 {
    data &= 0xF0;           /* clear lower nibble for control */
    control &= 0x0F;        /* clear upper nibble for data */
    P5->OUT = data | control;       /* RS = 0, R/W = 0 */
    P5->OUT = data | control | EN;  /* pulse E */
    SysTick_delayMS(10);
    P5->OUT = data;                 /* clear E */
    P5->OUT = 0;
}
void LCD_command2(int command)
{
    LCD_nibble_write2(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write2(command << 4, 0);      /* then lower nibble */
    if (command < 4)
        SysTick_delayMS(4);         /* commands 1 and 2 need up to 1.64ms */
    else
        SysTick_delayMS(1);         /* all others 40 us */
}
void LCD_data2(int data)
{
    LCD_nibble_write2(data & 0xF0, RS);    /* upper nibble first */
    LCD_nibble_write2(data << 4, RS);      /* then lower nibble  */
    SysTick_delayMS(1);
}










void SysTick_Init(void)
{
    //Initialize systic timer
    SysTick-> CTRL = 0;
    SysTick-> LOAD = 0x00FFFFFF;
    SysTick-> VAL = 0;
    SysTick-> CTRL = 0x00000005;
}
void SysTick_delayMS(uint16_t delay)
{
    //systick delay function
    SysTick->LOAD =((delay * 3000) - 1);       //delay for 1 msecond per delay value
    SysTick->VAL = 0;                                                 // any write to CVR clears it
    while((SysTick -> CTRL  &  0x00010000) == 0); // wait for flag to be SET
}
