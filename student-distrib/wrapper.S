.text
# need to make every wrapper global so they can be called in IDT.c
.globl PIT, RTC, KEYBOARD, initialize_pages, init_cursor, SYSTEM_CALL_ECE, flush_tlb, right_after_iret
.globl divide_by_zero_wrapper, debug_exception_wrapper, nmi_interrupt_wrapper, breakpoint_exception_wrapper
.globl overflow_exception_wrapper, boundrange_exceed_wrapper, invalid_opcode_wrapper
.globl device_not_available_wrapper, double_fault_wrapper, cp_seg_wrapper, invalid_tss_wrapper
.globl segment_not_present_wrapper, stack_fault_wrapper ,gen_protection_wrapper, page_fault_wrapper, fpu_wrapper
.globl allignment_check_wrapper, machine_check_wrapper, floating_point_wrapper
.globl switch_to_user_mode, CHANGE_EBP_AND_ESP, get_esp_wrapper, get_ebp_wrapper


# init_cursor:
# 	outb $14, $0x3D4      #; 14 tells the framebuffer to expect the highest 8 bits of the position
#     outb $0x00, $0x3D5    #; sending the highest 8 bits of 0x0050
#     outb $15, $0x3D4       #; 15 tells the framebuffer to expect the lowest 8 bits of the position
# 	outb $0x50, $0x3D5     #; sending the lowest 8 bits of 0x0050

# initialize_pages
#  DESCRIPTION: Assembly code needed to iniatilize paging
#  INPUTS: Pointer to the page_directory created in paging.c
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: enables the required bits to enable paging
#  shoutout / citation to http://littleosbook.github.io/#paging and http://wiki.osdev.org/Paging
#  for providing example code for the initialization process for paging
initialize_pages:
	push %ebp											#set up stack frame
	movl %esp, %ebp

	movl 8(%esp), %eax						#load in parameters
	movl %eax, %cr3
	movl %cr4, %ebx
																#Put the mask for the CR4
	orl $0x00000010 , %ebx
	movl %ebx, %cr4
	xorl %ebx, %ebx
	movl %cr0, %ebx
																#Put the second mask in CR0
	orl $0x80000000, %ebx
	movl %ebx, %cr0
	leave
	ret

# switch_to_user
#  DESCRIPTION: Takes the program into user mode using iret convention
#  INPUTS: EIP value after entrance into user mode, stack frame location
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: switches to user mode
switch_to_user_mode:
	cli														#clear interrupts

	movl %esp, %ebp								#partially set up stack frame
	xorl %eax, %eax

	mov	$0x2b, %ax								#push the DS value for iret
	mov %ax, %ds
	mov %esp, %eax
	pushl $0x2b

	pushl 8(%ebp)								#push the next ESP address

	pushf												#push eflags or'ed with 0x200
	pop %eax
	or $0x200, %eax
	push %eax

	pushl $0x23									#push CS value for iret

	pushl 4(%ebp)								#push the EIP value
	iret

#TLB flush
#Description: Assembly code needed to flush the TLB
#Inputs: Pointer to the page_directory created in syscalls.c
#Outputs: none
#Return: none
#Side effect: enables the required bits to enable paging and flushing TLB
flush_tlb:
	push %ebp
	movl %esp, %ebp
	movl %cr3, %eax
	movl %eax, %cr3
	leave
	ret

get_esp_wrapper:
	push %ebp
	movl %esp, %ebp
	movl %esp, %eax
	leave
	ret

get_ebp_wrapper:
	push %esp
	movl %esp, %ebp
	movl %ebp, %eax
	leave
	ret

CHANGE_EBP_AND_ESP:
	movl 4(%esp), %ebp
	movl 8(%esp), %esp
	leave
	ret

