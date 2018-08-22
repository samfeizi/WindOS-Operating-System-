#ifndef _TERMINAL_H
#define _TERMINAL_H



#include "lib.h"
#include "keyboard.h"
#include "types.h"
#define UPPER_LIMIT		0x80
#define KEYBOARD_PORT	0x60
#define ARRAY_SIZE		128


#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15



/* read from a terminal*/
//int32_t read_Terminal(int32_t fd, void *buf, int32_t nbytes);
/* write to the terminal*/
int32_t write_Terminal(int32_t fd, const void *buf, int32_t nbytes);
/* open a terminal*/
int32_t open_Terminal(const uint8_t* filename);	
/* close a terminal*/
int32_t close_Terminal(int32_t fd);


#endif
