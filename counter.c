/*
 * File:   counter.c
 * Author: C13114
 *
 * Created on July 13, 2012, 7:02 PM
 */

#include <xc.h>
#include "counter.h"
#include "main.h"

extern DIGITS segments;

void displayNum(uint8_t num) {
    PORTA = MSD_PORTA[num];
    PORTB = LSD_PORTB[num];
    PORTC = MSD_PORTC[num] | LSD_PORTC[num];
}

volatile uint8_t next_to_display[3] = {0, 0, 0};

void display(void) {
    // Display the characters we previously calculated
    PORTA = next_to_display[0];
    PORTB = next_to_display[1];
    PORTC = next_to_display[2];

}

void decrement(void) {

    if (segments.sec.l == 0) {
        segments.sec.l = 9;

        if (segments.sec.h == 0) {
            segments.sec.h = 5;

            // -- begin decrement minutes
            if (segments.min.l == 0) {
                segments.min.l = 9;

                if (segments.min.h == 0) {
                    segments.min.h = 5;

                    // -- begin decrement hours

                    if (segments.hour.l == 0){
                        if(segments.hour.h == 0)
                             segments.hour.l = 3;
                        else
                            segments.hour.l = 9;

                        if (segments.hour.h == 0) {
                            segments.hour.h = 2;

                            // -- begin decrement days
                            if (segments.day.l == 0) {
                                segments.day.l = 9;

                                if (segments.day.h == 0) {
                                    segments.day.h = 3;

                                    // need to decrement months

                                } else {
                                    segments.day.h--;
                                }
                            } else {
                                segments.day.l--;
                            }
                            // -- end decrement days
                        } else {
                            segments.hour.h--;
                        }
                    } else {
                            segments.hour.l--;
                    }
                    // -- end decrement hours
                } else {
                    segments.min.h--;
                }
            } else {
                segments.min.l--;
            }
            // -- end decrement minutes
        } else {
            segments.sec.h--;
        }
    } else {
        segments.sec.l--;
    }


    // Determine which to next display
    if (segments.day.h != 0 || segments.day.l != 0) {
#if defined(WE_DISPLAY_FIRST_SEGMENT)
        next_to_display[0] = MSD_PORTA[segments.day.h];
        next_to_display[1] = LSD_PORTB[segments.day.l];
        next_to_display[2] = MSD_PORTC[segments.day.h] | LSD_PORTC[segments.day.l];
#elif defined(WE_DISPLAY_MIDDLE_SEGMENT)
        next_to_display[0] = MSD_PORTA[segments.hour.h];
        next_to_display[1] = LSD_PORTB[segments.hour.l];
        next_to_display[2] = MSD_PORTC[segments.hour.h] | LSD_PORTC[segments.hour.l];
#else // defined(WE_DISPLAY_LAST_SEGMENT)
        next_to_display[0] = MSD_PORTA[segments.min.h];
        next_to_display[1] = LSD_PORTB[segments.min.l];
        next_to_display[2] = MSD_PORTC[segments.min.h] | LSD_PORTC[segments.min.l];
#endif
    } else if (segments.hour.h != 0 || segments.hour.l != 0) {
#if defined(WE_DISPLAY_FIRST_SEGMENT)
        next_to_display[0] = MSD_PORTA[segments.hour.h];
        next_to_display[1] = LSD_PORTB[segments.hour.l];
        next_to_display[2] = MSD_PORTC[segments.hour.h] | LSD_PORTC[segments.hour.l];
#elif defined(WE_DISPLAY_MIDDLE_SEGMENT)
        next_to_display[0] = MSD_PORTA[segments.min.h];
        next_to_display[1] = LSD_PORTB[segments.min.l];
        next_to_display[2] = MSD_PORTC[segments.min.h] | LSD_PORTC[segments.min.l];
#else // defined(WE_DISPLAY_LAST_SEGMENT)
        next_to_display[0] = MSD_PORTA[segments.sec.h];
        next_to_display[1] = LSD_PORTB[segments.sec.l];
        next_to_display[2] = MSD_PORTC[segments.sec.h] | LSD_PORTC[segments.sec.l];
#endif
    } else if (segments.min.h != 0 || segments.min.l != 0) {
#if defined(WE_DISPLAY_FIRST_SEGMENT)
        next_to_display[0] = MSD_PORTA[0];
        next_to_display[1] = LSD_PORTB[0];
        next_to_display[2] = MSD_PORTC[0] | LSD_PORTC[0];
#elif defined(WE_DISPLAY_MIDDLE_SEGMENT)
        next_to_display[0] = MSD_PORTA[segments.min.h];
        next_to_display[1] = LSD_PORTB[segments.min.l];
        next_to_display[2] = MSD_PORTC[segments.min.h] | LSD_PORTC[segments.min.l];
#else // defined(WE_DISPLAY_LAST_SEGMENT)
        next_to_display[0] = MSD_PORTA[segments.sec.h];
        next_to_display[1] = LSD_PORTB[segments.sec.l];
        next_to_display[2] = MSD_PORTC[segments.sec.h] | LSD_PORTC[segments.sec.l];
#endif
    } else // only one part left - if (segments.sec.h != 0 || segments.sec.h != 0)
    {
#if defined(WE_DISPLAY_FIRST_SEGMENT)
        next_to_display[0] = MSD_PORTA[0];
        next_to_display[1] = LSD_PORTB[0];
        next_to_display[2] = MSD_PORTC[0] | LSD_PORTC[0];
#elif defined(WE_DISPLAY_MIDDLE_SEGMENT)
        next_to_display[0] = MSD_PORTA[0];
        next_to_display[1] = LSD_PORTB[0];
        next_to_display[2] = MSD_PORTC[0] | LSD_PORTC[0];
#else // defined(WE_DISPLAY_LAST_SEGMENT)
        next_to_display[0] = MSD_PORTA[segments.sec.h];
        next_to_display[1] = LSD_PORTB[segments.sec.l];
        next_to_display[2] = MSD_PORTC[segments.sec.h] | LSD_PORTC[segments.sec.l];
#endif
    }

}