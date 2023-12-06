/* This file contains a driver for the XHC
 * host controller. It is primarily used
 * for USB 3.
 */

#include "../../types.h"
#include "../../mem.h"
#include "../../graphics/screen.h"
#include "../../timer.h"
#include "usb_ehci.h"
#include "usb_xhci.h"
#include "usb_hubs.h"
#include "usb.h"


/* Initializes some variables, arrays and lists.
 * Ica means input context array. Consult the
 * XHCI specification for more information
 * about that.
 */
void init_hci_mem_resources()
{
        trb_address_field = 0;
        xhci_get_descriptor_first_time_used = 1;
        link_trb_flag = 0;
        uint32_t* hci_rs_ptr = hci_mem_resources;
        for(uint32_t i = 0; i < 4096;i++) {
                *(hci_rs_ptr + i) = 0x00000000;

        }

        ica_list = (struct input_contexts*)kmem_4k_allocate();
        slot_endpoint_start = (struct xhci_endpoint*)kmem_4k_allocate();
        zero_usbms_mem_6((uint32_t*)slot_endpoint_start);

        /* init to zero */
        memsetbyte((uint8_t*)ica_list, 0x0, 4096);
        ica_list->ica_next = ica_list;
        ica_list_start = ica_list;
        hci_mem_resources_index = 0;
        hci_mem_resources[hci_mem_resources_index] = (uint32_t)ica_list;
        hci_mem_resources_index++;
        for(uint32_t i = 0; i < 4;i++) {
                ica_list->ica_address = (uint32_t*)kmem_4k_allocate();
                memsetbyte((uint8_t*)ica_list->ica_address, 0x00, 4096);
                hci_mem_resources[hci_mem_resources_index] = (uint32_t)ica_list;
                hci_mem_resources_index++;
                ica_list->ica_next++;
                ica_list = ica_list->ica_next;
        }

        ica_list = ica_list_start;

        struct xhci_usb_device_slot* usb_slot_start = (struct xhci_usb_device_slot*) kmem_4k_allocate();
        memsetbyte((uint8_t*)usb_slot_start, 0x0, 4096);
        hci_mem_resources[hci_mem_resources_index] = (uint32_t)usb_slot_start;
        hci_mem_resources_index++;
}


/* Returns the USB CMD register. All the registers
 * below are for a XHC. 
 */
uint32_t get_xhci_usbcmd()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000000);
        return xbaddr5;
}

/* Prints the USB STS register */
void print_xhci_usbsts()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Returns the USB STS register */
uint32_t get_xhci_usbsts()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004);
        return xbaddr5;
}

/* Checks if the controller is not ready. */
uint32_t get_xhci_cnr()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004);
        xbaddr5 >>= 11;
        xbaddr5 &= 0x00000001;
        return xbaddr5;
}

/* Reads a byte from a memory address. */
uint8_t read_byte(const uint32_t base_addr, const uint32_t offset)
{
        volatile uint8_t byte = *((uint8_t *) (base_addr + offset));
        return byte;
}

/* Stores the address of the XHC's operational base.
 * xhci_mem_address_attr_doorb is the address of the 
 * beginning of the XHC's register space.
 */
void store_hci_operational_base()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000000);
        xbaddr5 &= 0x000000ff;
        hci_operational_base = (volatile uint32_t)((volatile uint32_t)xhci_mem_address_attr_doorb + (volatile uint32_t)xbaddr5);
}

/* Stores a piece of information. The information is about
 * whether the XHC is capable of 64-bit addressing. 
 */
void store_check_xhci_64_bit_capable()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000010);
        xbaddr5 &= 0x00000001;
        xhci_64bit_capable = xbaddr5;
}

/* Stores the maximum of device slots of the XHC. */
void store_xhci_max_device_slots()
{
       uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000004);
       xhci_max_device_slots = xbaddr5 & 0x000000ff;
}

/* Stores information about the indication of an existance
 * of an extended capabilities list.
 */
void store_hci_ext_cap_ptr()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000010);
        xbaddr5 >>= 16;
        xbaddr5 <<= 2;
        hci_ext_cap_ptr = xhci_mem_address_attr_doorb + xbaddr5;
}

/* Stores the maximum event ring segment table slots */
void store_xhci_max_er_segment_table_slots()
{
       uint32_t temp9 = 1;
       uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000008);
       uint32_t xhci_max_er_segment_table_slots_counter = (uint32_t)(((xbaddr5) >> 4) & 0x0000000f);
        if(xhci_max_er_segment_table_slots_counter == 0) {

                xhci_max_er_segment_table_slots = temp9;
                return;

        }
        else {

                for(uint32_t k = 0; k < xhci_max_er_segment_table_slots_counter+1;k++) {
                        temp9 *= 2;
                }

                temp9 /= 2;

        }

        xhci_max_er_segment_table_slots = temp9;
}

/* Stores the maximum amount of interrupters.
 * The interrputer is a big data structure 
 * which contains information about event rings
 * among other things.
 */
void store_xhci_max_interrupters()
{
       uint32_t xbaddr9 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000004);
       xbaddr9 >>= 8; 
       xbaddr9 &= 0x000007ff; 
       hci_max_interrupters = xbaddr9;
}

/* Stores the address of the runtime base registers. */
void store_xhci_runtimebase()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000018);
        xbaddr5 &= 0xffffffe0;
        xhci_runtime_base = (volatile uint32_t)((volatile uint32_t)xhci_mem_address_attr_doorb + (volatile uint32_t)xbaddr5);
}

/* Stores the event ring table size. */
void store_event_ring_segment_table_size()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000028);
        xbaddr5 &= 0xffff0000;
        xbaddr5 |= xhci_max_er_segment_table_slots;       
        write_dword((const uint32_t)xhci_runtime_base, (const uint32_t)0x00000028, (const uint32_t)xbaddr5);
}

/* Stores the maximum of event ring segment table entries. 
 * A number of 7 is interpreted as 2 to the power of 7
 * which then represents 128 entries.
 */
void store_hci_max_erst()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000008);
        xbaddr5 >>= 4;
        xbaddr5 &= 0x0000000f;
        hci_max_erst = xbaddr5;
}

/* Handles the event rings which need to be initialized. */
void store_event_ring_table_and_rings()
{
        /* The erst address low */
        volatile uint32_t xbaddr5 = read_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000030); 
        xbaddr5 &= 0x0000003f;
        volatile uint32_t temp = (volatile uint32_t)kmem_4k_allocate();
        hci_er_segment_table_ba = temp;
        temp |= xbaddr5;

        temp &= 0xffffff00;
        zero_usbms_mem_6((uint32_t*)temp);

        uint32_t* xbaddr9 = (uint32_t*)hci_er_segment_table_ba;
        temp = (uint32_t)kmem_4k_allocate();
        hci_er = temp;
        hci_er_start = temp;

        /* low dword first, high dword second if not 64 bit capable */
        if(xhci_64bit_capable) {

                *(xbaddr9) = temp;
                *(xbaddr9 + 1) = 0x00000000;

        }
        else {
                *(xbaddr9) = temp;
        }

        /* ring segment size */
        (*(xbaddr9 + 2)) = 64;

        /* event ring dequeue pointer low */
        xbaddr5 = read_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000038);
        xbaddr5 &= 0x0000003f;
        temp &= 0xffffffc0;

        hci_er_dq_ptr = temp;
        temp |= xbaddr5;

        temp |= (volatile uint32_t)8;

        hci_er_segment_table_dq_reg = read_dword_addr((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000038);
        
        if(xhci_64bit_capable) {
                /* low dword first, high dword second if not 64 bit capable */
                write_dword((const uint32_t)xhci_runtime_base, (const uint32_t)0x00000038, (const uint32_t)temp);
                write_dword((const uint32_t)xhci_runtime_base, (const uint32_t)0x0000003c, (const uint32_t)0x00000000);

        }
        else {
                write_dword((const uint32_t)xhci_runtime_base, (const uint32_t)0x00000038, (const uint32_t)temp);

        } 

        temp &= 0xffffff00; 
        zero_usbms_mem_6((uint32_t*)temp);

        if(xhci_64bit_capable) { 
                /* low dword first, high dword second if not 64 bit capable */
                write_dword((const uint32_t)xhci_runtime_base, (const uint32_t)0x00000030, (const uint32_t)hci_er_segment_table_ba);
                write_dword((const uint32_t)xhci_runtime_base, (const uint32_t)0x00000034, (const uint32_t)0x00000000);

        }
        else {
                write_dword((const uint32_t)xhci_runtime_base, (const uint32_t)0x00000030, (const uint32_t)hci_er_segment_table_ba);

        }

}

