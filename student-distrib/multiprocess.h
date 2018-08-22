#ifndef _MULTIPROCESS_H
#define _MULTIPROCESS_H

#include "Paging.h"
#include "keyboard.h"

#define PAGE_ENTRY_INIT        0x25
#define VIDEO_MEMORY_OFFSET    0xBA000
#define VID_MEM                0xB8000
#define FOUR_KBS               0x1000
#define EIGHT_MB  					(1 << 23)
#define EIGHT_KB 					(1 << 13)
#define LAST_PAGE                      4
#define FOUR_MB  										(1 << 22)

typedef struct multiterminal{
  uint8_t saved_keyboard_buffer[128];
  uint16_t cur_buffer_Size;
  uint32_t * video_mem_hist;
  uint8_t execute_status;
  uint16_t pos_x;
  uint16_t pos_y;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esp0;
  int32_t PID;
  int32_t PPID;
} multiterminal_t;

multiterminal_t three_term_buf[3];

void multi_terminal_init(uint8_t terminal_number);

void switch_terminal(uint8_t cur_terminal, uint8_t next_terminal, uint8_t * buffer);

uint8_t get_nxt_term();

#endif
