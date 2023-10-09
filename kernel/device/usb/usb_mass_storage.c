/* This is the kernel's USB Mass Storage driver. */ 

#include "../../types.h"
#include "../../graphics/screen.h"
#include "usb.h"
#include "usb_ehci.h"
#include "usb_xhci.h"
#include "usb_mass_storage.h"
#include "../../mem.h"
#include "../../timer.h"
#include "../../isr.h"
#include "../../irq.h"


/* Initializes some structure pointers and an array. */
void init_scsi_structs()
{
	    cbw_16_1 = (struct cbw_read_16*)kmem_4k_allocate();
	    inq = (struct inquiry*)kmem_4k_allocate();
	    r_s = (struct request_sense*)kmem_4k_allocate();
	    read_capacity = (struct read_capacity_10*)kmem_4k_allocate();
	    r_capacity_16 = (struct read_capacity_16*)kmem_4k_allocate();
	    cbw_6_1 = (struct cbw_read_6*)kmem_4k_allocate();	
	    cbw_10_1 = (struct cbw_read_10*)kmem_4k_allocate();
	    cbw_12_1 = (struct cbw_read_12*)kmem_4k_allocate();
	    tur = (struct test_unit_ready*)kmem_4k_allocate();
	    cbw_w_10_1 = (struct cbw_write_10*)kmem_4k_allocate();
        idtf = (struct identify*)kmem_4k_allocate();
        usbms_extra_data[0] = 0x00000000;
        usbms_extra_data[1] = 0x00000000;
}

/* Sets the values correctly in a structure so that it later can
 * perform a Read 16 command.
 */
struct cbw_read_16* set_cbw_16_data(uint8_t lba_low1, uint8_t lba_low2)
{
    struct cbw_read_16* cbw_16_2 = cbw_16_1;
    uint8_t* pointr = (uint8_t*)cbw_16_2;
		    for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
				    *(pointr + counter2) = 0x00;
	    cbw_16_1->signature_low = 0x55;
	    cbw_16_1->signature_low2 = 0x53;
	    cbw_16_1->signature_low3 = 0x42;
	    cbw_16_1->signature_high = 0x43;
	    cbw_16_1->tag_low = 0x0f;
	    cbw_16_1->tag_low2 = 0x0b;
	    cbw_16_1->tag_low3 = 0x0a;
	    cbw_16_1->tag_high = 0x0a;
	    cbw_16_1->data_transfer_length_low = 0x00;
	    cbw_16_1->data_transfer_length_low2 = 0x02;
	    cbw_16_1->data_transfer_length_low3 = 0x00;
	    cbw_16_1->data_transfer_length_high = 0x00;
	    cbw_16_1->flags = 0x80;
	    cbw_16_1->lun = 0x00;
	    cbw_16_1->cbwcw_length = 0x10;
	    cbw_16_1->operation_code = 0x88;
	    cbw_16_1->meta_data = 0x00; 
	    cbw_16_1->lba_high_dword_low = 0x00;
	    cbw_16_1->lba_high_dword_low2 = 0x00;
	    cbw_16_1->lba_high_dword_low3 = 0x00;
	    cbw_16_1->lba_high_dword_high = 0x00;
	    cbw_16_1->lba_low_dword_low = 0x00;
	    cbw_16_1->lba_low_dword_low2 = 0x00;
	    cbw_16_1->lba_low_dword_low3 = 0x00;
	    cbw_16_1->lba_low_dword_high = 0x00;
	    cbw_16_1->group_number = 0x00;
	    cbw_16_1->transfer_length_high_word_low = 0x00;
	    cbw_16_1->transfer_length_high_word_high = 0x00;
	    cbw_16_1->transfer_length_low_word_low = 0x00;
	    cbw_16_1->transfer_length_low_word_high = 0x01; 
	    cbw_16_1->control_byte = 0x00;
	    return cbw_16_1;
}

/* Sets the values correctly in a structure so that it later can
 * perform an Inqury command.
 */
struct inquiry* set_inquiry()
{
	    struct inquiry* inq2 = inq;
	    struct inquiry* inq3 = inq;
	    uint8_t* pointr = (uint8_t*)inq3;
			    for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
					    *(pointr + counter2) = 0x00;
	    inq2->signature_low = (volatile uint8_t)0x55;
	    inq2->signature_mid_low = (volatile uint8_t)0x53;
	    inq2->signature_low_high = (volatile uint8_t)0x42;
	    inq2->signature_mid_high = (volatile uint8_t)0x43;

	    inq2->tag_low = (volatile uint8_t)0xbb;
	    inq2->tag_low1 = (volatile uint8_t)0xbb;
	    inq2->tag_low2 = (volatile uint8_t)0xcc;
	    inq2->tag_high = (volatile uint8_t)0xdd;
	    inq2->data_transfer_length_low = (volatile uint8_t)0x24;
	    inq2->data_transfer_length_low1 = (volatile uint8_t)0x00;
	    inq2->data_transfer_length_low2 = (volatile uint8_t)0x00;
	    inq2->data_transfer_length_high = (volatile uint8_t)0x00;
	    inq2->flags = (volatile uint8_t)0x80;
	    inq2->lun = (volatile uint8_t)0x00;
	    inq2->cbwcw_length = (volatile uint8_t)0x06;
	    inq2->operation_code = (volatile uint8_t)0x12;
	    inq2->meta_data = (volatile uint8_t)0x00;
	    inq2->page_code = (volatile uint8_t)0x00;
	    inq2->allocation_length_low = (volatile uint8_t)0x00;
	    inq2->allocation_length_high = (volatile uint8_t)0x24;
	    inq2->control = (volatile uint8_t)0x00;
	    inq2->zero_1 = (volatile uint8_t)0x00;
	    inq2->zero_2 = (volatile uint8_t)0x00;
	    inq2->zero_3 = (volatile uint8_t)0x00;
	    inq2->zero_4 = (volatile uint8_t)0x00;
	    inq2->zero_5 = (volatile uint8_t)0x00;
	    inq2->zero_6 = (volatile uint8_t)0x00;
	    inq2->zero_7 = (volatile uint8_t)0x00;
	    inq2->zero_8 = (volatile uint8_t)0x00;
	    inq2->zero_9 = (volatile uint8_t)0x00;
	    inq2->zero_10 = (volatile uint8_t)0x00;
	    
	    return inq2;
}

/* Sets the values correctly in a structure so that it later can
 * perform a Request Sense command.
 */
struct request_sense* set_rs()
{
	    struct request_sense* rs2 = r_s;
	    uint8_t* pointr = (uint8_t*)rs2;
			    for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
					    *(pointr + counter2) = 0x00;
	    r_s->signature_low = 0x55;
	    r_s->signature_low2 = 0x53;
	    r_s->signature_low3 = 0x42;
	    r_s->signature_high = 0x43;

	    r_s->tag_low = 0xcc;
	    r_s->tag_low2 = 0xbb;
	    r_s->tag_low3 = 0xcc;
	    r_s->tag_high = 0xdd;
	    r_s->data_transfer_length_low = 0x12;
	    r_s->data_transfer_length_low2 = 0x00;
	    r_s->data_transfer_length_low3 = 0x00;
	    r_s->data_transfer_length_high = 0x00;
	    r_s->flags = 0x80;
	    r_s->lun = 0x00;
	    r_s->cbwcw_length = 0x06;
	    r_s->operation_code = 0x03;
	    r_s->desc = 0x00;
	    r_s->reserved1 = 0x00;
	    r_s->reserved2 = 0x00;
	    r_s->allocation_length = 0x12;
	    r_s->control = 0x00;
	    r_s->zero_1 = 0x00;
	    r_s->zero_2 = 0x00;
	    r_s->zero_3 = 0x00;
	    r_s->zero_4 = 0x00;
	    r_s->zero_5 = 0x00;
	    r_s->zero_6 = 0x00;
	    r_s->zero_7 = 0x00;
	    r_s->zero_8 = 0x00;
	    r_s->zero_9 = 0x00;
	    r_s->zero_10 = 0x00;
	    
	    return r_s;
}

/* Sets the values correctly in a structure so that it later can
 * perform a Read Capacity 10 command.
 */
struct read_capacity_10* set_read_capacity()
{
	    struct read_capacity_10* read_capacity2 = read_capacity;
	    uint8_t* pointr = (uint8_t*)read_capacity2;
			    for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
			            *(pointr + counter2) = 0x00;
	    read_capacity->signature_low = 0x55;
	    read_capacity->signature_low2 = 0x53;
	    read_capacity->signature_low3 = 0x42;
	    read_capacity->signature_high = 0x43;

