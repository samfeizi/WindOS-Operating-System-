#include "terminal.h"



extern char buffer[ARRAY_SIZE];
uint16_t bufferSize;

/*
 *   open_terminal
 *   DESCRIPTION: opens a new terminal
 *   INPUTS: filename - filename of new terminal
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t open_Terminal(const uint8_t* filename)
{
	return 0;
}
/*
 *   close_terminal
 *   DESCRIPTION: closes a terminal
 *   INPUTS: fd - file directory for stuff to be learned later
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t close_Terminal(int32_t fd)
{
	return 0;
}

/*
 *   write_terminal
 *   DESCRIPTION: writes to a terminal
 *   INPUTS: fd - file directory for stuff to be learned later
 *			 buf - buffer to be written out to terminal
 *			 nbytes - number of bytes to be read
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t write_Terminal(int32_t fd, const void *buf, int32_t nbytes) // this is like cout (outputs characters to the screen)
{
	//get the current x and y values in the terminal 
	uint16_t terminal_x = getX();
	uint16_t terminal_y = getY();
	int i;
	if(terminal_y == (TERMINAL_HEIGHT + 2))
		terminal_y = terminal_y - 1;
	if(terminal_y == (TERMINAL_HEIGHT + 1))
	{
		//if we are on the last line then want to scroll everything up so the whole file can be printed
		int j = 0;
		terminal_y = terminal_y - 1;
					
		for(j = 0; j < terminal_y; j++)
		{
			if(j < TERMINAL_HEIGHT)
				memcpy(((uint8_t *)VIDEO + ((j)) * NUMBYTES_IN_LINE), (uint8_t *)VIDEO + ((j + 1) * NUMBYTES_IN_LINE), NUMBYTES_IN_LINE);
		}
		//print the last line	
		for(j = 0; j < TERMINAL_WIDTH; j++)
			WriteCharacter(' ', BLACK, 0, j, terminal_y);
	}
	//if more bytes were supposed to be copied then there are bytes then this is bad
	//loops through the number of bytes were supposed to print
	for(i = 0; i < nbytes; i++)
	{
		//if current character is newline character then we want to change our cursor position
		if(*(char*)(buf + i) == '\n')
		{
			terminal_y++;
			terminal_x = 0;
			if(terminal_y == TERMINAL_HEIGHT + 1)
			{
				setX(terminal_x);
				setY(terminal_y);
				scroll();
				terminal_x = getX();
				terminal_y = getY();
			}
		}
		else // else we just print the character normally
		{
			WriteCharacter(*(char*)(buf + i), BLACK, 0, terminal_x, terminal_y);
			//update x and y
			terminal_x++;
			if(terminal_x == (TERMINAL_WIDTH) && terminal_y < (TERMINAL_HEIGHT+1))
			{
				terminal_y++;
				terminal_x = 0;
			}
			// check if have reached the final line
			
			if(terminal_y == (TERMINAL_HEIGHT + 1))
			{
				//if we are on the last line then want to scroll everything up so the whole file can be printed
				int j = 0;
				terminal_y = terminal_y - 1;;
					
				for(j = 0; j < terminal_y; j++)
				{
					if(j < TERMINAL_HEIGHT)
						memcpy(((uint8_t *)VIDEO + ((j)) * NUMBYTES_IN_LINE), (uint8_t *)VIDEO + ((j + 1) * NUMBYTES_IN_LINE), NUMBYTES_IN_LINE);
				}
				//print the last line	
				for(j = 0; j < TERMINAL_WIDTH; j++)
					WriteCharacter(' ', BLACK, 0, j, terminal_y);
			}
		}
		//update the cursor when a character is printed
		fb_move_cursor(terminal_x, terminal_y);
	}
	//reset the x and y values in keyboard.c
	fb_move_cursor(terminal_x, terminal_y);
	//setCharactersOnScreen(terminal_charsOnScreen);
	setX(terminal_x);
	setY(terminal_y);

	return 0;
}

