#include "RTC.h"

int counter = 1;  //Not A magic number used to initalize our counter
char RTC_iflag = 0;

/*
 * RTC_init
 *   DESCRIPTION: Initialize the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Iniatilizes a real time clock in our operating system - this will allow us to keep track of the current time
 *		shoutout / citation to http://wiki.osdev.org/RTC, most of our initialization code was based off of this
 */
void
RTC_init(){
  char prev;
  outb(REGISTER_B, DISABLE_NMI); /// we are going to write to register b
  prev = inb(READ_WRITE_CMOS);
  outb(REGISTER_B, DISABLE_NMI); //select register c
  outb((PORT2 | prev), READ_WRITE_CMOS); // port two OR'd with prev
}

/*
 * RTC_open
 *   DESCRIPTION: Opens the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */
int32_t
RTC_Open(const uint8_t* filename){
	return 0;
}

/*
 * RTC_Read
 *   DESCRIPTION: Reads the RTC after an interrupt
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: resets the RTC interrupt flag
 */
int32_t
RTC_Read(int32_t fd, void* buf, int32_t nbytes){
  sti();
  RTC_iflag = 0;

  while(RTC_iflag == 0){}     //spins until the interrupt flag is set
  cli();
  return 0;

}


/*
 * RTC_handler
 *   DESCRIPTION: RTC handler that controls the RTC in Linux
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes all cleanups and frees alls structure use to
 *                 store cleanup function information
 */
void
RTC_handler(){

	outb(REGISTER_C , DISABLE_NMI); // select register C
	inb(READ_WRITE_CMOS);    	 // just throw away contents

	get_RTC_iflag(1);

	send_eoi(RTC_INTERRUPT);
}

/*
 * RTC_Write
 *   DESCRIPTION: Changes the frequency of the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Adjusts the speed of the OS' clock
 */
int32_t
RTC_Write (int32_t fd, const void* buf, int32_t nbytes){

  uint16_t frequency = DEFAULT_FREQ_FOR_RTC;

  RTC_set_frequency(frequency);

  return 0;
}

/*
 * RTC_Write
 *   DESCRIPTION: Sets the frequency for the RTC
 *   INPUTS: freq - frequency to set the RTC
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Adjusts the speed of the OS' clock
 */

int32_t
RTC_set_frequency(uint16_t freq){

  uint8_t rate = 0;

  uint16_t new_freq = freq;

  char prev;

  while (new_freq != 1){
    new_freq = new_freq >> 1;
    rate ++;
  }


  if(rate < LOWRANGE || rate > HIGHRANGE){  //make sure the rate is in a valid range
    rate = DEFAULT_RATE_FOR_RTC;  //if outside of bounds, reset to default rate
  }

  rate = rate & LOWER_FOUR_BIT_MASK;    //use the lower four bits of the rate only

  outb(REGISTER_A, DISABLE_NMI);        //disable NMI and set the index to register A
  prev = inb(READ_WRITE_CMOS);          //load in the value of register A
  outb(REGISTER_A, DISABLE_NMI);        //disable NMI and set the index to register A
  outb((prev & UPPER_FOUR_BIT_MASK) | rate, READ_WRITE_CMOS); //write the correct rate to register A

  return 0;
}

/*
 * RTC_Close
 *   DESCRIPTION: Closes the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: just marks the end of RTC usage
 */
int32_t
RTC_Close (int32_t fd){
  return 0;          //close the RTC
}

/*
 * get_RTC_iflag
 *   DESCRIPTION: Sets the RTC interrupt flag
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sets the RTC interrupt flag
 */
int
get_RTC_iflag(int val){
  return RTC_iflag = val;         //set the RTC flag
}
