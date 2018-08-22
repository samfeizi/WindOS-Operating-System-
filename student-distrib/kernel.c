/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "IDT.h"
#include "keyboard.h"
#include "RTC.h"
#include "Paging.h"
#include "terminal.h"
#include "file_sys.h"
#include "syscalls.h"

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

boot_t* bootloaded; //create global variable

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;

	/* Clear the screen. */
	clear();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	if (CHECK_FLAG (mbi->flags, 3)) {
		int mod_count = 0;
		int i;

		module_t* mod = (module_t*)mbi->mods_addr;
		bootloaded = (void *)mod->mod_start;

		while(mod_count < mbi->mods_count) {
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<16; i++) {
				printf("0x%x ", *((char*)(mod->mod_start+i)));
			}
			printf("\n");
			mod_count++;
			mod++;
		}
	}
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = 0x0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = 0x0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 0x9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000;
		ltr(KERNEL_TSS);
	}


	//Test the IDT first, then the PIC, then the RTC, then the keyboard, then the STI
	//IDT works
	i8259_init();

	IDT_init();

	init_paging();

	/* Init the PIC */


	/* Initialize devices, memory, filesystem, enable device interrupts on the
	 * PIC, any other initialization stuff... */



	//TEST RTC
	RTC_init();
	clear();
	/* Enable interrupts */
	/* Do not enable the following until after you have set up your
	 * IDT correctly otherwise QEMU will triple fault and simple close
	 * without showing you any output */

	/*	Enable the IRQs for the PICs*/
	// 8 AND 2 NEEDED FOR RTC, 1 NEEDED FOR KEYBOARD
	enable_irq(2);
	enable_irq(8);
	enable_irq(1);
	// for(i = 0; i < 80; i++)
	// {
	// 	//WriteCharacter('a', 255, 0, i, 0);
	// 	fb_move_cursor(i, 0);
	// }

	init_cursor_c();
	// char test[] = "ECE391 \n is \n cool";
 //    write_Terminal(0, test, 15);
	/*	Allow interrupts to be enabled*/
	sti();

	//Testing the filesystem
	//1. Read dentry by name - we can get the inode from that
	//2. Read data into a char buffer given the inode (0 offset for whole file, length can be arbitrary)
	//3. Print the data onto the screen from the char buffer

	initialize_boot(bootloaded);

	//init_function_table();
	init_PCB_array();


// /* THIS TESTS READ ENTRY BY INDEX */
// 	dentry_t* dptr = &dentry;
// 	read_dentry_by_index (5, dptr);
// 	write_Terminal(0, dptr->file_name, strlen((char*)(dptr->file_name)));

	//TEST CASES
	 // uint8_t buf[40000];
/******************************************** 1. READ_DENTRY_BY_NAME ****************************************/
	//read_dentry_by_name((const uint8_t*)("cat"), &dentry);
	// read_dentry_by_name((const uint8_t*)("counter"), &dentry);
	// read_dentry_by_name((const uint8_t*)("fish"), &dentry);
	//  read_dentry_by_name((const uint8_t*)("frame0.txt"), &dentry);
	// read_dentry_by_name((const uint8_t*)("frame1.txt"), &dentry);
	// read_dentry_by_name((const uint8_t*)("grep"), &dentry);
	// read_dentry_by_name((const uint8_t*)("hello"), &dentry);
	// read_dentry_by_name((const uint8_t*)("ls"), &dentry);
	// read_dentry_by_name((const uint8_t*)("pingpong"), &dentry);
	// read_dentry_by_name((const uint8_t*)("shell"), &dentry);
	// read_dentry_by_name((const uint8_t*)("sigtest"), &dentry);
	// read_dentry_by_name((const uint8_t*)("syserr"), &dentry);
	// read_dentry_by_name((const uint8_t*)("testprint"), &dentry);
	// read_dentry_by_name((const uint8_t*)("verylargetextwithverylongname.txt"), &dentry);
	// printf("read_dentry_by_name\n");
	// printf(" FILENAME: %s", dentry.file_name);
	// printf(" TYPE: %u", dentry.file_type);
	// printf(" INODE: %u", dentry.inode_num);
	// printf(" SIZE: %u\n", ((inode_t*)(bootloaded + 1 + dentry.inode_num))->b_length);

/******************************************** 2. READ_DENTRY_BY_INDEX ****************************************/
	/*dentry_t dentry_idx;
	int i = 0;
	printf("read_dentry_by_index\n");
	for (i = 0; i < 16; ++i) {
		read_dentry_by_index (i, &dentry_idx);
		printf(" FILENAME: %s", dentry_idx.file_name);
		printf(" TYPE: %u", dentry_idx.file_type);
		printf(" INODE: %u", dentry_idx.inode_num);
		printf(" SIZE: %u\n", ((inode_t*)(bootloaded + 1 + dentry_idx.inode_num))->b_length);
	}*/

/*************************************************  3. READ_DATA *********************************************/
	 // read_data(dentry.inode_num, 0, buf, 8000);
	 // read_directory(0, buf, 8000);

/*************************************************     4. RTC    *********************************************/
	// outb (14,0x3D4);
	// outb (0x00,0x3D5);
	// outb (15,0x3D4);
	// outb (0,0x3D5);

	// RTC_Write(0,0,0);

	// while(1){
	// 	RTC_Read(0,0,0);
	// 	getchar1('1');
	// }

/*********************************************** 5. PRINT DAT SHIT *********************************************/
	 //write_Terminal(0, buf, strlen((char*)(buf)));

/*********************************************** 6. READ DIRECTORY *********************************************/
	//read_directory(0);


	PIT_init(10);
	multi_terminal_init(2);
	multi_terminal_init(1);
	multi_terminal_init(0);

	/* Execute the first program (`shell') ... */
	/* Spin (nicely, so we don't chew up cycles) */
	asm volatile(".1: hlt; jmp .1;");
}
