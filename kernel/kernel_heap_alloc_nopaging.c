/* This is a kernel heap. It's small and could expand more.
 * This heap starts at 0x500000 so it assumes that the 
 * RAM memory is at least 5 MB. See mem.c for the foundation
 * for this kind of memory management.
 */

#include "types.h"
#include "mem.h"
#include "graphics/screen.h"
#include "kernel_heap_alloc_nopaging.h"
#include "util.h"



/* Initializes some arrays and some variables. Os_memmap_size_info
 * and Os_memmap_free_info are keys to the heap. They hold
 * information about if a region with a set size that the user will
 * define, is free or not. The set sizes are stored in the
 * Os_memmap_size_info array.
 */
void init_memallocarrays_nopaging()
{
        OS_memory_bit_pos = (KERNEL_HEAP_NO_PAGING_START - KERNEL_HEAP_START) / 4096;
        for(int i = 0; i < 262144;i++) {
                Os_memmap_size_info[i] = (uint16_t)0x0000;                
        }

        for(int i = 0; i < 262144;i++) {
                Os_memmap_free_info[i] = (uint32_t)0x00;             
        }
        mem_array_address_start = (uint32_t)&(Os_memmap_size_info[0]);
}

/* Checks for a free 4k page and calls scan_address to look for 
 * details of this memory region.
 */
uint32_t kmem_allocate(uint16_t bt_size) 
{
        mbt_freeflag = 0;
        int16_t successfully_scanned_pos = 0;
        uint32_t successfully_scanned_addr = KERNEL_HEAP_NO_PAGING_START;
        /* mem_bt_temp holds the position in bits in the kernel's
         * memory map array which processes bits.
         */
        mem_bt_temp = check_free_memory(OS_memory_bit_pos);

        if (mem_bt_temp >= 0x264) {
	            print("\n\nWarning: reached the end of the Os memory map array.\nHalting.");
                asm("hlt");
        }

        while((mem_bt_temp < 0x264)) {
                /* Now we call scan_address for checking for regions of lesser sizes 
                 * in that 4k page we just allocated */
                successfully_scanned_pos = scan_address(bt_size, (mem_bt_temp - 0x200));
                if(successfully_scanned_pos > 4096) {
                        continue;
                }
                else {
                       /* Transform the bit notation to a real address */
                       successfully_scanned_addr = KERNEL_HEAP_START + (mem_bt_temp*4096) + ((uint32_t)successfully_scanned_pos);
                       return successfully_scanned_addr;
                }
        }
        return 0;
}

/* This funtion is the core of the heap. It checks for free spots
 * and the sizes of the free spots and if the requested size
 * is smaller or equal to the size it finds about the free spot, 
 * etcetera.
 */
