#include "multiprocess.h"

uint8_t nxt_term;
uint8_t count;
extern int32_t sys_call_execute(const uint8_t* command);
extern uint32_t get_esp(uint8_t term);
extern uint32_t get_ebp(uint8_t term);
extern uint32_t get_esp_fuck(uint8_t process_ID);
extern uint32_t get_ebp_bitch(uint8_t process_ID);
extern int32_t get_current_process_ID(uint8_t term);
extern void set_current_process_ID(int32_t new_PID);
extern int32_t get_current_process_ID_without_term();
extern void set_parent_process_ID(int32_t new_PID);
extern int32_t get_parent_process_ID();
extern int32_t is_PCB_array_full();
extern void flush_tlb();

/*
 *   multi_terminal_init
 *   DESCRIPTION: Initializes a multi-terminal structure
 *   INPUTS: terminal number
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes the paging directory and table for the terminals
 */
void multi_terminal_init(uint8_t terminal_number){
  /*******STEP 1: CREATE A NEW PAGE FOR EACH TERMINAL*****/

  // uint32_t page_dir_entry = 0x25;
  uint32_t page_table_entry_ptr;
  uint32_t phys_addr = VIDEO_MEMORY_OFFSET + (terminal_number * FOUR_KBS);

  page_table_terminals[phys_addr >> 12] = phys_addr | USER_BIT | READ_WRITE_BIT | PRESENT_BIT;

  page_table_entry_ptr = (uint32_t) page_table_terminals;

  page_directory[PAGE_ENTRY_INIT] = (uint32_t) page_table_entry_ptr | USER_BIT | READ_WRITE_BIT | PRESENT_BIT;

  uint32_t * page_loc = (uint32_t * )((PAGE_ENTRY_INIT << 22) | (uint32_t)phys_addr);

  //flush the TLB
	flush_tlb();

  three_term_buf[terminal_number].video_mem_hist = (uint32_t *)page_loc;
  three_term_buf[terminal_number].cur_buffer_Size = 0;
  three_term_buf[terminal_number].execute_status = 0;
  three_term_buf[terminal_number].pos_x = 0;
  three_term_buf[terminal_number].pos_y = 0;
  // three_term_buf[terminal_number].PID = terminal_number;

  if(terminal_number == 0){
    uint32_t esp_save;
    uint32_t ebp_save; 
    asm volatile("\t movl %%esp,%0" : "=r"(esp_save));
    asm volatile("\t movl %%ebp,%0" : "=r"(ebp_save));
    three_term_buf[terminal_number].esp = esp_save;
    three_term_buf[terminal_number].ebp = ebp_save;
    // three_term_buf[terminal_number].video_mem_hist = (uint32_t *)page_loc;
    // three_term_buf[terminal_number].cur_buffer_Size = 0;
    //three_term_buf[terminal_number].execute_status = 1;
    // three_term_buf[terminal_number].pos_x = 0;
    // three_term_buf[terminal_number].pos_y = 0;
    three_term_buf[terminal_number].PID = terminal_number;
    uint8_t execute_command[6] = "shell\0";
    int32_t check = sys_call_execute (execute_command);
    while(check == 0)
    {
      clear();
      setX(0);
      setY(0);
      check = sys_call_execute (execute_command);
    }
  }

}

  uint32_t esp_save;
  uint32_t ebp_save;
  uint8_t execute_command[6] = "shell\0";

  int i;
  uint32_t ESP_old;
  uint32_t EBP_old;


/*
 *   switch_terminal
 *   DESCRIPTION: Switches from cur_terminal to next_terminal, also takes in the buffer
 *   INPUTS: Current temrinal, next terminal, buffer
 *   OUTPUTS: Switches the screens from one terminal to another
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes the paging directory and table for the terminals
 */