	    read_capacity->tag_low = 0xdd;
	    read_capacity->tag_low2 = 0xbb;
	    read_capacity->tag_low3 = 0xcc;
	    read_capacity->tag_high = 0xdd;
	    read_capacity->data_transfer_length_low = 0x08;
	    read_capacity->data_transfer_length_low2 = 0x00;
	    read_capacity->data_transfer_length_low3 = 0x00;
	    read_capacity->data_transfer_length_high = 0x00;
	    read_capacity->flags = 0x80;
	    read_capacity->lun = 0x00;
	    read_capacity->cbwcw_length = 0x0a;
	    read_capacity->operation_code = 0x25;
	    read_capacity->reserved = 0x00; 
	    read_capacity->lba_low = 0x00;
	    read_capacity->lba_low_1 = 0x00;
	    read_capacity->lba_low_2 = 0x00;
	    read_capacity->lba_high = 0x00;
	    read_capacity->reserved2 = 0x00;
	    read_capacity->reserved3 = 0x00;
	    read_capacity->pmi = 0x00;
	    read_capacity->control = 0x00;
	    read_capacity->zero_1 = 0x00;
	    read_capacity->zero_2 = 0x00;
	    read_capacity->zero_3 = 0x00;
	    read_capacity->zero_4 = 0x00;
	    read_capacity->zero_5 = 0x00;
	    read_capacity->zero_6 = 0x00;
	    
	    return read_capacity;
}

/* Sets the values correctly in a structure so that it later can
 * perform a Read Capacity 16 command.
 */
struct read_capacity_16* set_read_capacity_16()
{
	    struct read_capacity_16* read_capacity2 = r_capacity_16;
	    uint8_t* pointr = (uint8_t*)read_capacity2;
			    for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
					    *(pointr + counter2) = 0x00;
	    r_capacity_16->signature_low = 0x55;
	    r_capacity_16->signature_low2 = 0x53;
	    r_capacity_16->signature_low3 = 0x42;
	    r_capacity_16->signature_high = 0x43;
	    r_capacity_16->tag_low = 0xdd;
	    r_capacity_16->tag_low2 = 0xbb;
	    r_capacity_16->tag_low3 = 0xcc;
	    r_capacity_16->tag_high = 0xdd;
	    r_capacity_16->data_transfer_length_low = 0x08;
	    r_capacity_16->data_transfer_length_low2 = 0x00;
	    r_capacity_16->data_transfer_length_low3 = 0x00;
	    r_capacity_16->data_transfer_length_high = 0x00;
	    r_capacity_16->flags = 0x80;
	    r_capacity_16->lun = 0x00;
	    r_capacity_16->cbwcw_length = 0x10;
	    r_capacity_16->operation_code = 0x9e;
	    r_capacity_16->service_action = 0x10;
	    r_capacity_16->lba_low = 0x00;
	    r_capacity_16->lba_low_1 = 0x00;
	    r_capacity_16->lba_low_2 = 0x00;
	    r_capacity_16->lba_low_3 = 0x00;
	    r_capacity_16->lba_low_4 = 0x00;
	    r_capacity_16->lba_low_5 = 0x00;
	    r_capacity_16->lba_low_6 = 0x00;
	    r_capacity_16->lba_high = 0x00;
	    r_capacity_16->allocation_length_low = 0x00;
	    r_capacity_16->allocation_length_low1 = 0x00;
	    r_capacity_16->allocation_length_low2 = 0x00;
	    r_capacity_16->allocation_length_high = 0x08;
	    r_capacity_16->pmi = 0x00;

	    return r_capacity_16;
}

/* Sets the values correctly in a structure so that it later can
 * perform a Read 6 command.
 */
struct cbw_read_6* set_cbw_6_data(volatile uint8_t lba_low_low, volatile uint8_t lba_low_high)
{
        struct cbw_read_6* cbw_6_2 = cbw_6_1;
        uint8_t* pointr = (uint8_t*)cbw_6_2;
		        for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
				        *(pointr + counter2) = 0x00;
	    cbw_6_1->signature_low = 0x55;
	    cbw_6_1->signature_low2 = 0x53;
	    cbw_6_1->signature_low3 = 0x42;
	    cbw_6_1->signature_high = 0x43;
	    cbw_6_1->tag_low = 0x0a;
	    cbw_6_1->tag_low2 = 0x01;
	    cbw_6_1->tag_low3 = 0x02;
	    cbw_6_1->tag_high = 0x0d;
	    cbw_6_1->data_transfer_length_low = 0x00;
	    cbw_6_1->data_transfer_length_low2 = 0x02;
	    cbw_6_1->data_transfer_length_low3 = 0x00;
	    cbw_6_1->data_transfer_length_high = 0x00;
	    cbw_6_1->flags = 0x80;
	    cbw_6_1->lun = 0x00;
	    cbw_6_1->cbwcw_length = 0x06;
	    cbw_6_1->operation_code = 0x08;
	    cbw_6_1->reserved = 0x00; 
	    cbw_6_1->lba_high = lba_low_high;
	    cbw_6_1->lba_low = lba_low_low;
	    cbw_6_1->transfer_length = 0x01;
	    cbw_6_1->control_byte = 0x00;
	    cbw_6_1->zero_1 = 0x00;
	    cbw_6_1->zero_2 = 0x00;
	    cbw_6_1->zero_3 = 0x00;
	    cbw_6_1->zero_4 = 0x00;
	    cbw_6_1->zero_5 = 0x00;
	    cbw_6_1->zero_6 = 0x00;
	    cbw_6_1->zero_7 = 0x00;
	    cbw_6_1->zero_8 = 0x00;
	    cbw_6_1->zero_9 = 0x00;
	    cbw_6_1->zero_10 = 0x00;
	    return cbw_6_1;	
}

/* Sets the values correctly in a structure so that it later can
 * perform a Read 10 command.
 */
struct cbw_read_10* set_cbw_10_data(volatile uint8_t lba_low_low, volatile uint8_t lba_low_high, volatile uint32_t length)
{
        /* length in bytes. for ehci as of now */
        if((length > 20000) && (length < 512))
                return 0;
        if ((length % 512) != 0)
                return 0;

        volatile uint8_t cbwcw_length_read10 = (volatile uint8_t)(length / (volatile uint32_t)512);
        volatile uint8_t cbw_length_low1 = (volatile uint8_t)(length/((volatile uint32_t)255));
        struct cbw_read_10* cbw_10_2 = cbw_10_1;
        uint8_t* pointr = (uint8_t*)cbw_10_2;
		        for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
				        *(pointr + counter2) = 0x00;
	    cbw_10_1->signature_low = 0x55;
	    cbw_10_1->signature_low2 = 0x53;
	    cbw_10_1->signature_low3 = 0x42;
	    cbw_10_1->signature_high = 0x43;
	    cbw_10_1->tag_low = 0xbb;
	    cbw_10_1->tag_low2 = 0xcc;
	    cbw_10_1->tag_low3 = 0xbb;
	    cbw_10_1->tag_high = 0xdd;
	    cbw_10_1->data_transfer_length_low = 0x00;
	    cbw_10_1->data_transfer_length_low2 = cbw_length_low1;
	    cbw_10_1->data_transfer_length_low3 = 0x00;
	    cbw_10_1->data_transfer_length_high = 0x00;
	    cbw_10_1->flags = 0x80;
	    cbw_10_1->lun = 0x00;
	    cbw_10_1->cbwcw_length = 0x0a;
	    cbw_10_1->operation_code = 0x28;
	    cbw_10_1->meta_data = 0x00; 
	    cbw_10_1->lba_high_word_low = 0x00;
	    cbw_10_1->lba_high_word_high = 0x00;
	    cbw_10_1->lba_low_word_low = lba_low_high; 
	    cbw_10_1->lba_low_word_high = lba_low_low; 
	    cbw_10_1->group_number = 0x00;
	    cbw_10_1->transfer_length_low_byte= 0x00;
	    cbw_10_1->transfer_length_high_byte = cbwcw_length_read10;
	    cbw_10_1->control_byte = 0x00;
	    cbw_10_1->zero_1 = 0x00;
	    cbw_10_1->zero_2 = 0x00;
	    cbw_10_1->zero_3 = 0x00;
	    cbw_10_1->zero_4 = 0x00;
	    cbw_10_1->zero_5 = 0x00;
	    cbw_10_1->zero_6 = 0x00;
	    return cbw_10_1;	
}

/* Sets the values correctly in a structure so that it later can
 * perform a Write 10 command.
 */
struct cbw_write_10* set_cbw_w_10_data(volatile uint8_t lba_low_low, volatile uint8_t lba_low_high)
{
		struct cbw_write_10* cbw_w_10_2 = cbw_w_10_1;
        uint8_t* pointr = (uint8_t*)cbw_w_10_2;
		for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
				*(pointr + counter2) = 0x00;

