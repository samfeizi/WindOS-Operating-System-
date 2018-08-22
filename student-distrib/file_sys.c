#include "file_sys.h"
#include "syscalls.h"

dentry_t* dentryglob;
uint8_t read_directory_index = 0;
extern uint8_t current_process_ID;

/*
 *   initialize_boot
 *   DESCRIPTION: This copies a booter into bootmebb
 *   INPUTS: boot pointer
 *   OUTPUTS: None
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Modifies booter
 */
void initialize_boot(boot_t* booter) {
	bootmebb = booter;
}

/*
 *  read_dentry_by_name
 *   DESCRIPTION: This function takes a file name, a data entry pointer and copies the dentry corresponding to that file into *dentry
 *   INPUTS: filename, data entry pointer
 *   OUTPUTS: None
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: Modifies *dentry
 */
int32_t read_dentry_by_name (const uint8_t *fname, dentry_t* dentry) {
	int i =0 ;
	int check;
	while (i<SIZEFILES) {             //cycles through 63 possible files
		check = strncmp((const int8_t*)bootmebb->d_table[i].file_name, (const int8_t*)fname, NAMESIZE);     //sets a flag if both strings are same
		if (!check) {
			memcpy(dentry, (const void*)&bootmebb->d_table[i], SIZEBUF);        //if they are the same string memcpy to dentry
			return 0;
		}
	i++;                     //increment file
	}
	return -1;            //return -1 on failure
}

/*
 *  read_dentry_by_index
 *   DESCRIPTION: This function copies the data entry of the iNode we pass in
 *   INPUTS: index for the inode and the data entry pointer
 *   OUTPUTS: Writes the data entry into dentry
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: Modifies *dentry
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {

	if (index > SIZEFILES)   //check to see valid file size
		return -1;

	memcpy(dentry, (const void*)&bootmebb->d_table[index], SIZEBUF);          //copy the proper dentry from boot into dentry
	return 0;
}


/*
 *   file_open
 *   DESCRIPTION: This function opens a file
 *   INPUTS: Filename
 *   OUTPUTS: Nothing
 *   RETURN VALUE: 0 by default
 *   SIDE EFFECTS: None
 */
int32_t file_open(const uint8_t* filename) {

	return 0;
}

/*
 *   read_data
 *   DESCRIPTION: This function takes an inode number, an offset, a buffer and the length. It copies length bytes
 *				  of the data pointed to by inode starting from the offset into buf
 *   INPUTS: none
 *   OUTPUTS: Writes stuff into buffer
 *   RETURN VALUE: 0 if successful, -1 if fail
 *   SIDE EFFECTS: Changes buf
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
	//Load the iNode address
	inode_t* iNode_address = (inode_t*)(bootmebb + 1 + inode);

	//Load the size of the file we're copying
	unsigned long size_of_file = iNode_address->b_length;

	if (inode > bootmebb->num_inodes)  //checks to see valid inode
			return -1;

	if (offset > size_of_file)       //checks to see if reaches end of file
			return 0;

	if (offset + length > size_of_file)
		length = size_of_file - offset;

	int i = 0;                            //initalize lots of variables
	uint32_t left_to_copy = length;
	uint32_t copied_so_far = 0;
	uint32_t buffer_idx = 0;
	uint32_t blocks_to_copy = 0;
	uint32_t remainder_to_copy = 0;
	uint32_t bytes_to_offset = 0;
	uint32_t blocks_to_offset = 0;
	uint32_t counter = 0;

	//Load the number of iNodes
	unsigned long number_of_iNodes = bootmebb->num_inodes;

	//Load the datablock address
	inode_t* datablock_address = (inode_t*)(bootmebb + 1 + number_of_iNodes);

 	blocks_to_offset = offset / BLOCKSIZE;      //calculates the blocks and remainder bytes to offset
 	bytes_to_offset = offset % BLOCKSIZE;

    //first case if we have an offset
	if (offset > 0) {
		datablock_address += iNode_address->in_table[blocks_to_offset];      //calculates datablock address
		for (i=bytes_to_offset; i<BLOCKSIZE; ++i) {                         //loop through remainder of the 4kb block
			buf[i-bytes_to_offset] = ((uint8_t*)datablock_address)[i];           //copy into buffer
			++counter;                                                     //inc counter, amount copied and dec remainder to copy
			copied_so_far++;
			left_to_copy--;
			if (left_to_copy == 0) {                                        //if run out of amount to copy return amt copied
				// buf[counter] = 0;
				return copied_so_far;
			}
		}
		buffer_idx = copied_so_far;                                      //reinitialize amount to buffer index
		blocks_to_offset = blocks_to_offset + 1;                         //reinitialize blocks to offset
	}

	blocks_to_copy = left_to_copy / BLOCKSIZE;                           //calculate blocks left to copy and remainder bytes left to copy
	remainder_to_copy = left_to_copy % BLOCKSIZE;


	//second case if we have full 4kb left to copy
	while(blocks_to_copy > 0) {
		datablock_address = (inode_t*)(bootmebb + 1 + number_of_iNodes);                //calculate datablock address
		datablock_address += iNode_address->in_table[blocks_to_offset];
		for (i=0; i<BLOCKSIZE; ++i) {                                     //loop through the 4kb block
			buf[buffer_idx+i] = ((uint8_t*)datablock_address)[i];       //copy into buffer
			++counter;													//inc counter, amount copied and dec remainder to copy
			copied_so_far++;
			left_to_copy--;
			if (left_to_copy == 0) {									//if run out of amount to copy return amt copied
				// buf[counter] = 0;
				return copied_so_far;
			}
		}
		buffer_idx = copied_so_far;										//reinitialize amount to buffer index
		blocks_to_offset = blocks_to_offset + 1;						 //reinitialize blocks to offset
		blocks_to_copy = left_to_copy / BLOCKSIZE;						//calculate blocks left to copy and remainder bytes left to copy
		remainder_to_copy = left_to_copy % BLOCKSIZE;
	}

	// third case when the last block is partial
	if (left_to_copy > 0) {
		datablock_address = (inode_t*)(bootmebb + 1 + number_of_iNodes);             //calculate datablock address
		datablock_address += iNode_address->in_table[blocks_to_offset];
		for (i=0; i<remainder_to_copy; ++i) {                             //loop through remainder of the 4kb block
			buf[buffer_idx+i] = ((uint8_t*)datablock_address)[i];        //copy into buffer
			copied_so_far++;                                             //inc counter, amount copied and dec remainder to copy
			left_to_copy--;
			++counter;
			if (left_to_copy == 0) {                                    //if run out of amount to copy return amt copied
				// buf[counter] = 0;
				return copied_so_far;
			}
		}

	}
	// buf[counter] = 0;
	return copied_so_far;                                               //return amount copied
}

/*
 *   file_read
 *   DESCRIPTION: This function calls read data on a valid file
 *   INPUTS: fd - location of the file in PCB
 *			 buf - buf that will get filled with data
 *			 nbytes - number of bytes that will be read fro mthe file
 *   OUTPUTS: Nothing
 *   RETURN VALUE: 0 by default
 *   SIDE EFFECTS: loads the passed in buffer with data from the file
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes)
{
	uint32_t inode = PCB_array[current_process_ID]->file_array[fd].inode;
	uint32_t file_pos = PCB_array[current_process_ID] -> file_array[fd].file_position;
	inode_t* iNode_address = (inode_t*)(bootmebb + 1 + inode);

	if (file_pos >= iNode_address->b_length)
		return 0;

	uint32_t temp_value = read_data(inode, file_pos, buf, nbytes);
	PCB_array[current_process_ID] -> file_array[fd].file_position += temp_value;
	file_pos = PCB_array[current_process_ID] -> file_array[fd].file_position;
	return temp_value;
}
/*
 *   file_close
 *   DESCRIPTION: This function closes a file
 *   INPUTS: Filename
 *   OUTPUTS: Nothing
 *   RETURN VALUE: 0 by default
 *   SIDE EFFECTS: None
 */