void switch_terminal(uint8_t cur_terminal, uint8_t next_terminal, uint8_t * buffer){
  /************STEP 1: SAVE EVERYTHING ***************/

  nxt_term = next_terminal;
  three_term_buf[cur_terminal].pos_x = getX();
  three_term_buf[cur_terminal].pos_y = getY();
  three_term_buf[cur_terminal].PID = get_current_process_ID_without_term();
  three_term_buf[cur_terminal].PPID = get_parent_process_ID();
  /************STEP 2: CHANGE THE EFFECTIVE BUFFER *********/
  setX(three_term_buf[next_terminal].pos_x);
  setY(three_term_buf[next_terminal].pos_y);
  fill_buffer(three_term_buf[cur_terminal].saved_keyboard_buffer, 128);
  three_term_buf[cur_terminal].cur_buffer_Size = getBufferSize();

  /************STEP 3: START ACCESS OF VIDEO MEMORY AGAIN *********/

  memcpy(three_term_buf[cur_terminal].video_mem_hist, (uint32_t *)VID_MEM, FOUR_KBS);

  clear();

  memcpy((uint32_t *)VID_MEM, three_term_buf[next_terminal].video_mem_hist, FOUR_KBS);

  fb_move_cursor(three_term_buf[next_terminal].pos_x,three_term_buf[next_terminal].pos_y);

  for(i = 0; i < 128; i++)
  {
      buffer[i] = three_term_buf[next_terminal].saved_keyboard_buffer[i];
  }

   setBufferSize(three_term_buf[next_terminal].cur_buffer_Size);
    asm volatile("\t movl %%esp,%0" : "=r"(esp_save));
    asm volatile("\t movl %%ebp,%0" : "=r"(ebp_save));
    three_term_buf[cur_terminal].esp = esp_save;
    three_term_buf[cur_terminal].ebp = ebp_save;
   if(three_term_buf[next_terminal].execute_status == 0){

     uint8_t check = sys_call_execute (execute_command);

     while(check == 0)
    {
      clear();
      setX(0);
      setY(0);
      check = sys_call_execute (execute_command);
    }
   }

   set_parent_process_ID(three_term_buf[next_terminal].PPID);
   set_current_process_ID(three_term_buf[next_terminal].PID);
   tss.ss0 = KERNEL_DS;
   //int ret = get_current_process_ID(next_terminal);
   tss.esp0 = EIGHT_MB - EIGHT_KB - (EIGHT_KB * get_current_process_ID(next_terminal))  - LAST_PAGE;
   //tss.esp0 =  three_term_buf[next_terminal].esp0;
   ESP_old = get_esp(next_terminal);

   EBP_old = get_ebp(next_terminal);

    page_directory[32] = (EIGHT_MB  + (get_current_process_ID(next_terminal) * FOUR_MB)) | SIZE_BIT | USER_BIT | READ_WRITE_BIT | PRESENT_BIT;
  
    flush_tlb();


  CHANGE_EBP_AND_ESP(three_term_buf[next_terminal].ebp, three_term_buf[next_terminal].esp);

  asm volatile(
    "leave \n"
    "ret   \n"
    : :);


}

/*
 *   get_nxt_term
 *   DESCRIPTION: Return next terminal
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
uint8_t get_nxt_term(){
  return nxt_term;
}

/*
 *   scheduler
 *   DESCRIPTION: Scheduling function
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Modifies RTC and stack and paging structures
 */
void scheduler(){

  /*********SAVE ESP, EBP FOR WR.PROCESS*************/



  /*******************PAGING 128 MB******************/


  /*********************TSS.ESP0********************/

  /*******REPLACE ESP, EBP, FOR NEW PROCESS*********/

  
 /* count = count % 6;
  count++;*/
  
  // if(PCB_array[cur_process]->isActive == 0){
  //   return;
  // }

 /* while  (PCB_array[count]->isActive != 1 ) {
    count++;
  } */
  
  // uint32_t esp_save;
  // uint32_t ebp_save;
  //   asm("\t movl %%esp,%0" : "=r"(esp_save));
  //   asm("\t movl %%ebp,%0" : "=r"(ebp_save));
  //
  // three_term_buf[terminal_number].esp = esp_save;
  // three_term_buf[terminal_number].ebp = ebp_save;
  
 /* uint32_t ESP_old = get_esp_fuck(count);
  uint32_t EBP_old = get_ebp_bitch(count);
  asm("mov %%ebx,%%esp": : "b" (ESP_old));		//reset ESP
  asm("mov %%ebx,%%ebp": : "b" (EBP_old));		//reset EBP*/

}