/* init event ring */
void store_hci_er() 
{
        uint32_t* xbaddr5 = (uint32_t*)hci_er_segment_table_ba;
        volatile uint32_t temp = (volatile uint32_t)kmem_4k_allocate();
        hci_er = temp;
        zero_usbms_mem_6((uint32_t*)temp);

        
        if(xhci_64bit_capable) {
                /* low dword first, high dword second if not 64 bit capable */
                (*(xbaddr5)) = temp;
                *(xbaddr5 + 1) = 0x00000000;

        }
        else {
                (*(xbaddr5)) = temp;

        }

        /* ring segment size */
        (*(xbaddr5 + 2)) = 16;
}

/* Stores the Device context base address array pointer, I guess. */
void store_xhci_dcbaap()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000030);
        xhci_dcbaap_base = (volatile uint32_t*)xbaddr5;
}

/* Stores the device context base address, and handles
 * an eventual scratchpad or scratchpads.
 */
void store_hci_dcaba()
{
        volatile uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000030);
        volatile uint32_t temp = (volatile uint32_t)kmem_4k_allocate();

        zero_usbms_mem_6((uint32_t*)temp);

        volatile uint32_t temp5 = 0;
        volatile uint32_t temp59 = hci_scratchpad_pagesize;
        volatile uint32_t temp95 = 0;
        volatile uint32_t* temp959 = (volatile uint32_t*)temp;

        if(hci_scratchpad_size) {
                if(xhci_64bit_capable) {

                        *temp959 = kmem_4k_allocate();
                        zero_usbms_mem_6((uint32_t*)*temp959);
                        /* scratch pad array address */
                        *(temp959 + 1) = 0x00000000;

                }
                else {

                        *temp959 = kmem_4k_allocate();
                        zero_usbms_mem_6((uint32_t*)*temp959);

                }

                volatile uint32_t* scratchpad_table_base = (volatile uint32_t*)(*temp959);
                hci_scratchpad_table_base = scratchpad_table_base;

                uint32_t page_size_calc = 1;

                for(uint32_t k = 1; k < temp59;k++) {
                                
                        page_size_calc *= 2;                 
                }
                temp95 = hci_scratchpad_size*2;
                for(uint32_t i = 0; i < temp95; i+=2) {
                        uint32_t k = 0;
                        volatile uint32_t temp9 = 0;
                        for(; k < page_size_calc ; k++) {
                                temp9 = (volatile uint32_t)kmem_4k_allocate();
                                zero_usbms_mem_6((uint32_t*)(temp9));

                        }

                            temp5 = temp9;
                    
                        if(xhci_64bit_capable) {

                                *(scratchpad_table_base + i) = temp5;
                                *(scratchpad_table_base + i + 1) = 0x00000000;

                        }
                        else {

                                *(scratchpad_table_base + i) = temp5;

                        }

                }
        }
        else {

                *temp959 = 0x00000000;
                *(temp959 + 1) = 0x00000000;
        }


        /* Commented code: uint32_t* temp5959 = (uint32_t*)hci_dcaba;
        temp5959 += 2;  

        uint32_t temp9595 = (uint32_t)kmem_4k_allocate();  
        if(xhci_64bit_capable) {

                *(temp5959) = temp9595;
                zero_usbms_mem_6((uint32_t*)temp9595);
                *(temp5959 + 1) = 0x00000000;


        }
        else {
                *(temp5959) = temp9595;
                zero_usbms_mem_6((uint32_t*)temp9595);


        } */

        xbaddr5 |= temp;

        if(xhci_64bit_capable) {

                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000030, (const uint32_t)xbaddr5);
                write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000034, (const uint32_t)0x00000000);
        }
        else {
                
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000030, (const uint32_t)xbaddr5);

        }

        hci_dcaba = (volatile uint32_t*)temp;

}

/* Stores the address of the base of the doorbell array. */
void store_xhci_doorbellbase()
{
        volatile uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000014);
        xhci_doorbell_base =  (volatile uint32_t)((volatile uint32_t)xhci_mem_address_attr_doorb + ((volatile uint32_t)xbaddr5));
}

/* Stores the address of the command ring control register. */
void store_xhci_commandringctrl_base()
{
        volatile uint32_t* xbaddr5 = read_dword_addr((const uint32_t)hci_operational_base,(const uint32_t)0x00000018);
        xhci_cmdring_ctrl_base = xbaddr5;
}

/* Aborts a command. */
void xhci_abort_command()
{
        volatile uint32_t* xbaddr5 = read_dword_addr((const uint32_t)hci_operational_base,(const uint32_t)0x00000018);
        (*((uint32_t*)xbaddr5)) |= 4;
}

/* Puts the address of a command ring in the command ring
 * control register.
 */ 
void write_store_xhci_commandringbase()
{
        volatile uint32_t temp = (volatile uint32_t)kmem_4k_allocate();
        zero_usbms_mem_6((uint32_t*)temp);

        if(xhci_64bit_capable) {

                write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000018, (const uint32_t)temp);   
                write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x0000001c, (const uint32_t)0x00000000);

        }
        else {

                write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000018, (const uint32_t)temp);                

        }

        temp &= (volatile uint32_t)0xffffff00;
        xhci_commandring_base = (volatile uint32_t*)temp;
        zero_usbms_mem_6((uint32_t*)temp);
        hci_mem_resources[hci_mem_resources_index] = (volatile uint32_t)xhci_cmdring_ctrl_base;
        hci_mem_resources_index++;
}

/* Stores information in hci_cic. Hci_cic contains a 1 or a 0.
 * A 1 indicates that the controller supports configuration
 * information capability.
 */
void store_hci_cic()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x0000001c);
        if(xbaddr5 & (1 << 5)) {
                hci_cic = 1;
        }
        else {
                hci_cic = 0;
        }
}

/* Stores information in hci_context_size. A 1 means that
 * the XHC uses 64 bytes context data structures. A 0
 * indicates that the controller uses 32 bytes context
 * data structures.
 */
void store_hci_context_size()
{
        volatile uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000010);
        xbaddr5 >>= 2;
        xbaddr5 &= 0x00000001;
        hci_context_size = xbaddr5;
}

/* Stores information in hci_cie. The information is about
 * what's called Configuration Information Enable. 
 * It's related to the input context fields.
  */
void store_hci_cie() 
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000038);
        if(xbaddr5 & (1 << 9)) {
                hci_cie = 1;
        }
        else {
                hci_cie = 0;
        }
}

/* Stores the size of the scratchpad in hci_scratchpad_size. */
void store_hci_scratchpad_size()
{
       uint32_t temp = 0;
       uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000008);
       xbaddr5 >>= 21; 
       temp |= xbaddr5;
       temp &= 0x0000003f;
       temp <<= 5;
       xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000008);
       xbaddr5 >>= 27;
       xbaddr5 &= 0x0000003f;
       temp |= xbaddr5;
       hci_scratchpad_size = temp;
}

/* Stores information in hci_scratchpad_size. The information
 * is about how many bytes a page should be in the scratchpad.
 */
void store_hci_scratchpad_pagesize()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000008);
        if(xbaddr5 == 0) {
                print("\nerror. or driver error. no such xhci pagesize. halting.");
                asm("hlt");
        }

        uint32_t k = 0;
        while(xbaddr5 > 0) {
                k++;
                xbaddr5/= 2;
        }
        hci_scratchpad_pagesize = k;
}
/* Writes to the configuration register. */
void write_store_config_register(const uint32_t hci_config_value)
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000038);
        xbaddr5 |= hci_config_value;
        write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000038, (const uint32_t)xbaddr5);
}

/* Stops the XHC. */
void stop_xhci()
{
        uint32_t temp = get_xhci_usbcmd();
        temp &= (~(1));
        write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000000, (const uint32_t)temp);
}

/* Starts the XHC. */
void start_xhci()
{
        uint32_t temp = get_xhci_usbcmd();
        temp |= 1;
        write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000000, (const uint32_t)temp);
}

/* Resets the XHC. */
void reset_xhci()
{
        uint32_t temp = get_xhci_usbcmd();
        temp |= 2;
        write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000000, (const uint32_t)temp);
}

/* Clears the event interrupt, I guess. */
void clear_sts_eint()
{
        uint32_t temp = get_xhci_usbsts();
        temp |= 8;
        write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000004, (const uint32_t)temp);
}

/* ********************************************************** End of store and start stop functions etc. *********************************************************** */

/* This function is kin of unused. */
uint32_t* copy_device_context_to_input_context(uint32_t dca_index)
{
        uint32_t* temp = (uint32_t*)hci_dcaba;
        temp += (dca_index*2);
        uint32_t* temp5 = (uint32_t*)*temp; // temp 5 points to address of dc
        if(dca_index == 0) {
                print("\nerror copying device context to input context\nhalting");
                asm("hlt");

        }
        ica_list = ica_list_start;

        for(uint32_t l = 1; l < dca_index; l++) {
                ica_list = ica_list->ica_next;

        }
        
        /* copy the whole structure code */
        for(uint32_t i = 0; i < 32*8; i++) {
                *(ica_list->ica_address + i + 8) = *(temp5 + i);
        }

        return ica_list->ica_address;
}

