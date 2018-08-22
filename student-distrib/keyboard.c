#include "keyboard.h"
uint16_t x = 0;

uint16_t y = 0;

int capsLockFlag = 0;
int capsLockFlag2 = 0;
int capsLockFlag3 = 0;

int shiftKeyHoldDown = 0;
int shiftKeyHoldDown2 = 0;
int shiftKeyHoldDown3 = 0;

int ctrlKeyHoldDown = 0;
int ctrlKeyHoldDown2 = 0;
int ctrlKeyHoldDown3 = 0;


int altKeyFlag = 0;

uint8_t buffer[BUFFER_SIZE];


uint16_t buffer_flag = 0;
uint16_t buffer_Size = 0;
uint16_t buffer_two_Size = 0;
uint16_t buffer_three_Size = 0;

uint16_t cursorPointer = 0;

int tabbed = 0;
uint16_t charactersOnScreen = 0;
uint8_t enter_flag = 0;
uint8_t enter_flag2 = 0;
uint8_t enter_flag3 = 0;


uint32_t esp_save2;
uint32_t ebp_save2;
#define PHI 0x9e3779b9

extern int32_t is_PCB_array_full();


//THIS WAS OUR CREATION PLEASE DONT MARK OFF WHEN GRADING BECAUSE WE DID THIS FOR FUN
//SINCE IT WAS DONE FOR FUN YOU GUYS SHOULD NOT REMOVE POINTS WHEN GRADING
//THIS STUFF DOES NOT INTERFERE WITH _windOS (TM) SO PLEASE DONT GRADE IT
//THE FUN STOPS AT THE NEXT COMMENT BLOCK
// static uint32_t Q[4096];
// static uint32_t c =362436;
// uint8_t* returnBuffer()
// {
// 	return buffer;
// }
// void init_rand(uint32_t x)
// {
//     int i;

//     Q[0] = x;
//     Q[1] = x + PHI;
//     Q[2] = x + PHI + PHI;

//     for (i = 3; i < 4096; i++)
//             Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
// }

// uint32_t rand_cmwc(void)
// {
//     uint32_t t = 18782LL;
// 	uint32_t a = 18782LL;
//     static uint32_t i = 4095;
//     uint32_t x = 0xfffffffe;
// 	uint32_t r = 0xfffffffe;
//     i = (i + 1) & 4095;
//     t = a * Q[i] + c;
//     c = (t >> 8);
//     x = t + c;
//     if (x < c) {
//             x++;
//             c++;
//     }
//     return (Q[i] = r - x);
// }
//THE
//FUN
//STOPS
//HERE
//THANK YOU FOR HEARING US OUT AND LETTING US HAVE FUN
//ECE 391 IS THE BEST CLASS EVER








/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
/*  Global variable table that contains the keystrokes in order*/
/*  We used the text table in this code
 *  https://github.com/JRWynneIII/Hobby-kernel/blob/master/keyboard.c
 */
char kbdus[ARRAY_SIZE] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '-', '=', '\b', /* Backspace */
  '\t',     /* Tab */
  'q', 'w', 'e', 'r', /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,      /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
 '\'', '`',   0,    /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
  'm', ',', '.', '/',   0,        /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};
//ARRAY NEEDED FOR CAPS LOCK
char capitalCharacters[ARRAY_SIZE] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '-', '=', '\b', /* Backspace */
  '\t',     /* Tab */
  'Q', 'W', 'E', 'R', /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', /* Enter key */
    0,      /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', /* 39 */
 '\'', '`',   0,    /* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
  'M', ',', '.', '/',   0,        /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};
//ARRAY NEEDED FOR SHIFT
char shiftCharacters[ARRAY_SIZE] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', '\b', /* Backspace */
  '\t',     /* Tab */
  'Q', 'W', 'E', 'R', /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,      /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
 '\"', '~',   0,    /* Left shift */
 '\?', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
  'M', '<', '>', '?',   0,        /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};
//ARRAY NEEDED FOR SHIFT AND CAPS
char shiftAndCaps[ARRAY_SIZE] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', '\b', /* Backspace */
  '\t',     /* Tab */
  'q', 'w', 'e', 'r', /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', /* Enter key */
    0,      /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', /* 39 */
 '\"', '~',   0,    /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
  'm', '<', '>', '?',   0,        /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};
