#include "msp.h"

//Defining the musical notes needed for the song (frequencies)
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
#define MAX_NOTE 45 // How many notes are in the song below

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


void SetupTimer32s();

int note = 0;       //The note in the music sequence we are on
int breath = 0;     //Take a breath after each note.  This creates seperation

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    SetupTimer32s();                                //Initializes Timer32_1 as a non-interrupt timer and Timer32_2 as a interrupt timers.  Also initializes TimerA and P2.4 for music generation.
    __enable_interrupt();                           //Enable all interrupts for MSP432 (that are turned on)

    while(1)
    {
    }
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
    }
}

/*-------------------------------------------------------------------------------------------------------------------------------
 *
 * void TA0_N_IRQHandler()
 *
 * Interrupt Handler for Timer A0.  The name of this function is set in startup_msp432p401r_ccs.c
 *
 * This handler clears the status of the interrupt for Timer32_A0 CCTL 1 and 2
 *
 * Toggles P2.0 for every interrupt (LED2 Red) through cooperation with Timer_A0
 * Turns on when CCTL1 interrupts.  Turns off with CCTL2 interrupts.
 *
-------------------------------------------------------------------------------------------------------------------------------*/

void TA0_N_IRQHandler()
{
    if(TIMER_A0->CCTL[4] & BIT0) {                  //If CCTL1 is the reason for the interrupt (BIT0 holds the flag)
    }

}
/*-------------------------------------------------------------------------------------------------------------------------------
 *
 * void SetupTimer32s()
 *
 * Configures Timer32_1 as a single shot (runs once) timer that does not interrupt so the value must be monitored.
 * Configures Timer32_2 as a single shot (runs once) timer that does interrupt and will run the interrupt handler 1 second
 * after this function is called (and the master interrupt is enabled).
 *
-------------------------------------------------------------------------------------------------------------------------------*/

void SetupTimer32s()
{
    TIMER32_1->CONTROL = 0b11000011;                //Sets timer 1 for Enabled, Periodic, No Interrupt, No Prescaler, 32 bit mode, One Shot Mode.  See 589 of the reference manual
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