/* Initializes some values for the input context of a device. */
uint32_t* init_and_send_input_context(uint32_t device_context_index, uint32_t port_num, uint32_t hci_port_speed)
{
        struct xhci_usb_device_slot* slot;

        slot = usb_slot_start;
        for(uint32_t i = 1; i < device_context_index; i++) {

                slot = slot->slot_next;

        } 

        slot->slot_num_endpoints = 1;

        slot->default_ep = (struct xhci_default_endpoint*) kmem_4k_allocate();
        zero_usbms_mem_6((uint32_t*)slot->default_ep);


        ica_list = ica_list_start;
        for(uint32_t l = 1; l < device_context_index; l++) {

                ica_list = ica_list->ica_next;

        }

        volatile uint32_t* temp = ica_list->ica_address;
        volatile uint32_t xhci_context_index = (volatile uint32_t)((32 << hci_context_size) / 4);

        /* a0 and a1 enabled */
        *(temp + 1) = 3; 

        /* at the slot context now
         * slot context. 0 stands for route string
         */
        *(temp + xhci_context_index) = 0 | ((0 << 20) | (1 << 27)); 
        *(temp + xhci_context_index + 1) = (port_num << 16);
        slot->port_number = port_num;
        slot->port_speed = xport_speed;
        slot->configuration_value = 0;

        /* default ep context. << 16 is the max packet size field */
        *(temp + xhci_context_index + xhci_context_index + 1) = 0 | ((3 << 1) | (4 << 3) | (0 << 16)); 
        slot->default_ep->xhci_max_packet_size = 0;
        slot->default_ep->dequeue_cycle_state = 1;
        uint32_t temp5 = (volatile uint32_t)kmem_4k_allocate();
        slot->default_ep->endpoint_trb_address = (uint32_t*)temp5;
        volatile uint32_t* temp9 = (volatile uint32_t*)kmem_4k_allocate();
        slot->default_ep->setup_trb = (struct trb*)temp9;
        volatile uint32_t* temp59 = (volatile uint32_t*)kmem_4k_allocate();
        slot->default_ep->data_stage_trb = (struct trb*)temp59;
        volatile uint32_t* temp95 = (volatile uint32_t*)kmem_4k_allocate();
        slot->default_ep->status_stage_trb = (struct trb*)temp95;
        uint32_t* temp959 = (uint32_t*) kmem_4k_allocate();
        slot->default_ep->data_stage_buffer = temp959;

        /* default ep context */ 
        *(temp + xhci_context_index + xhci_context_index + 2) = temp5 | 1; 
        /* default ep context */
        *(temp  + xhci_context_index + xhci_context_index + 3) = 0x00000000; 

        zero_usbms_mem_6((uint32_t*)temp5); 
        zero_usbms_mem_6((uint32_t*)temp9);
        zero_usbms_mem_6((uint32_t*)temp59);
        zero_usbms_mem_6((uint32_t*)temp95);

        /* Put the input context in the correct address. */
        temp = (volatile uint32_t*)hci_dcaba;
        temp += (device_context_index << xhci_64bit_capable); 


        volatile uint32_t temp595 = (volatile uint32_t)kmem_4k_allocate();  
        slot->device_slot_address = (volatile uint32_t*)temp595;

        if(xhci_64bit_capable) {

                *(temp) = temp595;
                zero_usbms_mem_6((uint32_t*)temp595);
                *(temp + 1) = 0x00000000;

        }
        else {
                *(temp) = temp595;
                zero_usbms_mem_6((uint32_t*)temp595);

        } 

        return ica_list->ica_address;

}

/* Puts the contents of a structure into the correct address space. */
void xhci_trb_to_address(volatile uint32_t* address_field, struct trb * trb_to_transfer)
{
        *(address_field) = trb_to_transfer->field_0;
        *(address_field + 1) = trb_to_transfer->field_1;
        *(address_field + 2) = trb_to_transfer->field_2;
        *(address_field + 3) = trb_to_transfer->field_3;
} 

/* Prints a buffer. */
void print_xhci_buffer(uint8_t* buffer_address, uint32_t buffer_length)
{    
        print("\nrequested buffer below:\n\n");
        for(uint32_t i = 0; i < buffer_length;i++) {

                print_hex_byte((uint8_t)(*(buffer_address + i)));
                print(" ");
                if((((i % 25) == 0)) && (i > 0)) {
                        print("\n");
                }

        }

        print("\n");
}

/* Performs USB transactions before the 
 * USB device is in a configured state. 
 */
