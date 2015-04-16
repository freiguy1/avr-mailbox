#include <avr/io.h>       //sets up some constants and 
                          // names of pins
#include <util/delay.h>   //gives me _delay_ms() and 
                          // _delay_us()
#include <avr/interrupt.h>

volatile uint8_t isDisplayingTensDigit = 0;
volatile uint8_t tempToDisplay = 0;


void digitToDisplay(uint8_t digit) {
    if(digit > 9) {
        return;
    }
    uint8_t aFilter = PORTA & 0x1B;
    uint8_t bFilter = PORTB & 0xF0;
    switch(digit) {
        case 0:
            PORTA = aFilter | 0x00;
            PORTB = bFilter | 0x04;
            break;
        case 1:
            PORTA = aFilter | 0xC0;
            PORTB = bFilter | 0x07;
            break;
        case 2:
            PORTA = aFilter | 0x84;
            PORTB = bFilter | 0x00;
            break;
        case 3:
            PORTA = aFilter | 0x80;
            PORTB = bFilter | 0x01;
            break;
        case 4:
            PORTA = aFilter | 0x40;
            PORTB = bFilter | 0x03;
            break;
        case 5:
            PORTA = aFilter | 0x20;
            PORTB = bFilter | 0x01;
            break;
        case 6:
            PORTA = aFilter | 0x20;
            PORTB = bFilter | 0x00;
            break;
        case 7:
            PORTA = aFilter | 0x80;
            PORTB = bFilter | 0x07;
            break;
        case 8:
            PORTA = aFilter | 0x00;
            PORTB = bFilter | 0x00;
            break;
        case 9:
            PORTA = aFilter | 0x00;
            PORTB = bFilter | 0x03;
            break;
    }
}



ISR(TIM0_OVF_vect) {
    if(isDisplayingTensDigit) { // Displaying tens, now display ones
        isDisplayingTensDigit = 0;
        PORTA &= ~(1 << PA4);
        digitToDisplay(tempToDisplay % 10);
        PORTA |= (1 << PA3);
    } else { // Displaying ones, now display tens
        isDisplayingTensDigit = 1;
        PORTA &= ~(1 << PA3);
        digitToDisplay(tempToDisplay / 10);
        PORTA |= (1 << PA4);
    }
}

ISR(ADC_vect) {
    uint8_t low = ADCL;
    uint16_t adcTemp = ADCH;
    adcTemp = (adcTemp << 8) | low;
    //tempToDisplay = (uint8_t)((float)adcTemp / 5.1717 -  72); //1.1v internal ref
    tempToDisplay = (uint8_t)((float)adcTemp * .1933 - 57); //1.1v internal ref
}

int main( void ){

    // Configure 7-seg LED output pins
    DDRB |= 0x07;
    PORTB &= ~(0x07);
    DDRA |= 0xFC;
    PORTA &= ~(0xFC);

    // Configure 7-seg LED 60 Hz strobe
    TCCR0B |= 0x02; // clk/1 (1000000 hz / 256 / 8 = fast)
    TIMSK0 |= 0x01; // overflow interrupt enable


    // Temperature ADC setup
    ADMUX |= 0x81; // 10 (internal 1.1v ref) 000001 (ADC1)
    ADCSRA |= 0x8F; // ADC Enable, ADC Int enable, 128 division

    sei();



    while(1){ 
        ADCSRA |= 1 << ADSC;
        _delay_ms(5000);
    }

    // avr-gcc -mmcu=attiny84 -DF_CPU=8000000 -O src/main.c -o main

}
