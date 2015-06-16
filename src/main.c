#include <avr/io.h>       //sets up some constants and 
                          // names of pins
#include <util/delay.h>   //gives me _delay_ms() and 
                          // _delay_us()
#include <avr/interrupt.h>

#define FLAG_VALUE_UP 670
#define FLAG_VALUE_DOWN 1350
#define FLAG_STATUS_UP 1
#define FLAG_STATUS_DOWN 0
#define LIGHT_STATUS_OFF 0
#define LIGHT_STATUS_ON 1
#define LIGHT_STATUS_TURNING_ON 2

volatile uint8_t doorOpen = 0;
volatile uint8_t beamDetected = 0;

volatile uint8_t flagStatus = 0;
volatile uint8_t flagTimer = 0;

volatile uint8_t lightStatus = 0;
volatile uint8_t lightCounter = 0;

void turnOffLight() {
    OCR0A = 0;
    DDRB &= ~(1<<PB2);
}

void handleDoorOpen() {
    doorOpen = 1;
    PORTA |= (1<<PA3);
    turnOffLight();
    DDRB |= (1<<PB2);
    lightCounter = 0;
    lightStatus = LIGHT_STATUS_TURNING_ON;
}

void handleDoorClose() {
    doorOpen = 0;
    PORTA &= ~(1<<PA3);
    turnOffLight();
    lightStatus = LIGHT_STATUS_OFF;
}

void handleInfraredBeamBroken() {
    beamDetected = 0;
    PORTA |= (1<<PA4);
    flagTimer = 0;
    if(flagStatus == FLAG_STATUS_DOWN) {
        OCR1A = FLAG_VALUE_UP;
        flagStatus = FLAG_STATUS_UP;
    }
}

void handleInfraredBeamDetected() {
    beamDetected = 1;
    PORTA &= ~(1<<PA4);
}

inline void setServoDegrees(uint8_t degree) {
    uint16_t result = (50 * (uint16_t)degree) / 9 + 650;
    OCR1A = result;
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

// EVERY 20 ms or 50hz
ISR(TIM1_OVF_vect) {
    if (flagStatus == FLAG_STATUS_UP) {
        flagTimer++;
        if (flagTimer >= 150) {
            OCR1A = FLAG_VALUE_DOWN;
            flagStatus = FLAG_STATUS_DOWN;
        }
    }

    if (lightStatus == LIGHT_STATUS_TURNING_ON) {
        lightCounter++;
        if (lightCounter >= 150) {
            lightStatus = LIGHT_STATUS_ON;
            OCR0A = 255;
        } else {
            //OCR0A = (uint8_t)(((uint16_t)255 * lightCounter) / 200);
            OCR0A = (uint8_t)((uint16_t)lightCounter * lightCounter / 88);
        }
    }
}

int main( void ){

    // DOOR SETUP
    PORTA |= (1<<PA0); // Enable pull up resistor for door
    GIMSK |= (1<<PCIE0); // Enable PCINT 7:0 for door
    PCMSK0 |= (1<<PCINT0); // Enable interrupt 0 for door

    // LASER SETUP
    ACSR |= (1<<ACIE); // Enable analog comparator interrupt on toggle
    PORTA |= (1<<PA2);

    // FLAG SETUP
    ICR1 = 0x4E1F;
    DDRA |= (1<<PA6);
    TCCR1A = (1<<COM1A1) | (1<<WGM11);
    TCCR1B = (1<<WGM12) | (1<<WGM13) | (1<<CS10);
    TIMSK1 = (1<<TOIE1);
    OCR1A = FLAG_VALUE_DOWN;

    // LIGHT SETUP
    TCCR0A |= (1<<COM0A1) | (1<<WGM01) | (1<<WGM00);
    TCCR0B |= (1<<CS00);
    OCR0A = 0;

    //Testing things
    DDRA |= (1<<PA3) | (1<<PA4); // PA3 = Door; PA4 = Laser
    // PORTA |= (1<<PA3) | (1<<PA4);
    //End Testing things

    sei();

    while(1){ 
        asm("sleep");
    }
}
