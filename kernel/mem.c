/* This file contains some code for basic memory management.
 * It constitutes a foundation for more complex code as
 * code for kernel heaps, for instance.
 */

#include "types.h"
#include "device/usb/usb.h"
#include "mem.h"
#include "graphics/screen.h"
#include "util.h"




/* This function sets a specific value at a specific memory address. */
void setmembyte(uint8_t * pointer_pt, uint32_t start_offset, uint32_t blocks_times_4096, uint8_t value)
{
        for(uint32_t i = start_offset; i < blocks_times_4096; i = i + 4) {
                *(pointer_pt + i + 3) = value; // big endian format?
                *(pointer_pt + i + 2) = value;
                *(pointer_pt + i + 1) = value;
                *(pointer_pt + i) = value;
        }
}


/* Right now I don't remember what this function does. */
uint32_t read_mem_address(uint32_t* value, uint8_t counter1)
{
        uint32_t x = (uint32_t)counter1;
        uint32_t memvalue = 0;

        uint32_t temp = 0;
        uint32_t temp2 = (uint32_t)counter1;
        uint32_t temp3;
        uint32_t v;
        uint32_t po;
        for (;temp <= temp2; temp++) {
                v = (uint32_t)value[temp];
                po = power_of(10,x);

                printi(po);
                temp3 = (v)*(po);

                memvalue += temp3;
                x--;
        }

        return memvalue;
}

/* This function reads a memory address and changes it's content 
 * according to the operation which is shown in the code. 
 * I guess it should zero out byte number
 * two of the resulting value.
 */
uint32_t read_dword_2(const uint32_t base_addr, const uint32_t offset)
{
        volatile uint32_t dword = (*((uint32_t *) (base_addr + offset))) & (~(0x000000ff << 8));
        return dword;
}

/* This function reads the contents of a memory address */
uint32_t read_dword(const uint32_t base_addr, const uint32_t offset)
{
        volatile uint32_t dword = *((uint32_t *) (base_addr + offset));
        return dword;
}

/* Returns an address based of a base address and the offset */
uint32_t * read_dword_addr(const uint32_t base, const uint32_t offset)
{
        uint32_t * dword = ((uint32_t*) (base + offset));
        return dword;
}

/* This function writes a value to a memory address */
void write_dword(const uint32_t base, const uint32_t offset, const uint32_t value)
{
        *((uint32_t *) (base + offset)) = value;
}

/* Writes a byte value or values in bytes into a memory address
 * or the following amount of addresses specified by count.
 * The vaiable dest could point to astruct to fill, for instance. 
 */
void memset(void *dest, uint8_t val, uint32_t count) 
{
        uint8_t *temp = (uint8_t *)dest;
        for( ; count != 0; count--) 
                *temp++ = val;
}

/* The functions below Behaves almost like the function above. */
void memset2(uint8_t *dest, uint8_t* val, uint32_t count) 
{

        for(uint32_t count1 = 0; count1 < count; count1++) {
                *dest = *val;
                dest++;
                val++;
        }
}

/* This function writes a value to a memory address in bytes
 * a specific amount of times. Often this value is zero.
 */
void memsetbyte(uint8_t *dest, uint8_t val, uint32_t count)
{
        for(uint32_t count1 = 0; count1 < count; count1++) {
                *dest = val;
                dest++;
        }

}

/* Writes a value to a memory address in dwords
 * a specific amount of times. Often this value is zero.
 */
void memsetint(uint32_t *dest, uint32_t val, uint32_t count)
{

        for(uint32_t count1 = 0; count1 < count; count1++) {
                *dest = val;
                dest++;
        }
}

/* Writes a value to a memory address in words
 * a specific amount of times. Often this value is zero.
 */
void memsetshort(uint16_t *dest, uint16_t val, uint32_t count) 
{

        for(uint32_t count1 = 0; count1 < count; count1++) {
                *dest = val;
                dest++;
        }
}


