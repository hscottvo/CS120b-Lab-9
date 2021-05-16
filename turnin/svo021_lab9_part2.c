/*	Author: lab
 *  Partner(s) Name: Scott Vo
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *  Demo Link: https://youtu.be/o1LnlaXpIP0
 */
#include <avr/io.h>
#include<avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

unsigned char three_bit = 0x00;
unsigned char three_val = 0x00;
unsigned char blink_val = 0x08;

enum three_states {three_shift} three_state;
enum blink_states {blink_not} blink_state;
enum set_states {set_port} set_state;

void three_tick() {
    switch(three_state) {
        case three_shift: 
            three_val = 0x01 << (three_bit);
            three_bit = (three_bit + 1) % 3;
            break;
        default:
            three_val = 0x00;
    }
}

void blink_tick() {
    switch(blink_state) {
        case blink_not:
            blink_val = ~blink_val & 0x08;
            break;
        default: 
            blink_val = 0x00;
    }
}

void set_tick() {
    switch(set_state) {
        case set_port: 
            PORTB = three_val | blink_val;
            break;
        default: 
            PORTB = 0x00;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    // DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    unsigned short shift_time = 300;
    unsigned short blink_time = 1000;
    unsigned short shift_time_val = 0;
    unsigned short blink_time_val = 0;

    TimerSet(1);
    TimerOn();
    three_state = three_shift;
    blink_state = blink_not;
    set_state = set_port;
    /* Insert your solution below */
    while (1) {
        if (shift_time_val >= shift_time) {
            three_tick();
            shift_time_val = 0;
        } else {
            shift_time_val += 1;
        }
        if (blink_time_val >= blink_time) {
            blink_tick();
            blink_time_val = 0;
        } else {
            blink_time_val += 1;
        }
        set_tick();
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}
