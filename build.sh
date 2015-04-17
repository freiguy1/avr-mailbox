#!/bin/bash

mkdir bin

avr-gcc -mmcu=attiny44a -DF_CPU=1000000UL -O -o bin/main.bin src/main.c && \
avr-objcopy -O ihex bin/main.bin bin/main.hex && \
avrdude -c usbtiny -p t44 -U flash:w:bin/main.hex

rm -rf bin

# avrdude -c usbtiny -p t44 -U lfuse:w:0x62:m -U hfuse:w:0x5f:m
