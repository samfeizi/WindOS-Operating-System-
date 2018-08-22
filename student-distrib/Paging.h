#ifndef _PAGING_H
#define _PAGING_H

//change names to be more descriptive

#include "lib.h"
#define NUMBER_OF_ENTRIES					1024
#define SIZE_OF_TABLE						4096
#define STARTING_ADDRESS_OF_FRAMEBUFFER		0xB8000
#define SYSTEM_CALL							0x80
#define KERNEL_PHYSICAL_ADDRESS				0x00400000
#define VIRTUAL_VALUE						12
#define MASK3								0x00000003
#define MASK4								0x100
#define PRESENT_BIT							0x1
#define READ_WRITE_BIT						0x2
#define USER_BIT							0x4
#define SIZE_BIT							0x80


uint32_t page_directory [NUMBER_OF_ENTRIES] __attribute__((aligned (SIZE_OF_TABLE)));
uint32_t page_table [NUMBER_OF_ENTRIES] __attribute__((aligned (SIZE_OF_TABLE)));
uint32_t page_table_vidmap [NUMBER_OF_ENTRIES]  __attribute__((aligned(SIZE_OF_TABLE)));

uint32_t page_table_terminals[NUMBER_OF_ENTRIES]  __attribute__((aligned(SIZE_OF_TABLE)));

/*function prototype needed to initialize paging */
void init_paging();


/*
    Acknowledgements:
        http://littleosbook.github.io/#paging
        http://wiki.osdev.org/Paging
        https://en.wikipedia.org/wiki/Control_register#CR0
        https://courses.engr.illinois.edu/ece391/secure/assignments/mp/mp3/mp3.pdf
*/


#endif