/* Zeroes five page regions */
void zero_usbms_mem_1 (volatile uint8_t* ms_buffer_ptr0, volatile uint8_t* ms_buffer_ptr1, volatile uint8_t* ms_buffer_ptr2, volatile uint8_t* ms_buffer_ptr3, volatile uint8_t* ms_buffer_ptr4)
{
        uint32_t counter1;
        uint32_t counter2;
        volatile uint8_t* ms_pointr;

        for(counter1 = 0; counter1 < 5; counter1++) {

                if(counter1 == 0)
                        ms_pointr = ms_buffer_ptr0;
                else if(counter1 == 1)
                        ms_pointr = ms_buffer_ptr1;
                else if(counter1 == 2)
                        ms_pointr = ms_buffer_ptr2;
                else if(counter1 == 3)
                        ms_pointr = ms_buffer_ptr3;
                else if(counter1 == 4)
                        ms_pointr = ms_buffer_ptr4;
                for(counter2 = 0; counter2 < 4096; counter2++)
                        *(ms_pointr + counter2) = 0x00;
        }
}

/* Zeroes four page regions */
void zero_usbms_mem_2 (volatile uint32_t ms_buffer_ptr1, volatile uint32_t ms_buffer_ptr2, volatile uint32_t ms_buffer_ptr3, volatile uint32_t ms_buffer_ptr4)
{
	    uint32_t counter1;
	    uint32_t counter2;
	    volatile uint8_t* ms_pointr;
	    volatile uint32_t ms_ptr1 = ms_buffer_ptr1;
	    volatile uint32_t ms_ptr2 = ms_buffer_ptr2;
	    volatile uint32_t ms_ptr3 = ms_buffer_ptr3;
	    volatile uint32_t ms_ptr4 = ms_buffer_ptr4;

	    for(counter1 = 1; counter1 < 5; counter1++) {

			    if(counter1 == 1)
				        ms_pointr = (volatile uint8_t*)ms_ptr1;
			    else if(counter1 == 2)
				        ms_pointr = (volatile uint8_t*)ms_ptr2;
			    else if(counter1 == 3)
				        ms_pointr = (volatile uint8_t*)ms_ptr3;
			    else if(counter1 == 4)
				        ms_pointr = (volatile uint8_t*)ms_ptr4;
			    for(counter2 = 0; counter2 < 4096; counter2++)
				        *(ms_pointr + counter2) = 0x00;
	    }
}
/* Zeroes five page regions. This function behaves almost as zero_usbms_mem_1 */
void zero_usbms_mem_3 (volatile uint32_t ms_buffer_ptr0, volatile uint8_t* ms_buffer_ptr1, volatile uint32_t ms_buffer_ptr2, volatile uint32_t ms_buffer_ptr3, volatile uint32_t ms_buffer_ptr4)
{
	    uint32_t counter1;
	    uint32_t counter2;
	    volatile uint8_t* ms_pointr;
	    volatile uint32_t ms_ptr0 = ms_buffer_ptr0;
	    volatile uint8_t* ms_ptr1 = ms_buffer_ptr1;
	    volatile uint32_t ms_ptr2 = ms_buffer_ptr2;
	    volatile uint32_t ms_ptr3 = ms_buffer_ptr3;
	    volatile uint32_t ms_ptr4 = ms_buffer_ptr4;


	    for(counter1 = 0; counter1 < 5; counter1++) {

			    if(counter1 == 0)
				        ms_pointr = (volatile uint8_t*)ms_ptr0;
			    if(counter1 == 1)
				        ms_pointr = ms_ptr1;
			    else if(counter1 == 2)
				        ms_pointr = (volatile uint8_t*)ms_ptr2; 
			    else if(counter1 == 3)
				        ms_pointr = (volatile uint8_t*)ms_ptr3;
			    else if(counter1 == 4)
				        ms_pointr = (volatile uint8_t*)ms_ptr4;
			    for(counter2 = 0; counter2 < 4096; counter2++)
				        *(ms_pointr + counter2) = 0x00; 
	    }
}

/* Behaves almost as memsetbyte but it zeroes a page */
void zero_usbms_mem_4(volatile uint8_t* ms_buffer_ptr)
{
	    uint32_t counter;
	    volatile uint8_t* ms_pointr = ms_buffer_ptr;
	    for(counter = 0; counter < 4096; counter++)
		    *(ms_pointr + counter) = 0x00;
}

