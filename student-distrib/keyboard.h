#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "IDT.h"
#include "RTC.h"
#include "terminal.h"
#include "multiprocess.h"

#define UPPER_LIMIT		0x80
#define KEYBOARD_PORT	0x60
#define ARRAY_SIZE		128


#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15
#define VIDEO                   0xB8000
#define TERMINAL_WIDTH          80
#define TERMINAL_HEIGHT         24
#define NUMBYTES_IN_LINE        160
#define BLACK					255
#define	TAB_SPACES				4
#define BUFFER_SIZE				128
#define BIT_SHIFT				8
#define BIT_SHIFT2				4
#define MASK2					0x0F
/*
        SCANCODES
*/
#define ENTER_PRESSED       0x1C
#define CAPSLOCK_PRESSED    0x3A
#define BACKSPACE_PRESSED   0x0E
#define TAB_PRESSED         0x0F

#define LSHIFT_PRESSED      0x2A
#define RSHIFT_PRESSED      0x36
#define LSHIFT_RELEASED     0xAA
#define RSHIFT_RELEASED     0xB6


#define CNTRL_PRESSED       0x1D
#define CNTRL_RELEASED      0x9D
#define CNTRL_L_PRESSED     0x26

#define RALT_PRESSED        0x38
#define RALT_RELEASED       0xB8

#define RNDM_PRESSED        0xE0
#define PAGEUP_PRESSED      0x49
#define PAGEDOWN_PRESSED      0x51
#define PRINT_SCREEN_PRESSED	0x37
#define PAUSE_PRESSED	0x14





#define RIGHTCURSOR_PRESSED 0x4D
#define LEFT_CURSOR_PRESSED 0x4B
#define UP_CURSOR_PRESSED 0x48
#define DOWN_CURSOR_PRESSED 0x50

#define INSERT_PRESSED 0x52
#define DELETE_PRESSED 0x53
#define HOME_PRESSED 0x47
#define END_PRESSED 0x4F


#define F1_PRESSED			0x3B
#define F2_PRESSED			0x3C
#define F3_PRESSED			0x3D
#define F4_PRESSED			0x3E
#define F5_PRESSED			0x3F
#define F6_PRESSED			0x40
#define F7_PRESSED			0x41
#define F8_PRESSED			0x42
#define F9_PRESSED			0x43
#define F10_PRESSED			0x44
#define F11_PRESSED			0x57
#define F12_PRESSED			0x58
#define NUMLOCK_PRESSED			0x45
#define SCROLL_LOCK_PRESSED			0x46


/* fuction prototype needed to get a character from the keyboard */
void getchar();
void getchar1(char print);
/*	Write a character to a certain memory address*/
void WriteCharacter(unsigned char c, unsigned char forecolour, unsigned char backcolour, uint16_t x, uint16_t y);

/*	Move the cursor to a certain row and column*/
void fb_move_cursor(uint16_t row, uint16_t col);

/*	Initialize the cursor*/
void init_cursor_c();

/*	Scroll down the screen*/
void scroll();
/*	fill the keyboard buffer*/
void fill_buffer(uint8_t * newBuffer, int32_t size);
/*	return x coordinate*/
uint16_t getX();
/*	return y coordinate*/
uint16_t getY();
/*	get number of characters on screen*/
uint16_t getCharactersOnScreen();

uint8_t get_enter_flag();
/*	set x location*/
void setX(uint16_t terminal_x);
/*	set y location */
void setY(uint16_t terminal_y);
/*	set number of characters on screen*/
void setCharactersOnScreen(uint16_t terminal_characterOnScreen);
int32_t read_Terminal(int32_t fd, void *buf, int32_t nbytes);
uint16_t getBufferSize();
void setBufferSize(uint16_t newBufferSize);
uint16_t getBufferFlag();
/*
    Acknowledgements:
    https://github.com/JRWynneIII/Hobby-kernel/blob/master/keyboard.c
    http://wiki.osdev.org/PS2_Keyboard
*/

#endif
