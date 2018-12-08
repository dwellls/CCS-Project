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
void SysTick_Initialize(void); //Systick init
void SysTick_delay(uint16_t delay); //Systick 
void ADC14_init(void); //ADC init
void PortADC_init(void); //Port6 ADC init
void SysTick_init_interrupt(void); //Systick INit
void LCD_init(void); //LCD init
void LCD_nibble_write(unsigned char data, unsigned char control); //Nibble send LCD
void LCD_command(unsigned char command); //LCD commands
void LCD_data(unsigned char data); //LCD write
void green_LED_pins(void); //LED with timerA2 init
void green_LED_pins2(void); //LED with timerA2 init
void SetupTimer32s(); 
void TA_init(void); //TimerA init
void writeOutput(char *string); // write output charactrs to the serial port
void readInput(char* string); // read input characters from INPUT_BUFFER that are valid
void setupSerial(); // Sets up serial for use and enables interrupts


//Variable
volatile uint32_t timeout;
static volatile uint16_t result;
float nADC; //ADC value
float temp_c; //temp of celcius
float temp_f; //temp of fahrenheit
char temp[10]; 
char second[60]; //seconds array
char minute[60]; //minutes array
char hour[60]; //hours array
int i; //increment variable /flag
int j; //increment varaible /flag
char time[12];
char alarm[16];
char alarmtime[12];
char INPUT_BUFFER[100];
// initializing the starting position of used buffer and read buffer


//macros for LCD
#define RS BIT1
#define RW BIT2
#define EN BIT3
//macros for speaker frequencies
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
// Making a buffer of 100 characters for serial to store to incoming serial data



#endif /* ALARM_HEADER_H_ */