/* Behaves almost as zero_usbms_mem_4 but it omits the
 * volatile keyword.
 */
void zero_usbms_mem_5(uint8_t* ms_buffer_ptr)
{
	    uint32_t counter;
	    uint8_t* ms_pointr = ms_buffer_ptr;
	    for(counter = 0; counter < 4096; counter++)
		        *(ms_pointr + counter) = 0x00;
}

/* This function behaves almost the same as memsetint
 * but it zeroes a page.
 */ 
void zero_usbms_mem_6(uint32_t* ms_buffer_ptr)
{
	    uint32_t counter;
	    uint32_t* ms_pointr = ms_buffer_ptr;
				    for(counter = 0; counter < 1024; counter++)
					    *(ms_pointr + counter) = 0x00000000;
}

/* Zeroes a page with the \0 notation. */
void zero_usbms_mem_7(int8_t* ms_buffer_ptr)
{
	uint32_t counter;
	    int8_t* ms_pointr = ms_buffer_ptr;
	    for(counter = 0; counter < 4096; counter++)
		        *(ms_pointr + counter) = '\0';
}




/********************************************************************************************* SHOW MEMMAP BYTE ********************************************************************************************* */

/* Shows a memory map that the BIOS has provided to the kernel */
void show_memmap(uint8_t explaination)
{

		uint8_t* memorymap2 = (uint8_t*) memorymap_addr;
		if (explaination == 1) {
		        print("\n\n");
		        print("Memory map. Shows buffers about 6 different memory regions. In this order:\n");     
		        print("\nSize of this buffer (minus 4, the info about the buffer itself) in bytes,\nMemory field address start low bits (in bytes),\nMemory field address start high bits (in bytes)");
         		print(",\nMemory field lenght low bits (in bytes),\n");
		        print("Memory field lenght high bits (in bytes),\n");
		        print("Type: (1=available, 2=reserved, 3=ACPI recl. mem, 4=ACPI nvs mem, 5=bad mem)");
		        
		        print("\n\nSo in the first field, we have an available memory field\nstarting from 0 up to 654336 (0x9FC00) bytes (length)\n");
		        print("Some fields could be bugged. Like in Qemu, for example.\nThe last field could be bugged, there.");
		}

	    print("\n\n");
		uint8_t mi = 0;
		for (int b = 1; b < 7; b++) {
		        for (int a = 1; a < 7; a++) {

				        print("0x");

				        print_hex_byte(memorymap2[mi + 3]);
				        
				        print_hex_byte(memorymap2[mi + 2]);
					        
				        print_hex_byte(memorymap2[mi + 1]);
					        
				        print_hex_byte(memorymap2[mi]);
					        mi += 4;
				        if (a < 6)
				        print(", ");


		        }
		        print("\n");

		}	

}  

/* ********************************************************************************************* SET AND COPY REGIONS *************************************************************************************************************** */



/* This function fills a page with a certain value */
void set_mem(uint32_t * pointer_ptr, uint32_t start, uint32_t blocks_times_4096, uint32_t value)
{
        for(uint32_t i = start; i < blocks_times_4096; i++)
                *(pointer_ptr + i) = value;
}

/* Fills a memory map with zeroes. This map is then used 
 * in the kernel heap.
 */
void init_memoryhandler()
{
        uint32_t * Os_memmapptr = Os_memmap;
        if((uint32_t)Os_memmapptr < 1000001) {
                print("\nentered a forbidden memory address. Operating system halting....");
                asm("hlt");
        }
        memsetint(Os_memmapptr, 0x00000000, 1900);

        mem_bt_temp = 0;
        mbt_freeflag = 0;
}

//*******************************************************************************************************'ALLOC AND FREE MEM ********************************************************************************

/* Allocates a page of memory at the first free spot */
uint32_t alloc_free_mem()
{
        uint32_t a = check_first_free_memory();
        set_bit(a);
        uint32_t addr = get_addr_from_bit(a);
        return addr;
}


/* This frees a page and zeroes all values in that page */
void free_mem(void * address)
{
        uint32_t a = get_bit_from_addr((uint32_t*)address);
        zero_bit(a);
        uint8_t * tempaddress = (uint8_t*) address;
        setmembyte(tempaddress, 0, 4096, 0x00);
}