	    cbw_w_10_1->signature_low = 0x55;
	    cbw_w_10_1->signature_low2 = 0x53;
	    cbw_w_10_1->signature_low3 = 0x42;
	    cbw_w_10_1->signature_high = 0x43;
	    cbw_w_10_1->tag_low = 0x01;
	    cbw_w_10_1->tag_low2 = 0x02;
	    cbw_w_10_1->tag_low3 = 0x03;
	    cbw_w_10_1->tag_high = 0x06;
	    cbw_w_10_1->data_transfer_length_low = 0x00;
	    cbw_w_10_1->data_transfer_length_low2 = 0x02;
	    cbw_w_10_1->data_transfer_length_low3 = 0x00;
	    cbw_w_10_1->data_transfer_length_high = 0x00;
	    cbw_w_10_1->flags = 0x00;
	    cbw_w_10_1->lun = 0x00;
	    cbw_w_10_1->cbwcw_length = 0x0a;
	    cbw_w_10_1->operation_code = 0x2a;
	    cbw_w_10_1->meta_data = 0x00; 
	    cbw_w_10_1->lba_high_word_low = 0x00;
	    cbw_w_10_1->lba_high_word_high = 0x00;
	    cbw_w_10_1->lba_low_word_low = lba_low_high; 
	    cbw_w_10_1->lba_low_word_high = lba_low_low; 
	    cbw_w_10_1->group_number = 0x00;
	    cbw_w_10_1->transfer_length_low_byte= 0x00;
	    cbw_w_10_1->transfer_length_high_byte = 0x01;
	    cbw_w_10_1->control_byte = 0x00;
	    cbw_w_10_1->zero_1 = 0x00;
	    cbw_w_10_1->zero_2 = 0x00;
	    cbw_w_10_1->zero_3 = 0x00;
	    cbw_w_10_1->zero_4 = 0x00;
	    cbw_w_10_1->zero_5 = 0x00;
	    cbw_w_10_1->zero_6 = 0x00;
	    return cbw_w_10_1;
}

/* Sets the values correctly in a structure so that it later can
 * perform a Read 12 command.
 */
struct cbw_read_12* set_cbw_12_data(volatile uint8_t lba_low_low, volatile uint8_t lba_low_high)
{
        struct cbw_read_12* cbw_12_2 = cbw_12_1;
        uint8_t* pointr = (uint8_t*)cbw_12_2;
		for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
				*(pointr + counter2) = 0x00;
	    cbw_12_1->signature_low = 0x55;
	    cbw_12_1->signature_low2 = 0x53;
	    cbw_12_1->signature_low3 = 0x42;
	    cbw_12_1->signature_high = 0x43;
	    cbw_12_1->tag_low = 0x04;
	    cbw_12_1->tag_low2 = 0x03;
	    cbw_12_1->tag_low3 = 0x02;
	    cbw_12_1->tag_high = 0x01;
	    cbw_12_1->data_transfer_length_low = 0x00;
	    cbw_12_1->data_transfer_length_low2 = 0x02;
	    cbw_12_1->data_transfer_length_low3 = 0x00;
	    cbw_12_1->data_transfer_length_high = 0x00;
	    cbw_12_1->flags = 0x80;
	    cbw_12_1->lun = 0x00;
	    cbw_12_1->cbwcw_length = 0xc;
	    cbw_12_1->operation_code = 0xa8;
	    cbw_12_1->meta_data = 0x00; 
	    cbw_12_1->lba_high_word_low = 0x00;
	    cbw_12_1->lba_high_word_high = 0x00;
	    cbw_12_1->lba_low_word_low = lba_low_high; 
	    cbw_12_1->lba_low_word_high = lba_low_low;
	    cbw_12_1->transfer_length_high_high = 0x00;
	    cbw_12_1->transfer_length_high_low = 0x00;
	    cbw_12_1->transfer_length_low_high = 0x00;
	    cbw_12_1->transfer_length_low_low = 0x01;
	    cbw_12_1->group_number = 0x00;
	    cbw_12_1->control_byte = 0x00;
	    cbw_12_1->zero_1 = 0x00;
	    cbw_12_1->zero_2 = 0x00;
	    cbw_12_1->zero_3 = 0x00;
	    cbw_12_1->zero_4 = 0x00;
	    return cbw_12_1;	
}

/* Sets the values correctly in a structure so that it later can
 * perform a Test Unit Ready command.
 */
struct test_unit_ready* set_tur()
{
	    struct test_unit_ready* tur2 = tur;
	    uint8_t* pointr = (uint8_t*)tur2;
			    for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
					    *(pointr + counter2) = 0x00;
	    tur->signature_low = 0x55;
	    tur->signature_low2 = 0x53;
	    tur->signature_low3 = 0x42;
	    tur->signature_high = 0x43;

	    tur->tag_low = 0xdd;
	    tur->tag_low2 = 0xcc;
	    tur->tag_low3 = 0xbb;
	    tur->tag_high = 0xaa;
	    tur->data_transfer_length_low = 0x00;
	    tur->data_transfer_length_low2 = 0x00;
	    tur->data_transfer_length_low3 = 0x00;
	    tur->data_transfer_length_high = 0x00;
	    tur->flags = 0x80;
	    tur->lun = 0x00;
	    tur->cbwcw_length = 0x06;
	    tur->operation_code = 0x00;
	    tur->reserved1 = 0x00;
	    tur->reserved2 = 0x00;
	    tur->reserved3 = 0x00;
	    tur->reserved4 = 0x00;
	    tur->control = 0x00;
	    tur->zero_1 = 0x00;
	    tur->zero_2 = 0x00;
	    tur->zero_3 = 0x00;
	    tur->zero_4 = 0x00;
	    tur->zero_5 = 0x00;
	    tur->zero_6 = 0x00;
	    tur->zero_7 = 0x00;
	    tur->zero_8 = 0x00;
	    tur->zero_9 = 0x00;
	    tur->zero_10 = 0x00;
    
	    return tur;
}

/* Sets the values correctly in a structure so that it later can
 * perform a Identify command.
 */
struct identify* set_identify()
{
	    struct identify* idtf2 = idtf;
	    uint8_t* pointr = (uint8_t*)idtf2;
			    for(uint32_t counter2 = 0; counter2 < 4096; counter2++)
					    *(pointr + counter2) = 0x00;
	    idtf->signature_low = 0x55;
	    idtf->signature_low2 = 0x53;
	    idtf->signature_low3 = 0x42;
	    idtf->signature_high = 0x43;

	    idtf->tag_low = 0xdd;
	    idtf->tag_low2 = 0xcc;
	    idtf->tag_low3 = 0xbb;
	    idtf->tag_high = 0xaa;
	    idtf->data_transfer_length_low = 0x00;
	    idtf->data_transfer_length_low2 = 0x00;
	    idtf->data_transfer_length_low3 = 0x00;
	    idtf->data_transfer_length_high = 0x00;
	    idtf->flags = 0x80;
	    idtf->lun = 0x00;
	    idtf->cbwcw_length = 0x01;
	    idtf->idtf_message = 0x80;
	    idtf->zero_pad1 = 0x00;
	    idtf->zero_pad2 = 0x00;
	    idtf->zero_pad3 = 0x00;
	    idtf->zero_pad4 = 0x00;
	    idtf->zero_pad5 = 0x00;
	    idtf->zero_1 = 0x00;
	    idtf->zero_2 = 0x00;
	    idtf->zero_3 = 0x00;
	    idtf->zero_4 = 0x00;
	    idtf->zero_5 = 0x00;
	    idtf->zero_6 = 0x00;
	    idtf->zero_7 = 0x00;
	    idtf->zero_8 = 0x00;
	    idtf->zero_9 = 0x00;
	    idtf->zero_10 = 0x00;
	    
	    return idtf;
}