# SYSTEM_CALL
#  DESCRIPTION: jump table for the system calls
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: redirects to the proper system call
SYSTEM_CALL_ECE:
	push %ebp										#push all necessary registers and set up stack frame
	mov %esp, %ebp
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %ebp
	pushl %esi
	pushl %edi
	pushfl
	pushl %es
	pushl %ds

	cmpl $1, %eax
	jl INVALID_SYS_CALL				#if value in eax is below 1 then we want to return -1
	cmpl $10, %eax
	jg INVALID_SYS_CALL				#if value in eax is greater than 10 then we want to return -1
	pushl %edx						#if the value in eax is good then we push arguments
	pushl %ecx
	pushl %ebx
	decl  %eax						#decrement eax b/c it contains value 1-10 and we need value 0-9 for jump table
	call *jump_table(, %eax, 4)
	popl %ebx						#if the value in eax is good then we push arguments
	popl %ecx
	popl %edx
	popl %ds					#restore stack pointer and reset variables
	popl %es
	popfl
	popl %edi
	popl %esi
	popl %ebp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ebp
	iret
right_after_iret:
	leave
	ret
INVALID_SYS_CALL:
	popl %ds					#restore stack pointer and reset variables
	popl %es
	popfl
	popl %edi
	popl %esi
	popl %ebp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ebp
	movl $-1, %eax
	iret

# PIT
#  DESCRIPTION: Call the PIT handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Executes the PIT handler

	PIT:
		pushal
		call PIT_handler
		popal
		iret


# RTC
#  DESCRIPTION: Call the RTC handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Executes the RTC handler

RTC:
	pushal
	call RTC_handler
	popal
	iret


# keyboard
#  DESCRIPTION: Call the keyboard handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: executes the keyboard_handler

KEYBOARD:
	pushal
	call keyboard_handler
	popal
	iret


# divide_by_zero_wrapper
#  DESCRIPTION: Call divide by zero error handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: executes the divide_by_zero handler

divide_by_zero_wrapper:
	pushal
	call divide_by_zero
	popal
	iret



# debug_exception_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

debug_exception_wrapper:
	pushal
	call debug_exception
	popal
	iret


# nmi_interrupt_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

nmi_interrupt_wrapper:
	pushal
	call nmi_interrupt
	popal
	iret


# breakpoint_exception_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

breakpoint_exception_wrapper:
	pushal
	call breakpoint_exception
	popal
	iret


# overflow_exception_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

overflow_exception_wrapper:
	pushal
	call overflow_exception
	popal
	iret


# boundrange_exceed_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

boundrange_exceed_wrapper:
	pushal
	call boundrange_exceed
	popal
	iret


# invalid_opcode_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

invalid_opcode_wrapper:
	pushal
	call invalid_opcode
	popal
	iret



# device_not_available_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

device_not_available_wrapper:
	pushal
	call device_not_available
	popal
	iret



# double_fault_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

double_fault_wrapper:
	pushal
	call double_fault
	popal
	iret


# cp_seg_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

cp_seg_wrapper:
	pushal
	call cp_seg
	popal
	iret


# invalid_tss_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

invalid_tss_wrapper:
	pushal
	call invalid_tss
	popal
	iret


# segment_not_present_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

segment_not_present_wrapper:
	pushal
	call segment_not_present
	popal
	iret



# stack_fault_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

stack_fault_wrapper:
	pushal
	call stack_fault
	popal
	iret


# gen_protection_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

gen_protection_wrapper:
	pushal
	call gen_protection
	popal
	iret



# page_fault_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

page_fault_wrapper:
	pushal
	call page_fault
	popal
	iret



# fpu_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

fpu_wrapper:
	pushal
	call fpu
	popal
	iret



# allignment_check_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

allignment_check_wrapper:
	pushal
	call allignment_check
	popal
	iret


# machine_check_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

machine_check_wrapper:
	pushal
	call machine_check
	popal
	iret


# floating_point_wrapper
#  DESCRIPTION: Generic exception handler, after storing set flags; restores flags after calling handler
#  INPUTS: none
#  OUTPUTS: none
#  RETURN VALUE: none
#  SIDE EFFECTS: Generic exception handler function

floating_point_wrapper:
	pushal
	call floating_point
	popal
	iret

jump_table:
.long sys_call_halt, sys_call_execute, sys_call_read, sys_call_write, sys_call_open, sys_call_close, sys_call_getargs, sys_call_vidmap, sys_call_set_handler, sys_call_sigreturn
.end