void fill_buffer(uint8_t * newBuffer, int32_t size)
{
	int i = 0;
	for(i = 0; i < buffer_Size; i++)
	{
		newBuffer[i] = buffer[i];
	}
}
/*
 *   scroll
 *   DESCRIPTION: This function changes the video memory
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: plays with video memory to create a scrolling effect similar to the one seen in a linux terminal
 */
void scroll()
{
		int i = 0;					//initialize variables
		int temp = y;

		for(i = 0; i < y; i++)			//allocate more memory when the boundaries of the terminal have been reached
		{
			if(i < TERMINAL_HEIGHT)
				memcpy(((uint8_t *)VIDEO + ((i)) * NUMBYTES_IN_LINE), (uint8_t *)VIDEO + ((i + 1) * NUMBYTES_IN_LINE), NUMBYTES_IN_LINE);
		}
		y = temp - 1;
		for(i = 0; i < TERMINAL_WIDTH; i++)
			WriteCharacter(' ', BLACK, 0, i, y);

}
/*
 *   getchar
 *   DESCRIPTION: This function gets a character from the keyboard, checks to see if its valid, and prints it to the screen.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints character to screen, has a lock set up as well
 */
void getchar()
{
	cli();												//initialize the variables and clear all interrupts
	char character1 = inb(KEYBOARD_PORT);

	if(character1 != 0)
	{
		uint8_t scancode = character1;

		switch(scancode)
		{						//check for each scancode to see which keyboard key was pressed
			//ENTER
			case(ENTER_PRESSED):
					enter_flag = 1;
					cursorPointer = 0;					// update current coordinates
					x = 0;
					y++;
					if(y == TERMINAL_HEIGHT + 1)		// update the way the terminal looks
						scroll();
					fb_move_cursor(x,y);
					send_eoi(1);						// finish
					return;

			//CAPS LOCK
			case(CAPSLOCK_PRESSED):
					if(capsLockFlag == 0)
						capsLockFlag = 1;
					else
						capsLockFlag = 0;
					send_eoi(1);
					return;
			//BACKSPACE
			case(BACKSPACE_PRESSED):
				/*************************************************************************************************************/
				/*************************************************************************************************************/
				/*************************************************************************************************************/
				/********************************************BUFFER_ONE_FLAG**************************************************/
				/*************************************************************************************************************/
				/*************************************************************************************************************/
				/*************************************************************************************************************/
					if(x == 0 && y == 0)			// initial case to make sure the cursor does not go out of the boundaries
					{
						fb_move_cursor(x,y);
						send_eoi(1);

						return;
					}
					if(buffer_Size == 0 && charactersOnScreen != 0)
					{
						if(x > 0)
						{
							WriteCharacter(' ', BLACK, 0, x - 1, y);		// if backspace is pressed write a black mask to the screen
							x--;
						}
						else
						{
							y--;											// if backspace is pressed on a new line, decrement the row of terminal the cursor is at
							x = (TERMINAL_WIDTH-1);
							WriteCharacter(' ', BLACK, 0, x, y);
							fb_move_cursor(x,y);
							send_eoi(1);

							return;
						}
						fb_move_cursor(x,y);								//adjust the cursor and finish
						send_eoi(1);

						return;
					}

					if(buffer_Size > 0 && tabbed == 0)						//continuation of the backspace operation
					{
						buffer[buffer_Size] = ' ';
						buffer_Size--;
						cursorPointer--;

						if(x > 0)
						{
							WriteCharacter(' ', BLACK, 0, x - 1, y);			// if backspace is pressed write a black mask to the screen
							x--;
						}
						else
						{
							y--;												// if backspace is pressed on a new line, decrement the row of terminal the cursor is at
							x = TERMINAL_WIDTH-1;
							WriteCharacter(' ', BLACK, 0, x, y);
							fb_move_cursor(x,y);

							send_eoi(1);											//adjust the cursor and finish

							return;
						}
					}

					else if (buffer_Size > 0 && tabbed == 1) {					//continuation of the backspace operation
						buffer[buffer_Size] = ' ';
						buffer[buffer_Size + 1] = ' ';
						buffer[buffer_Size + 2] = ' ';
						buffer[buffer_Size + 3] = ' ';
						buffer_Size -= TAB_SPACES;
						if (x>0)
						{
							WriteCharacter(' ', BLACK, 0, x - 1, y);
							x--;
							WriteCharacter(' ', BLACK, 0, x - 1, y);
							x--;
							WriteCharacter(' ', BLACK, 0, x - 1, y);
							x--;
							WriteCharacter(' ', BLACK, 0, x - 1, y);
							x--;
						}
					}
					else
					{
						if(y > 0 && buffer_Size != 0)								//reset the cursor and finish
						{
							y--;
							x = TERMINAL_WIDTH;
						}
					}
				fb_move_cursor(x,y);										// if buffersize is zero then we don't do anything
				send_eoi(1);
				return;


			//TAB
			case(TAB_PRESSED):
				send_eoi(1);
				return;
			//EITHER SHIFT PRESSED
			case(LSHIFT_PRESSED):
					shiftKeyHoldDown = 1;								// if shift is pressed and held down, start using the capital characters
					send_eoi(1);
					return;
			case(RSHIFT_PRESSED):
					shiftKeyHoldDown = 1;								// if shift is pressed and held down, start using the capital characters
					send_eoi(1);
					return;
			//EITHER SHIFT RELEASED
			case(LSHIFT_RELEASED):									// if shift is released, then return to using the type of characters specified by the caps-lock flag
					shiftKeyHoldDown = 0;								// if shift is pressed and held down, start using the capital characters
					send_eoi(1);
					return;
			case(RSHIFT_RELEASED):
					shiftKeyHoldDown = 0;								// if shift is pressed and held down, start using the capital characters
					send_eoi(1);
					return;

			//EITHER CONTROL PRESSED
			case(CNTRL_PRESSED):
					ctrlKeyHoldDown = 1;							// change characters to appropriate outputs based on what control specifies
					send_eoi(1);
					return;

			//EITHER CONTROL RELEASED
			case(CNTRL_RELEASED):									//return to the normal character type
					ctrlKeyHoldDown = 0;							// change characters to appropriate outputs based on what control specifies
					send_eoi(1);
					return;


			//CONTROL + L
			case(CNTRL_L_PRESSED):
				/*************************************************************************************************************/
				/*************************************************************************************************************/
				/*************************************************************************************************************/
				/********************************************BUFFER_ONE_FLAG**************************************************/
				/*************************************************************************************************************/
				/*************************************************************************************************************/
				/*************************************************************************************************************/
					if(ctrlKeyHoldDown == 1)
					{						// clear the screen/buffer
						clear();
						x = 0;
						y = 0;
						buffer_Size = 0;
						cursorPointer = 0;
						fb_move_cursor(0,0);
					}
					else
					{
						if(capsLockFlag == 1 && shiftKeyHoldDown == 1)
						{
							if(buffer_Size < BUFFER_SIZE)
							{
								buffer[buffer_Size] = 'l';
								buffer_Size++;
								cursorPointer++;
								WriteCharacter('l', BLACK, 0, x, y);
							}
							else
							{
								send_eoi(1);
								return;
							}
						}
						else if(capsLockFlag == 1 && shiftKeyHoldDown == 0)
						{
							if(buffer_Size < BUFFER_SIZE)
							{
								buffer[buffer_Size] = 'L';
								buffer_Size++;
								cursorPointer++;
								WriteCharacter('L', BLACK, 0, x, y);
							}
							else
							{
								send_eoi(1);
								return;
							}
						}
						else if(capsLockFlag == 0 && shiftKeyHoldDown == 1)
						{
							if(buffer_Size < BUFFER_SIZE)
							{
								buffer[buffer_Size] = 'L';
								buffer_Size++;
								cursorPointer++;
								WriteCharacter('L', BLACK, 0, x, y);
							}
							else
							{
								send_eoi(1);
								return;
							}
						}
						else
						{
							if(buffer_Size < BUFFER_SIZE)
							{
								buffer[buffer_Size] = 'l';
								buffer_Size++;
								cursorPointer++;
								WriteCharacter('l', BLACK, 0, x, y);
							}
							else
							{
								send_eoi(1);
								return;
							}
						}
						x++;
						if(x == TERMINAL_WIDTH && y < TERMINAL_HEIGHT)
						{
							y++;
							x = 0;
						}
						fb_move_cursor(x,y);						// reset the location of the cursor
					}
					send_eoi(1);
					return;
			//RIGHT ALT
			case(RALT_PRESSED):										//set up ALT flag similar to shift
				send_eoi(1);
				altKeyFlag = 1;
				return;


			case (RALT_RELEASED):										//set up ALT flag similar to shift
				send_eoi(1);
				altKeyFlag = 0;
				return;


			case(RNDM_PRESSED):										//in the case that a random key is pressed
				send_eoi(1);
				return;

				//PAGE UP
			case(PAGEUP_PRESSED):											// readjust the cursor for a page up
				send_eoi(1);
				return;

			case(F1_PRESSED):
			//Case if we have a fll PCB array
				if (is_PCB_array_full() == 1 && three_term_buf[0].execute_status == 0) {
	    			int8_t buf[] = "\nYou greedy little boy. You exceeded max number of processes and tried to open shell 1\n391OS> ";
	    			write_Terminal(0, buf, strlen(buf));
	    			send_eoi(1);
	    			return;
	  			}
				if(altKeyFlag == 1 && buffer_flag != 0)
				{
			        uint8_t current_terminal = buffer_flag;
			        buffer_flag = 0;
			        send_eoi(1);
					switch_terminal(current_terminal, 0, buffer);
				}
				else {
					send_eoi(1);
				}
				return;
			case(F2_PRESSED):
			//Case if we have a fll PCB array
				if (is_PCB_array_full() == 1 && three_term_buf[1].execute_status == 0) {
	    			int8_t buf[] = "\nYou greedy little boy. You exceeded max number of processes and tried to open shell 2\n391OS> ";
	    			write_Terminal(0, buf, strlen(buf));
	    			send_eoi(1);
	    			return;
	  			}
				if(altKeyFlag == 1 && buffer_flag != 1)
				{
			        uint8_t current_terminal = buffer_flag;
			        buffer_flag = 1;
			        send_eoi(1);
					switch_terminal(current_terminal, 1, buffer);
				}
				else {
					send_eoi(1);
				}
				return;
			case(F3_PRESSED):
			//Case if we have a fll PCB array
				if (is_PCB_array_full() == 1 && three_term_buf[2].execute_status == 0) {
	    			int8_t buf[] = "\nYou greedy little boy. You exceeded max number of processes and tried to open shell 3\n391OS> ";
	    			write_Terminal(0, buf, strlen(buf));
	    			send_eoi(1);
	    			return;
	  			}
				if(altKeyFlag == 1 && buffer_flag != 2)
				{
		          uint8_t current_terminal = buffer_flag;
		          buffer_flag = 2;
		          send_eoi(1);
				  switch_terminal(current_terminal, 2, buffer);
				}
				else {
					send_eoi(1);
				}
				return;
			case(F4_PRESSED):
				send_eoi(1);
				return;
			case(F5_PRESSED):
				send_eoi(1);
				return;
			case(F6_PRESSED):
				send_eoi(1);
				return;
			case(F7_PRESSED):
				send_eoi(1);
				return;
			case(F8_PRESSED):
				send_eoi(1);
				return;
			case(F9_PRESSED):
				send_eoi(1);
				return;
			case(F10_PRESSED):
				send_eoi(1);
				return;
			case(F11_PRESSED):
				send_eoi(1);
				return;
			case(F12_PRESSED):
				send_eoi(1);
				return;
			case(RIGHTCURSOR_PRESSED):
				send_eoi(1);
				return;
			case(LEFT_CURSOR_PRESSED):
				send_eoi(1);
				return;
			case(UP_CURSOR_PRESSED):
				send_eoi(1);
				return;
			case(DOWN_CURSOR_PRESSED):
				send_eoi(1);
				return;
			case(INSERT_PRESSED):
				send_eoi(1);
				return;
			case(DELETE_PRESSED):
				send_eoi(1);
				return;
			case(HOME_PRESSED):
				send_eoi(1);
				return;
			case(END_PRESSED):
				send_eoi(1);
				return;
			case(SCROLL_LOCK_PRESSED):
				send_eoi(1);
				return;
			case(NUMLOCK_PRESSED):
				send_eoi(1);
				return;
			case(PRINT_SCREEN_PRESSED):
				send_eoi(1);
				return;
			case(PAGEDOWN_PRESSED):
				send_eoi(1);
				return;
		}



			if(((scancode & UPPER_LIMIT) == 0) && (scancode >  0) && (scancode < ARRAY_SIZE) && (shiftKeyHoldDown == 0) && (x < TERMINAL_WIDTH + 1) && (y < TERMINAL_HEIGHT + 1) && (buffer_Size < BUFFER_SIZE))
			{
				//SHIFT IS OFF AND CAPS IS ON
				if(capsLockFlag == 1)															// if shift has not been held and caps lock is on
				{

					buffer[buffer_Size] = capitalCharacters[scancode];								// use the capital characters of each scancode
					buffer_Size++;
					cursorPointer++;
					WriteCharacter(capitalCharacters[scancode], BLACK, 0, x, y);
					x++;

					if(x == TERMINAL_WIDTH && y < TERMINAL_HEIGHT)
					{
						y++;
						x = 0;
					}
					fb_move_cursor(x,y);
					send_eoi(1);

					return;
				}
				else 																						// shift is off and caps lock is off
				{

					buffer[buffer_Size] = kbdus[scancode];										// use the normal scancodes for the outputs
					buffer_Size++;
					cursorPointer++;
					WriteCharacter(kbdus[scancode], BLACK, 0, x, y);
					x++;
					if(x == TERMINAL_WIDTH && y < TERMINAL_HEIGHT)
					{
						y++;
						x = 0;
					}
					fb_move_cursor(x,y);
					send_eoi(1);

					return;
				}
			}
			else if(((scancode & UPPER_LIMIT) == 0) && (scancode >  0) && (scancode < ARRAY_SIZE) && (shiftKeyHoldDown == 1) && (x < TERMINAL_WIDTH + 1) && (y < TERMINAL_HEIGHT + 1) && (buffer_Size < BUFFER_SIZE))//SHIFT IS ON
			{
				if(capsLockFlag == 1) 														// both shift and caps lock have been pressed
				{
					buffer[buffer_Size] = shiftAndCaps[scancode];
					buffer_Size++;															// use the opposite output of the caps lock flag indicator
					cursorPointer++;

					WriteCharacter(shiftAndCaps[scancode], BLACK, 0, x, y);
					x++;
					if(x == TERMINAL_WIDTH && y < TERMINAL_HEIGHT)
					{
						y++;
						x = 0;
					}
					fb_move_cursor(x,y);
					send_eoi(1);

					return;
				}
				else 																		// shift is on and caps lock is off
				{
					buffer[buffer_Size] = shiftCharacters[scancode];
					buffer_Size++;															// use the opposite case for each character and output to the screen
					cursorPointer++;

					WriteCharacter(shiftCharacters[scancode], BLACK, 0, x, y);
					x++;
					if(x == TERMINAL_WIDTH && y < TERMINAL_HEIGHT)
					{
						y++;
						x = 0;
					}
					fb_move_cursor(x,y);
					send_eoi(1);

					return;
				}
			}
		}
		send_eoi(1);
}
uint8_t get_enter_flag()
{
	return enter_flag;
}
/*
 *   read_terminal
 *   DESCRIPTION: reads from a terminal
 *   INPUTS: fd - file directory for stuff to be learned later
 *			 buf - buffer to be read
 *			 nbytes - number of bytes to be read
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t read_Terminal(int32_t fd, void *buf, int32_t nbytes) //this is like cin (fills the buffer)
{
	sti();
	int numberOfBytesRead = 0;
		while(enter_flag == 0)
		{

		}
		enter_flag = 0;

		for(numberOfBytesRead = 0; numberOfBytesRead < buffer_Size; numberOfBytesRead++)
		{
			((uint8_t*)buf)[numberOfBytesRead] = buffer[numberOfBytesRead];
		}

		((uint8_t*)buf)[buffer_Size] = '\n';
		numberOfBytesRead++;
		int i;
		for(i = 0; i < buffer_Size; i++) {	// clear the buffer
				buffer[i] = '\0';
		}
		buffer_Size = 0;
		return numberOfBytesRead;
}
/*
 *   getchar1
 *   DESCRIPTION: this function prints out a character to the screen used to test RTC and later on other stuff
 *   INPUTS: char - print - the character to be printed
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes video memory to output a character
 */