/* Prepares a device for a bulk transfer. */
volatile uint8_t* prepare_usb_storage_bulk_transfer(uint8_t transfer_type, uint8_t direction, uint8_t cbwlun, uint32_t length, uint32_t* usd_extra_data)
{

        struct usb_storage_data* usd = (struct usb_storage_data*)kmem_4k_allocate();
        volatile uint8_t cbwcw_length_read10 = 0;
        volatile uint8_t cbw_length_low1 = 0;
        volatile uint8_t* d_area;
        volatile uint8_t cbw_length_low0 = (volatile uint8_t)(length % (uint32_t)256);
        memsetbyte((uint8_t*)usd, 0x00, 64);
	    usd->signature_low = 0x55;
	    usd->signature_low2 = 0x53;
	    usd->signature_low3 = 0x42;
	    usd->signature_high = 0x43;
	    usd->tag_low = 0xbb;
	    usd->tag_low2 = 0xcc;
	    usd->tag_low3 = 0xbb;
	    usd->tag_high = 0xdd;
        usd->data_transfer_length_low3 = 0x00;
        usd->data_transfer_length_high = 0x00;

        usd->flags = direction;
        usd->lun = cbwlun;
        usd->operation_code = transfer_type;
        volatile uint32_t lba;
        volatile uint8_t lba_low = 0;
        volatile uint8_t lba_high = 0;

        if(usd_extra_data[0] == USB_BULKSTORAGE_SCSI_LBA) {
                if(transfer_type == USB_BULKSTORAGE_SCSI_READ10)
                        usd->cbwcw_length = 0x0a;
                lba = (volatile uint32_t)usd_extra_data[1];
                lba_high = (volatile uint8_t)(((lba >> 8) & 0x000000ff));
		        lba_low = (volatile uint8_t)((lba & 0x000000ff));

                if (length > 0) {

                        if((length > 20000) && (length < 512))
                                return 0;
                        if ((length % 512) != 0)
                                return 0;

                        cbwcw_length_read10 = (volatile uint8_t)(length / 512);
                        cbw_length_low1 = (volatile uint8_t)(length/((volatile uint32_t)256));
                }

                usd->data_transfer_length_low = cbw_length_low0;
                usd->data_transfer_length_low2 = cbw_length_low1;
	            usd->extra_data[3] = lba_high;
	            usd->extra_data[4] = lba_low; 
	            usd->extra_data[7] = cbwcw_length_read10;


        }

        else if(usd_extra_data[0] == USB_BULKSTORAGE_SCSI_CTL) {

                if(transfer_type == USB_BULKSTORAGE_SCSI_INQURY) {

                        usd->data_transfer_length_low = 0x24;
                        usd->cbwcw_length = 0x06;
                        usd->extra_data[3] = 0x24;

                }

                else if(transfer_type == USB_BULKSTORAGE_SCSI_REQUEST_SENSE) {

                        usd->data_transfer_length_low = 0x12;
                        usd->cbwcw_length = 0x06;
                        usd->extra_data[3] = 0x12;

                }

                else if(transfer_type == USB_BULKSTORAGE_SCSI_READ_CAPACITY10) {

                        usd->data_transfer_length_low = 0x08;
                        usd->cbwcw_length = 0x0a;
                        usd->extra_data[3] = 0x00;

                }

        }
        if(hci_in_use == HCI_XHCI) {

                print("no relevant code for xhci yet, halting..");
                stop_xhci();
                asm("hlt");
        }
        else if(hci_in_use == HCI_EHCI) {

                d_area = ehci_transfer_bulkstorage_data(usd);
                free_mem_uint((uint32_t)usd);
                return d_area;
        }
                
        d_area = (volatile uint8_t*)0;
        free_mem_uint((uint32_t)usd);
        return d_area;

}

/* Extracts the lba low data from the block value. */
uint8_t select_lba_low(uint32_t l)
{
	    return (uint8_t)(l % 255);
}

/* Extracts the lba low high data from the block value. */
uint8_t select_lba_low2(uint32_t l)
{
	    uint8_t a = 0;

	    if (l > 255)
		        a = (uint8_t)(l/((uint32_t)255));

	    return a;
}

/* Extracts the lba high data from the block value. */
uint8_t select_lba_high(uint32_t l)
{
	    uint8_t a = 0;
	    while((l /= 256) > 0) 
		        a++;		
	    return a;
}

/* Prepares for a get max lun command. */
void usbms_get_max_lun(uint32_t usbms_slot_index)
{
        if(hci_in_use == 4) {
                print("\nhci in use is 4");
                xhci_get_max_lun(usbms_slot_index);
        }

        else {

                print("\nclear feature: unsupported hci function");
        }
}

/* Prepares for a mass storage reset command. */
void usbms_mass_storage_reset(uint32_t usbms_slot_index)
{
        if(hci_in_use == 4) {
                xhci_mass_storage_reset(usbms_slot_index);
        }
        else {
                print("\nclear feature: unsupported hci function");
        }
}

/* Attempts to make a bulk transfer using the XHC. */
uint32_t init_xhci_bulk_transfer(uint32_t slot_index, uint32_t transfer_type, uint32_t transfer_length, volatile uint8_t lba_low, volatile uint8_t lba_high) 
{

        /* transfer type: 1 = inquiry, 2 = request sense, 3 = read capacity, 4 = read 10 */
        struct xhci_usb_device_slot* slot = usb_slot_start;
        slot->slot_endpoint = slot_endpoint_start;

        uint32_t temp;
        volatile uint32_t* xdevice_slot;

        struct inquiry* inqry; struct request_sense* rqs; struct read_capacity_10* rc10; struct cbw_read_10* usbmsd_read_10; struct test_unit_ready* tstur; struct identify* idtf3;
        if (slot->configured != 1) {
                print("\nusb mass storage, xhci: device not configured"); 
                return 1;
        }

        ica_list = ica_list_start;
        ica_list->ica_address += 66;


        ica_list->ica_address -= 16;
        slot->slot_endpoint->trb_address += 4;
        volatile uint32_t* data_buffer_in = (slot->slot_endpoint->data_buffer);

        slot->slot_endpoint = slot->slot_endpoint->xhci_endpoint_next;
        slot->slot_endpoint->trb_address += 4;

        volatile uint32_t* transfer_out_trb = (volatile uint32_t*)(slot->slot_endpoint->transfer_trb);
        print("\nbulk out trb address in struct: ");
        print_hex((uint32_t)slot->slot_endpoint->transfer_trb);

        print("\nbulk out address field in struct: ");
        print_hex((uint32_t)(slot->slot_endpoint->trb_address));

        print("\nslot endpoint out trb address in struct: ");
        print_hex((uint32_t)(slot->slot_endpoint->transfer_trb));

        *(trb_address_bulk_out + 1) = (volatile uint32_t)0x00000000;
        *(trb_address_bulk_out + 2) = (volatile uint32_t)(0 | 31);

        *(trb_address_bulk_out + 3) = (volatile uint32_t)(0 << 0) | ((1 << 0) | (1 << 5) | (1 << 10));
        *(trb_address_bulk_in) = (volatile uint32_t)data_buffer_in;

        *(trb_address_bulk_in + 1) = (volatile uint32_t)0x00000000;
        *(trb_address_bulk_in + 2) = (volatile uint32_t)transfer_length;
        *(trb_address_bulk_in + 3) = (volatile uint32_t) (0 << 0) | ((1 << 0) | (1 << 5) | (1 << 10));
        /* inquiry */
        if (transfer_type == 1) {
                inqry = set_inquiry();
                print("\ninqry address: ");
                print_hex((uint32_t)inqry);

                *(trb_address_bulk_out) = (volatile uint32_t)inqry;
                print("\nvalue of the first inq: ");
                print_hex((uint32_t)(*((uint32_t*)inqry)));  
        }
        /* request sense */
        if (transfer_type == 2) {
                rqs = set_rs();
                *(transfer_out_trb) = (volatile uint32_t)rqs;
        }
        /* read capacity 10 */
        if (transfer_type == 3) {
                rc10 = set_read_capacity();
                *(transfer_out_trb) = (volatile uint32_t)rc10;           
        }
        /* inquiry */
        if (transfer_type == 4) { 
                usbmsd_read_10 = set_cbw_10_data(lba_low, lba_high, 512); 
                *(trb_address_bulk_out) = (volatile uint32_t)usbmsd_read_10;
        }
        /* read 10 */
        if (transfer_type == 5) { 
                tstur = set_tur(); 
                *(trb_address_bulk_out) = (volatile uint32_t)tstur;
                print("\nvalue of the first test unit ready struct: ");
                print_hex((uint32_t)(*((uint32_t*)tstur)));  
        }

        /* identify */
        if (transfer_type == 6) {
                idtf3 = set_identify(); 
                *(trb_address_bulk_out) = (volatile uint32_t)idtf3;
                print("\nvalue of the first test unit ready struct: ");
                print_hex((uint32_t)(*((uint32_t*)idtf3)));  
        }

        print("\ntesting what is actuallty in the address: ");

        print_hex((uint32_t)(*(trb_address_bulk_out)));
        volatile uint32_t* temp595 = (volatile uint32_t*)(*(trb_address_bulk_out));

        print("\nthe actual value of the first int in inq or test unit rdy strct:\n");
        print_hex((uint32_t)*temp595);


        ring_doorbell(4, 2);
        kernel_delay_100(5);
        handle_event(xhci_interrupt_status);
        kernel_delay_100(50);

        if((hci_er == hci_er_start) && (link_trb_flag == 0))
                temp = 0;
        else if((hci_er == hci_er_start) && (link_trb_flag == 1))
                temp = (62*4); 
        else
                temp = ((hci_er - 16 - hci_er_start)/4);

        xdevice_slot = (volatile uint32_t*)get_hci_event_ring(0, temp);
        if(  ( (((*(xdevice_slot + 2)) >> 24) & 0x000000ff)!= 1)  ) {
                print("\nbulk out cbw data stage failed");
                print("\nevent ring status: ");

                print_hex((uint32_t)(((*(xdevice_slot + 2)) >> 24) & 0x000000ff));
                return 1;
        }

        print("\nevent ring status: ");
        print_hex((uint32_t)(((*(xdevice_slot + 2)) >> 24) & 0x000000ff));

        (*(trb_address_bulk_out + 3)) &= (~(1 << 0));

        trb_address_bulk_out += 4;
        slot->slot_endpoint = slot_endpoint_start;

        ring_doorbell(4, 3);
        kernel_delay_100(5);

        handle_event(xhci_interrupt_status);
        kernel_delay_100(50);

        if((hci_er == hci_er_start) && (link_trb_flag == 0))
                temp = 0;
        else if((hci_er == hci_er_start) && (link_trb_flag == 1))
                temp = (62*4);
        else
                temp = ((hci_er - 16 - hci_er_start)/4);

        xdevice_slot = get_hci_event_ring(0, temp);
        if(  ( (((*(xdevice_slot + 2)) >> 24) & 0x000000ff)!= 1)  ) {

                print("\nbulk in data stage failed");
                return 1;

        }

        (*(trb_address_bulk_in + 3)) &= (~(1 << 0));
        trb_address_bulk_in += 4;

        print_bulk_data_buffer((uint8_t*)data_buffer_in, transfer_length);
        data_buffer_in += (volatile uint32_t)(((volatile uint32_t)(transfer_length/4)) + (volatile uint32_t)1);

        *(trb_address_bulk_in) = (volatile uint32_t)data_buffer_in;
        *(trb_address_bulk_in + 1) = 0x00000000;

        *(trb_address_bulk_in + 2) = 13;
        *(trb_address_bulk_in + 3) = (1 << 0) | (1 << 5) | (1 << 10);

        ring_doorbell(4, 3);
        kernel_delay_100(5);

        handle_event(xhci_interrupt_status);
        kernel_delay_100(50);

        if((hci_er == hci_er_start) && (link_trb_flag == 0))
                temp = 0;
        else if((hci_er == hci_er_start) && (link_trb_flag == 1))
                temp = (62*4); 
        else
                temp = ((hci_er - 16 - hci_er_start)/4);


        xdevice_slot = get_hci_event_ring(0, temp);

        if(  ( (((*(xdevice_slot + 2)) >> 24) & 0x000000ff)!= (1 << 0))  ) {

                print("\nbulk in data stage failed");
                return 1;

        }

        (*(trb_address_bulk_in + 3)) &= (~(1 << 0));
        trb_address_bulk_in += 4;
        /* Prints the results. */
        print_bulk_data_buffer((uint8_t*)data_buffer_in, 13);

        return 0;

}

