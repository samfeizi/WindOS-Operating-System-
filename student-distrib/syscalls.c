#include "syscalls.h"
uint8_t parent_process_ID = 0;
uint8_t current_process_ID = 0;
device RTC_DEV;
device DIR_DEV;
device FILE_DEV;
device STDIN;
device STDOUT;



/*
 * init_PCB_array
 *   DESCRIPTION: Initializes the process control block array
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes PCB array
 */
void init_PCB_array(void) {
	int i = 0;
	for (i = 0; i < SIZE_OF_PCB_ARRAY; ++i) {
		PCB_array[i] = (PCB* )(EIGHT_MB - EIGHT_KB - (i * EIGHT_KB)) ;  //initialize space for 6 processes
		PCB_array[i]->flag = 0;
		PCB_array[i]->isActive = 0;
		PCB_array[i]->terminalNumber = -1;
	}
}

/*
 * sys_call_halt
 *   DESCRIPTION: Implements the HALT system call
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 or the value of status as a parameter for SYSTEM_CALL_ECE assembly
 *   SIDE EFFECTS: none
 */
int32_t sys_call_halt(uint8_t status)
{
	//close any FDs
	int i;
	int32_t ESP_old = PCB_array[parent_process_ID]->esp;
	int32_t EBP_old = PCB_array[parent_process_ID]->ebp;

	// tss.ss0 = KERNEL_DS;
	tss.esp0 = PCB_array[current_process_ID]->parent_ESP0;

	for (i = 0; i < SIZE_OF_FILE_ARRAY; i++) {
		PCB_array[current_process_ID] -> file_array[i].file_op_table_ptr = 0;
		PCB_array[current_process_ID] -> file_array[i].inode = -1;
		PCB_array[current_process_ID] -> file_array[i].file_position = -1;
		PCB_array[current_process_ID] -> file_array[i].flags = 0;
	}

	//restore parent data
	//CASE FOR current_process_ID = 0
	if (PCB_array[current_process_ID] -> parent_PCB == PCB_array[current_process_ID]) {
		PCB_array[current_process_ID] -> flag = 0;
	}

	else {
		PCB_array[current_process_ID]->parent_PCB->isActive = 1;
		PCB_array[current_process_ID]->isActive = 0;
		PCB_array[current_process_ID]->flag = 0;
		PCB_array[current_process_ID] -> terminalNumber = -1;
		//LOST POINT CUZ OF THIS LINE BELOW FUCK THIS BULLSHIT ASS SHIT
		parent_process_ID = PCB_array[current_process_ID]->parent_PCB->process_id;
		//^this line lmao fuck this shit
		PCB* curr = PCB_array[current_process_ID] -> parent_PCB;
		PCB_array[current_process_ID]->parent_PCB = 0;
		PCB_array[current_process_ID]->esp = 0;
		PCB_array[current_process_ID]->ebp = 0;
		current_process_ID = curr-> process_id;

	}

	//restore paging data

	page_directory[PAGE_DIRECTORY_ENTRY] = (EIGHT_MB  + ( current_process_ID * FOUR_MB)) | SIZE_BIT | USER_BIT | READ_WRITE_BIT | PRESENT_BIT;
	// flush_tlb();
		asm volatile(
			"movl %%cr3, %%eax \n"
			"movl %%eax, %%cr3"
			: :);
	volatile uint32_t tempstatus = status;
	//jump to execute return
	asm("mov %%ebx, %%eax" : :"b"(tempstatus));
	asm("mov %%ebx,%%esp": : "b" (ESP_old));		//reset ESP
	asm("mov %%ebx,%%ebp": : "b" (EBP_old));		//reset EBP
	asm("leave 		     \n"
		"ret" : : );
	return status;
}

