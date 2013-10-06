all:
	avr-gcc -mmcu=atmega8 -DF_CPU=8000000 instrtest.c instrtest.S -o instrtest.elf
	avr-objdump -S instrtest.elf > instrtest.lss
