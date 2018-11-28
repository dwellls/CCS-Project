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

//Variable
enum states
{
    INITIAL,
    TIMEDISPLAY,
    SETTIME,
    SETALARM
};

#endif /* ALARM_HEADER_H_ */