/* Prints the contents of a buffer. */
void print_bulk_data_buffer(uint8_t* buffer, uint32_t buffer_length)
{
        print("\nbulk in data buffer below\n");

        for(uint32_t i = 0; i < buffer_length; i++) {

                if((i % 23) == 0)
                        print("\n");
                print_hex_byte((uint8_t)(*(buffer + i)));  print(" ");

        }
}

/* Atempts to do a set address command using the EHC. */
uint8_t set_addr()
{

        timer_install();
        asm("sti");
        uint8_t addr_success = 0;
        uint32_t temp;
        volatile uint32_t temp2;

        struct setup_data* device_descriptor = (struct setup_data*)kmem_4k_allocate(); 


        device_descriptor->RequestType = USB_HOST_TO_DEVICE;
        device_descriptor->Request = USB_SET_ADDRESS;
        device_descriptor->value_low = (volatile uint8_t)0x01;
        device_descriptor->value_high = (volatile uint8_t)0x00; 
        device_descriptor->index_low = (volatile uint8_t)0x00;
        device_descriptor->index_high = (volatile uint8_t)0x00;
        device_descriptor->length_low = (volatile uint8_t)0x00;
        device_descriptor->length_high = (volatile uint8_t)0x00;

        if (ehci_64bit_e == 0) {
                temp2 = (volatile uint32_t)qtd31->buffer_ptr0;
                temp2 &= 0xfffff000;
                qtd31->buffer_ptr0 = (volatile uint8_t*)temp2;
                qtd30->buffer_ptr0 = (volatile uint8_t*)device_descriptor;
        }
        else {
                temp2 = (volatile uint32_t)qtd31->qt_extended_buffers[0];
                temp2 &= 0xfffff000;
                qtd31->qt_extended_buffers[0] = (volatile uint32_t)temp2;
                qtd30->qt_extended_buffers[0] = (volatile uint32_t)device_descriptor;
        }

        qtd30->next_qtd_ptr = (struct queue_transfer_descriptor *)qtd31;
        qtd30->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(1) | EHCI_QTD_BYTES_TTRANSFER(8) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(0) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_SETUP_TOKEN | EHCI_QTD_SET_ACTIVE));

        qtd31->next_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd31->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(1) | EHCI_QTD_BYTES_TTRANSFER(0) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(1) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));



        QHhead20->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);

        QHhead20->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd30;
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead20 | EHCI_QH_TYPE_QH);

        kernel_delay_100(5);

        
        /* Commented code: print("\ntransaction status for qtd30 setup data: ");
         * print_hex((uint32_t)(qtd30->transfer_info & (volatile uint32_t)0x000000ff));
         * print("\ntransaction status for the status transfer from the device: ");
         * print_hex((uint32_t)(qtd31->transfer_info & (volatile uint32_t)0x000000ff));
         * print("\n"); 
         */
         
        if ((volatile uint32_t)(qtd30->transfer_info & (volatile uint32_t)0x000000ff) < (volatile uint32_t)0x00000002)
		        addr_success++;
        if ((volatile uint32_t)(qtd31->transfer_info & (volatile uint32_t)0x000000ff) < (volatile uint32_t)0x00000002)
		        addr_success++; 
        

        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);

        temp = (uint32_t)device_descriptor;
        free_mem_uint(temp);

        irq_uninstall_handler(0);
        asm("cli");
        return addr_success;

}

/* Attempts to do a set configuration command using the EHC. */
uint8_t set_config()
{

        timer_install();
        asm("sti");
        uint8_t addr_success = 0;
        uint32_t temp;
        volatile uint32_t temp2;


        struct setup_data* device_descriptor_2 = (struct setup_data*)kmem_4k_allocate();
        memsetbyte((uint8_t*)device_descriptor_2, 0x00, 0x08);

        device_descriptor_2->RequestType = USB_HOST_TO_DEVICE;
        device_descriptor_2->Request = USB_SET_CONFIGURATION;
        device_descriptor_2->value_low = (volatile uint8_t)0x01;
        device_descriptor_2->value_high = (volatile uint8_t)0x00; 
        device_descriptor_2->index_low = (volatile uint8_t)0x00;
        device_descriptor_2->index_high = (volatile uint8_t)0x00;
        device_descriptor_2->length_low = (volatile uint8_t)0x00;
        device_descriptor_2->length_high = (volatile uint8_t)0x00;

        if (ehci_64bit_e == 0) {
                temp2 = (volatile uint32_t)qtd31->buffer_ptr0;
                temp2 &= 0xfffff000;
                qtd31->buffer_ptr0 = (volatile uint8_t*)temp2;
                qtd30->buffer_ptr0 = (volatile uint8_t*)device_descriptor_2;
        }
        else {
                temp2 = (volatile uint32_t)qtd31->qt_extended_buffers[0];
                temp2 &= 0xfffff000;
                qtd31->qt_extended_buffers[0] = (volatile uint32_t)temp2;
                qtd30->qt_extended_buffers[0] = (volatile uint32_t)device_descriptor_2;
        }


        /* at least for qemu, the addressed state on the USB drive
         * needs to be set before we proceed with this
         */
        QHhead20->endpointch1 = (volatile uint32_t)(0x00000000 | EHCI_QH_MAX_NAK_COUNTER | EHCI_QH_MAX_PACKET_SIZE(64) | EHCI_QH_USE_QTD_TOGGLE | (2 << 12) | EHCI_QH_DEVICE_ADDRESS(1)); 

        // Commented code: zero_usbms_mem_4(qtd31->buffer_ptr1);


        qtd30->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(0) | EHCI_QTD_BYTES_TTRANSFER(8) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(0) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_SETUP_TOKEN | EHCI_QTD_SET_ACTIVE));

        qtd31->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(0) | EHCI_QTD_BYTES_TTRANSFER(0) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(1) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));

        QHhead20->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd30;
        // Commented code: QHhead20->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead20 | EHCI_QH_TYPE_QH);


        kernel_delay_100(5);
        
        /* Commented code: print("\ntransaction status for set configuration to the device: ");
         * print_hex((uint32_t)(qtd30->transfer_info & (volatile uint32_t)0x000000ff));
         * print("\ntransaction status for the first transfer from the device: ");
         * print_hex((uint32_t)(qtd31->transfer_info & (volatile uint32_t)0x000000ff));
         * print("\n"); 
         */
         

        if ((volatile uint32_t)(qtd30->transfer_info & (volatile uint32_t)0x000000ff) < (volatile uint32_t)0x00000002)
		        addr_success++;
        if ((volatile uint32_t)(qtd31->transfer_info & (volatile uint32_t)0x000000ff) < (volatile uint32_t)0x00000002)
		        addr_success++; 

        // Commented code: zero_usbms_mem_4(qtd31->buffer_ptr2);


        /* Commented code: print("\naddress success value is now: ");
        printi((uint32_t)addr_success);
        print("\n"); */

        temp = (uint32_t)device_descriptor_2;
        free_mem_uint(temp);

        // Commented code: QHhead20->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead20 | EHCI_QH_TYPE_QH);
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);


        QHhead21->endpointch1 = (volatile uint32_t)(0x00000000 | EHCI_QH_MAX_NAK_COUNTER | EHCI_QH_MAX_PACKET_SIZE(512) | EHCI_QH_USE_QTD_TOGGLE | EHCI_QH_HIGH_SPEED | EHCI_QH_EP_NUM(2) | EHCI_QH_DEVICE_ADDRESS(1));
        QHhead22->endpointch1 = (volatile uint32_t)(0x00000000 | EHCI_QH_MAX_NAK_COUNTER | EHCI_QH_MAX_PACKET_SIZE(512) | EHCI_QH_USE_QTD_TOGGLE | EHCI_QH_HIGH_SPEED | EHCI_QH_EP_NUM(1) | EHCI_QH_DEVICE_ADDRESS(1));
        QHhead21->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)1;
        QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)1;
        QHhead21->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);
        QHhead22->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);
        // Commented code: QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead21 | EHCI_QH_TYPE_QH);
	    qtd30->next_qtd_ptr = (struct queue_transfer_descriptor*)1;
	    qtd31->next_qtd_ptr = (struct queue_transfer_descriptor*)1;

        irq_uninstall_handler(0);
        asm("cli");
        return addr_success;

}

