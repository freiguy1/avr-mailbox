#!/bin/bash

avr-gcc -mmcu=attiny44a -DF_CPU=1000000UL -O -o main.bin main.c && \
avr-objcopy -O ihex main.bin main.hex && \
avrdude -c usbtiny -p t44 -U flash:w:main.hex

rm main.hex main.bin

# avrdude -c usbtiny -p t44 -U lfuse:w:0x62:m -U hfuse:w:0x5f:m