/* Allocates some pages and stores the allocated addresses in an array. */
void alloc_free_blocks(uint32_t b)
{
        for (uint32_t i = 0; i < b; i++) {
                uint32_t a = check_first_free_memory();
                set_bit(a);
                uint32_t addr = get_addr_from_bit(a);
                mem_block[i] = addr;
        }
}
//************************************************************************************************************************************************************************************************************

/* This function sets a bit to 1, in the kernel heap structure. 
 * A bit corresponds to a page that is 4096 bits long.
 * bitss is notated in pages.
 */
void set_bit(uint32_t bitss)
{
        uint32_t a = bitss / 32;
        Os_memmap[a] |= (1 << (bitss%32));
}

/* Sets a bit to 0, in the kernel heap structure. 
 * A bit corresponds to a page that is 4096 bits long.
 * bitsss is notated in pages.
 */
void zero_bit(uint32_t bitsss)
{
        uint32_t b = bitsss / 32;
        Os_memmap[b] &= (~(1 << (bitsss % 32)));

}

/* Checks if a bit in the heap array is set to 1 or zero. */
uint32_t check_bit(uint32_t bitssss)
{
        uint32_t temp4;
        uint32_t c = bitssss / 32;

         /* bit steps an amount of steps to the right.*/
        temp4 = Os_memmap[c] >> (bitssss % 32);

        /* highlight only the rightmost bit */
        temp4 = temp4 & 0x00000001; 
        return temp4;
}

/* Checks to see which bit that corresponds to a page that
 * the address is linked to.
 */
uint32_t get_bit_from_addr(uint32_t * addr)
{
        uint32_t bits;
        uint32_t addrs = (uint32_t)addr;
        bits = (addrs - KERNEL_HEAP_START)/4096; 
        return bits;
}

uint32_t get_addr_from_bit(uint32_t bitsssss)
{
        uint32_t address = bitsssss*4096 + KERNEL_HEAP_START; 
        return address;
}

/* checks for the first free page after the start of the entire heap */
uint32_t check_first_free_memory()
{
        uint32_t memloop = 0;
        uint32_t temp2 = 1;

		for(;(memloop <= 60800) && (temp2 == 1);memloop++) {				

				temp2 = check_bit(memloop);

		}

	    return memloop - 1;
}
/* Checks for a free page after a specified start location */
uint32_t check_free_memory(uint32_t start)
{
        uint32_t memloop2 = start;
        uint32_t temp3;
        temp3 = 1;

        /* example case: 1900*32 */ 
        for(;(memloop2 < 60800) && (temp3 == 1); memloop2++) {
	            temp3 = check_bit(memloop2);
	    }
        return memloop2 - 1;
}

/* Returns a memory address that is the start of
 * a free page of memory.
 */
uint32_t kmem_4k_allocate() 
{
        uint32_t mem_address;
        uint32_t temp;

        temp = check_first_free_memory();
        if (temp < 60800) {
		        set_bit(temp);
		        mem_address = KERNEL_HEAP_START + (temp*4096);
		        return mem_address;
        }

        else  {
		        print("\n\nWarning: reached the end of the Os memory map array.");
		        print("\nSetting memory address to the reserved heap starting area.");
		        return (uint32_t)KERNEL_HEAP_START;
        }
}

/* Frees a page of memory and puts a zero to all the values in it. */
void free_mem_uint(uint32_t address)
{
        uint32_t a = get_bit_from_addr((uint32_t*)address);
        zero_bit(a);
        uint8_t * tempaddress = (uint8_t*) address;
        memsetbyte(tempaddress, 0x00, 4096);
}

/* This function seems to be unfinished. */
uint32_t kmem_less4k_allocate(uint32_t size) 
{
        uint32_t address;
        /* find a 4k aligned spot */
        uint32_t mem = kmem_4k_allocate(); 
        address = mem + size;

        /* cannot be bigger than 4096 in total this time. might change it later */
        if (((address - mem) + size) >= 4095) 
	        return 0;
        else
	        return address;

} 
