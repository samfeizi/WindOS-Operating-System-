/* IDT.h - Defines used in interactions with the IDT
 * created by student
 *
 */


#ifndef _IDT_H
#define _IDT_H

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "keyboard.h"
#include "RTC.h"
#include "PIT.h"

#define DISABLE_NMI			0x70
#define READ_WRITE_CMOS		0x71
#define REGISTER_C			0x0C
#define RTC_INTERRUPT		8
#define SYSTEM_CALL			0x80
#define RTC_LABEL			0x28
#define PIT_LABEL         0x20
#define KEYBOARD_LABEL		0x21
#define PRIVILEGE_LEVEL		0x3
#define FIRST_32_INTERRUPTS	32
/* IDTR description */
/* 47----IDT BASE REGISTER----16 15-----IDT LIMIT ----- 0 */

/* IDT */
/* 31 ----Gate for interrupt---0 */
/* 255-0 long */
/* LIDT loads IDTR */

/* interrupt gate */
/* 31 ----offset---- 16  P 14--DPL--13 12---0D110-8 7--OOO---4 3--0 Reserved

 switch to kernel mode, save some processor registers

 must set up a system call handler IDT at index 0x80
can differentiate based on EAX parameter

hardware interrupt handlers and exception handlers should have their DPL set o 0

system call handler should have its DPL set to 3 so int is usable
TSS  = SS0 stack segment ESP0 stack pointer


Initialize both PICs */
void IDT_init(void);

/* fucntion prototypes for the exceptions */
extern void divide_by_zero();
extern void debug_exception();
extern void nmi_interrupt();
extern void breakpoint_exception();
extern void overflow_exception();
extern void boundrange_exceed();
extern void invalid_opcode();
extern void device_not_available();
extern void double_fault();
extern void cp_seg();
extern void invalid_tss();
extern void segment_not_present();
extern void stack_fault();
extern void gen_protection();
extern void page_fault();
extern void fpu();
extern void allignment_check();
extern void machine_check();
extern void floating_point();

#endif /* _IDT_H */
