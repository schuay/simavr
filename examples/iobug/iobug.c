#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>

#include "sim_avr.h"
#include "avr_ioport.h"
#include "sim_elf.h"

avr_t * avr = NULL;

void divider_pin_changed(struct avr_irq_t * irq, uint32_t value, void * param)
{
	static int divider;
	//printf( "pin change called\n" );
	if ( value )
	{
		divider ^= 1;
		avr_raise_irq( avr_io_getirq( avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0 ), 
				divider );
	}
	
	// If we update port even when divider didn't change, this generates
	// incorrect pin change interrupts in simulated AVR
	//avr_raise_irq( avr_io_getirq( avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0 ), 
	//		divider );
}

int main(int argc, char *argv[])
{
	elf_firmware_t f;
	const char * fname =  "atmega48_iobug.axf";
	elf_read_firmware(fname, &f);

	printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);

	avr = avr_make_mcu_by_name(f.mmcu);
	if (!avr) {
		fprintf(stderr, "%s: AVR '%s' not known\n", argv[0], f.mmcu);
		exit(1);
	}
	avr_init(avr);
	avr_load_firmware(avr, &f);

	avr_irq_register_notify( avr_io_getirq( avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 1 ),
			divider_pin_changed, NULL );
	
	do {
		avr_run(avr);
	}
	while ( avr->state != cpu_Done );
	
	return 0;
}