/* Attempts to do a get configuration command using the EHC. */
uint8_t confirm_data()
{

        timer_install();
        asm("sti");
        uint8_t addr_success = 0;
        uint32_t temp;
        volatile uint32_t temp2;


        struct setup_data* device_descriptor = (struct setup_data*)kmem_4k_allocate();
        memsetbyte((uint8_t*)device_descriptor, 0x00, 0x08);

        device_descriptor->RequestType = USB_DEVICE_TO_HOST;
        device_descriptor->Request = USB_GET_CONFIGURATION;
        device_descriptor->value_low = (volatile uint8_t)0x00;
        device_descriptor->value_high = (volatile uint8_t)0x00;
        device_descriptor->index_low = (volatile uint8_t)0x00;
        device_descriptor->index_high = (volatile uint8_t)0x00;
        device_descriptor->length_low = (volatile uint8_t)0x01;
        device_descriptor->length_high = (volatile uint8_t)0x00;


        qtd31->next_qtd_ptr = (struct queue_transfer_descriptor *)qtd32;


         if (ehci_64bit_e == 0) {
                temp2 = (volatile uint32_t)qtd31->buffer_ptr0;
                temp2 &= 0xfffff000;
                qtd31->buffer_ptr0 = (volatile uint8_t*)temp2;
                qtd30->buffer_ptr0 = (volatile uint8_t*)device_descriptor;
        }
        else {
                temp2 = (volatile uint32_t)qtd31->qt_extended_buffers[0];
                temp2 &= 0xfffff000;
                qtd31->qt_extended_buffers[0] = (volatile uint32_t)temp2;
                qtd30->qt_extended_buffers[0] = (volatile uint32_t)device_descriptor;
        }


        qtd30->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(0) | EHCI_QTD_BYTES_TTRANSFER(8) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(0) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_SETUP_TOKEN | EHCI_QTD_SET_ACTIVE));



        qtd31->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(1) | EHCI_QTD_BYTES_TTRANSFER(1) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(1) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));
           qtd32->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(1) | EHCI_QTD_BYTES_TTRANSFER(0) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(2) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_OUT_TOKEN | EHCI_QTD_SET_ACTIVE));


        qtd30->buffer_ptr0 = (volatile uint8_t*)device_descriptor;;


        QHhead20->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd30;
        //QHhead20->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead20 | EHCI_QH_TYPE_QH);


        timer_ticks = 0;
        while(timer_ticks < 3);
        
        /* Commented code: print("\ntransaction status for confirm config transfer to the device: ");
        print_hex((uint32_t)(qtd30->transfer_info & (volatile uint32_t)0x000000ff));
        print("\ntransaction status for the config value send from the device: ");
        print_hex((uint32_t)(qtd31->transfer_info & (volatile uint32_t)0x000000ff));
        print("\ntransaction status for the status packet: ");
        print_hex((uint32_t)(qtd32->transfer_info & (volatile uint32_t)0x000000ff));
        print("\n"); */ 
         
        
        if ((volatile uint32_t)(qtd30->transfer_info & (volatile uint32_t)0x000000ff) < (volatile uint32_t)0x00000002)
		        addr_success++;
        if ((volatile uint32_t)(qtd31->transfer_info & (volatile uint32_t)0x000000ff) < (volatile uint32_t)0x00000002)
		        addr_success++; 
        if ((volatile uint32_t)(qtd32->transfer_info & (volatile uint32_t)0x000000ff) < (volatile uint32_t)0x00000002)
		        addr_success++;


        /*  Commented code: print("\naddress success value is now: ");
        printi((uint32_t)addr_success);
        print("\n"); */


        temp = (uint32_t)device_descriptor;
        free_mem_uint(temp);

         print("\nthe value of the configuration: ");
        print_hex_byte((uint8_t)(*(qtd31->buffer_ptr1)));
        print("\n"); 

        // Commented code: QHhead20->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead20 | EHCI_QH_TYPE_QH);
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);

        QHhead21->endpointch1 = (volatile uint32_t)(0x00000000 | EHCI_QH_MAX_NAK_COUNTER | EHCI_QH_MAX_PACKET_SIZE(512) | EHCI_QH_USE_QTD_TOGGLE | EHCI_QH_HIGH_SPEED | EHCI_QH_EP_NUM(2) | EHCI_QH_DEVICE_ADDRESS(1));
        QHhead22->endpointch1 = (volatile uint32_t)(0x00000000 | EHCI_QH_MAX_NAK_COUNTER | EHCI_QH_MAX_PACKET_SIZE(512) | EHCI_QH_USE_QTD_TOGGLE | EHCI_QH_HIGH_SPEED | EHCI_QH_EP_NUM(1) | EHCI_QH_DEVICE_ADDRESS(1));
        QHhead21->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)1;
        QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)1;
        QHhead21->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);
        QHhead22->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);
        // Commented code: QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead21 | EHCI_QH_TYPE_QH);
	    qtd30->next_qtd_ptr = (struct queue_transfer_descriptor*)1;
	    qtd31->next_qtd_ptr = (struct queue_transfer_descriptor*)1;

	        
        irq_uninstall_handler(0);
        asm("cli");

        return addr_success;

}