/*
 * sys_call_execute
 *   DESCRIPTION: Implements the EXECUTE system call
 *   INPUTS: command - two words, system call and argument respectively
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */
int32_t sys_call_execute(const uint8_t* command)
{
	/*
	PART 1: PARSE ARGUMENTS - convert command into 2 different words
	*/

	uint8_t word1[MAX_SIZE_OF_WORD];
	uint8_t word2[MAX_SIZE_OF_WORD];
	uint32_t i = 0,j = 0;
	uint32_t len = strlen((int8_t *)command);
	int idx = 0;
	uint8_t buf[FIRST_40_BYTES];


	for (i = 0; i < len; ++i) {
		if (command[i] == ' ') {
			idx = i;
		}
	}

	//Case if we have just one word passed in
	if (idx == 0) {
		idx = len;
	}

	//First word in word1
	for (i = 0; i < idx; ++i) {
		word1[i] = command[i];
	}
	word1[idx] = '\0';


	//Second word in word2. Do this if and only if we have more than one word
	if (idx == len-1) {
		for (j = idx + 1; j < len; ++j) {
			word2[j - idx - 1] = command[j];
		}
		word2[j - idx] = '\0';
	}

	/*
	PART 2: CHECK FILE VALIDITY - fill a data entry based on the filename, then call read data on that data entry, and find the buffer
	*/
	//Fill data entry based on the filename
	dentry_t dentry;
	if (read_dentry_by_name(word1, &dentry) == -1) {
		return -1;
	}

	//Fill buffer with data from data entry
	if (read_data(dentry.inode_num, 0, buf, FIRST_40_BYTES) == -1) {
		uint8_t invReadData[SIZE_OF_STRING] = "Read data died";
		write_Terminal(0, invReadData, SIZE_OF_STRING);
		return -1;
	}

	//Check the first 4 bytes of the buffer - MAGIC NUMBers
	if (buf[0] != FIRST_FILE_BYTE || buf[1] != SECOND_FILE_BYTE || buf[2] != THIRD_FILE_BYTE || buf[3] != FOURTH_FILE_BYTE) {
		return -1;
	}


	/*
	PART 3: CREATE PCB/ OPEN FDs
	This part of the code searches for an open PCB. If there is no open PCB, we return -1.
	The index we get from searching for an open PCB (in the for loop below), is used to index into the PCB array.
	Once we have that index, we initialize file array and other member variables of that PCB.
	The first entry of the file array is STDIN, and the second entry of the file array is STDOUT
	We initialize the stuff of that PCB (inodes, flags etc.) to not being read and invalid because we havent seen a file yet
	*/
	//Flag to indicate if there are any open file
	uint8_t available_process = 0;
	for (i = 0; i < SIZE_OF_PCB_ARRAY; ++i) {
		if (PCB_array[i]->flag == 0) {
			PCB_array[i] -> process_id = i;
			available_process = 1;
			PCB_array[i]->flag = 1;
			current_process_ID = i;
			break;
		}
	}
	//Reset the global command 40 is not magic used once
	for (i = 0; i < 40; ++i) {
		PCB_array[current_process_ID] -> command_global[current_process_ID][i] = 0;
	}
	//Set the name of the second argument
	for (i = 0; i < len; ++i) {
		PCB_array[current_process_ID] -> command_global[current_process_ID][i] = command[i];
	}

	//If there are no open files then we just quit
	if (available_process == 0) {
		int8_t buf[] = "You greedy little boy. You exceeded max number of processes\n";
		write_Terminal(0, buf, strlen(buf));
		return 0;
	}

	//initialize STDIN to in use - ASSIGN IN OPEN

	STDIN.read =  &read_Terminal;
	STDIN.open = &open_Terminal;
	STDIN.write = 0;
	STDIN.close = &close_Terminal;
	PCB_array[current_process_ID] -> file_array[0].file_op_table_ptr = &STDIN;
	PCB_array[current_process_ID] -> file_array[0].inode = -1;
	PCB_array[current_process_ID] -> file_array[0].file_position = 0;
	PCB_array[current_process_ID] -> file_array[0].flags = 1;

	//initialize STDOUT to in use

	STDOUT.write = &write_Terminal;
	STDOUT.read =  0;
	STDOUT.open = &open_Terminal;
	STDOUT.close = &close_Terminal;

	PCB_array[current_process_ID] -> file_array[1].file_op_table_ptr = &STDOUT;
	PCB_array[current_process_ID] -> file_array[1].inode = -1;
	PCB_array[current_process_ID] -> file_array[1].file_position = 0;
	PCB_array[current_process_ID] -> file_array[1].flags = 1;

	//Loop through the rest of the file array and initialize those file array values
	for (j = FILE_ARRAY_START; j< SIZE_OF_FILE_ARRAY; j++) {
		PCB_array[current_process_ID] -> file_array[j].file_op_table_ptr = 0;
		PCB_array[current_process_ID] -> file_array[j].inode = -1;
		PCB_array[current_process_ID] -> file_array[j].file_position = 0;
		PCB_array[current_process_ID] -> file_array[j].flags = 0;
	}


	// Accessing the file array within the PCB_array using the current_process_ID
	// Start with the first
	if (current_process_ID != 0) {
		PCB_array[current_process_ID] -> parent_PCB = PCB_array[parent_process_ID];
		parent_process_ID = current_process_ID;
	}
	else {
		PCB_array[current_process_ID] -> parent_PCB = PCB_array[current_process_ID];
	}

	/*
	PART 4: PAGING
	We need to load the program into virtual addresses 128 - 132 MB. Each page directory corresponds to 4 MB of the memory space.
	The page directory entry values are specified here: http://wiki.osdev.org/Paging
	We want to enable the P, R, U and S bits
	*/
	page_directory[PAGE_DIRECTORY_ENTRY] = (EIGHT_MB  + ( current_process_ID * FOUR_MB)) | SIZE_BIT | USER_BIT | READ_WRITE_BIT | PRESENT_BIT;
	// flush_tlb();
	asm volatile(
		"movl %%cr3, %%eax \n"
		"movl %%eax, %%cr3"
		: :);
	/*
	PART 5: LOAD FILE INTO MEMORY
	*/
	unsigned long size_of_file = ((inode_t*)(bootmebb + 1 + dentry.inode_num))->b_length;
	if (read_data(dentry.inode_num, 0, (uint8_t*) VIRTUAL_ADDRESS_PROGRAM_SPACE, size_of_file) == -1) {
		return -1;
	}
	uint8_t nxt_term = get_nxt_term();
	if(three_term_buf[nxt_term].execute_status == 0){
		PCB_array[current_process_ID] -> parent_PCB = PCB_array[current_process_ID];
		three_term_buf[nxt_term].execute_status = 1;
		PCB_array[current_process_ID] -> isActive = 1;
		PCB_array[current_process_ID] -> terminalNumber = nxt_term;

	}
	if (PCB_array[current_process_ID] -> parent_PCB != PCB_array[current_process_ID]) 
	{
		PCB_array[current_process_ID] -> parent_PCB -> isActive = 0;
		PCB_array[current_process_ID] -> isActive = 1;
		PCB_array[current_process_ID] -> terminalNumber = PCB_array[current_process_ID] -> parent_PCB -> terminalNumber;
	}

	uint32_t word3 = THIRD_WORD;
	uint8_t word2_ = MASK_FOR_BOTTOM_4_BITS;
	uint8_t currentChar = buf[BYTE_27];
	word2_ = word2_ & currentChar;
	word3 = word3 | buf[BYTE_27];
	word3 = word3 << BIT_SHIFT;
	word3 = word3 | buf[BYTE_26];
	word3 = word3 << BIT_SHIFT;
	word3 = word3 | buf[BYTE_25];
	word3 = word3 << BIT_SHIFT;
	word3 = word3 | buf[BYTE_24];

	// tss.ss0 = KERNEL_DS;
	PCB_array[current_process_ID] -> parent_ESP0 = tss.esp0;
	tss.esp0 = EIGHT_MB - EIGHT_KB - (EIGHT_KB * current_process_ID)  - LAST_PAGE; //subtract 8kb for each process id
	uint32_t fuck;
    asm("\t movl %%esp,%0" : "=r"(fuck));
    uint32_t bitch;
    asm("\t movl %%ebp,%0" : "=r"(bitch));
    PCB_array[current_process_ID] -> esp = fuck;
    PCB_array[current_process_ID] -> ebp = bitch;

	// IRET
	switch_to_user_mode(word3, ONE_TWENTY_EIGHT_MIBIBYTES + FOUR_MB - LAST_PAGE); // this value is gonna change depending on which process we are on

	return 0;
}


