#ifndef MEM_H
#define MEM_H


#define KERNEL_HEAP_START                                              0x300000


uint32_t max_mem_address;
uint32_t memorymap_addr;
uint32_t kmem_4k_allocate();


void memset(void *dest, uint8_t val, uint32_t count);
void memset2(uint8_t* dest, uint8_t* val, uint32_t count);
int charlength(char * testchar);
void memsetbyte(uint8_t *dest, uint8_t val, uint32_t count);
void memsetint(uint32_t *dest, uint32_t val, uint32_t count);
void memsetshort(uint16_t *dest, uint16_t val, uint32_t count);


void free_mem_uint(uint32_t address);
uint32_t kmem_less4k_allocate(uint32_t size);



void alloc_free_blocks(uint32_t b);


uint32_t mem_block[128];
uint8_t floppy_kb_buffer[8];
int id;


uint32_t hextemp2;
uint32_t hexbytetemp2;
uint32_t read_dword_2(const uint32_t base_addr, const uint32_t offset);

uint32_t read_dword(const uint32_t base_addr, const uint32_t offset);
uint32_t* read_dword_addr(const uint32_t base, const uint32_t offset);
void write_dword(const uint32_t base, const uint32_t offset, 
                 const uint32_t value);


uint32_t read_mem_address(uint32_t* value, uint8_t counter1);
uint32_t get_bit_from_addr(uint32_t * addr);
uint32_t get_addr_from_bit(uint32_t bitsssss);


void init_memoryhandler();
uint32_t alloc_free_mem();
void free_mem(void * address);

uint32_t check_first_free_memory();
uint32_t check_free_memory(uint32_t start);
uint32_t check_bit(uint32_t bitssss);

/* size: 1900 * 32 * 4096 = 249036800 bytes. that's the kernel heap. */
uint32_t Os_memmap[1900]; 
uint16_t* mmap_size;
uint8_t* mmap_free;



void setmembyte(uint8_t * pointer_pt, uint32_t start_offset, \
                uint32_t blocks_times_4096, uint8_t value);
void set_mem(uint32_t * pointer_ptr, uint32_t start, uint32_t blocks_times_32, \
             uint32_t value);

void show_memmap(uint8_t explaination);


uint32_t program1[32];
void set_bit(uint32_t bitss);
void zero_bit(uint32_t bitsss);


uint32_t * Os_memmapptr;
void init_4k_page(uint32_t temp);
uint32_t mem_bt_temp;
int mbt_freeflag;

void zero_usbms_mem_1 (volatile uint8_t* ms_buffer_ptr0, 
                       volatile uint8_t* ms_buffer_ptr1, 
                       volatile uint8_t* ms_buffer_ptr2, 
                       volatile uint8_t* ms_buffer_ptr3, 
                       volatile uint8_t* ms_buffer_ptr4);

void zero_usbms_mem_2 (volatile uint32_t ms_buffer_ptr1, 
                       volatile uint32_t ms_buffer_ptr2, 
                       volatile uint32_t ms_buffer_ptr3, 
                       volatile uint32_t ms_buffer_ptr4);

void zero_usbms_mem_3 (volatile uint32_t ms_buffer_ptr0, 
                       volatile uint8_t* ms_buffer_ptr1, 
                       volatile uint32_t ms_buffer_ptr2, 
                       volatile uint32_t ms_buffer_ptr3,
                       volatile uint32_t ms_buffer_ptr4);


void zero_usbms_mem_4(volatile uint8_t* ms_buffer_ptr);
void zero_usbms_mem_5(uint8_t* ms_buffer_ptr);
void zero_usbms_mem_6(uint32_t* ms_buffer_ptr);
void zero_usbms_mem_7(int8_t* ms_buffer_ptr);



#endif
