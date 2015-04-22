#!/bin/bash

build() {
    avr-gcc -mmcu=attiny44a -DF_CPU=1000000UL -O -o bin/main.bin src/main.c && \
    avr-objcopy -O ihex bin/main.bin bin/main.hex
}

mkdir -p bin

case "$1" in
    build)
        build
        ;;
    burn)
        build && avrdude -c usbtiny -p t44 -U flash:w:bin/main.hex
        ;;

    fuses)
        avrdude -c usbtiny -p t44 -U lfuse:w:0x62:m -U hfuse:w:0x5f:m
        ;;
    clean)
        rm -rf bin
        ;;
    *)
        echo "Usage: $0 {build|burn|fuses|clean}"
        exit 2
esac

# avrdude -c usbtiny -p t44 -U lfuse:w:0x62:m -U hfuse:w:0x5f:m
