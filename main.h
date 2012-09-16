/* 
 * File:   main.h
 * Author: C14651
 *
 * Created on July 13, 2012, 7:16 PM
 */

#ifndef __MAIN_H_
#define	__MAIN_H_

#define WE_DISPLAY_FIRST_SEGMENT 

#define     _XTAL_FREQ      4000000

//Timer 1 stuff
#define     NUM             23875     //put a breakpoint at decrement() and time it as close to 1s as possible
									  //remember that this rolls over twice to achieve this
									  //make it just slight under 1s since it will take it a few instructions to call
								      //and actually write to the PORTs
#define     TMR1H_REG       ((NUM & 0xFF00) >> 8)
#define     TMR1L_REG       (NUM & 0x00FF)

//UART stuff
#define     UART_TX_TRIS    TRISCbits.TRISC6
#define     UART_RX_TRIS    TRISCbits.TRISC7

#define     UART_BUFFER_SIZE    12 //start/stop + (sec * 2 bytes) + (min * 2 bytes)........ year = 13

//transmission sequence:
/**
 *  Each individual digit has a byte
 *
 *  1:                2->10:
 * 
 *  START/LOAD     time ( secs(RRIGHT) ->  secs(LEFT) -> min(RIGHT) ...... hours(LEFT) )
 *
 */

/** Unique "start the display" command. Implies that this is the last digit. */
#define     START_CMND      's'
/** UART start command indicating that this is the middle digit */
#define     START_CMND_MID  ('t'+0x80)
/** UART start command indicating that this is the first digit */
#define     START_CMND_FIRST ('u'+0x80)
/** unique "here is the time" command */
#define     LOAD_CMND       'l'

////bytes for time to load into

typedef enum {
    SUCCESS, //succesfully loaded new numbers
    ERROR, //error occured (maybe too many bytes sent?)
} MESSAGES;

typedef enum {
    IDLE,
    DECR,
    LOAD,
    LOADING,
}RX_STATUS;

//encapsulates a single two digit display

typedef struct {
   uint8_t h; //right digit
   uint8_t l; //left digit
} SINGLE;

//encapsulates the whole array of the 3 displays each with 2 digits
//@warning important to keep them in this order since we point to the start
//         of this struct and fill it in linearly assuming secs is first
typedef struct {
  SINGLE sec;
   SINGLE min;
    SINGLE hour;
     SINGLE day;
    SINGLE month;
   SINGLE year;
} DIGITS;

//rx

typedef struct {
    uint8_t rx; //for receiving commands from UART
    uint8_t offset; //keeping how many byte received
    RX_STATUS status;
    uint8_t* digitPtr; ///compiler problem
} UART_RX;


extern void generalInit(void);
extern void TMR1Init(void);
extern void UARTInit(void);
extern void uartReset(void);
extern void flashMessage(MESSAGES msg);
extern void timerReset(void);
extern uint8_t asciiToHex(char c);
extern void eepromRead(void);
extern void eepromWrite(void);

#endif	/* MAIN_H */

