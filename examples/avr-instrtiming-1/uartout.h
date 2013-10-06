#include <stdio.h>
#include <avr/io.h>

#if defined(UCSRA)
	#define UN(name,...) name##__VA_ARGS__
#elif defined(UCSR0A)
	#define UN(name,...) name##0##__VA_ARGS__
#elif defined(UCSR1A)
	#define UN(name,...) name##1##__VA_ARGS__
#endif

#ifndef UARTOUT_BAUD
	#define UARTOUT_BAUD 57600
#endif
	
static int uartout_putchar( char c, FILE* s )
{
	(void) s;
	loop_until_bit_is_set( UN(UCSR,A), UN(UDRE) );
	UN(UDR) = c;
	return 0;
}

static FILE uartout = FDEV_SETUP_STREAM( uartout_putchar, NULL, _FDEV_SETUP_WRITE );

static FILE* uartout_init( void )
{
	#ifdef U2X
		UN(UCSR,A) = 1<<U2X;
		int const ubrr = F_CPU /  8.0 / UARTOUT_BAUD + 0.5 - 1;
	#else
		int const ubrr = F_CPU / 16.0 / UARTOUT_BAUD + 0.5 - 1;
	#endif
	UN(UBRR,H) = ubrr >> 8 & 0xFF;
	UN(UBRR,L) = ubrr >> 0 & 0xFF;
	UN(UCSR,B) = 1<<UN(TXEN); // transmit-only
	#ifdef URSEL
		UN(UCSR,C) = 1<<URSEL | 1<<UN(UCSZ,1) | 1<<UN(UCSZ,0);
	#else
		UN(UCSR,C) =            1<<UN(UCSZ,1) | 1<<UN(UCSZ,0);
	#endif
	return &uartout;
}

#undef UN
