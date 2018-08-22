/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* 
 * 	i8259_init
 *   DESCRIPTION: initialize both PICs
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: initializes the pics for interrupts
 */
void
i8259_init(void)
{
	outb(MASK, MASTER_DATA);
	outb(MASK, SLAVE_DATA);

	//ICW2: Master PIC vector offset
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW2_MASTER, MASTER_DATA);
	outb(ICW3_MASTER, MASTER_DATA);
	outb(ICW4, MASTER_DATA);
	
	outb(ICW1, SLAVE_8259_PORT);
	
	//ICW2: Slave PIC vector offset
	outb(ICW2_SLAVE, SLAVE_DATA);
	
	//ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(ICW3_SLAVE, SLAVE_DATA);
	
	//ICW4: tell Slave PIC its cascade identity (0000 0010)
	outb(ICW4, SLAVE_DATA);
	
	//Mask the ports
	outb(MASK, MASTER_DATA);
	outb(MASK, SLAVE_DATA);

}

/* 
 * 	 enable_irq
 *   DESCRIPTION: Enables IRQs on the PICs
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Enables IRQ on the given irq_num
 */
void
enable_irq(uint32_t irq_num)
{
	uint16_t port;
	uint8_t val;

	if (irq_num < HALFWAY) {
		port = MASTER_DATA;
	}	
	else {
		port = SLAVE_DATA;
		irq_num =irq_num - HALFWAY;
	}

	val = inb(port) & ~(1 << irq_num);
	outb(val, port);
}

/* 
 * 	 disable_irq
 *   DESCRIPTION: Disabke
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Disables IRQ on the given irq_num
 */
void
disable_irq(uint32_t irq_num)
{
	uint16_t port;
	uint8_t val;

	if (irq_num < HALFWAY) {
		port = MASTER_DATA;
	}	
	else {
		port = SLAVE_DATA;
		irq_num =irq_num - HALFWAY;
	}

	val = inb(port) | (1 << irq_num);
	outb(val, port);
}

/* 
 *   send_eoi
 *   DESCRIPTION: Send end-of-interrupt character
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Signal that we've reached the end of an interrupt
 */
void
send_eoi(uint32_t irq_num)
{
	if(irq_num >= HALFWAY) {
		outb(EOI | (irq_num-HALFWAY), SLAVE_8259_PORT);
		outb(EOI | ANOTHER_MASK, MASTER_8259_PORT);
	}

	outb(EOI | irq_num, MASTER_8259_PORT);
}
