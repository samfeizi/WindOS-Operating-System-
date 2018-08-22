
#ifndef _SYSCALLS_H
#define _SYSCALLS_H
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "keyboard.h"
#include "RTC.h"
#include "terminal.h"
#include "file_sys.h"
#include "Paging.h"
#include "multiprocess.h"


// 	MAGIC NUMBERS


#define RTC			 									0
#define DIRECTORY	 									1
#define SINGLE_FILE		 								2
#define EIGHT_MB  										(1 << 23)
#define FOUR_MB  										(1 << 22)
#define VIRTUAL_ADDRESS_PROGRAM_SPACE 					0x08048000
#define SIXTEEN_KB 										(1 << 14)
#define EIGHT_KB 										(1 << 13)
#define FOUR_KB                       (1 << 12)
#define ONE_FIFTY_MEGABYTES 					0x9000000
#define ONE_TWENTY_EIGHT_MIBIBYTES		0x8000000
#define ONE_TWENTY_EIGHT_MEGABYTES    0x7A12000
#define ONE_THIRTY_TWO_MEBIBYTES     0x8400000
#define SIZE_OF_PCB_ARRAY             6
#define SIZE_OF_FILE_ARRAY            8
#define FILE_ARRAY_START              2
#define PAGE_DIRECTORY_ENTRY          32
#define MAX_SIZE_OF_WORD              128
#define FIRST_40_BYTES                40
#define SIZE_OF_STRING                14
#define FIRST_FILE_BYTE               0x7f
#define SECOND_FILE_BYTE               0x45
#define THIRD_FILE_BYTE               0x4c
#define FOURTH_FILE_BYTE               0x46
#define BYTE_27                       27
#define BYTE_26                         26
#define BYTE_25                       25
#define BYTE_24                         24
#define LAST_PAGE                      4
#define LAST_FILE_ARRAY_INDEX           7
#define MASK_FOR_BOTTOM_4_BITS            0xF
#define BIT_SHIFT               8
#define VIDEO_MEMORY                   0xb8000
#define FOUR_KB_LOCATION                0x000
#define SIZE_OF_BUF                     40
#define THIRD_WORD						0x0000
/*
	We have a table that keeps track of pointers (PCB array), process id is an index into this array to give us the current process
	PCB array has pointers to PCB structures
	When we call execute("shell"), we're gonna wanna
		-create a page for that process at a specific address
		-create a PCB for that process at the specific address
		-make the PCB_array[process_id] point to that PCB
*/


typedef struct {
    int32_t (*read)(int32_t, void*, int32_t);
	int32_t (*write)(int32_t, const void*, int32_t);
    int32_t (*open)(const uint8_t*);
    int32_t (*close)(int32_t);
} device ;

typedef struct {
	device* file_op_table_ptr;
	int32_t inode;
	int32_t file_position;
	int32_t flags;

} File_array_block_t;

typedef struct PCB {
	File_array_block_t file_array[8];
	int32_t process_id;
	uint8_t flag;
	struct PCB* parent_PCB;
	uint32_t esp;
	uint32_t ebp;
	uint32_t parent_ESP0;
	uint8_t command_global[6][40];
	uint8_t isActive;
	uint8_t terminalNumber;
} PCB;

//PCB array that holds information about the processes. All of this is initalized to zero when we call execute for the first time
PCB* PCB_array[6];

PCB * get_PCB_array();

// system call HALT prototype
extern int32_t sys_call_halt(uint8_t status);

// system call EXECUTE prototype
extern int32_t sys_call_execute(const uint8_t* command);

// system call READ prototype
extern int32_t sys_call_read(int32_t fd, void* buf, int32_t nbytes);

// system call WRITE prototype
extern int32_t sys_call_write(int32_t fd, const void* buf, int32_t nbytes);

// system call OPEN prototype
extern int32_t sys_call_open(const uint8_t* filename);

// system call CLOSE prototype
extern int32_t sys_call_close(int32_t fd);

// system call GETARGS prototype
extern int32_t sys_call_getargs(uint8_t* buf, int32_t nbytes);

// system call VIDMAP prototype
extern int32_t sys_call_vidmap(uint8_t** screen_start);

// system call SET_HANDLER prototype
extern int32_t sys_call_set_handler(int32_t signum, void* handler_address);

// system call SIGRETURN prototype
extern int32_t sys_call_sigreturn(void);

// initialize PCB array prototype
void init_PCB_array(void);

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

#endif