/*
 * sys_call_read
 *   DESCRIPTION: Implements the READ system call
 *   INPUTS: fd - the index in the file array pointer
 			 buf - the buffer we read the data into?
 			 nbytes - the number of bytes we want to read from the buffer
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes read if we're within limits, 0 if we're out of limits
 *   SIDE EFFECTS: none
 */
int32_t sys_call_read(int32_t fd, void* buf, int32_t nbytes)
{

	if ( (fd == 1) || (fd>LAST_FILE_ARRAY_INDEX) || (fd < 0) || (PCB_array[current_process_ID] -> file_array[fd].flags == 0)) return -1;

	return PCB_array[current_process_ID] -> file_array[fd].file_op_table_ptr->read(fd,buf,nbytes);
}

/*
 * sys_call_write
 *   DESCRIPTION: Implements the WRITE system call
 *   INPUTS: file descriptor, buffer, number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */
int32_t sys_call_write(int32_t fd, const void* buf, int32_t nbytes)
{
	
	if ( (fd == 0) || (fd>LAST_FILE_ARRAY_INDEX) || (fd < 0) || (PCB_array[current_process_ID] -> file_array[fd].flags == 0)) return -1;
	{
		return PCB_array[current_process_ID] -> file_array[fd].file_op_table_ptr->write(fd,buf,nbytes);;
	}
}