void xhci_get_descriptor(uint32_t slot_index, uint32_t desc_type, uint32_t transf_length, uint32_t control_transfer_flags) // control_transfer_flags: 0x80 = set_configuration
{
        struct xhci_usb_device_slot* usb_slot;
        usb_slot = usb_slot_start;
        for(uint32_t i = 1; i < slot_index; i++) {

                usb_slot = usb_slot->slot_next;
        }


        volatile uint32_t* temp5 = 0;
        uint32_t temp = 0;
        volatile uint32_t td_packets_left = 0;
        uint8_t* temp9 = 0;

        if(xhci_get_descriptor_first_time_used) {

                td_packets_left = 1;
                /* at ep 0 trb address */
                temp = (8 << hci_context_size) + 2; 

                temp5 = usb_slot->device_slot_address;

                usb_slot->device_slot_address += temp;

                /* trb address as a pointer */
                trb_address_field = (uint32_t*)((uint32_t)((*(usb_slot->device_slot_address)) - 1)); 
                usb_slot->default_ep->ep0_trb_address = trb_address_field;

        }

        else {

                td_packets_left = (volatile uint32_t)(transf_length / (usb_slot->default_ep->xhci_max_packet_size));
                if((transf_length % usb_slot->default_ep->xhci_max_packet_size) != 0) {

                        td_packets_left++;

                }

        }

        /* setting up all 3 trbs. setup , data stage and status stage. setup, then in, then status */
        if(control_transfer_flags == 0) {

                usb_slot->default_ep->setup_trb->field_0 = 0 | (USBF_BMREQUESTTYPE_D2H | USBF_BMREQUESTTYPE_GET_DESCRIPTOR | (desc_type << 24));
                usb_slot->default_ep->setup_trb->field_1 = 0 | ((transf_length << 16));
                /* I guess the setup data length should always be 8 since it's a part of the USB protocol */
                usb_slot->default_ep->setup_trb->field_2 = XHCI_SETUP_TRB_SETUPDATA_LENGTH; 
                /* bit 16 below: 3 is IN data stage. 0 is no data stage */
                usb_slot->default_ep->setup_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_IMMEDIATE_DATA | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_INDATA); 


                usb_slot->default_ep->data_stage_trb->field_0 = (volatile uint32_t)(usb_slot->default_ep->data_stage_buffer);
                usb_slot->default_ep->data_stage_trb->field_1 = 0;

                /* 1 << 17 is the td count which should be 0 in the last trb */
                usb_slot->default_ep->data_stage_trb->field_2 = 0 | (transf_length | (td_packets_left << 17)); 
                /* 3 is IN data stage */
                usb_slot->default_ep->data_stage_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | (XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_OUTDATA)); 

        }

        /* set configuration */
        else if(control_transfer_flags == XHCI_SET_CONFIGURATION_1) {

                usb_slot->default_ep->setup_trb->field_0 = 0 | (USBF_BMREQUESTTYPE_H2D | USBF_BMREQUESTTYPE_SET_CONFIGURATION | (usb_slot->configuration_value << 16));
                usb_slot->default_ep->setup_trb->field_1 = 0 | ((transf_length << 16));
                usb_slot->default_ep->setup_trb->field_2 = XHCI_SETUP_TRB_SETUPDATA_LENGTH;
                usb_slot->default_ep->setup_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_IMMEDIATE_DATA | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_NODATA); 

        }

        /* set configuration 0 */
        else if(control_transfer_flags == XHCI_SET_CONFIGURATION_0) {
                usb_slot->default_ep->setup_trb->field_0 = 0 | (USBF_BMREQUESTTYPE_H2D | USBF_BMREQUESTTYPE_SET_CONFIGURATION | (0 << 24));
                usb_slot->default_ep->setup_trb->field_1 = 0 | ((transf_length << 16));
                usb_slot->default_ep->setup_trb->field_2 = XHCI_SETUP_TRB_SETUPDATA_LENGTH;
                usb_slot->default_ep->setup_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_IMMEDIATE_DATA | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_NODATA); 

        }

        /* get configuration 0 */
        else if(control_transfer_flags == XHCI_GET_CONFIGURATION) {

                usb_slot->default_ep->setup_trb->field_0 = 0 | (USBF_BMREQUESTTYPE_D2H | USBF_BMREQUESTTYPE_GET_CONFIGURATION  | (0 << 24));
                usb_slot->default_ep->setup_trb->field_1 = 0 | ((transf_length << 16));
                usb_slot->default_ep->setup_trb->field_2 = XHCI_SETUP_TRB_SETUPDATA_LENGTH;
                usb_slot->default_ep->setup_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_IMMEDIATE_DATA | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_INDATA); 

                usb_slot->default_ep->data_stage_trb->field_0 = (volatile uint32_t)(usb_slot->default_ep->data_stage_buffer);
                usb_slot->default_ep->data_stage_trb->field_1 = 0;

                usb_slot->default_ep->data_stage_trb->field_2 = 0 | (transf_length | (td_packets_left << 17)); 
                usb_slot->default_ep->data_stage_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | (XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_OUTDATA));

        }

        /* clear feature endpoint */
        else if ((control_transfer_flags & 0xfffc) == XHCI_CLEAR_FEATURE) {

                /* 0 << 24 = clear halt or function halt */
                usb_slot->default_ep->setup_trb->field_0 = 0 | ((control_transfer_flags & 0x0003) | USBF_BMREQUESTTYPE_CLEAR_FEATURE | (0 << 24)); 
                /* desc_type = endpoint */
                usb_slot->default_ep->setup_trb->field_1 = desc_type | ((transf_length << 16)); 
                usb_slot->default_ep->setup_trb->field_2 = XHCI_SETUP_TRB_SETUPDATA_LENGTH;
                usb_slot->default_ep->setup_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_IMMEDIATE_DATA | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_NODATA); 

        }

        /* mass storage reset */
        else if (control_transfer_flags == XHCI_MASS_STORAGE_RESET) {

                usb_slot->default_ep->setup_trb->field_0 = 0 | (33 | USBF_BMREQUESTTYPE_MS_RESET | (0 << 24));
                usb_slot->default_ep->setup_trb->field_1 = 1 | ((transf_length << 16)); 
                usb_slot->default_ep->setup_trb->field_2 = XHCI_SETUP_TRB_SETUPDATA_LENGTH;
                usb_slot->default_ep->setup_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_IMMEDIATE_DATA | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_NODATA);

        }
        /* get max lun */
        else if (control_transfer_flags == XHCI_GET_MAX_LUN) {

                usb_slot->default_ep->setup_trb->field_0 = 0 | (161 | USBF_BMREQUESTTYPE_GET_MAX_LUN | (0 << 24));
                usb_slot->default_ep->setup_trb->field_1 = 1 | ((transf_length << 16));
                usb_slot->default_ep->setup_trb->field_2 = XHCI_SETUP_TRB_SETUPDATA_LENGTH;
                usb_slot->default_ep->setup_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_IMMEDIATE_DATA | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_INDATA);

                usb_slot->default_ep->data_stage_trb->field_0 = (volatile uint32_t)(usb_slot->default_ep->data_stage_buffer);
                usb_slot->default_ep->data_stage_trb->field_1 = 0;

                usb_slot->default_ep->data_stage_trb->field_2 = 0 | (transf_length | (td_packets_left << 17)); 
                usb_slot->default_ep->data_stage_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | (XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_SETUP | XHCI_SETUP_TRB_OUTDATA)); 

        }


        if((control_transfer_flags == XHCI_SET_CONFIGURATION_1) || (control_transfer_flags == XHCI_SET_CONFIGURATION_0) || ((control_transfer_flags & 0xfffc) == XHCI_CLEAR_FEATURE) || (control_transfer_flags == XHCI_MASS_STORAGE_RESET)) {

                usb_slot->default_ep->status_stage_trb->field_0 = 0;
                usb_slot->default_ep->status_stage_trb->field_1 = 0;
                usb_slot->default_ep->status_stage_trb->field_2 = 0;
                usb_slot->default_ep->status_stage_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | (XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_STATUS | XHCI_STATUS_TRB_DIRECTION_IN)); 
            
        }

        else {

                usb_slot->default_ep->status_stage_trb->field_0 = 0;
                usb_slot->default_ep->status_stage_trb->field_1 = 0;
                usb_slot->default_ep->status_stage_trb->field_2 = 0;
                usb_slot->default_ep->status_stage_trb->field_3 = (XHCI_SETUP_TRB_CYCLE_BIT(1) | (XHCI_SETUP_TRB_IOC | XHCI_SETUP_TRB_STATUS | XHCI_STATUS_TRB_DIRECTION_OUT)); 
            
        }

        
        xhci_trb_to_address(trb_address_field, usb_slot->default_ep->setup_trb);


        ring_doorbell(4, 1);
        kernel_delay_100(200);
        handle_event(xhci_interrupt_status);
        kernel_delay_100(50);

        if((hci_er == hci_er_start) && (link_trb_flag == 0)) {
                temp = 0;
        }
        else if((hci_er == hci_er_start) && (link_trb_flag == 1)) {
                temp = (62*4); 
        }
        else {
                temp = ((hci_er - 16 - hci_er_start)/4);
        }

        uint32_t* xdevice_slot = get_hci_event_ring(0, temp);
        if(((((*(xdevice_slot + 2)) >> 24) & 0x000000ff)!= 1)) {
                print("\nsetup stage failed");
                print("\ndebug:\n");
                print_hex((uint32_t)(((*(xdevice_slot + 2)) >> 24) & 0x000000ff));
                return;

        }

        (*(trb_address_field + 3)) &= (~(1));

        trb_address_field += 4;
        if((control_transfer_flags == XHCI_SET_CONFIGURATION_1) || (control_transfer_flags == XHCI_SET_CONFIGURATION_0) || ((control_transfer_flags & 0xfffc) == XHCI_CLEAR_FEATURE) || (control_transfer_flags == XHCI_MASS_STORAGE_RESET)) {
                goto skipped_data_stage;
        }

        xhci_trb_to_address(trb_address_field, usb_slot->default_ep->data_stage_trb); 
        ring_doorbell(4, 1);
        kernel_delay_100(200);
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
                print("\ndata stage failed");
                return;
        }


        (*(trb_address_field + 3)) &= (~(1));
        trb_address_field += 4;
        skipped_data_stage:


        xhci_trb_to_address(trb_address_field, usb_slot->default_ep->status_stage_trb);
        ring_doorbell(4, 1);
        kernel_delay_100(200);
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

                print("\nstatus stage failed");
                return;

        }


        (*(trb_address_field + 3)) &= (~(1));
        trb_address_field += 4;
        if(((control_transfer_flags != XHCI_SET_CONFIGURATION_1) && (control_transfer_flags != XHCI_SET_CONFIGURATION_0)) && 
        (((control_transfer_flags & 0xfffc) != XHCI_CLEAR_FEATURE)) && (control_transfer_flags != XHCI_MASS_STORAGE_RESET)) {

                if(control_transfer_flags == 0x4) {

                        print("\nat get max lun:\n\n");

                }

                 print_xhci_buffer((uint8_t*)(usb_slot->default_ep->data_stage_buffer), transf_length); 

        }
        
        if(xhci_get_descriptor_first_time_used == 1) {

                temp9 = (uint8_t*)usb_slot->default_ep->data_stage_buffer;
                temp9 += 7;
                usb_slot->default_ep->xhci_max_packet_size = (uint32_t)(*(temp9));
                /* at the ep0 trb address */
                usb_slot->device_slot_address = temp5; 
                xhci_get_descriptor_first_time_used = 0;
        }

}

/* Calls the xhci_get_descriptor function with other parameters
 * for clear feature.
 */
void xhci_clear_feature(uint32_t slot_index, uint32_t type, uint32_t value) // type: 0 = device, 1 = interface, 2 = endpoint. value = if/ep number
{
        xhci_get_descriptor(slot_index, value, 0, (0x4000 | type));
}

/* Calls the xhci_get_descriptor function with other parameters
 * for get max lun.
 */
void xhci_get_max_lun(uint32_t slot_index)
{
        xhci_get_descriptor(slot_index, 0, 1, 0x4);
}

/* Calls the xhci_get_descriptor function with other parameters
 * for mass storage reset.
 */
void xhci_mass_storage_reset(uint32_t slot_index)
{
        xhci_get_descriptor(slot_index, 0, 0, 0x8);
}

/* Disables interrupts. */
void disable_hci_interrupts()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000000);
        xbaddr5 &= (~(1 << 2));
        write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000000, (const uint32_t)xbaddr5);
}

/* Initializes a device slot by getting a device descriptor and
 * sets the address of the device.
 */
