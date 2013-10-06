#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

static PROGMEM const unsigned crcs [] = {
	#include "correct.h"
};

extern void instrs( void );
extern void instrs_end( void );
extern unsigned test_instr( unsigned );

#include "uartout.h"

int main( void )
{
	// Route stdout to UART
	stdout = uartout_init();
	
	cli();
	
	printf( "// Starting\n" );
	DDRB = 0;
	
	// without this linker strips table from asm code
	static volatile char c = 123;
	c = 0;
	
	char failed = 0;
	unsigned addr = (unsigned) &instrs;
	const unsigned* p = crcs;
	while ( addr < (unsigned) &instrs_end )
	{
		unsigned crc = test_instr( addr );
		unsigned correct = pgm_read_word( p++ );
		printf( "0x%04X,", crc );
		if ( crc != correct )
		{
			printf( " // +0x%03x mismatch; table shows 0x%04X", addr*2, correct );
			failed = 1;
		}
		printf( "\n" );
		
		addr += 2;
		if ( pgm_read_word( addr*2+2 ) == 0 )
			// prev instr was a skip which uses four slots, so skip the last two
			addr += 2;
	}
	
	if ( !failed )
		printf( "// Passed\n" );
	else
		printf( "// Failed\n" );
	
	return 0;
}