int16_t scan_address(uint16_t size, uint32_t parent_pos)
{
        /* These first lines scan the free spot info and the size info.
         * This first step converts the bit position in the Os memory
         * bitmap in mem.c to the correct positions in the
         * Os_memmap_free_info array and the Os_memmap_size_info array.
         */
        uint32_t i = 0;
        uint32_t temp = 0;
        uint32_t temp2 = 0;
        uint16_t next_page_size = 0;
        uint8_t page_taken_check = 0;
        uint8_t* takeninfo_pos = &(Os_memmap_free_info[0]);
        temp = (4096*parent_pos);
        takeninfo_pos += temp;
        uint16_t* sizeinfo_pos = &(Os_memmap_size_info[0]);
        sizeinfo_pos += (parent_pos*4096);

        while(i < 4096) {
                /* Now, using these arrays we first check if the size is less
                 * than 4096 or if the size of equal to 4096. 
                 */
                if(size <= 4096) {
                        if((*(takeninfo_pos + i)) == 1) {
                                /* If the spot is taken and it spans beyond the page. */ 
                                if((*(sizeinfo_pos + i)) > (4096 - i)) {
                                        /* return and increase the bit size. */
                                        mem_bt_temp++;
                                        return 4097;
                                }
                                else {
                                        i += (uint32_t)(*(sizeinfo_pos + i));
                                        continue; 
                                }
                        }
                        /* If the requested size is equal to or less than 4096 and the spot is
                         * not taken the program execution goes in here.
                         */
                        else if((*(takeninfo_pos + i)) == 0) { 
                                /* If the spot is not taken and the size info is zero and the requested
                                 * size is 4096 or spans the rest of the page or spans less than the size
                                 * of the page. 
                                 */ 
                                if( ((*(sizeinfo_pos + i)) == 0) && (size <= (4096 - i))    ) {
                                        (*(takeninfo_pos + i)) = 1;

                                        if(size == 4096)
                                                (*(sizeinfo_pos + i)) = 4097;
                                        else
                                                (*(sizeinfo_pos + i)) = size;

                                        page_taken_check = check_if_page_taken((mem_bt_temp - (uint32_t)0x200));

                                        if(page_taken_check) {
                                                set_bit(mem_bt_temp);
                                        }

                                         return (uint16_t)i;
                                } 
                                /* If the spot is not taken and the size info is zero and the requested
                                 * size is 4096 or spans the rest of the page or spans beyond the page. 
                                 */ 
                                if( ((*(sizeinfo_pos + i)) == 0) && (size > (4096 - i))    ) {
                                        temp = i;
                                        next_page_size = size - (4096 - i);
                                        /* Go to the next page. */ 
                                        i += (4096 - i);

                                        if( ((*(takeninfo_pos )) == 1) || ( (*(sizeinfo_pos + i)) == 4097) || ( (*(sizeinfo_pos + i)) < next_page_size)) {
                                                mem_bt_temp++;
                                                return 4097;
                                        }

                                        (*(takeninfo_pos + i)) = 1;
                                        (*(sizeinfo_pos + i)) = next_page_size; 
                                        mem_bt_temp++;
                                        page_taken_check = check_if_page_taken((mem_bt_temp - (uint32_t)0x200));

                                        if(page_taken_check) {
                                                set_bit(mem_bt_temp);
                                        } 

                                        i = temp;
                                        (*(takeninfo_pos + i)) = 1;
                                        (*(sizeinfo_pos + i)) = size;
                                        mem_bt_temp--;
                                        page_taken_check = check_if_page_taken((mem_bt_temp - (uint32_t)0x200));
                                        if(page_taken_check) {
                                                set_bit(mem_bt_temp);
                                        } 

                                         return (uint16_t)i;
                                } 

                                /* If the spot is not taken and the requested size is 4096 or spans the rest of the page
                                 * and if the requested size is less or equal to the size info.
                                 */ 
                                if (((size == (4096 - i)) && ((*(sizeinfo_pos + i)) < 4097) && (size <= (*(sizeinfo_pos + i))))) {
                                        (*(takeninfo_pos + i)) = 1;
                                        /* In this function and kernel_free 4097 is a reserved value.
                                         * It is there if the rest of the size spans the whole page 
                                         * It's a protection against
                                         * the next page being erroneously overwritten. It is also
                                         * an error value.
                                         */
                                        if((size == 4096) && (i == 0)) {
                                                (*(takeninfo_pos + i)) = 1;
                                                (*(sizeinfo_pos + i)) = 4097;
                                                set_bit(mem_bt_temp);
                                                return (uint16_t)i;
                                        }

                                        page_taken_check = check_if_page_taken((mem_bt_temp - (uint32_t)0x200));
                                        if(page_taken_check) {
                                                set_bit(mem_bt_temp);
                                        }

                                         (*(sizeinfo_pos + i)) = size;
                                         return (uint16_t)i;
                                }
                                /* If the spot is not taken and if the requested size is within the page. and if the
                                 * requested size is less or equal to the size info.
                                 */  
                                else if((size < (4096 - i)) && ((*(sizeinfo_pos + i)) < (4096 - i)) && (size <= (*(sizeinfo_pos + i)))) {
                                        /* mark the spot as taken */
                                        (*(takeninfo_pos + i)) = 1;
                                        (*(sizeinfo_pos + i)) = size;
                                        page_taken_check = check_if_page_taken(mem_bt_temp - (uint32_t)0x200);
                                        if(page_taken_check) {
                                                set_bit(mem_bt_temp);
                                        }
                                        return (uint16_t)i;
                                }
                                /* If the size is equal to or less than 4096 and the spot is not taken
                                 * and if the requested size is and within the page and within the next page.
                                 */
                                else if(size <= (*(sizeinfo_pos + i))) {
                                        /* check if the relevant area in the next page is free */
                                        temp = i;
                                        next_page_size = size - (4096 - i);
                                        /* We go into the next page. */
                                        i += (4096 - i);
                                        /* At the next page now. If the value is 4097 then it spans the whole page. */
                                        if((*(sizeinfo_pos + i)) == 4097) {
                                                /* return an error and check the next page */
                                                mem_bt_temp++;
                                                return 4097;
                                        }

                                        if((*(takeninfo_pos + i)) == 1) {
                                                /* return an error and check the next page */
                                                mem_bt_temp++;
                                                return 4097;
                                        }
                                        /* We are at the next page. If there are no errors and the size is within or equal
                                         * to, or greater than the next pages.
                                         */
                                        else {
                                                /* Checks if the size at the next page is less than the 
                                                 * requested size.
                                                 */
                                                if((next_page_size <= (*(sizeinfo_pos + i)))) {
                                                        (*(takeninfo_pos + i)) = 1;
                                                        (*(sizeinfo_pos + i)) = next_page_size;
                                                }
                                                /* We have set the taken mark. Checks if the whole page is taken. */
                                                page_taken_check = check_if_page_taken(mem_bt_temp - (uint32_t)0x200 + (uint32_t)1);
                                                if(page_taken_check)
                                                        set_bit(mem_bt_temp + 1);
                                                /* Sets the variable i to it's former value. We go back to the former page.*/
                                                i = temp; 
                                                (*(takeninfo_pos + i)) = 1;
                                                /* The size takes up the rest of the former page. We put the value 4097, here. */
                                                (*(sizeinfo_pos + i)) = size;
                                                /* Checks if the whole former page is taken. */ 
                                                page_taken_check = check_if_page_taken(mem_bt_temp - (uint32_t)0x200);
                                                if(page_taken_check)
                                                        set_bit(mem_bt_temp);
                                        }                    
                                        return (uint16_t)i;
                                }
                                else {
                                            print("Kernel memory allocation: Unexpected value of size. Halting.");
                                            asm("hlt");
                                    }
                        }
                } 
                else {
                        /* if size is bigger than 4096 then check if a spot is free.
                         * It is likely that some spot is free and will span the whole
                         * area as big as the size. The bits have been checked in the
                         * caller function. The if statement below will check whether the spot 
                         * is taken or not.
                         */
                        if((*(takeninfo_pos + i)) == 1) {
                                /* If we are at the beginning of the page and it's busy. */
                                if((*(sizeinfo_pos + i)) == 4097) {
                                        /* return and increase the bit size. This ought not to happen often if at all */
                                        mem_bt_temp++;
                                        return 4097;
                                }
                                i += (uint32_t)(*(sizeinfo_pos + i));
                                continue;
                        }
                        /* If the size is bigger than 4096 and if the spot is not taken.*/
                        else if((*(takeninfo_pos + i)) == 0) {
                                /* If the sizeinfo is less than the requested size, then jump forward. */
                                if(((*(sizeinfo_pos + i)) != 0) &&  ((*(sizeinfo_pos + i))< size)) {
                                        i += (uint32_t)(*(sizeinfo_pos + i));
                                        continue;
                                }
                                /* If sizeinfo is bigger or equal to the requested size. */                            
                                uint16_t a = size; 
                                /* Store i in the temporary variable. */
                                temp = i;
                                /* Store the mem_bt_temp variable in temp4. */
                                uint32_t temp4 = mem_bt_temp;
                                /* Goes to the next page. */
                                i += (4096 - i);
                                /* Size decreases by the rest of the former page. */
                                a -= (4096 - i);
                                mem_bt_temp++;
                                /* If this size spans the whole of the next page and not more than that
                                 * then return.
                                 */ 
                                /* Commented code: if (a == 4096) {

                                        (*(takeninfo_pos + i)) = 1;
                                        (*(sizeinfo_pos + i)) = 4097;
                                        set_bit(mem_bt_temp);
                                        i = temp;
                                        (*(takeninfo_pos + i)) = 1;
                                        (*(sizeinfo_pos + i)) = size;
                                        mem_bt_temp--;
                                        page_taken_check = check_if_page_taken(mem_bt_temp - (uint32_t)0x200);
                                        if(page_taken_check)
                                                set_bit(mem_bt_temp);
                                        return (int16_t)i;

                                } */
                                /* While the size is still bigger than zero. It will decrease as we move to
                                 * other pages.
                                 */ 
                                while (a > 0) {
                                        /* If the size spans across this page and goes beyond the page. */
                                        if(a > 4096) {
                                                /* Checks if the page is taken. */
                                                temp2 = check_bit(mem_bt_temp);
                                                /* If the page is not taken then decrease the size by the size of a page 
                                                 * set the marks, and move to the next page. 
                                                 */
                                                if(!temp2) {
                                                        a -= 4096;                                                        
                                                        i+= 4096;                                                        
                                                        set_bit(mem_bt_temp);
                                                        mem_bt_temp++;                                                                
                                                }
                                                /* The page is taken. */ 
                                                else {
                                                        mem_bt_temp++;
                                                        return 4097;
                                                }
                                        }
                                        /* If the size is equal to the size of a page. */ 
                                        else if (a == 4096) {
                                                a -= 4096;
                                                i+= 4096;
                                                set_bit(mem_bt_temp);
                                                mem_bt_temp++;
                                        }
                                        else {
                                                print("\nKernel memory heap: unexpected value of a. Halting.");
                                                asm("hlt");
                                        }
                                        /* If the size has shrunk to a size less than a page. */
                                        if (a < 4096) {
                                                /* Some errors that should not happen, but they might happen. */ 
                                                if((*(takeninfo_pos + i)) == 1 && (a > 0)) {
                                                        mem_bt_temp++;
                                                        return 4097;
                                                }
                                                else if(((*(sizeinfo_pos + i)) < a) && ((*(sizeinfo_pos + i)) != 0)) {        
                                                        mem_bt_temp++;
                                                        return 4097;
                                                }
                                                else {
                                                        /* put it in the correct spots from here and back and to here again */
                                                        if(a > 0) {
                                                                (*(takeninfo_pos + i)) = 1;
                                                                (*(sizeinfo_pos + i)) = a;
                                                        }

                                                        page_taken_check = check_if_page_taken(mem_bt_temp - (uint32_t)0x200);
                                                        if(page_taken_check)
                                                                set_bit(mem_bt_temp);

                                                        /* Store the shrunk size in c. */
                                                        uint16_t c = a;
                                                        /* We go back to the first page. */
                                                        i = temp;
                                                        /* The requested size increases to the original size. */ 
                                                        a = size;
                                                        (*(takeninfo_pos + i)) = 1;
                                                        (*(sizeinfo_pos + i)) = size; 
                                                        mem_bt_temp = temp4;
                                                        page_taken_check = check_if_page_taken(mem_bt_temp - (uint32_t)0x200);

                                                        if(page_taken_check)
                                                                set_bit(mem_bt_temp);

                                                        /* Go to the next page. */
                                                        i += (4096 - i);
                                                        /* Shrink the original requested size by the rest of the page. */
                                                        a -= (4096 - i);
                                                        /* Shrink the size by the beginning of the last page. Now the size can be
                                                         * measured in pages.
                                                         */ 
                                                        a -= c;
                                                        /* Put the number of pages that the value of size now has, in the
                                                         * variable b. 
                                                         */ 
                                                        uint16_t b = a/4096;
                                                        for(;b > 0;b--) {
                                                               (*(takeninfo_pos + i)) = 1;
                                                                (*(sizeinfo_pos + i)) = 4097;
                                                                i+=4096;                        
                                                        }
                                                        i = temp;
                                                        return (uint16_t)i;
                                                }
                                        }
                                }
                        }                                
                }               
        }
        return (int16_t)i;
}