/* Attempts to make a bulk transfer using the EHC. */
volatile uint8_t* ehci_transfer_bulkstorage_data(struct usb_storage_data* data)
{

	    timer_install();
	    asm("sti");
        volatile uint32_t temp;
	    volatile uint8_t* pointr;

        ehci_data_toggle1 ^= 1;
        ehci_data_toggle2 ^= 1; 
        

	    volatile uint32_t data_toggle_3 = 0;
	    volatile uint32_t addr_success = 0;

        data_toggle_3 = ehci_data_toggle2 ^ 1;

        if (ehci_64bit_e == 0) {
                temp = (volatile uint32_t)qtd30->buffer_ptr0;
                temp &= 0xfffff000;
                qtd30->buffer_ptr0 = (volatile uint8_t*)temp;

                temp = (volatile uint32_t)qtd31->buffer_ptr0;
                temp &= 0xfffff000;
                qtd31->buffer_ptr0 = (volatile uint8_t*)temp;

                temp = (volatile uint32_t)qtd32->buffer_ptr0;
                temp &= 0xfffff000;
                qtd32->buffer_ptr0 = (volatile uint8_t*)temp;
                qtd30->buffer_ptr0 = (volatile uint8_t*)data;
                zero_usbms_mem_4(qtd31->buffer_ptr1);
        }
        else {
                temp = (volatile uint32_t)qtd30->qt_extended_buffers[0];
                temp &= 0xfffff000;
                qtd30->qt_extended_buffers[0] = (volatile uint32_t)temp;

                temp = (volatile uint32_t)qtd31->qt_extended_buffers[0];
                temp &= 0xfffff000;
                qtd31->qt_extended_buffers[0] = (volatile uint32_t)temp;

                temp = (volatile uint32_t)qtd32->qt_extended_buffers[0];
                temp &= 0xfffff000;
                qtd32->qt_extended_buffers[0] = (volatile uint32_t)temp;
                qtd30->qt_extended_buffers[0] = (volatile uint32_t)data;
                zero_usbms_mem_6((uint32_t*)(qtd31->qt_extended_buffers[1]));
        }



        uint32_t btt = (data->data_transfer_length_low2 * 256) + (data->data_transfer_length_low);


	    qtd30->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(ehci_data_toggle1) | EHCI_QTD_BYTES_TTRANSFER(31) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(0) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_OUT_TOKEN | EHCI_QTD_SET_ACTIVE));
        if (data->operation_code != 0) {
	            qtd31->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(ehci_data_toggle2) | EHCI_QTD_BYTES_TTRANSFER(btt) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(1) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));
                // Commented code: QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd31;
        } 

        if (data->operation_code == 0) {

                QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd32;
	            qtd32->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(ehci_data_toggle2) | EHCI_QTD_BYTES_TTRANSFER(13) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(1) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));

        }
        else {

	            QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd31;
        }

	    QHhead21->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd30;
        QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd31;
        
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead21 | EHCI_QH_TYPE_QH);

	    kernel_delay_100(8);
        /* Commented code: QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);
        QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd31;
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead22 | EHCI_QH_TYPE_QH); */
	    // Commented code: timer_ticks = 0;
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead22 | EHCI_QH_TYPE_QH);
	    kernel_delay_100(8);
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);

        /* Now either a data token in has been sent or the status data token */
	    if ((qtd30->transfer_info & (volatile uint32_t)0x000000ff) < 2)
			    addr_success++;
        if (data->operation_code == 0) {
	            if (((qtd32->transfer_info & (volatile uint32_t)0x000000ff) < 2))
			            addr_success++;
        }
        else {
                if (((qtd31->transfer_info & (volatile uint32_t)0x000000ff) < 2))
			            addr_success++;
	            qtd32->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(data_toggle_3) | EHCI_QTD_BYTES_TTRANSFER(13) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(2) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));
                /* Commented code: qtd31->next_qtd_ptr = (struct queue_transfer_descriptor*)qtd32; */
                QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd32;
                QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead22 | EHCI_QH_TYPE_QH);
	            kernel_delay_100(8);


	            if ((qtd32->transfer_info & (volatile uint32_t)0x000000ff) < 2)
			            addr_success++;
                
        }

        timer_phase(18);

        if (ehci_64bit_e == 0) {
	            pointr = qtd31->buffer_ptr1;
        }
        else {
                pointr = (volatile uint8_t*)(qtd31->qt_extended_buffers[1]);
        }
        /* Commented code: if (data->operation_code == USB_BULKSTORAGE_SCSI_READ10) {

	            print("\n\nTrying to read the cbw data:\n");
	            for (uint32_t read = 0;read < 32;read++) {
		                if((read % 22) == 0)
			                    print("\n");
		                print(" ");
		                print_hex_byte((uint8_t)(*(pointr + read)));
	            }

	            pointr = qtd31->buffer_ptr1;
	            print("\n\nTrying to read the returned data:\n");
	            for (uint32_t read = 0;read < btt;read++) {
		                if((read % 24) == 0)
			                    print("\n");
		                print(" ");
		                print_hex_byte((uint8_t)(*(pointr + read)));
	            }

	            pointr = qtd32->buffer_ptr2;
	            print("\n\nTrying to read the csw data:\n");
	            for (uint32_t read = 0;read < 13;read++) {
		                if((read % 24) == 0)
			                    print("\n");
		                print(" ");
		                print_hex_byte((uint8_t)(*(pointr + read)));
	            }


        } */

        

        /* Commented code: print("\naddr success for inquiry: ");
        printi((uint32_t)addr_success);
        print("\n");
         */
        // Commented code: qtd31->next_qtd_ptr = (struct queue_transfer_descriptor*)1;
        // Commented code: QHhead21->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead21 | EHCI_QH_TYPE_QH);
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);
        // Commented code: QHhead22->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead21 | EHCI_QH_TYPE_QH);

	    if (addr_success == 3 || ((data->operation_code == 0) && (addr_success == 2)) ) {

			    irq_uninstall_handler(0);
			    asm("cli");
                if (data->operation_code != 0) {
			            return pointr;
                }
                else {
                        return 0;
                
                }
	    }
	    else {


			    irq_uninstall_handler(0);
			    asm("cli");

	    }

	    return ((volatile uint8_t*)0x600000);

}



/* Attempts to do a Read 10 operation using the EHC. */
uint8_t transfer_data_10(struct cbw_read_10* cbw_data_10, volatile uint32_t data_toggle_1, volatile uint32_t data_toggle_2)
{

	    timer_install();
	    asm("sti");

	    volatile uint8_t* pointr;
	    volatile uint8_t* pointr2;

	    volatile uint8_t* pointr3;
	    volatile uint8_t* pointr4;

	    uint32_t temp1;

	    //uint32_t async_success;
	    uint8_t addr_success = 0;
	    volatile uint32_t data_toggle_3 = 0;
	    if (data_toggle_2 == 1)
		        data_toggle_3 = 0;
	    else if (data_toggle_2 == 0)
		        data_toggle_3 = 1;


	    struct cbw_read_10* cbw_1 = cbw_data_10;
	    pointr = (volatile uint8_t*) cbw_1;

	    qtd30->buffer_ptr1 = pointr;

	    qtd30->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
	    pointr2 = qtd30->buffer_ptr0;

	    qtd31->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
	    pointr3 = qtd31->buffer_ptr0;

	    qtd32->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
	    pointr4 = qtd32->buffer_ptr0;

	    qtd31->next_qtd_ptr = (struct queue_transfer_descriptor*)1;
	    qtd32->next_qtd_ptr = (struct queue_transfer_descriptor*)1;
	    qtd30->next_qtd_ptr = (struct queue_transfer_descriptor*)1;

	    qtd30->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(data_toggle_1) | EHCI_QTD_BYTES_TTRANSFER(31) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(1) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_OUT_TOKEN | EHCI_QTD_SET_ACTIVE));
	    qtd31->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(data_toggle_2) | EHCI_QTD_BYTES_TTRANSFER(512) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(2) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE)); 
	    qtd32->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(data_toggle_3) | EHCI_QTD_BYTES_TTRANSFER(13) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(3) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));

	    QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd31;
	    QHhead21->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd30;

	    timer_ticks = 0;
	    while(timer_ticks < 3);

	    if ((qtd30->transfer_info & (volatile uint32_t)0x000000ff) == 0)
			    addr_success++;
	    if ((qtd31->transfer_info & (volatile uint32_t)0x000000ff) == 0)
			    addr_success++;

	    print("\nReading qtd32 transfer info: ");
	    print_hex((uint32_t)qtd30->transfer_info);

	    print("\nReading qtd32 transfer info: ");
	    print_hex((uint32_t)qtd31->transfer_info);

	    QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd32;

	    timer_ticks = 0;
	    while(timer_ticks < 2);

	    if ((qtd32->transfer_info & (volatile uint32_t)0x000000ff) == 0)
			    addr_success++;

	    print("\nReading qtd32 transfer info: ");
	    print_hex((uint32_t)qtd32->transfer_info);


	    pointr = qtd30->buffer_ptr1;
	    print("\n\nTrying to read the cbw data:\n");
	    for (uint16_t read = 0;read < 32;read++) {
		        if((read % 22) == 0)
			            print("\n");
		        print(" ");
		        print_hex_byte((uint8_t)*(pointr + read));
	    }

	    pointr = qtd31->buffer_ptr2;
	    print("\n\nTrying to read the returned data:\n");
	    for (uint16_t read = 0;read < 512;read++) {
		        if((read % 24) == 0)
			            print("\n");
		        print(" ");
		        print_hex_byte((uint8_t)*(pointr + read));
	    }

	    pointr = qtd32->buffer_ptr3;
	    print("\n\nTrying to read the csw data:\n");
	    for (uint16_t read = 0;read < 13;read++) {
		        if((read % 24) == 0)
			            print("\n");
		        print(" ");
		        print_hex_byte((uint8_t)*(pointr + read));
	    }


	    temp1 = 0x00000000;
	    temp1 = (uint32_t)pointr2;
	    free_mem_uint(temp1);

	    temp1 = 0x00000000;
	    temp1 = (uint32_t)pointr3;
	    free_mem_uint(temp1);

	    temp1 = 0x00000000;
	    temp1 = (uint32_t)pointr4;
	    free_mem_uint(temp1);
	    irq_uninstall_handler(0);
	    asm("cli");
	    return addr_success;

}