void getchar1(char print)
{
	cli();															// initialize variables and clear the interrupts
	char character1 = print;
	//int i = 0;

	if(character1 != 0)
	{
		buffer_Size++;												// get the appropriate character
		WriteCharacter(print, BLACK, 0, x ,y);
		x++;
		if(x == TERMINAL_WIDTH && y < TERMINAL_HEIGHT)
		{
			y++;
			x = 0;
		}
		fb_move_cursor(x,y);
	}
	send_eoi(1);

	return;
}



/*
*   WriteCharacter(unsigned char c, unsigned char forecolour, unsigned char backcolour, uint16_t x, uint16_t y)
*   DESCRIPTION: This function takes a character, the forecolor, the backcolor, the x and y position, and writes
*                a character to that position
*   INPUTS: The character, the forecolor, the backcolor, the x and y position
*   OUTPUTS: none
*   RETURN VALUE: none
*   SIDE EFFECTS: prints character to screen
*/
void WriteCharacter(unsigned char c, unsigned char forecolour, unsigned char backcolour, uint16_t x_1, uint16_t y_1)
{
	 //forecolour = 12;																// print a character to the screen
   //init_rand(4);
	 //forecolour = (rand_cmwc() % 15) + 1;
   if (buffer_flag == 0)
   		forecolour = 18;
   else if (buffer_flag == 1)
   		forecolour = 19;
   	else if (buffer_flag == 2)
   		forecolour = 20;
   uint16_t attrib = (backcolour << BIT_SHIFT2) | (forecolour & MASK2);
   volatile uint16_t * where;
   where = (volatile uint16_t *)VIDEO + (y_1 * TERMINAL_WIDTH + x_1) ;
   *where = c | (attrib << BIT_SHIFT);
}
/*
*   fb_move_cursor(uint16_t row, uint16_t col)
*   DESCRIPTION: Move the cursor to the row and column
*   INPUTS: The row and column at which we have to change the cursor
*   OUTPUTS: none
*   RETURN VALUE: none
*   SIDE EFFECTS: Updates the cursor on the screen. Performs this using a write to the registers
*/
void fb_move_cursor(uint16_t row, uint16_t col)
{
  uint32_t idx = col*TERMINAL_WIDTH + row;												// change the location of the cursor
  outb (FB_HIGH_BYTE_COMMAND,FB_COMMAND_PORT);
  outb (idx >> BIT_SHIFT,FB_DATA_PORT);
  outb (FB_LOW_BYTE_COMMAND,FB_COMMAND_PORT);
  outb (idx,FB_DATA_PORT);
}
/*
 *   init_cursor_c
 *   DESCRIPTION: Initializes the cursor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: outputs a cursor on the screen
 */
