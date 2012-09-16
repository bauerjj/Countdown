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
SEGMENT seg = SEGMENT_LAST;

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
        
        if (segments.sec.h == 0) {
            
            // -- begin decrement minutes
            if (segments.min.l == 0) {
                
                if (segments.min.h == 0) {
                    
                    // -- begin decrement hours
                    if (segments.hour.l == 0) {

                        if (segments.hour.h == 0) {
                            
                            // -- begin decrement days
                            if (segments.day.l == 0) {
                                
                                if (segments.day.h == 0) {
                                    
                                    // -- begin decrement months
                                    if (segments.month.l == 0) {
                                        
                                        if (segments.month.h == 0) {
                                            
                                            // -- begin decrement years
                                            if (segments.year.l == 0) {
                                                
                                                if (segments.year.h == 0) {
                                                    // Counter has hit zero
                                                    return; // effectively holds count at 0
                                                } else {
                                                    segments.year.h--;
                                                }
                                                segments.year.l = 9;
                                            } else {
                                                segments.year.l--;
                                            }
                                            // -- end decrement years
                                            segments.month.h = 3; // Note: This assumes 30 days in each month
                                        } else {
                                            segments.month.h--;
                                        }
                                        segments.month.l = 9;
                                    } else {
                                        segments.month.l--;
                                    }
                                    // -- end decrement months
                                    segments.day.h = 3;
                                } else {
                                    segments.day.h--;
                                }
                                segments.day.l = 9;
                            } else {
                                segments.day.l--;
                            }
                            // -- end decrement days
                            segments.hour.h = 2;
                            segments.hour.l = 3;
                        } else {
                            segments.hour.h--;
                        }
                        segments.hour.l = 9;
                    } else {
                        segments.hour.l--;
                    }
                    // -- end decrement hours
                    segments.min.h = 5;
                } else {
                    segments.min.h--;
                }
                segments.min.l = 9;
            } else {
                segments.min.l--;
            }
            // -- end decrement minutes
            segments.sec.h = 5;
        } else {
            segments.sec.h--;
        }
        segments.sec.l = 9;
    } else {
        segments.sec.l--;
    }


    // Determine which to next display
    if (segments.day.h != 0 || segments.day.l != 0) {
        if (seg == SEGMENT_FIRST) {
            next_to_display[0] = MSD_PORTA[segments.day.h];
            next_to_display[1] = LSD_PORTB[segments.day.l];
            next_to_display[2] = MSD_PORTC[segments.day.h] | LSD_PORTC[segments.day.l];
        } else if (seg == SEGMENT_MIDDLE) {
            next_to_display[0] = MSD_PORTA[segments.hour.h];
            next_to_display[1] = LSD_PORTB[segments.hour.l];
            next_to_display[2] = MSD_PORTC[segments.hour.h] | LSD_PORTC[segments.hour.l];
        } else {
            // Assume segment is SEGMENT_LAST if not FIRST or MIDDLE
            next_to_display[0] = MSD_PORTA[segments.min.h];
            next_to_display[1] = LSD_PORTB[segments.min.l];
            next_to_display[2] = MSD_PORTC[segments.min.h] | LSD_PORTC[segments.min.l];
        }
    } else if (segments.hour.h != 0 || segments.hour.l != 0) {
        if (seg == SEGMENT_FIRST) {
            next_to_display[0] = MSD_PORTA[segments.hour.h];
            next_to_display[1] = LSD_PORTB[segments.hour.l];
            next_to_display[2] = MSD_PORTC[segments.hour.h] | LSD_PORTC[segments.hour.l];
        } else if (seg == SEGMENT_MIDDLE) {
            next_to_display[0] = MSD_PORTA[segments.min.h];
            next_to_display[1] = LSD_PORTB[segments.min.l];
            next_to_display[2] = MSD_PORTC[segments.min.h] | LSD_PORTC[segments.min.l];
        } else {
            // Assume segment is SEGMENT_LAST if not FIRST or MIDDLE
            next_to_display[0] = MSD_PORTA[segments.sec.h];
            next_to_display[1] = LSD_PORTB[segments.sec.l];
            next_to_display[2] = MSD_PORTC[segments.sec.h] | LSD_PORTC[segments.sec.l];
        }
    } else if (segments.min.h != 0 || segments.min.l != 0) {
        if (seg == SEGMENT_FIRST) {
            next_to_display[0] = MSD_PORTA[0];
            next_to_display[1] = LSD_PORTB[0];
            next_to_display[2] = MSD_PORTC[0] | LSD_PORTC[0];
        } else if (seg == SEGMENT_MIDDLE) {
            next_to_display[0] = MSD_PORTA[segments.min.h];
            next_to_display[1] = LSD_PORTB[segments.min.l];
            next_to_display[2] = MSD_PORTC[segments.min.h] | LSD_PORTC[segments.min.l];
        } else {
            // Assume segment is SEGMENT_LAST if not FIRST or MIDDLE
            next_to_display[0] = MSD_PORTA[segments.sec.h];
            next_to_display[1] = LSD_PORTB[segments.sec.l];
            next_to_display[2] = MSD_PORTC[segments.sec.h] | LSD_PORTC[segments.sec.l];
        }
    } else {
        // only one part left - if (segments.sec.h != 0 || segments.sec.h != 0)
        if (seg == SEGMENT_FIRST) {
            next_to_display[0] = MSD_PORTA[0];
            next_to_display[1] = LSD_PORTB[0];
            next_to_display[2] = MSD_PORTC[0] | LSD_PORTC[0];
        } else if (seg == SEGMENT_MIDDLE) {
            next_to_display[0] = MSD_PORTA[0];
            next_to_display[1] = LSD_PORTB[0];
            next_to_display[2] = MSD_PORTC[0] | LSD_PORTC[0];
        } else {
            // Assume segment is SEGMENT_LAST if not FIRST or MIDDLE
            next_to_display[0] = MSD_PORTA[segments.sec.h];
            next_to_display[1] = LSD_PORTB[segments.sec.l];
            next_to_display[2] = MSD_PORTC[segments.sec.h] | LSD_PORTC[segments.sec.l];
        }
    }
}