/* Checks if a whole page is taken. If it is then the kernel will
 * usually skip the page.
 */
uint8_t check_if_page_taken(uint32_t p_number) {
        uint8_t flag_set = 0;
        uint8_t* taken_info = &(Os_memmap_free_info[0]);
        taken_info += (4096*p_number);
        uint16_t* size_info = &(Os_memmap_size_info[0]);
        size_info += (p_number*4096);  

        /* check if we have the reserved size of 4097 */
        if(((*(taken_info)) == 1) && ((*(size_info)) == 4097)) {
                return (uint8_t)1;
        }
        else if(((*(taken_info)) == 0) && ((*(size_info)) == 4097)) {
                return (uint8_t)0;
        }
  
        int32_t i = 0;
        while(i < 4096) {                
                if(((*(taken_info + i)) == 1) && (4096 - i) > ((*(size_info + i)))) {

                        if((*(size_info + i)) == 0)
                                flag_set = 1;

                        if(flag_set == 0) {
                                i += ((*(size_info + i)));
                                continue;
                        }                        
                }
                else if(((*(taken_info + i)) == 1) && (4096 - i) <= ((*(size_info + i)))) {
                        return (uint8_t)1;
                }
                else if( ( (*(taken_info + i)) == 1) && ((*(size_info + i)) == 4097)) {
                        return (uint8_t)1;                        
                }
                else if(((*(taken_info + i)) == 1) && ((*(size_info + i)) > 4097)) {
                        return (uint8_t)1;                        
                }
                else if (((*(taken_info + i)) == 0)) {
                        return (uint8_t)0;
                }
                else {
                        print("\nUnexpected value of taken_info. Halting.");
                        asm("hlt"); 
                }

                if(flag_set == 1) {
                        i+= 4;
                        flag_set = 0;
                }
        }
 
        return (uint8_t)1;
}