/*
 * sys_call_open
 *   DESCRIPTION: Implements the OPEN system call
 *   INPUTS: filename
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */
int32_t sys_call_open(const uint8_t* filename)
{
	if(strlen((int8_t*)filename) == 0)
		return -1;
	int i;
	int8_t arrayFlag = 0;

	dentry_t  currfile;
	//filename = "cat";
	//if the pcb is not full then we want to check if the file exists in the file system
	arrayFlag = read_dentry_by_name(filename, &currfile);
	//arrayFlag will be -1 if the filename is not inside the current data entry table
	if(arrayFlag == -1){
		return -1;
	}

	unsigned long type_of_file = currfile.file_type;
	arrayFlag = 0;
	//first check if there is an open spot in the PCB (in the file array)
	for(i = FILE_ARRAY_START; i < SIZE_OF_FILE_ARRAY; i++)
	{
		if(PCB_array[current_process_ID]->file_array[i].flags == 0)
		{
			arrayFlag = 1;
			PCB_array[current_process_ID]->file_array[i].inode = currfile.inode_num;
			PCB_array[current_process_ID]->file_array[i].file_position = 0;
			PCB_array[current_process_ID]->file_array[i].flags = 1; //mark as in use
			break;
		}
	}

	if(arrayFlag == 0){ //PCB is full so we return -1
		return -1;
	}

	//if we want to open rtc then we just call rtc open
	if (type_of_file == RTC){

		RTC_DEV.write = &RTC_Write;
		RTC_DEV.read =  &RTC_Read;
		RTC_DEV.open = &RTC_Open;
		RTC_DEV.close = &RTC_Close;
		PCB_array[current_process_ID] -> file_array[i].file_op_table_ptr = &RTC_DEV;
		return i;
	}
	else if (type_of_file == DIRECTORY){

		DIR_DEV.write = &write_directory;
		DIR_DEV.read =  &read_directory;
		DIR_DEV.open = &open_directory;
		DIR_DEV.close = &close_directory;
		PCB_array[current_process_ID] -> file_array[i].file_op_table_ptr = &DIR_DEV;
		return i;
	}
	else if (type_of_file == SINGLE_FILE){

		FILE_DEV.write = &file_write;
		FILE_DEV.read =  &file_read;
		FILE_DEV.open = &file_open;
		FILE_DEV.close = &file_close;
		PCB_array[current_process_ID] -> file_array[i].file_op_table_ptr = &FILE_DEV;
		return i;
	}
	return -1;
}