void init_device_context(uint32_t context, uint32_t port_speed) 
{
        /* topology is 1, here, and protocol is 0 for usb 2 and usb 3 */
        uint32_t err = 1;
        err = hci_set_address(context, (volatile uint32_t)(1 << 9), ((uint32_t)init_and_send_input_context(context, xport_num, port_speed)));
        if(err) {
                print("\ncould not set default address"); 
                return; 
        }

        xhci_get_descriptor(actual_xdevice_slot, 1, 18, 0);

        struct xhci_usb_device_slot* slot = usb_slot_start;
        slot->configured = 0;
        zero_usbms_mem_6((uint32_t*)slot->default_ep->data_stage_buffer);

        volatile uint32_t* temp = ica_list->ica_address;
        *(temp + (16 << hci_context_size) + 1) |= (slot->default_ep->xhci_max_packet_size << 16);

        err = hci_set_address(context, (volatile uint32_t)(0 << 9), (uint32_t)ica_list->ica_address);
        if(err == 0) {

                trb_address_field = slot->default_ep->ep0_trb_address;  
                zero_usbms_mem_6((uint32_t*)slot->default_ep->ep0_trb_address);
        }

        kernel_delay_100(50);
}

/* Makes sure that a slot is configured
 * and that the device attached to the slot
 * is also then configured.
 */
void configure_slot(uint32_t slot_index)
{

        volatile uint32_t* temp59;
        volatile uint32_t temp95;
        /* Commented code: uint32_t num_interfaces = 0; */
        /* Commented code: uint32_t num_endpoints = 0; */
        ica_list = ica_list_start;

        /*  Please let the code which is commented out in this function be
         * here for now.
         * Commented code: for(uint32_t l = 1; l < slot_index; l++) {
         *          ica_list = ica_list->ica_next;
         * } 
         */

        struct xhci_usb_device_slot* slot;
        slot = usb_slot_start;

        /*  Commented code: for(uint32_t l = 1; l < slot_index; l++) {
                    slot = slot->slot_next;
            } 
         */

        /* The configuration descriptor */
        xhci_get_descriptor(actual_xdevice_slot, 2, 44, 0);
        uint8_t* temp9 = (uint8_t*)slot->default_ep->data_stage_buffer;
        slot->configuration_value = (uint32_t)(*(temp9 + 5));
        /*  Commented code: num_interfaces = (uint32_t)(*(temp9 + 3));  
            uint8_t* temp5 = temp9;                    
         */
        uint32_t temp5;
        /* Commented code: for(uint32_t i = 0; i < 32;i++) {

                slot->slot_endpoints[i] = 0;

          } */


        /* Commented code: temp5 += 9;
         * if interface descriptor. should parse until the lenght of a descriptor is 0 
         */
        /* Commented code: if(((*(temp5)) == 9) && ((*(temp5+1)) == 4)) { 

                num_endpoints = (uint32_t)(*(temp5 + 4));

           } */

        /* Commented code: slot->slot_num_endpoints += num_endpoints; */


        /* Commented code: if(num_endpoints) { */

                /* Sets some variables in structures once the device is configured. */
                temp95 = (volatile uint32_t)kmem_4k_allocate();
                slot->slot_endpoint = (struct xhci_endpoint*)temp95;
                zero_usbms_mem_6((uint32_t*)(temp95));
                slot->slot_endpoint = (struct xhci_endpoint*)kmem_4k_allocate();
                zero_usbms_mem_6((uint32_t*)(slot->slot_endpoint));
                slot_endpoint_start = slot->slot_endpoint;
                /* Direction in. Also do the max packet size and allocate trb address and trb.
                 * after that set the values in the address fields and configure slot.
                 * then do a set configuration.
                 */
                slot->slot_endpoint->data_direction = 1; 
                temp95 = (volatile uint32_t)kmem_4k_allocate();
                slot->slot_endpoint->transfer_trb = (struct trb*)temp95;
                zero_usbms_mem_6((uint32_t*)(temp95));
                slot->slot_endpoint->data_buffer = (volatile uint32_t*) kmem_4k_allocate();
                zero_usbms_mem_6((uint32_t*)(slot->slot_endpoint->data_buffer));
                // Commented code: slot->slot_endpoint->trb_address = (volatile uint32_t*)kmem_4k_allocate();
                // Commented code: zero_usbms_mem_6((uint32_t*)slot->slot_endpoint->trb_address);

                slot->slot_endpoint->transfer_data_protocol = 6;

       /* } */

        /* Commented code: temp5 += 9; 
           uint32_t w_break = 0; 
         */

        /* Commented code: while( ( (!((*(temp5)) == 0)) && (w_break == 0) ) ) {
                  if( ((*(temp5 + 1)) == 5) ) { 
        

                         Commented code: slot->slot_endpoint->endpoint_max_packet_size = (uint32_t)(*(temp5 + 4));
                           slot->slot_endpoint->endpoint_max_packet_size |= (uint32_t)((*(temp5 + 5)) << 8);
                           slot->slot_endpoint->ep_address = (uint32_t)(*(temp5 + 2));

                           if((*(temp5 + (*(temp5)))) == 0x30) {

                                   slot->slot_endpoint->maxburst_size = *(temp5 + (*(temp5)) + 2);
                                   temp5 += (*(temp5));
                                   temp5 += (*(temp5));

                           }

                   w_break = 1;
                   }

                   else {

                           temp5 += (*(temp5));


                   } 
           } */


       /* Commented code: for(uint32_t k = 0; k < num_interfaces; k++) {

                for(uint32_t i = 1; i < num_endpoints;i++) { */

                        temp95 = (volatile uint32_t)kmem_4k_allocate();
                        slot->slot_endpoint->xhci_endpoint_next = (struct xhci_endpoint*)temp95;
                        zero_usbms_mem_6((uint32_t*)(temp95));
                        // Commented code: slot->slot_endpoint->xhci_endpoint_next = (struct xhci_endpoint*)kmem_4k_allocate();
                        // Commented code:  zero_usbms_mem_6((uint32_t*)slot->slot_endpoint);
                        slot->slot_endpoint = slot->slot_endpoint->xhci_endpoint_next;
                        temp95 = (volatile uint32_t)kmem_4k_allocate();
                        slot->slot_endpoint->transfer_trb = (struct trb*)temp95;
                        zero_usbms_mem_6((uint32_t*)(temp95));
                        slot->slot_endpoint->data_buffer = (volatile uint32_t*) kmem_4k_allocate();
                        zero_usbms_mem_6((uint32_t*)(slot->slot_endpoint->data_buffer));
                        // Commented code: slot->slot_endpoint->trb_address = (volatile uint32_t*)kmem_4k_allocate();
                        // Commented code: zero_usbms_mem_6((uint32_t*)slot->slot_endpoint->trb_address);
                        slot->slot_endpoint->transfer_data_protocol = 2;

                        /* Commented code: if((i % 2) > 0) {
                                slot->slot_endpoint->data_direction = 0; // out
                          } 

                          while( ( (!((*(temp5)) == 0)) && (w_break == 0) ) ) { */


                       /* Commented code: if( ((*(temp5 + 1)) == 5) ) {

                                slot->slot_endpoint->endpoint_max_packet_size = (*(temp5 + 4));
                                slot->slot_endpoint->endpoint_max_packet_size |= ((*(temp5 + 5)) << 8);
                                slot->slot_endpoint->ep_address = (*(temp5 + 2));
                                slot->slot_endpoint->transfer_data_protocol = ((*(temp5 + 3)) & 3);                   

                                if((*(temp5 + (*(temp5)))) == 0x30) {
                                        slot->slot_endpoint->maxburst_size = *(temp5 + (*(temp5)) + 2);
                                        temp5 += (*(temp5));
                                        temp5 += (*(temp5));

                                }

                          }

                         else {

                                temp5 += (*(temp5));


                         } */

                        /* These values should be put into the ica_address fields
                         * for endpoint 1 in and endpoint 2 out. then a configure
                         * slot will be performed. look at the cic and the cie values.
                         */
              /* } 


        } */

        temp59 = ica_list->ica_address;
        *temp59 = 0;

        /* A Shortcut. can parse this more, later. also have another parameter
         * to this function with the endpoints
         */ 
        *(temp59 + 1) = 13; 
        temp5 = slot->configuration_value;
        if((hci_cic == 1) && (hci_cie == 1)) {
                *(temp59 + 7) = temp5;

        }

        *(temp59 + 16) |= XHCI_SLOT_CONTEXT_ENTRIES(3);

        /* now at endpoint 1 in */
        temp59 += ( (32 << hci_context_size)); 
        slot->slot_endpoint = slot_endpoint_start;
        temp95 = (volatile uint32_t)kmem_4k_allocate();
        zero_usbms_mem_6((uint32_t*)temp95);

        /* Going to take shortcuts here. interval (<< 16) should be
         * nak rate, which can be found in one of the descriptors.
         * setting to 0 meanwhile also: slot->endpoint = endpoint_start; 
         * << 1 is error count. <<3 is endpoint type. 2 is bulk. 
         * This is described in the configuration descriptor. 
         * Taking a shortcut here. 
         */
        (*(temp59 + 1)) |= (XHCI_ENDPOINT_CONTEXT_MAX_ERRCOUNT | (slot->slot_endpoint->transfer_data_protocol << 3) | XHCI_ENDPOINT_CONTEXT_MAX_BURSTSIZE | XHCI_ENDPOINT_CONTEXT_MAX_PACKETSIZE(1024)); 
        *(temp59 + 2) = temp95 | (1 << 0);
        /* in */
        slot->slot_endpoint->trb_address = (volatile uint32_t*)temp95;
        trb_address_bulk_in = (volatile uint32_t*)temp95;
        print("\n");
        print_hex((uint32_t)slot->slot_endpoint->trb_address);
        *(temp59 + 3) |= 0;
        *(temp59 + 4) |= XHCI_ENDPOINT_CONTEXT_AVERAGETRBSIZE(512);
        /* endpoint 2, out */
        temp59 = temp59 - (8 << hci_context_size); 
        slot->slot_endpoint = slot->slot_endpoint->xhci_endpoint_next;

        (*(temp59 + 1)) |= (XHCI_ENDPOINT_CONTEXT_MAX_ERRCOUNT | (slot->slot_endpoint->transfer_data_protocol << 3) | XHCI_ENDPOINT_CONTEXT_MAX_BURSTSIZE | XHCI_ENDPOINT_CONTEXT_MAX_PACKETSIZE(1024));
         temp95 = (volatile uint32_t)kmem_4k_allocate();
        zero_usbms_mem_6((uint32_t*)temp95);
        *(temp59 + 2) = temp95 | (1 << 0);
        slot->slot_endpoint->trb_address = (volatile uint32_t*)temp95; // out
        trb_address_bulk_out = (volatile uint32_t*)temp95;

        print("\n");
        print_hex((uint32_t)slot->slot_endpoint->trb_address);
        *(temp59 + 3) |= 0;
        *(temp59 + 4) |= XHCI_ENDPOINT_CONTEXT_AVERAGETRBSIZE(31); 

        uint32_t configure_error = hci_configure_slot(slot_index, (0 << 16), (volatile uint32_t)ica_list->ica_address);
        if(configure_error) {

                print("\ncould not configure the slot");
                return;
        }

        slot->configured = 1;

        xhci_get_descriptor(slot_index, 0, 0, XHCI_SET_CONFIGURATION_1);

        kernel_delay_100(50);

}  