/* Checks if a whole page is freed. */
uint8_t check_if_page_freed(uint32_t page) {
        uint8_t* taken_info = &(Os_memmap_free_info[0]);
        taken_info += (4096*page);
        uint16_t* size_info = &(Os_memmap_size_info[0]);
        size_info += (page*4096);  
        /* the array is an unsigned short so change according to that */
        uint8_t flag_set = 0;

        print("\ncheck if page freed: mem address for size info: ");
        print_hex((uint32_t)size_info); 

        /* check if we have the reserved size of 4097 */
        if(((*(taken_info)) == 0) && ((*(size_info)) == 4097)) {
                return (uint8_t)1;
        }
        else if(((*(taken_info)) == 1) && ((*(size_info)) == 4097)) {
                return (uint8_t)0; 
        }
  
        int32_t i = 0;
        while(i < 4096) {                
                if(((*(taken_info + i)) == 0) && (4096 - i) > ((*(size_info + i)))) {

                        if((*(size_info + i)) == 0)
                                flag_set = 1;

                        if(flag_set == 0) {
                                i += ((*(size_info + i)));
                                continue;
                        }                        
                }
                else if(((*(taken_info + i)) == 0) && (4096 - i) <= ((*(size_info + i)))) { 
                        return (uint8_t)1;
                }
                else if(((*(taken_info + i)) == 0) && ((*(size_info + i)) == 4097)) {
                        return (uint8_t)1;                        
                }
                else if(((*(taken_info + i)) == 0) && ((*(size_info + i)) > 4097)) {
                        return (uint8_t)1;                        
                }
                else if (((*(taken_info + i)) == 1)) {
                        return (uint8_t)0;
                }                
                else {
                        print("\nUnexpected value of taken_info. Halting.");
                        asm("hlt"); 
                }

                if(flag_set == 1) {
                        i+= 4;
                        flag_set = 0;
                }               
        }

        return (uint8_t)1;
}