/*
 * sys_call_close
 *   DESCRIPTION: Opens the RTC
 *   INPUTS: file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */
int32_t sys_call_close(int32_t fd)
{
	if((fd < FILE_ARRAY_START) || (fd > LAST_FILE_ARRAY_INDEX))
		return -1;
	if(PCB_array[current_process_ID] -> file_array[fd].flags == 1)
	{
		PCB_array[current_process_ID] -> file_array[fd].flags= 0;
		PCB_array[current_process_ID] -> file_array[fd].file_op_table_ptr->close(fd);
		return 0;
	}
	else
		return -1;

}

/*
 * sys_call_getargs
 *   DESCRIPTION: Implements system call for getargs
 *   INPUTS: buf, number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */
int32_t sys_call_getargs(uint8_t* buf, int32_t nbytes)
{
	if (nbytes <= 0 || buf == 0) {			//check to see if there are no bytes to read or the buffer is empty
		return -1;
	}
	uint8_t words[SIZE_OF_BUF];								//set up and initilize buffers
	uint32_t i = 0,j = 0;

	for ( i = 0 ; i < SIZE_OF_BUF; ++i) {
		buf[i] = '\0';
		words[i] = '\0';
	}

	uint32_t len = strlen((int8_t*) PCB_array[current_process_ID] -> command_global[current_process_ID]);		//set up the process in the PCB array
	int idx = 0;
	for (i = 0; i < len; ++i) {
		if (PCB_array[current_process_ID] -> command_global[current_process_ID][i] == ' ') {
			idx = i;
			break;
		}
	}

	//Case if we have just one word passed in
	if (idx == 0) {
		return -1;
	}

	//Second word in word2. Do this if and only if we have more than one word
	for (j = idx + 1; j < len; j++) {
		words[j - idx - 1] = PCB_array[current_process_ID] -> command_global[current_process_ID][j];
	}
	words[j - idx] = '\0';

	for (i = 0; i < strlen((int8_t*)words); ++i) {
		buf[i] = words[i];
	}

	buf[strlen((int8_t*)words)] = '\0';

	return 0;
}

/*
 * sys_call_vidmap
 *   DESCRIPTION: Implements the VIDMAP system call
 *   INPUTS: location of screen's start
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */
int32_t sys_call_vidmap(uint8_t** screen_start)
{
	if((uint32_t)screen_start < ONE_TWENTY_EIGHT_MIBIBYTES || (uint32_t)screen_start > ONE_THIRTY_TWO_MEBIBYTES){		//check if the program has access to the screen start
			return -1;
	}

	*screen_start = (uint8_t *)ONE_FIFTY_MEGABYTES;											//initialize necessary variables
	uint32_t temp_screen_start = (uint32_t)*screen_start;
	uint32_t * page_table_loc_offset;
	uint8_t page_directory_index_vidmap = temp_screen_start >> 22;    //not a magic number


	page_table_vidmap[0] = VIDEO_MEMORY | USER_BIT | READ_WRITE_BIT | PRESENT_BIT;			//set up new page table entry as well as page table
	page_table_loc_offset = page_table_vidmap;

	page_directory[(uint32_t)page_directory_index_vidmap] = (uint32_t)(page_table_loc_offset) | USER_BIT | READ_WRITE_BIT | PRESENT_BIT;	//set up the page directory

	//flush the TLB
	asm volatile(
		"movl %%cr3, %%eax \n"
		"movl %%eax, %%cr3"
		: :);

	return 0;
}

/*
 * sys_call_set_handler
 *   DESCRIPTION: Implements the SET_HANDLER system call
 *   INPUTS: signum, handler_address
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */
int32_t sys_call_set_handler(int32_t signum, void* handler_address)
{
	printf("set_handler");
	return 0;
}

