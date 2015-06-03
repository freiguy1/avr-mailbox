#include <avr/io.h>       //sets up some constants and 
                          // names of pins
#include <util/delay.h>   //gives me _delay_ms() and 
                          // _delay_us()
#include <avr/interrupt.h>
#include <avr/delay.h>

volatile uint8_t doorOpen = 0;
volatile uint8_t flagUp = 0;
volatile uint8_t beamDetected = 0;


void handleDoorOpen() {
    doorOpen = 1;
    PORTA |= (1<<PA3);
}

void handleDoorClose() {
    doorOpen = 0;
    PORTA &= ~(1<<PA3);
}

void handleInfraredBeamBroken() {
    beamDetected = 0;
    PORTA |= (1<<PA4);
}

void handleInfraredBeamDetected() {
    beamDetected = 1;
    PORTA &= ~(1<<PA4);
}

ISR(ANA_COMP_vect) {
    uint8_t analogComparatorOutput = ACSR & 0x20;
    if(analogComparatorOutput) {
        handleInfraredBeamDetected();
    } else {
        handleInfraredBeamBroken();
    }
    _delay_ms(10);
}

ISR(PCINT0_vect) {
    // If door was closed but we're reading door open
    if(!doorOpen && (PINA & (1<<PCINT0))) {
        handleDoorOpen();
    // If door was open but we're reading door closed
    } else if(doorOpen && !(PINA & (1<<PCINT0))) {
        handleDoorClose();
    }
    _delay_ms(10);
}

int main( void ){

    // DOOR SETUP
    PORTA |= (1<<PA1) // Enable pull up resistor for door
    GIMSK |= (1<<PCIE0); // Enable PCINT 7:0 for door
    PCMSK0 |= (1<<PCINT0); // Enable interrupt 0 for door

    // LASER SETUP
    ACSR |= (1<<ACIE) // Enable analog comparator interrupt on toggle

    //Testing things
    DDRA |= (PA3<<1) | (PA4<<1); // PA3 = Door; PA4 = Laser
    PORTA |= (1<<PA3) | (1<<PA4);
    //End Testing things

    sei();

    while(1){ 
        asm("sleep");
    }
}
