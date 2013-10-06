#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega48");

#include <stdio.h>

static int uart_putchar(char c, FILE *stream) {
  if (c == '\n')
    uart_putchar('\r', stream);
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
                                         _FDEV_SETUP_WRITE);
ISR(PCINT0_vect)
{
	printf( "PCINT0" );
}

#define LINE printf( "\nline %d ", __LINE__ );

int main()
{
	stdout = &mystdout;
	
	// PB0 connected to output of divide-by-two
	// PB1 connected to input of divide-by-two
	
	PORTB = 0;
	DDRB = 2; // PB0 = input, PB1 = output
	
		// With PB0 as output, changing it changes PINB0 as expected
LINE	DDRB = 3; // PB0 = output
LINE	PORTB = 0; printf( "PINB: %02X", PINB ); // 00
LINE	PORTB = 1; printf( "PINB: %02X", PINB ); // 01
LINE	PORTB = 0; printf( "PINB: %02X", PINB ); // 00

		// Writing to PINB XORs value with current PINB reading
LINE	DDRB = 0; // PB0 = output
LINE	PINB = 0; printf( "PINB: %02X", PINB ); // 00
LINE	PINB = 1; printf( "PINB: %02X", PINB ); // 01
LINE	PINB = 1; printf( "PINB: %02X", PINB ); // 00
LINE	PINB = 0; printf( "PINB: %02X", PINB ); // 00

		// With PB0 as input, writing to PORTB still changes PINB
LINE	DDRB = 2; // PB0 = input
LINE	PORTB = 0; printf( "PINB: %02X", PINB ); // 00
LINE	PORTB = 1; printf( "PINB: %02X", PINB ); // 01
LINE	PORTB = 0; printf( "PINB: %02X", PINB ); // 00

		// Writing to PINB XORs value with current PINB reading
LINE	DDRB = 2; // PB0 = input
LINE	PINB = 0; printf( "PINB: %02X", PINB ); // 00
LINE	PINB = 1; printf( "PINB: %02X", PINB ); // 01
LINE	PINB = 1; printf( "PINB: %02X", PINB ); // 00
LINE	PINB = 0; printf( "PINB: %02X", PINB ); // 00

		// So if we have the divide-by-two output on PB0, we lose it
LINE	DDRB   =  2; // PB0 = input
LINE	PORTB  =  0; printf( "PINB: %02X", PINB ); // 00
LINE	PORTB |=  2; printf( "PINB: %02X", PINB ); // 03
LINE	PORTB &= ~2; printf( "PINB: %02X", PINB ); // 00 (should be 01)
LINE	PORTB |=  2; printf( "PINB: %02X", PINB ); // 02
LINE	PORTB &= ~2; printf( "PINB: %02X", PINB ); // 00
LINE	PORTB |=  2; printf( "PINB: %02X", PINB ); // 03
LINE	PORTB &= ~2; printf( "PINB: %02X", PINB ); // 00 (should be 01)
LINE	PORTB |=  2; printf( "PINB: %02X", PINB ); // 02
LINE	PORTB &= ~2; printf( "PINB: %02X", PINB ); // 00

		// Setup PB0 pin change
		PCMSK0 = 1<<PCINT0;
		PCIFR |= 1<<PCIF0;
		PCICR |= 1<<PCIE0;
	
		sei();
	
		// Make output port and toggle bit. We expect pin-change interrupt
LINE	DDRB = 1; // PB0 = output
LINE	PORTB = 0;
LINE	PORTB = 0;
LINE	PORTB = 1; // PCINT0
LINE	PORTB = 1;
LINE	PORTB = 0; // PCINT0
LINE	PORTB = 1; // PCINT0
		
		// Make it input port with pull-up disabled (to be sure simulator has no excuse)
		// This shouldn't trigger pin change interrupt.
		MCUCR |= 1<<PUD; // disable pull-ups on inputs
LINE	DDRB = 0; // PB0 = input
LINE	PORTB = 0; // PCINT0 (shouldn't)
LINE	PORTB = 0;
LINE	PORTB = 1; // PCINT0 (shouldn't)
LINE	PORTB = 1;
LINE	PORTB = 0; // PCINT0 (shouldn't)

LINE	DDRB   =  2; // PB1=output PB0=input
LINE	PORTB  =  0;
LINE	PORTB |=  2;
LINE	PORTB &= ~2;
LINE	PORTB |=  2; // PCINT0
LINE	PORTB &= ~2; // PCINT0 (shouldn't) since this incorrectly clears PB0
LINE	PORTB |=  2; // should do PCINT0 here
LINE	PORTB &= ~2;
LINE	PORTB |=  2; // PCINT0
LINE	PORTB &= ~2; // PCINT0 (shouldn't)

	printf( "\n" );
	cli();
	sleep_mode();
}