/*
 * sys_call_sigreturn
 *   DESCRIPTION: Implements the SIGRETURN system call
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */
int32_t sys_call_sigreturn(void)
{
	printf("sigreturn");
	return 0;
}

/*
 * get_esp
 *   DESCRIPTION: Get the esp of that terminal
 *   INPUTS: terminal number
 *   OUTPUTS: none
 *   RETURN VALUE: esp, 69 by default
 *   SIDE EFFECTS: none
 */
extern uint32_t get_esp(uint8_t term)
{
	int i;
	for (i = 0; i < SIZE_OF_PCB_ARRAY; i++) {
		if (PCB_array[i]->terminalNumber == term && PCB_array[i]->isActive == 1 ) {
			return PCB_array[i]->esp;
		}
	}
	return 69;
	
}

/*
 * get_ebp
 *   DESCRIPTION: Get the ebp of that terminal
 *   INPUTS: terminal number
 *   OUTPUTS: none
 *   RETURN VALUE: ebp, 69 by default
 *   SIDE EFFECTS: none
 */
extern uint32_t get_ebp(uint8_t term)
{
	int i;
	for (i = 0; i < SIZE_OF_PCB_ARRAY; i++) {
		if ((PCB_array[i]->terminalNumber == term) && PCB_array[i]->isActive == 1) {
			return PCB_array[i]->ebp;
		}
	}
	return 69;
}

/*
 * get_esp_fuck
 *   DESCRIPTION: Get the esp of that process ID
 *   INPUTS: process_ID
 *   OUTPUTS: none
 *   RETURN VALUE: esp
 *   SIDE EFFECTS: none
 */
extern uint32_t get_esp_fuck(uint8_t process_ID)
{
		return PCB_array[process_ID]->esp;
}

/*
 * get_ebp_bitch
 *   DESCRIPTION: Get the ebp of that process ID
 *   INPUTS: process_ID
 *   OUTPUTS: none
 *   RETURN VALUE: ebp
 *   SIDE EFFECTS: none
 */
extern uint32_t get_ebp_bitch(uint8_t process_ID)
{
		return PCB_array[process_ID]->ebp;
}

/*
 * get_current_process_ID
 *   DESCRIPTION: Get the process ID of that terminal
 *   INPUTS: terminal number
 *   OUTPUTS: none
 *   RETURN VALUE: process ID, 69 by default
 *   SIDE EFFECTS: none
 */
extern int32_t get_current_process_ID(uint8_t term)
{
	int i;
	for (i = 0; i < SIZE_OF_PCB_ARRAY; i++) {
		if ((PCB_array[i]->terminalNumber == term) && PCB_array[i]->isActive == 1) {
			return PCB_array[i]->process_id;
		}
	}
	return 69;
}

/*
 * set_current_process_ID
 *   DESCRIPTION: Sets the process ID 
 *   INPUTS: new terminal number
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
extern void set_current_process_ID(int32_t new_PID)
{
	current_process_ID = new_PID;
}

/*
 * get_current_process_ID_without_term
 *   DESCRIPTION: returns the current process id
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
extern int32_t get_current_process_ID_without_term()
{
	return current_process_ID;
}

/*
 * set_parent_process_ID
 *   DESCRIPTION: sets the new process id
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
extern void set_parent_process_ID(int32_t new_PID)
{
	parent_process_ID = new_PID;
}

/*
 * get_parent_process_ID
 *   DESCRIPTION: returns the parent process id
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: parent process ID
 *   SIDE EFFECTS: none
 */
extern int32_t get_parent_process_ID()
{
	return parent_process_ID;
}

/*
 * is_PCB_array_full
 *   DESCRIPTION: checks to see if the pcb array is full. if it isnt, return 0, else return 1
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 1 on success, 0 on failure
 *   SIDE EFFECTS: none
 */
extern int32_t is_PCB_array_full() {
	int i = 0, num_process = 0; 
	for (i = 0; i < 6; ++i) {
		if (PCB_array[i] -> flag == 1) {
			++num_process;
		}
	}

	if (num_process == 6) {
		return 1;
	}

	return 0;
}
