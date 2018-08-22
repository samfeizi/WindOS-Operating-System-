#ifndef _RTC_H
#define _RTC_H
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "IDT.h"
#include "keyboard.h"
#define DISABLE_NMI			0x70
#define READ_WRITE_CMOS		0x71
#define PORT2				0x40
/*
            REGISTERS
*/
#define REGISTER_A    0x8A
#define REGISTER_B			0x8B
/*
            BIT MASKERS
*/
#define UPPER_FOUR_BIT_MASK 0xF0
#define LOWER_FOUR_BIT_MASK 0x0F
/*
           RATE DEFINITIONS
*/

#define LOWRANGE 3
#define HIGHRANGE 15
#define DEFAULT_RATE_FOR_RTC 6
#define DEFAULT_FREQ_FOR_RTC 1024

/* function prototype to iniatilize real-time-clock */
void RTC_init();
void RTC_handler();
int get_RTC_iflag(int val);

/* This function wait for the interrupt flag */
int32_t RTC_Read(int32_t fd, void* buf, int32_t nbytes);

/* This function sets the frequency for the RTC */
int32_t RTC_Write (int32_t fd, const void* buf, int32_t nbytes);

/* This function creates an entry in the PCB that sets the file as open*/
int32_t RTC_Open(const uint8_t* filename);

/* This function creates an entry in the PCB that set the file as closed*/
int32_t RTC_Close(int32_t fd);

/* This is the helper function to RTC_Write which sets the freqency of the RTC */
int32_t RTC_set_frequency(uint16_t freq);




/*
    Ackhnowledgement:
    http://wiki.osdev.org/RTC
    http://wiki.osdev.org/CMOS
*/
#endif
