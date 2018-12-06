/*
 * Authors: David Brinkmeier & Danny Weller
 * Class: EGR 226 - 908
 * Date of code beginning: 11/16/2018
 * Due date of Project: 12/7/2018
 * Professor Zuidema
 * Description: Header file for the Alarm Project
 *
 *
 * Pins and attachments:
 * Buttons
 * ____________
 *
 * BLACK BUTTON For 1 second = 1 second  6.0
 * GREY BUTTON For 1 second = 1 minute 6.1
 * GREEN BUTTON For set time 6.4
 * WHITE BUTTON For set alarm 6.5
 * RED BUTTON for ON/OFF/UP 6.6
 * YELLOW BUTTON for SNOOZE/DOWN 6.7
 *
 */

#ifndef ALARM_HEADER_H_
#define ALARM_HEADER_H_

//Function prototypes
void BUTTONseconds(void);
void SysTick_Initialize(void);
void SysTick_delay(uint16_t delay);
void ADC14_init(void);
void PortADC_init(void);
void SysTick_init_interrupt(void);
void LCD_init(void);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void green_LED_pins(void);
void green_LED_pins2(void);
void SetupTimer32s();
void TA_init(void);


//Variable
volatile uint32_t timeout;
static volatile uint16_t result;
float nADC;
float temp_c;
float temp_f;
char temp[10];
char second[60];
char minute[60];
char hour[60];
int i;
int j;
char time[12];
char alarm[16];
char alarmtime[12];

//macros
#define RS BIT1
#define RW BIT2
#define EN BIT3
#define C4 261.63
#define D4 293.66
#define E4 329.63
#define F4 349.23
#define G4 392
#define A4 440
#define REST 0
//Defining the lengths of notes
#define QUARTER 1000000
#define HALF 2000000
#define BREATH_TIME 50000
#define WHOLE 4000000
#define MAX_NOTE 44 // How many notes are in the song below


#endif /* ALARM_HEADER_H_ */