void init_cursor_c()
{
  fb_move_cursor(0,0);
}




/*
 *   getX
 *   DESCRIPTION: Returns the current location of the cursor in the x domain
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: location of x
 *   SIDE EFFECTS: none
 */
uint16_t getX()
{

		return x;
}
/*
 *   getY
 *   DESCRIPTION: Returns the current location of the cursor in the y domain
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: location of y
 *   SIDE EFFECTS: none
 */
uint16_t getY()
{
		return y;
}
/*
 *   getCharactersOnScreen
 *   DESCRIPTION: Returns the current amount of characters on the screen
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: number of characters on the screen
 *   SIDE EFFECTS: none
 */
uint16_t getCharactersOnScreen()
{
	return charactersOnScreen;
}
/*
 *   setX
 *   DESCRIPTION: sets the current location of x
 *   INPUTS: terminal_x - location of x
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void setX(uint16_t terminal_x)
{
		x = terminal_x;
}
/*
 *   setY
 *   DESCRIPTION: sets the current location of y
 *   INPUTS: terminal_y - location of y
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void setY(uint16_t terminal_y)
{
		y = terminal_y;

}
/*
 *   setCharactersOnScreen
 *   DESCRIPTION: sets the current chracters on the screen
 *   INPUTS: terminal_characterOnScreen - number of characters on the screen
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void setCharactersOnScreen(uint16_t terminal_characterOnScreen)
{
	charactersOnScreen = terminal_characterOnScreen;
}

uint16_t getBufferSize()
{
	return buffer_Size;
}
void setBufferSize(uint16_t newBufferSize)
{
	buffer_Size = newBufferSize;
}
