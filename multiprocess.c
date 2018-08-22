#include "multiprocess.h"


void multi_terminal_init(uint8_t terminal_number){
  /*******STEP 1: CREATE A NEW PAGE FOR EACH TERMINAL*****/

  uint32_t page_dir_entry = PAGE_ENTRY_INIT;
  uint32_t page_table_entry_ptr;
  uint32_t phys_addr = VIDEO_MEMORY_OFFSET + (terminal_number * FOUR_KBS);

  page_table_terminals[terminal_number] = phys_addr | USER_BIT | READ_WRITE_BIT | PRESENT_BIT;

  page_table_entry_ptr = (uint32_t)page_table_terminals;

  page_directory[page_dir_entry] = (uint32_t)page_table_entry_ptr | USER_BIT | READ_WRITE_BIT | PRESENT_BIT;

  uint32_t * page_loc = (uint32_t * )((page_dir_entry << 22) | (uint32_t)phys_addr);

  //flush the TLB
	asm volatile(
		"movl %%cr3, %%eax \n"
		"movl %%eax, %%cr3"
		: :);

  /********STEP 2: INITIALIZE TERMINAL*********/
  three_term_buf[terminal_number].video_mem_hist = (uint32_t *)page_loc;
  three_term_buf[terminal_number].execute_status = 1;
  three_term_buf[terminal_number].pos_x = 0;
  three_term_buf[terminal_number].pos_y = 0;
  three_term_buf[terminal_number].PID = terminal_number;

  /********STEP 3: BEGIN THE SHELL***********/

  // NEEDS TO BE DONE, CHECK THE KERNEL.C EXECUTION OF THE SHELL
}


void switch_terminal(uint8_t cur_terminal, uint8_t next_terminal){
  /************STEP 1: SAVE EVERYTHING ***************/
  three_term_buf[cur_terminal].pos_x = getX();
  three_term_buf[cur_terminal].pos_y = getY();
  // three_term_buf[cur_terminal].video_mem_hist = (uint32_t *)VIDEO_MEMORY_OFFSET + (FOUR_KBS * cur_terminal);
  /************STEP 2: CHANGE THE EFFECTIVE BUFFER *********/
  setX(three_term_buf[next_terminal].pos_y);
  setY(three_term_buf[next_terminal].pos_x);
  fill_buffer(three_term_buf[next_terminal].saved_keyboard_buffer, 128);

  /************STEP 3: START ACCESS OF VIDEO MEMORY AGAIN *********/
  memcpy(three_term_buf[cur_terminal].video_mem_hist, (uint32_t *)VID_MEM, FOUR_KBS);
  memcpy((uint32_t *)VID_MEM, three_term_buf[next_terminal].video_mem_hist, FOUR_KBS);
}