/* This function is unused. */
void configure_slot_set_configuration(uint32_t d_slot_index)
{
        /* nothing here yet */
        print("hi");
}

/* Rings the doorbell so that the HC performs 
 * something, or things. 
*/
void ring_doorbell(uint32_t offset, uint32_t value)
{

        write_dword((const uint32_t)xhci_doorbell_base, (const uint32_t)offset, (const uint32_t)value);
}

/* Prints the first event ring after a transaction. */
void print_event_ring_zero(uint32_t ev_offset)
{  
        uint32_t* event_ring_zero_address = (uint32_t*)((uint32_t)((*((uint32_t*)hci_er_segment_table_ba)) & 0xfffffff0));
        print("\nevent ring zero address according to usage: "); 
        print_hex((uint32_t)hci_er);
        print("\n");
        for(uint32_t i = ev_offset; i < (ev_offset + 4); i++) {
                dec_to_bin((uint32_t)(*(event_ring_zero_address + i)));
                print("\n");
        }

        print("\n");
}


void print_hci_scratchpad_size()    
{
        print("\nscratchpad buffer array size: ");
        print_hex((uint32_t)hci_scratchpad_size);
}

/* Checks if the command ring is running. */
uint8_t check_command_ring_running()
{
        /* Command Ring Control register. */
        volatile uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000018);
        xbaddr5 >>= 3;
        xbaddr5 &= 0x00000001;
        return (uint8_t)xbaddr5;
}

/* Requests ownership of the XHC. */
void request_bios_handsoff()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_ext_cap_ptr,(const uint32_t)0x00000004);
        if((!(xbaddr5 & (1 << 24))) && (!(xbaddr5 & (1 << 16)))) {

                xbaddr5 ^= (1 << 24);
        }

        write_dword((const uint32_t)hci_ext_cap_ptr,(const uint32_t)0x00000004, (const uint32_t)xbaddr5);
}


/* Prepares to run some commands. */
uint32_t init_hci_command(volatile uint32_t hci_cmdring_command, volatile uint32_t device_slot, volatile uint32_t endpoint_id, volatile uint32_t cmd_flags, volatile uint32_t ep_flags, uint32_t er_counter, uint32_t ic_address)
{

        uint32_t temp;
        uint32_t slot_err = 1;
        /* A special case is when we don't have a slot yet. */
        if(hci_cmdring_command == XHCI_COMMAND_ENABLE_SLOT) {
               slot_err = write_cmd_on_hc_command_ring(XHCI_COMMAND_ENABLE_SLOT, 0, 0, 0, 0, 0, 0);

                if((hci_er == hci_er_start) && (link_trb_flag == 0))
                        temp = 0;
                else if((hci_er == hci_er_start) && (link_trb_flag == 1))
                        temp = (62*4);
                else
                        temp = ((hci_er - 16 - hci_er_start)/4);

                uint32_t* xdevice_slot = get_hci_event_ring(0, temp);
                if(  (( ((*(xdevice_slot + 2)) >> 24) & 0x000000ff)== 1)  ) {

                        actual_xdevice_slot = (uint32_t)((*(xdevice_slot + 3)) >> 24); 

                        return 0;

                }
                else {

                        print("\nhci driver or hci error on enabling slot\n");

                        *(xhci_commandring_base) = 0; // xslot_id usually 1, in this case
                        *(xhci_commandring_base + 1) =  0;
                        *(xhci_commandring_base + 2) = 0x00000000;
                        *(xhci_commandring_base + 3) = 0; // xslot_id usually 1, in this case
                         if(xhci_interrupt_status == 1) {
                                xhci_interrupt_status = 0;
                        }
                        return 1;

                }
        }

        else {
                /* The general handling of commands. */
                slot_err = write_cmd_on_hc_command_ring(hci_cmdring_command, device_slot, endpoint_id, 0, cmd_flags, ep_flags, ic_address);
        }

        if(xhci_interrupt_status == 1) {
                xhci_interrupt_status = 0;
        }
        /* Debugs a value. */
        print("\nslot err: ");
        printi((uint32_t)slot_err);

        return slot_err;

}

/* Attempts to enable a slot. */
uint32_t hci_enable_slot()
{
        uint32_t error = init_hci_command(XHCI_COMMAND_ENABLE_SLOT, 0, 0, 0, 0, 0, 0);
        return error;
}

/* Attempts to disable a slot. */
uint32_t hci_disable_slot(uint32_t slot)
{
        uint32_t error = init_hci_command(XHCI_COMMAND_DISABLE_SLOT, slot, 0, 0, 0, 0, 0);
        return error;
}

/* Attempts to set an address to a device. */
uint32_t hci_set_address(uint32_t slot_to_be_addressed, volatile uint32_t block_flag, volatile uint32_t i_context_address)
{
        uint32_t error = init_hci_command(XHCI_COMMAND_ADDRESS_DEVICE, slot_to_be_addressed, 0, block_flag, 0, 0, i_context_address);
        return error;
}

/* Attempts to configure a slot. */
uint32_t hci_configure_slot(uint32_t slot_to_be_configured, volatile uint32_t deconfigure_flag, volatile uint32_t input_context_address)
{
        uint32_t error = init_hci_command(XHCI_COMMAND_CONFIGURE_ENDPOINT, slot_to_be_configured, 0, 0 , deconfigure_flag, 0, input_context_address);
        return error;
}

/* Attempts to reset a slot. */
uint32_t reset_slot(uint32_t slot_to_be_reset)
{
        uint32_t error = init_hci_command(XHCI_COMMAND_RESET_DEVICE, slot_to_be_reset, 0, 0, 0, 0, 0);
        return error;
}

/* Attempts to reset an endpoint. */
uint32_t reset_endpoint (uint32_t slot_to_be_handled, uint32_t endpoint_to_be_reset)
{
        uint32_t error = init_hci_command(XHCI_COMMAND_RESET_ENDPOINT, slot_to_be_handled, 0, 0, endpoint_to_be_reset, 0, 0);
        return error;
}

/* Performs a XHCI no-op command. */
uint32_t xhci_no_op()
{
        uint32_t error = init_hci_command(XHCI_COMMAND_NOOP, 0, 0, 0, 0, 0, 0);
        return error;
}

