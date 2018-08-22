#ifndef _PIT_H
#define _PIT_H

#include "i8259.h"
#include "IDT.h"
#include "multiprocess.h"

#define MODE_COMMAND_REGISTER 	0x43
#define CHAN_ZERO 				0x40
#define PIT_IRQ					0
#define EIGHT_BIT_MASK			0xFF
#define THIRTY_SIX				0x36
#define HIGHEST_FREQ			1193180
#define LOW_FREQ				10
#define HIGH_FREQ			50

void PIT_init(uint32_t PIT_frequency);


/* References:
http://stackoverflow.com/questions/26321531/pit-not-sending-interrupts-to-irq0
*/

#endif
