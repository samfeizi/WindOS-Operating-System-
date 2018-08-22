/* IDT.c - Functions to implement
 * written by student
 */

#include "IDT.h"
#include "lib.h"
#include "keyboard.h"

extern int32_t sys_call_halt(uint8_t status);

/*
 * IDT_init(void)
 *   DESCRIPTION: Initialize the Interrupt Descriptor Table
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes the data structure and array that controls the
 *				   interrupts, exceptions and system calls
 */
void
IDT_init(void)

{
	/*Initialize the IDT table field values*/
	int i;
	for (i = 0; i < NUM_VEC; i++) {
		idt[i].present = 0x1;
		/*	Privilege variable*/
		idt[i].dpl = 0x0;
		idt[i].reserved0 = 0x0;
		idt[i].size = 0x1;
		idt[i].reserved1 = 0x1;
		idt[i].reserved2 = 0x1;
		idt[i].reserved3 = 0x1;
		idt[i].reserved4 = 0x00;
		idt[i].seg_selector = KERNEL_CS;

		/*	Reserved value is changed if there are interrupts > 80*/
		if (i >= FIRST_32_INTERRUPTS) {
			idt[i].reserved3 = 0x0;
		}
	}

	/*	Set up privilege for system calls*/
	idt[SYSTEM_CALL].dpl = PRIVILEGE_LEVEL;
	idt[SYSTEM_CALL].reserved3 = 0;

	/* PIT value init*/
	idt[PIT_LABEL].present = 1;

	/*	Call the wrapper functions for each of the interrupts and exceptions	*/
	SET_IDT_ENTRY(idt[0], divide_by_zero_wrapper);
	SET_IDT_ENTRY(idt[1], debug_exception_wrapper);
	SET_IDT_ENTRY(idt[2], nmi_interrupt_wrapper);
	SET_IDT_ENTRY(idt[3], breakpoint_exception_wrapper);
	SET_IDT_ENTRY(idt[4], overflow_exception_wrapper);
	SET_IDT_ENTRY(idt[5], boundrange_exceed_wrapper);
	SET_IDT_ENTRY(idt[6], invalid_opcode_wrapper);
	SET_IDT_ENTRY(idt[7], device_not_available_wrapper);
	SET_IDT_ENTRY(idt[8], double_fault_wrapper);
	SET_IDT_ENTRY(idt[9], cp_seg_wrapper);
	SET_IDT_ENTRY(idt[10], invalid_tss_wrapper);
	SET_IDT_ENTRY(idt[11], segment_not_present_wrapper);
	SET_IDT_ENTRY(idt[12], stack_fault_wrapper);
	SET_IDT_ENTRY(idt[13], gen_protection_wrapper);
	SET_IDT_ENTRY(idt[14], page_fault_wrapper);
	SET_IDT_ENTRY(idt[16], fpu_wrapper);
	SET_IDT_ENTRY(idt[17], allignment_check_wrapper);
	SET_IDT_ENTRY(idt[18], machine_check_wrapper);
	SET_IDT_ENTRY(idt[19], floating_point_wrapper);
	SET_IDT_ENTRY(idt[PIT_LABEL], PIT);
	SET_IDT_ENTRY(idt[RTC_LABEL], RTC);
	SET_IDT_ENTRY(idt[KEYBOARD_LABEL], KEYBOARD);
	SET_IDT_ENTRY(idt[0x80], SYSTEM_CALL_ECE);
	lidt(idt_desc_ptr); //load the idtr
}


/*
 * keyboard_handler
 *   DESCRIPTION:
 *   INPUTS: none
 *   OUTPUTS: Prints a character to the console
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void
keyboard_handler() {
	getchar();
}


/*
 * divide_by_zero
 *   DESCRIPTION: executes the divide_by_zero handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the divide_by_zero handler
 */
void divide_by_zero() {
	char* a = "divide by 0";
	printf("%s", a);
	sys_call_halt(1);
}


/*
 *   debug_exception
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void debug_exception() {
	char* a = "debug_exception";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * nmi_interrupt
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void nmi_interrupt() {
	char * a = "nmi_interrupt";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * breakpoint_exception
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void breakpoint_exception() {
	char * a = "breakpoint_exception";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * overflow_exception
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void overflow_exception() {
	char * a = "overflow_exception";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * boundrange_exceed
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void boundrange_exceed () {
	char * a = " boundrange_exceed";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * invalid_opcode
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void invalid_opcode() {
	char * a = "invalid_opcode";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * device_not_available
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void device_not_available() {
	char * a = "device_not_available";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * double_fault
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void double_fault() {
	char * a = "double_fault";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * cp_seg
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void cp_seg() {
	char * a = "cp_seg";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * invalid_tss
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void invalid_tss() {
	char * a = "invalid tss";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * segment_not_present
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void segment_not_present() {
	char * a = "segment_not_present";
	printf("%s", a);
	sys_call_halt(1);
}


/*
 * stack_fault
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void stack_fault() {
	char * a = "stack_fault";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * gen_protection
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void gen_protection() {
	char * a = "gen_protection";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * page_fault
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void page_fault() {
	char * a = "page_fault";
	uint32_t cr2;
	asm("\t movl %%cr2,%0" : "=r"(cr2));
	printf("%x\n", cr2);
	printf("%s", a);
	sys_call_halt(1);
}


/*
 * fpu
 *   DESCRIPTION:Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void fpu() {
	char * a = "fpu";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * allignment_check
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void allignment_check() {
	char * a = "allignment_check";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * machine_check
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void machine_check() {
	char * a = "machine_check";
	printf("%s", a);
	sys_call_halt(1);
}

/*
 * floating_point
 *   DESCRIPTION: Generic exception handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Generic exception handler function
 */
void floating_point() {
	char* a = "floating_point";
	printf("%s", a);
	sys_call_halt(1);
}