/* The general function to write commands on the
 * command ring.
 */
uint32_t write_cmd_on_hc_command_ring(volatile uint32_t command, volatile uint32_t xslot_id, volatile uint32_t xendpoint_id, volatile uint32_t ep_configuration_value, volatile uint32_t ctrb_flags, volatile uint32_t ctrb_ep_flags, uint32_t input_context_address) 
{

        /* 9-23 and 6, are the usual xhc commands. vendor defined: 48-63 
         * flag 0x00008000 = block address 
         * flag = 0x00004000; several endpoints and the next flag stands for
         * which endpoints, in bits, in quesiton 
         */
        
        uint32_t event_err = 1;
        volatile uint32_t temp;
        volatile uint32_t temp5;
        

        /* xslot_id usually 1, in this case */
        if(command == XHCI_COMMAND_NOOP) {

                *(xhci_commandring_base + 3) = (volatile uint32_t)XHCI_COMMAND_NOOP; 
                *(xhci_commandring_base + 4) =  (volatile uint32_t)xhci_commandring_base;
                *(xhci_commandring_base + 5) = 0x00000000;
                *(xhci_commandring_base + 6) = 0x00000000;
                *(xhci_commandring_base + 7) = (volatile uint32_t)(XHCI_LINK_TOGGLE_CYCLE(1) | XHCI_TAG_LINK_TRB); 
                
        }
        if(command == XHCI_COMMAND_ENABLE_SLOT) {

                *(xhci_commandring_base + 7) = (volatile uint32_t)1;
                *(xhci_commandring_base) = (volatile uint32_t)0; 
                *(xhci_commandring_base + 1) = (volatile uint32_t)0;
                *(xhci_commandring_base + 2) =  (volatile uint32_t)0;
                *(xhci_commandring_base + 3) = (volatile uint32_t)(0 | XHCI_COMMAND_ENABLE_SLOT);

                xhci_commandring_base += 4;
        }
        else if (command == XHCI_COMMAND_DISABLE_SLOT) {
                *(xhci_commandring_base + 7) = (volatile uint32_t)1;
                *(xhci_commandring_base) = (volatile uint32_t)0; 
                *(xhci_commandring_base + 1) = (volatile uint32_t)0;
                *(xhci_commandring_base + 2) =  (volatile uint32_t)0;
                *(xhci_commandring_base + 3) = (volatile uint32_t)(0 | ((xslot_id << 24) | XHCI_COMMAND_DISABLE_SLOT));


                xhci_commandring_base += 4;                
        }

        /* if there is an address command etc. copy the device context structure into an input context structure */
        else if (command == XHCI_COMMAND_ADDRESS_DEVICE) {

                *(xhci_commandring_base + 7) = 1;
                temp5 = (volatile uint32_t)input_context_address;

                *(xhci_commandring_base) = temp5;
                *(xhci_commandring_base + 3) = (0 | ctrb_flags | XHCI_COMMAND_ADDRESS_DEVICE | (xslot_id << 24));
                xhci_commandring_base += 4;

        }

        else if (command == XHCI_COMMAND_CONFIGURE_ENDPOINT) {

                *(xhci_commandring_base + 7) = 1;
                temp5 = (volatile uint32_t)input_context_address;

                *(xhci_commandring_base) = temp5;
                /* ep_flags can be 0 << 9 for configure */
                *(xhci_commandring_base + 3) = (0 | ctrb_ep_flags | XHCI_COMMAND_CONFIGURE_ENDPOINT | (xslot_id << 24)); 
                xhci_commandring_base += 4;
        }

        else if (command == XHCI_COMMAND_RESET_ENDPOINT) {

                *(xhci_commandring_base + 7) = 1;


                *(xhci_commandring_base) = 0;
                *(xhci_commandring_base + 1) = 0;
                *(xhci_commandring_base + 2) = 0;

                /* ep_flags can be 0 << 16 for reset. device context index */
                *(xhci_commandring_base + 3) = ( 0 | ctrb_ep_flags | XHCI_COMMAND_RESET_ENDPOINT | (xslot_id << 24));
                xhci_commandring_base += 4;

        }

        else if (command == XHCI_COMMAND_STOP_ENDPOINT) {

                temp = *(xhci_commandring_base + 3);
                temp |= (volatile uint32_t)((1 | XHCI_COMMAND_STOP_ENDPOINT | (xendpoint_id << 16) | (1 << 23) | (xslot_id << 24)));
                *(xhci_commandring_base + 3) = temp;

                temp = *(xhci_commandring_base + 4);
                temp |= (uint32_t)xhci_commandring_base;
                *(xhci_commandring_base + 4) =  temp;
                *(xhci_commandring_base + 5) = 0x00000000;

                temp = *(xhci_commandring_base + 6);
                temp &= 0x07ffffff;
                *(xhci_commandring_base + 6) = temp;
                temp = *(xhci_commandring_base + 7);

                temp |= (1 | XHCI_TAG_LINK_TRB);
                *(xhci_commandring_base + 7) = temp;
        }

        else if (command == XHCI_COMMAND_RESET_DEVICE) {

                *(xhci_commandring_base + 7) = 1;

                *(xhci_commandring_base) = 0;
                *(xhci_commandring_base + 1) = 0;
                *(xhci_commandring_base + 2) = 0;

                *(xhci_commandring_base + 3) = (0 | XHCI_COMMAND_RESET_DEVICE | (xslot_id << 24));

                xhci_commandring_base += 4;

        }

        ring_doorbell(0, 0);
        kernel_delay_100(50);
        event_err = handle_event(xhci_interrupt_status);
        kernel_delay_100(50);
        return event_err;

}

/* This function acts as the general interrupt handler 
 * for the XHC.
 */
void xhci_usb_handler()
{

        volatile uint32_t xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004);
        if((xbaddr9 & 0x00000004)) {

                xbaddr9 |= 0x00000004;
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004, (const uint32_t)xbaddr9);


        }

        /* event interrupt */
        else if((xbaddr9 & 0x00000008)) {
                xhci_interrupt_status = 1;
                xbaddr9 |= 0x00000008;
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004, (const uint32_t)xbaddr9);
                print("\nwritten to usb status register\n");
                xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000020);
                xbaddr9 |= 1;
                write_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000020, (const uint32_t)xbaddr9);
                print("\nwritten to interrupter\n");
                hci_er_dq_ptr += 0x10;
                hci_er += 0x10;
                write_event_dequeue_pointer(hci_er_dq_ptr);
                print("\nwritten to dequeue pointer register\n");

        }

        else if((xbaddr9 & 0x00000010)) {

                xbaddr9 |= 0x00000010;
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004, (const uint32_t)xbaddr9);


        }

        else if((xbaddr9 & 0x00000400)) {

                xbaddr9 |= 0x00000400;
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004, (const uint32_t)xbaddr9);


        }

}

/* Returns an event ring. */
uint32_t* get_hci_event_ring(uint32_t hci_ert_offset, uint32_t hci_er_offset)
{
        uint32_t* ptr = (uint32_t*)(hci_er_segment_table_ba + hci_ert_offset);
        uint32_t* another_ptr = (uint32_t*)((uint32_t)(*ptr));
        another_ptr += hci_er_offset;
        
        return (uint32_t*)((uint32_t)((uint32_t)another_ptr & (uint32_t)0xfffffff0));
}

/* Handles the dequeue pointer for events. */
uint32_t write_event_dequeue_pointer(uint32_t pointer_to_add)
{
        uint32_t ret_value = 1;
        uint32_t* xdevice_slot = get_hci_event_ring(0, ((hci_er - hci_er_start)/4));
        if(  (( ((*(xdevice_slot + 2)) >> 24) & 0x000000ff)== 1)  )
                ret_value = 0;
        else
                ret_value = 1;


        hci_er += 16;
        hci_er_dq_ptr += 16;

        volatile uint32_t xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000038);
        xbaddr9 |= (pointer_to_add | 8);
        write_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000038, (const uint32_t)xbaddr9);


        xdevice_slot = get_hci_event_ring(0, ((hci_er - hci_er_start)/4));
                /* link trb */
                if( (((*(xdevice_slot + 3)) >> 10) & 0x0000003f) == 6) {
                        hci_er = (*(xdevice_slot));
                        hci_er_start = hci_er;
                        hci_er_dq_ptr = hci_er;
                        print("\nnon interrupt handler: link trb found\n");
                        link_trb_flag = 1;
                }

        return ret_value;
}


/* Attempts to deconfigure, set address to a 0, disable a slot and reset
 * the XHC, etc.
 */
