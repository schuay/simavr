all:
	avr-gcc -mmcu=atmega8 -DF_CPU=8000000 instrtiming.c instrtiming.S -o instrtiming.elf
	avr-objdump -S instrtiming.elf > instrtiming.lss