int32_t file_close(int32_t fd) {
	return 0;
}


/*
 *   file_write
 *   DESCRIPTION: This function writes a file
 *   INPUTS: Filename
 *   OUTPUTS: Nothing
 *   RETURN VALUE: -1 by default
 *   SIDE EFFECTS: None
 */

int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
	return -1;          //cant write to a read only system so error
}


/*
 *   read_directory
 *   DESCRIPTION: Reads the contacts of the directory
 *   INPUTS: filename
 *   OUTPUTS: None
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

int32_t read_directory(int32_t fd, void* buf, int32_t nbytes) {
	dentry_t dentry_idx;  //declares a blank dentry


	uint16_t j = 0;
	uint16_t currentCopied = 0;
	uint32_t length;

	if (read_directory_index > NUM_DIRECTORY) {
		read_directory_index = 0;
		return 0;
	}
	if (read_dentry_by_index (read_directory_index, &dentry_idx) == -1) {
		return 0;        //grab the proper dentry based on index
	}
	length = strlen((int8_t*)dentry_idx.file_name);
	if (length > MAX_NUM_CHAR) {
		length = MAX_NUM_CHAR;
	}
 	for(j = 0; j < length; j++) {                      //loopthrough INODES NOT MAGIC
 		// *(((uint8_t*)buf) + currentCopied) = dentry_idx.file_name[j];
 		((uint8_t*)buf)[j] = dentry_idx.file_name[j];
 		currentCopied++;
 	}
	 ++read_directory_index;
	 return currentCopied;
}

/*	Functions that don't serve a real purpose because they aren't used. They return default values.*/

/*
 *   write_directory
 *   DESCRIPTION: Writes to directorys
 *   INPUTS: file descriptor, buffer, nbytes
 *   OUTPUTS: None
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int32_t write_directory(int32_t fd, const void* buf, int32_t nbytes)
{
	return -1;
}

/*
 *   open_directory
 *   DESCRIPTION: Opens directrory
 *   INPUTS: filename
 *   OUTPUTS: None
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t open_directory(const uint8_t* filename)
{
	return 0;
}

/*
 *   close_directory
 *   DESCRIPTION: closes directrory
 *   INPUTS: file descriptor
 *   OUTPUTS: None
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t close_directory(int32_t fd)
{
	return 0;
}
