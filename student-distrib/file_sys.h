#ifndef _FILE_SYS_H
#define _FILE_SYS_H

#include "lib.h"
#include "keyboard.h"

#define NAMESIZE 32
#define SIZEBUF 64
#define SIZEFILES 63
#define BLOCKSIZE 4096
#define NODETABLESIZE 1023
#define BOOTTABLESIZE 63
#define RESBOOT 13
#define RESENTRY 6
#define NUM_DIRECTORY 16
#define MAX_NUM_CHAR 32


/* Declares a structure for the dentry blocks */
typedef struct {
	char file_name[NAMESIZE]; // upto 32 chars zero padded
	unsigned long file_type; // 1 for dir 2 for reg file 0 for rtc
	unsigned long inode_num; // index for inode
	unsigned long res_entry[RESENTRY]; //24B Reserved

} dentry_t;


/* Declares a structure for the boot block */
typedef struct {
	unsigned long num_dir_entry; // # of dir entries
	unsigned long num_inodes; // # of inodes (N)
	unsigned long data_blocks;  // # of data blocks (D)
	unsigned long res_boot[RESBOOT]; // 52 B Reserved
	dentry_t d_table[BOOTTABLESIZE]; // max of 63 files

} boot_t;

/* Declares a structure for the inode blocks */
typedef struct {
	unsigned long b_length; //size of table
	unsigned long in_table[NODETABLESIZE]; //data for inode

} inode_t;


/* Initialized boot block var*/
void initialize_boot(boot_t* booter);

/* Fills in a dentry based on name */
int32_t read_dentry_by_name (const uint8_t *fname, dentry_t* dentry);

/* Fills in a dentry based on index */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);





/* Reads data from memory and copies into buf */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/* Read a file in file system */
int32_t file_read(int32_t fd,  void* buf, int32_t nbytes);

/* Attempts to write to a file (error) */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

/* Opens a file  */
int32_t file_open(const uint8_t* filename);

/* Closes a file */
int32_t file_close(int32_t fd);


/* Reads the contacts of the directory */
int32_t read_directory(int32_t fd, void* buf, int32_t nbytes);

/* Attempts to write into directory */
int32_t write_directory(int32_t fd, const void* buf, int32_t nbytes);

/* Opens a directory */
int32_t open_directory(const uint8_t* filename);

/* Closes a directory */
int32_t close_directory(int32_t fd);


boot_t* bootmebb;     //create global variables
/*
    Acknowledgements:
        http://littleosbook.github.io/#paging
        http://wiki.osdev.org/Paging
        https://en.wikipedia.org/wiki/Control_register#CR0
        https://courses.engr.illinois.edu/ece391/secure/assignments/mp/mp3/mp3.pdf
*/


#endif
