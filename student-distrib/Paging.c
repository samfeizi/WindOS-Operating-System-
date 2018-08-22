#include "Paging.h"
#include "x86_desc.h"

/*
* init_paging()
*   Inputs: none
*   Return Value: none
*   Function: Initialize the structure for paging
*/
void init_paging() {
    int i;
    int VIDEO_MEMORY = STARTING_ADDRESS_OF_FRAMEBUFFER;
    for (i = 0; i < NUMBER_OF_ENTRIES; ++i) {
        //Set the page directory to zero, give it everything but r/w access
        page_directory[i] = SYSTEM_CALL;
        page_table[i] = 0;

		//we want the right-most bit of the first page directory to be one, as well as hold the page_table value 
        if (i == 0) {
            page_directory[0] = ((unsigned int) page_table) | 1;
        }

        if (i == 1) {
            page_directory[1] = KERNEL_PHYSICAL_ADDRESS | SYSTEM_CALL | MASK4 | 0x1;
        }
    }
    page_table[VIDEO_MEMORY >> VIRTUAL_VALUE] = MASK3 | VIDEO_MEMORY;
    initialize_pages(&page_directory);
}

