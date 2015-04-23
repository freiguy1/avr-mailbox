#include <avr/io.h>       //sets up some constants and 
                          // names of pins
#include <util/delay.h>   //gives me _delay_ms() and 
                          // _delay_us()
#include <avr/interrupt.h>

volatile uint8_t doorOpen = 0;
volatile uint8_t flagUp = 0;
volatile uint8_t beamDetected = 0;


void handleDoorOpen() {
    doorOpen = 1;
    PORTA |= (1<<PA2);
}

void handleDoorClose() {
    doorOpen = 0;
    PORTA &= ~(1<<PA2);
}

void handleInfraredBeamBroken() {
    beamDetected = 0;
    PORTA |= (1<<PA3);
}

void handleInfraredBeamDetected() {
    beamDetected = 1;
    PORTA &= ~(1<<PA3);
}

ISR(PCINT0_vect) {
    // If door was closed but we're reading door open
    if(!doorOpen && (PINA & PCINT1)) {
        handleDoorOpen();
    // If door was open but we're reading door closed
    } else if(doorOpen && !(PINA & PCINT1)) {
        handleDoorClose();
    }

    // If beam detected and now it is not
    if(beamDetected && (PINA & PCINT0)) {
        handleInfraredBeamBroken();
    // If beam not detected and now it is
    } else if(!beamDetected && !(PINA & PCINT0)) {
        handleInfraredBeamDetected();
    }
}

int main( void ){

    PORTA |= (PA0<<1) | (PA1<<1); // Enable pull up resistor for door

    GIMSK |= PCIE0; // Enable PCINT 7:0
    PCMSK0 |= (PCINT0 | PCINT1); // Select which pin interrupt is enabled

    //Testing things
    DDRA |= (PA2<<1) | (PA3<<1);
    //End Testing things

    sei();
    

    while(1){ 
        asm("sleep");
    }

}