/* Frees a spot within a page of within a set of pages. */
void kernel_free(void* to_be_freed)
{
        /* A memory address will correspond to an index in the free and size array.
         * Now the function will find that spot in those arrays.
         */
        uint16_t temp = 0;
        uint32_t current_page_spot = 0;
        uint32_t current_page_offset = 0;
        uint8_t freed_check = 0;

        /* find the page first */
        current_page_spot = (uint32_t)to_be_freed;
        print("\nkernel free: current page spot: ");
        print_hex(current_page_spot);
        current_page_spot = current_page_spot - KERNEL_HEAP_NO_PAGING_START;
        /* Commented code: print("\nkernel free: current page spot is: ");
        print_hex(current_page_spot); */
        current_page_spot /= 4096;

        /* Commented code: print("\nkernel free: current page spot is: ");
        print_hex(current_page_spot); */ 
        mem_bt_temp = current_page_spot + 0x200;
        /* then find the offset */
        current_page_offset = (uint32_t)to_be_freed;
        current_page_offset %= 4096;
        /* now the function will pick the correct array spots.
         * After that it will check if the size spans another page or other pages.
         */ 
        uint8_t* kfree_takeninfo_pos = &(Os_memmap_free_info[0]);
        kfree_takeninfo_pos += (4096*current_page_spot);
        uint16_t* kfree_sizeinfo_pos = &(Os_memmap_size_info[0]);
        kfree_sizeinfo_pos += (current_page_spot*4096);  

        kfree_takeninfo_pos += current_page_offset;
        kfree_sizeinfo_pos += current_page_offset;
        print("\nkernel free: current page offset: ");
        print_hex(current_page_offset);

        /* if not then set the position to indicate that it's a free spot */
        if ((*(kfree_sizeinfo_pos)) == 4097) {
                (*(kfree_takeninfo_pos)) = 0;
                (*(kfree_sizeinfo_pos)) = 0;
                freed_check = check_if_page_freed(current_page_spot);
                if(freed_check)
                        zero_bit(mem_bt_temp);
        }
        /* Checks if the requested size spans to another page or other pages. */
        if((*(kfree_sizeinfo_pos)) > (4096 - current_page_offset)) {
                /* check how many pages the size spans and make a loop */
                int32_t kfree_span_pages = (int32_t)(((*(kfree_sizeinfo_pos))) / 4096);
                (*(kfree_takeninfo_pos)) = 0;
                /* Goes to the next page. */ 
                kfree_takeninfo_pos += ((uint32_t)4096 - (uint32_t)kfree_takeninfo_pos);
                kfree_sizeinfo_pos += ((uint32_t)4096 - (uint32_t)kfree_sizeinfo_pos);
                mem_bt_temp++;

                /* if size is 4097 then a loop will start which checks how many pages the size crosses. */
                if((*(kfree_sizeinfo_pos)) == 4097) { 

                        /* Commented code: for(int32_t a = 1; temp_flag == 0;a++) { */
                        /* we already crossed a page so a is set to 1 */
                        for(int32_t a = 1; a < kfree_span_pages;a++) {
                                /* Go through the pages and zero the contents therein. */
                                (*(kfree_takeninfo_pos)) = 0;
                                zero_bit(mem_bt_temp);
                                mem_bt_temp++;
                                kfree_sizeinfo_pos += 4096;
                                kfree_sizeinfo_pos += 4096;
                                /* If we are at the last page. */
                                if(((*(kfree_sizeinfo_pos)) != 4097) && ((*(kfree_sizeinfo_pos)) != 0)){
                                        /* Commented code: (*(kfree_takeninfo_pos)) = 0;
                                        freed_check = check_if_page_freed(kfree_takeninfo_pos, kfree_sizeinfo_pos);
                                        if(freed_check)
                                                zero_bit(mem_bt_temp); */

                                        /* a size of 4097 is not the real size. It indicates that the first spot
                                         * in the next page is used by another variable. The function will go back
                                         *  a page and check if the size is 4097 
                                         */
                                        kfree_sizeinfo_pos -= 4096;
                                        temp = (*(kfree_sizeinfo_pos));
                                        kfree_sizeinfo_pos += 4096;
                                        if ((*(kfree_sizeinfo_pos)) > 0 && (temp != 4097)) {
                                                (*(kfree_takeninfo_pos)) = 0;
                                                freed_check = check_if_page_freed(mem_bt_temp - (uint32_t)0x200);
                                                if(freed_check)
                                                        zero_bit(mem_bt_temp);
                                        }
                                }                                                                      
                        }
                }
                else {

                        (*(kfree_takeninfo_pos)) = 0;
                }
        }
        else {
                print("\nkernel free memory: goes in here!");
                (*(kfree_takeninfo_pos)) = 0;
                freed_check = check_if_page_freed(current_page_spot);
                if(freed_check)
                        zero_bit(mem_bt_temp);
        }        
}