/* Attempts to do a write 10 operation using the EHC. */
uint8_t transfer_data_10_w(struct cbw_write_10* cbw_data_w_10, volatile uint32_t data_toggle_1, volatile uint32_t data_toggle_2, volatile uint8_t* transfer_buffer)
{

	    timer_install();
	    asm("sti");
	    volatile uint8_t* pointr;
	    volatile uint8_t* transfer_pointr;
	    volatile uint8_t* pointr2;
	    volatile uint8_t* pointr3;
	    volatile uint8_t* pointr4;


	    uint32_t temp1;
	    volatile uint32_t addr_success = 0;
	    volatile uint32_t data_toggle_3 = 0;
	    if (data_toggle_1 == 1)
		        data_toggle_3 = 0;
	    else if (data_toggle_1 == 0)
		        data_toggle_3 = 1;

	    struct cbw_write_10* cbw_1 = cbw_data_w_10;
	    pointr = (volatile uint8_t*) cbw_1;

	    transfer_pointr = transfer_buffer;

	    qtd30->buffer_ptr1 = pointr;

	    qtd31->buffer_ptr2 = transfer_pointr;

	    qtd30->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
	    pointr2 = qtd30->buffer_ptr0;

	    qtd31->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
	    pointr3 = qtd31->buffer_ptr0;

	    qtd32->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
	    pointr4 = qtd32->buffer_ptr0;

	    qtd32->next_qtd_ptr = (struct queue_transfer_descriptor*)1;
	    qtd31->next_qtd_ptr = (struct queue_transfer_descriptor*)1;
	    qtd30->next_qtd_ptr = (struct queue_transfer_descriptor*)qtd31;
	    qtd30->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(data_toggle_1) | EHCI_QTD_BYTES_TTRANSFER(31) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(1) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_OUT_TOKEN | EHCI_QTD_SET_ACTIVE));
	    qtd31->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(data_toggle_2) | EHCI_QTD_BYTES_TTRANSFER(512) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(2) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_OUT_TOKEN | EHCI_QTD_SET_ACTIVE)); 


	    QHhead21->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd30;

	    timer_ticks = 0;
	    while(timer_ticks < 3);

	    print("\nReading qtd32 transfer info: ");
	    print_hex((uint32_t)qtd30->transfer_info);

	    print("\nReading qtd31 transfer info: ");
	    print_hex((uint32_t)qtd31->transfer_info);

	    if ((qtd30->transfer_info & (volatile uint32_t)0x000000ff) == 0)
			    addr_success++;


	    if ((qtd31->transfer_info & (volatile uint32_t)0x000000ff) == 0)
			    addr_success++;

	    qtd32->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(data_toggle_3) | EHCI_QTD_BYTES_TTRANSFER(13) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(3) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));
	    QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd32;

	    timer_ticks = 0;
	    while(timer_ticks < 2);

	    if ((qtd32->transfer_info & (volatile uint32_t)0x000000ff) == 0)
			    addr_success++;

	    print("\nReading qtd32 transfer info: ");
	    print_hex((uint32_t)qtd32->transfer_info);

	    pointr = qtd32->buffer_ptr3;
	    print("\n\nTrying to read the csw data:\n");
	    for (uint16_t read = 0;read < 13;read++) {
		        if((read % 24) == 0)
			            print("\n");
		        print(" ");
		        print_hex_byte((uint8_t)*(pointr + read));
	    }

	    temp1 = 0x00000000;
	    temp1 = (uint32_t)pointr2;
	    free_mem_uint(temp1);

	    temp1 = 0x00000000;
	    temp1 = (uint32_t)pointr3;
	    free_mem_uint(temp1);

	    temp1 = 0x00000000;
	    temp1 = (uint32_t)pointr4;
	    free_mem_uint(temp1);

	    irq_uninstall_handler(0);
	    asm("cli");
	    return addr_success;

}

/* Attempts to do a test unit ready operation using the EHC. */
uint8_t test_unit_ready(struct test_unit_ready* t_unit_r, uint32_t data_toggle_1, uint32_t data_toggle_2)
{

	    timer_install();
	    asm("sti");
	    volatile uint8_t* pointr;
	    uint8_t addr_success = 0;


	    struct test_unit_ready* cbw_1 = t_unit_r;
	    pointr = (volatile uint8_t*)cbw_1;

	    zero_usbms_mem_4(qtd30->buffer_ptr0);
	    zero_usbms_mem_4(qtd31->buffer_ptr0);
	    zero_usbms_mem_4(qtd30->buffer_ptr1);
	    zero_usbms_mem_4(qtd31->buffer_ptr2);
	    qtd30->buffer_ptr1 = pointr;


	    qtd30->next_qtd_ptr = (struct queue_transfer_descriptor*)1;
	    qtd31->next_qtd_ptr = (struct queue_transfer_descriptor*)1;
	    qtd30->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(data_toggle_1) | EHCI_QTD_BYTES_TTRANSFER(31) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(1) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_OUT_TOKEN | EHCI_QTD_SET_ACTIVE));
	    qtd31->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(data_toggle_2) | EHCI_QTD_BYTES_TTRANSFER(13) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(2) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE)); 

	    QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd31;
	    QHhead21->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd30;

	    timer_ticks = 0;
	    while(timer_ticks < 1);


	    if ((qtd30->transfer_info & 0x000000ff) == 0)
			    addr_success++;
	    if ((qtd31->transfer_info & 0x000000ff) == 0)
			    addr_success++;

	    if (get_reclamation_bit())
		        print("\n\nWarning: Reclamation bit still set.");
	    QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)1;
	    qtd31->next_qtd_ptr = (struct queue_transfer_descriptor*)qtd32;
	    
	    irq_uninstall_handler(0);
	    asm("cli");
	    return addr_success;

}

/* This function will attempt to configure and set up the 
 * USB storage device before reading storage data.
 */
uint8_t sequence_1() 
{

        /* suggestion: pointer to functions as arguments would be very nice.
         * and control statements regarding how long it should last. 
         */
	    asm("sti");
	    volatile uint8_t* init_success_3;
	    // Commented code: uint8_t init_success_6;
	    uint32_t ktemp = 0;
	    uint8_t kreset_error = 0;
	    asm("cli");
        /* set address and configure the device */
        print("\nSetting the address and the configuration\nof the USB device..");
	    init_success_1 = set_addr();
	    asm("sti");
	    if (init_success_1 == 2) {
			    asm("cli");
			    /* confirm data */
			    init_success_2 = set_config();
			    asm("sti");
				    
					    if (init_success_2 != 2) {
							    print_mr_error();
							    ktemp = 1;
							    stop_ehci();
							    reset_ehci();
					    } 
					    else {
							    /* inqury */
							    asm("cli");
							    usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_CTL;
                                init_success_3 = prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_INQURY, USB_DEVICE_TO_HOST, 0, 0x24, usbms_extra_data);
							    asm("sti");
							    if (init_success_3 == (volatile uint8_t*)0x600000) {
                                        print("\ninqury did not succeed\n");
									    print_mr_error();
									    ktemp = 1;
									    stop_ehci();
									    reset_ehci();
							    }
							    else {
									    /* request sense */
									    asm("cli");
    							        usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_CTL;
                                        init_success_3 = prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_REQUEST_SENSE, USB_DEVICE_TO_HOST, 0, 0x12, usbms_extra_data);
									    asm("sti");
									    if (init_success_3 == (volatile uint8_t*)0x600000) {
											    print_mr_error();
											    ktemp = 1;
											    stop_ehci();
											    reset_ehci();
									    }
									    else {

											    /* read capacity 10 */
											    asm("cli");
											    usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_CTL;
                                                init_success_3 = prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_READ_CAPACITY10, USB_DEVICE_TO_HOST, 0, 0x08, usbms_extra_data);
											    asm("sti");
											    if (init_success_3 == (volatile uint8_t*)0x600000) {
													    print_mr_error();
													    ktemp = 1;
													    stop_ehci();
													    reset_ehci();
											    }
											    else {
													    /* request sense */
													    asm("cli");
													    usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_CTL;
                                                        init_success_3 = prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_REQUEST_SENSE, USB_DEVICE_TO_HOST, 0, 0x12, usbms_extra_data);
													    asm("sti");
													    if (init_success_3 == (volatile uint8_t*)0x600000) {
															    print_mr_error();
															    ktemp = 1;
															    stop_ehci();
															    reset_ehci();
													    }
												    


											    }
									    }
							    }
					    }
	    }

	    else {

		        print_mr_error();
		        ktemp = 1;
		        stop_ehci();
		        timer_ticks = 0;
	            while(timer_ticks < 1);
	            kreset_error = reset_ehci();
	            timer_ticks = 0;
	            while(timer_ticks < 1); 
       
	    }


	    if (ktemp == 0) {
			    asm("cli");
			    return 1;

	    }
	    else if (kreset_error == 1) {

			    print("\n\nSecond try with resetting the ehc.");
			    reset_ehci();
		        timer_ticks = 0;
	            while(timer_ticks < 1);

	    }

        asm("cli");
        return 0;

}

/* Prints error messages. */
void print_mr_error()
{
	    print("\n\nAn error occured during USB device configuration.");
	    print("\nPlease try to type ra (reset address) or pr (port reset).");
	    print("\nAnd then type init to try to get it to configure, again.");
	    print("\nOr just type init.");
	    print("\nAutomatic error correction to be added later (i hope).");
}