void hci_reset_sequence(uint32_t d_slot_index)
{
        struct xhci_usb_device_slot* slot = usb_slot_start;
        ica_list = ica_list_start;

        uint32_t configure_error = hci_configure_slot(d_slot_index, (1 << 16), (volatile uint32_t)ica_list->ica_address);

        kernel_delay_100(50);

        if(configure_error) {

                print("\ncould not deconfigure the slot");
                return; 
        }

        slot->configured = 0;
        xhci_get_descriptor(d_slot_index, 0, 0, 0x800);
        kernel_delay_100(50);
        xhci_get_descriptor(d_slot_index, 0, 1, 0x8000);
        kernel_delay_100(50);

        if(actual_xdevice_slot) {

                slot_error = hci_disable_slot(actual_xdevice_slot);
                kernel_delay_100(50);
                actual_xdevice_slot = 0;
                if(slot_error == 0)
                         hci_enable_slot();
                slot_error = 1;
                if(actual_xdevice_slot) {

                        slot_error = hci_set_address(actual_xdevice_slot, (volatile uint32_t)(1 << 9), (uint32_t)ica_list->ica_address);
                        kernel_delay_100(50);
                }
                else {

                        print("\n\nslot has not the required value\n\n");
                }
                if(slot_error == 0) {

                        slot_error = hci_disable_slot(actual_xdevice_slot);

                        if(slot_error) {
                                print("\ncould not disable slot");
                        } 
                }
                kernel_delay_100(50);

        } 

        reset_port(1);
        kernel_delay_100(85);

        hci_clear_interrupts();
        handle_ev_non_dq(xhci_interrupt_status);
        kernel_delay_100(50);

        hci_clear_interrupts();
        handle_ev_non_dq(xhci_interrupt_status);
        kernel_delay_100(50);
        free_hci_mem_resources();            
        stop_xhci();
        kernel_delay_100(5);
        reset_xhci();
        kernel_delay_100(50);

        *(xhci_commandring_base) = (volatile uint32_t)0; 
        *(xhci_commandring_base + 1) = (volatile uint32_t)0;
        *(xhci_commandring_base + 2) =  (volatile uint32_t)0;
        *(xhci_commandring_base + 3) =  (volatile uint32_t)0;
        *(xhci_commandring_base + 4) = (volatile uint32_t)0x00000000;
        *(xhci_commandring_base + 5) = (volatile uint32_t)0x00000000;
        *(xhci_commandring_base + 6) = (volatile uint32_t)0; 
        *(xhci_commandring_base + 7) = (volatile uint32_t)0;
        *(xhci_commandring_base + 8) = (volatile uint32_t)0;
        *(xhci_commandring_base + 9) =  (volatile uint32_t)0;
        *(xhci_commandring_base + 10) =  (volatile uint32_t)0;
        *(xhci_commandring_base + 11) = (volatile uint32_t)0x00000000;
}

/* Enables interrupts by setting the IE bit. */
void enable_interrupts_and_interrupter()
{
        /* Checks if we can set the IE bit on the interrupter register. */
        volatile uint32_t xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)xhci_runtime_base, (const uint32_t)0x00000020);
        if((xbaddr9 & 0x00000002)) {

                xbaddr9 |= 0x00000002;
                write_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000020, (const uint32_t)xbaddr9);

        } 
        /* Allows general interrupts on the host system. Msi 
         * for example.
         */
        xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000000);
        if((xbaddr9 & 0x00000004)) {

                xbaddr9 |= 0x00000004;
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000000, (const uint32_t)xbaddr9);

        }
}

/* Clears the event handler busy bit. */
void write_interrupter_not_busy()
{
        volatile uint32_t xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)xhci_runtime_base, (const uint32_t)0x00000038);
        xbaddr9 |= 0x00000008;
        write_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000038, (const uint32_t)xbaddr9);
}

/* Clears some interrupts. */
void hci_clear_interrupts()
{
        /* Checks and eventually clears the host system error bit
         * on the status register.
        */
        volatile uint32_t xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004);
        if((xbaddr9 & 0x00000004)) {

                xbaddr9 |= 0x00000004;
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004, (const uint32_t)xbaddr9);

        }
        /* Checks and eventually clears the port change detect bit
         * on the status register.
        */
       if((xbaddr9 & 0x00000010)) {

                xbaddr9 |= 0x00000010;
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004, (const uint32_t)xbaddr9);

        }
        /* Checks and eventually clears the controller not ready bit
         * on the status register.
        */
        if((xbaddr9 & 0x00000800)) {

                xbaddr9 |= 0x00000800;
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004, (const uint32_t)xbaddr9);

        }
}

/* Checks for interrupts and if an event has occured.
 * Ignores the dequeue pointer register.
 */
void handle_ev_non_dq(uint8_t interrupt_handl)
{
        if(xhci_interrupt_status == 0) {
                volatile uint32_t xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004);


                if((xbaddr9 & 8)) {   

                        xbaddr9 |= 0x00000008;
                        write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004, (const uint32_t)xbaddr9);

                }

                xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000020);
                if((xbaddr9 & 1)) {

                        xbaddr9 |= 1;
                        write_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000020, (const uint32_t)xbaddr9);

                }
        }
}

/* Checks for interrupts and if an event has occured. */
uint32_t handle_event(uint8_t interrupt_handling)
{
        uint32_t event_error = 1;
        if(xhci_interrupt_status == 0) {

                volatile uint32_t xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004);

                /* Checks for an event interrupt. */
                if((xbaddr9 & 8)) {   

                        xbaddr9 |= 0x00000008;
                        write_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000004, (const uint32_t)xbaddr9);
                }

                kernel_delay_100(50);

                xbaddr9 = (volatile uint32_t)read_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000020);
                /* Checks if an interrupt is pending. */
                if((xbaddr9 & 1)) {

                        xbaddr9 |= 1;
                        write_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000020, (const uint32_t)xbaddr9);

                }

                kernel_delay_100(50);
                /* Writes on the dequeue pointer register. */
                event_error = write_event_dequeue_pointer(hci_er_dq_ptr);

        }

        return event_error;
}

/* Initializes the HC and performs some tests with it.
 * The function calls many other functions that
 * are defined in this file. It will attempt to 
 * go all the way to configure a device.
 */
uint32_t test_init_hci()
{
        slot_error = 1;
        actual_xdevice_slot = 0;
        xhci_interrupt_status = 0;
        hci_context_size = 0;
        xport_speed = 0;
        xport_num = 0;
        init_hci_mem_resources();
        store_hci_operational_base();

        stop_xhci();
        kernel_delay_100(50);

        store_hci_ext_cap_ptr();

        request_bios_handsoff();
        kernel_delay_100(50);

        reset_xhci();
        kernel_delay_100(50);
        hci_clear_interrupts();
        kernel_delay_100(50);
        store_hci_operational_base();
        /* Finds out where the runtime base is. */
        store_xhci_runtimebase();
        store_hci_context_size();

        handle_ev_non_dq(xhci_interrupt_status);

        store_xhci_maxports();
        store_check_xhci_64_bit_capable();
        store_xhci_max_interrupters();

        store_hci_cic();
        store_hci_cie();

        store_xhci_max_device_slots();
        store_xhci_max_er_segment_table_slots();
        store_xhci_doorbellbase();
        enable_interrupts_and_interrupter();

        handle_ev_non_dq(xhci_interrupt_status);
        write_store_xhci_commandringbase();
        store_event_ring_segment_table_size();
        store_event_ring_table_and_rings();
        store_hci_scratchpad_size();
        store_hci_scratchpad_pagesize();

        store_hci_dcaba();
        write_store_config_register((const uint32_t)xhci_max_device_slots);

        start_xhci();

        kernel_delay_100(50);
        xhci_portx_enumeration(1);
        kernel_delay_100(50);

        hci_clear_interrupts();
        kernel_delay_100(50);
        handle_ev_non_dq(xhci_interrupt_status);
        kernel_delay_100(50);
        /* Attempts to enable a slot. */
        uint32_t slot_error = hci_enable_slot();

        /* Attempts to address a device and to configure
         * a device on the slot.
         */
        if(slot_error == 0) {
                init_device_context(actual_xdevice_slot, xport_speed);
                kernel_delay_100(50);

                configure_slot(actual_xdevice_slot);
                kernel_delay_100(5);
                return 0;

        }


        reset_port(1);
        kernel_delay_100(85);
        hci_clear_interrupts();
        handle_ev_non_dq(xhci_interrupt_status);

        kernel_delay_100(50);
        free_hci_mem_resources();            
        stop_xhci();
        kernel_delay_100(5);
        reset_xhci();
        kernel_delay_100(50);


        return 1;          
}

/* Frees the hce_mem_resources array. */
void free_hci_mem_resources()
{
        for(uint32_t i = hci_mem_resources_index+1; i != 0; i--) {
                free_mem_uint(hci_mem_resources[i-1]);
        }
}

