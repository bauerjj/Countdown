/* 
 * File:   main.c
 * Author: C14651
 *
 * Created on July 13, 2012, 7:10 PM
 */

#include <xc.h>
#include <stdio.h>
#include <stdint.h>


#include "main.h"
#include "counter.h"


__CONFIG(FOSC_XT & WDTE_OFF & PWRTE_OFF & CP_OFF & BOREN_OFF & LVP_OFF & CPD_OFF & WRT_ON);

UART_RX uart; //uart stuff
DIGITS segments; //keep two copies - one as buffer to load next time
uint8_t toggle = 0; //use this to indicate when to decrement
uint8_t rollOver = 0; //TMR1 rollover
uint8_t colonToggle = 0; 

void main(void) {
    generalInit(); //tris stuff
    TMR1Init();	//setup tmr1 as our RTC
    UARTInit(); //uart for comms
     
    eepromRead(); 			//restore values that were last writen to
    TMR1ON = 1; 			//start decrementing right away
    uart.status = DECR; 	//do this too

    while (1) {
        if (toggle) {
            //want to decrement here so that inside interrupt we only display the
            //the segments. This should be take a CONSTANT time!
            //Decrementing values inside of the ISR can take different lengths
            //of time, so it is hard to fine tune TMR1 to rollover every 1s to compensate
            decrement();
            
            toggle = 0; //wait for TMR1 rollover to set this again before decrementing
            //eepromWrite(); //write new values to EEPROM
            
            //RB3 ^=1 ; //don't work
            
            colonToggle  ^= 0xFF;
            if(colonToggle)
            	RB3 = 0;
            else
            	RB3 = 1;
        }
    }
}

void generalInit(void) {
    TRISA = 0x00; //all outputs
    TRISB = 0x00;
    TRISC = 0x00;
    ADCON1 = 0b00000110; //digital

    PORTA = PORTB = PORTC = 0;

    INTCONbits.PEIE = 1; //allow peirpherals (TMR1) to create interrupt
    INTCONbits.GIE = 1; //enable gloabls
}

//slave device ONLY

void UARTInit(void) {
    UART_TX_TRIS = 1; //set as inputs for module to control
    UART_RX_TRIS = 1;

    TXSTAbits.SYNC = 0; //async
    TXSTAbits.TX9 = 0; //8-bit transmission
    TXSTAbits.TXEN = 1; //enable transmisison
    RCSTAbits.RX9 = 0; //8-bit data
    RCSTAbits.SREN = 0; //DNC
    RCSTAbits.CREN = 1; //continuous receive
    RCSTAbits.ADDEN = 0; //no address detect
    BRGH = 1; //high speed
    SPBRG = 25; //9600 BAUD

    RCSTAbits.SPEN = 1; //enable module

    uartReset();

    PIR1bits.RCIF = 0; //clear flag
    PIE1bits.RCIE = 1; //enable UART RX interrupt
}

void TMR1Init(void) {
    //Timer 1 setup to rollover every 1s
    T1CONbits.T1CKPS = 0b11; //1:8 preScale
    T1CONbits.T1OSCEN = 0; //oscillator disabled
    T1CONbits.T1INSYNC = 0; //DNC
    T1CONbits.TMR1CS = 0; //use internal clock (Fosc/4)

    TMR1H = TMR1H_REG; //to rollover every .5 sec
    TMR1L = TMR1L_REG;

    PIR1bits.TMR1IF = 0; //clear flag just in case
    PIE1bits.TMR1IE = 1; //allow TMR1 to create an interrupt
    T1CONbits.TMR1ON = 0; //leave OFF for now
}

void flashMessage(MESSAGES msg) {
    uint8_t i = 4;

    switch (msg) {
        case ERROR:
            while (i--) {
                __delay_ms(200);
                PORTC ^= 0xFF;
                PORTB ^= 0xFF;
                PORTA ^= 0xFF;
            }
            break;
        case SUCCESS:
            while (i--) {
                __delay_ms(200);
                PORTC ^= 0x0F;
                PORTB ^= 0xF0;
                PORTA ^= 0x0F;
            }
            break;
        default:
            break;
    }


}
void eepromRead(void){
	uint8_t i;
	
	uart.digitPtr = &segments.sec.h;
	for(i=0; i!= 13; i++){
	 *(uart.digitPtr++) = eeprom_read(i); 
	}
	uart.digitPtr = &segments.sec.h;
}

void eepromWrite(void){
	uint8_t i;
	
	uart.digitPtr = &segments.sec.h;
	for(i=0; i!= 13; i++){
		eeprom_write(i, *(uart.digitPtr++)); //write struct to EEPROM
	}
	uart.digitPtr = &segments.sec.h;
}

void uartReset(void) {
    uart.offset = 0; //reset
    uart.digitPtr = &segments.sec.h;
    uart.status = IDLE;
}

void timerReset(void) {
    TMR1H = TMR1H_REG; //reload values
    TMR1L = TMR1L_REG;

    T1CONbits.TMR1ON = 0;
}

uint8_t asciiToHex(char c) {
    return (uint8_t) (c - '0');

}

//enter here on rollover every 500ms or uart rx

void interrupt isr() {
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;

        if (uart.status == DECR && rollOver++ == 2) {
            display();
            toggle = 1;
            rollOver = 0;
        }

        T1CONbits.TMR1ON = 0;
        TMR1H = TMR1H_REG; //reload values
        TMR1L = TMR1L_REG;
        T1CONbits.TMR1ON = 1;
    }

    if (PIR1bits.RCIF) {
        PIR1bits.RCIF = 0;

        TXREG = RCREG; //send back

        uart.rx = RCREG;
        switch (uart.status) {
            case IDLE:
                if (uart.rx == START_CMND) {
                    uart.status = DECR;
                    decrement(); //in order to fill 'next_to_display'
                    display();
                    toggle = 1;
                    T1CONbits.TMR1ON = 1;
                }
                break;
            case DECR:
                break;
            case LOAD:
                uart.rx = asciiToHex(RCREG);
                if (uart.rx <= 9) { //only valid numbers allowed
                    *(uart.digitPtr++) = uart.rx;
                    displayNum(uart.rx);
                    if (uart.offset++ == UART_BUFFER_SIZE) {
                        flashMessage(SUCCESS);
                        uartReset();
                        eepromWrite();
                    }
                } else {
                    uartReset();
                    flashMessage(ERROR);
                }
                break;
        }


        switch (uart.offset) {
            case 0: //start/load
                if (uart.rx == START_CMND) { //are we to start counting down?
                    //start decrementing
                    uart.status = DECR;
                    // uart.offset++; //increment here
                } else if (uart.rx == LOAD_CMND) { //load new values
                    timerReset();
                    uart.status = LOAD; //wait for the address sent next byte to match first before loading
                    uart.offset++; //increment here
                } else
                    uart.offset = 0; //stay here

            default:
                break;

        }
    }
}






