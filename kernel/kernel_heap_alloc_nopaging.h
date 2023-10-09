#ifndef KERNEL_HEAP_ALLOC_NOPAGING_H
#define KERNEL_HEAP_ALLOC_NOPAGING_H

#define KERNEL_HEAP_NO_PAGING_START                           0x500000 

uint32_t OS_memory_bit_pos;
uint8_t Os_memmap_free_info[262144];
uint32_t* mmap_free_nopaging_ptr;
uint16_t Os_memmap_size_info[262144];
uint32_t mem_array_address_start;

uint16_t* mmap_size_nopaging_ptr;
int16_t scan_address(uint16_t size, uint32_t parent_pos);
void kernel_free(void* to_be_freed);
uint32_t kmem_allocate(uint16_t bt_size);
uint8_t check_if_page_taken(uint32_t p_number);
uint8_t check_if_page_freed(uint32_t page); 
void init_memallocarrays_nopaging();


#define kfree(x)                                 kernel_free((void*)x)
 

#endif
