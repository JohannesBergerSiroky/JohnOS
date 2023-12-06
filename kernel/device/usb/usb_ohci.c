/* This is the kernel's OHCI driver. */

#include "../../types.h"
#include "../pci.h"
#include "../../system.h"
#include "../../mem.h"
#include "../../isr.h"
#include "../../irq.h"
#include "../../graphics/screen.h"
#include "usb.h"
#include "usb_ohci.h"
#include "../../timer.h"
#include "usb_hubs.h"


/* Sets some values in the command register on 
 * the PCI Configuration Space for the OHC.
 */
void set_ohci_commandreg()
{
        uint16_t temp_cmd = PCI_Config_RW(found_ohci_bus,found_ohci_device,found_ohci_function,4);
        uint16_t temp_cmd2 = PCI_Config_RW(found_ohci_bus,found_ohci_device,found_ohci_function,6);
        uint32_t temp_cmd3 = (uint32_t)(temp_cmd2*65536);

        uint32_t temp_cmd4 = (uint32_t)temp_cmd;
        uint32_t temp_cmd5 = temp_cmd3 + temp_cmd4;
        temp_cmd5 &= (~(1));
        temp_cmd5 |= 0x106;


        uint32_t address = (uint32_t)((found_ohci_bus << 16) | (found_ohci_device << 11) | (found_ohci_function << 8) | (4 & 0xfc) | ((uint32_t)0x80000000));
        outportl(0xCF8, address);
        outportl(0xCFC, temp_cmd5);
}

/* Wites a default value to the frame interval register. */
void write_fm_interval()
{
        const uint32_t temp = 0 | ((0 << 31) | (0x2778 << 16) | 11999); 
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000034, temp);
}

void toggle_fm_interval()
{
        uint32_t fmintervaltmp = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000034);
        if(((fmintervaltmp >> 31) & 0x00000001) == 1)
                fmintervaltmp &= (~(1 << 31));
        else
                fmintervaltmp |= 0x80000000;

        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000034, (const uint32_t)fmintervaltmp);
}

/* for ohci's interrupt status register: */

/* schedule overrun */
void resetpc_ohci_so() 
{
        uint32_t ohci_intsts = get_ohci_intstatus();
        ohci_intsts |= 1; // 1 << 0
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c, (const uint32_t)ohci_intsts);
}

/* write back done head */
void resetpc_ohci_wdh()  
{
        uint32_t ohci_intsts = get_ohci_intstatus();
        ohci_intsts |= 2; // 1 << 0
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c, (const uint32_t)ohci_intsts);
}

/* start of frame */
void resetpc_ohci_sf() 
{
        uint32_t ohci_intsts = get_ohci_intstatus();
        ohci_intsts |= 4; 
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c, (const uint32_t)ohci_intsts);
}

/* resume detected */
void resetpc_ohci_rd() 
{
        uint32_t ohci_intsts = get_ohci_intstatus();
        ohci_intsts |= 8;
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c, (const uint32_t)ohci_intsts);
}

/* uncoverable error */
void resetpc_ohci_ue() 
{
        uint32_t ohci_intsts = get_ohci_intstatus();
        ohci_intsts |= 16;
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c, (const uint32_t)ohci_intsts);
}

/* frame number overflow */
void resetpc_ohci_fno() 
{
        uint32_t ohci_intsts = get_ohci_intstatus();
        ohci_intsts |= 32;
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c, (const uint32_t)ohci_intsts);
}

/* root hub status change */
void resetpc_ohci_rhsc() 
{
        uint32_t ohci_intsts = get_ohci_intstatus();
        ohci_intsts |= 64;
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c, (const uint32_t)ohci_intsts);
}

/* ownership change */
void resetpc_ohci_oc() 
{
        uint32_t ohci_intsts = get_ohci_intstatus();
        ohci_intsts |= 0x40000000;
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c, (const uint32_t)ohci_intsts);
}

/* This function acts as the general OHCI 
 * interrupt handler.
 */
void ohci_usb_handler()
{
        uint32_t ohci_intstatus = get_ohci_intstatus();
        if(ohci_intstatus & 1) {
                resetpc_ohci_so();
                print("\n\nSchedule overrun.");
        }

        if(ohci_intstatus & 2) {
                copy_done_queue();
                print("\ntransfer descriptor to done queue.");
                resetpc_ohci_wdh();
        }
        if(ohci_intstatus & 4)
                resetpc_ohci_sf();
        if(ohci_intstatus & 8) {
                resetpc_ohci_rd();
                print("\n\nResume detected.");
        }
        if(ohci_intstatus & 16) {
                resetpc_ohci_ue();
                print("\n\nUncovered system error! Resetting the ohc.");
                print(csptr);
        }

        /* Frame number overflow */
        if(ohci_intstatus & 32) {
                resetpc_ohci_fno();

        }
        if(ohci_intstatus & 64) {
                resetpc_ohci_rhsc();
        }
        if(ohci_ports_busy == 0) {
                Get_Ohci_Port_Status();
        }
        if(ohci_intstatus & 0x40000000) {
                resetpc_ohci_oc();

        }
}

/* Initializes som transfer descriptors 
 * and endpoint descriptors
 * at hard coded addresses. These are the
 * basis for getting a device descriptor
 * in another function.
 */
void init_ohci_descriptors() 
{

        ohci_hcca = (uint32_t)kmem_4k_allocate();
        uint32_t* temp = (uint32_t*)ohci_hcca;
        zero_usbms_mem_6(temp);

        done_queue_location = (uint8_t*)ohci_hcca;
        done_queue_location += 0x84; 

        done_queue_copy = (uint32_t)kmem_4k_allocate();
        temp = (uint32_t*)done_queue_copy;
        zero_usbms_mem_6(temp);

        done_queue_index = 0;
        ohci_bulk_td1 = (struct general_transfer_descriptor*)kmem_4k_allocate();


        ohci_setup_ed = (struct endpoint_descriptor*)0x400000;
        zero_usbms_mem_6((uint32_t*)0x400000);
        ohci_setup_td1 = (struct general_transfer_descriptor*)0x400010;
        ohci_setup_td2 = (struct general_transfer_descriptor*)0x400020;
        ohci_setup_td3 = (struct general_transfer_descriptor*)0x400030;
        ohci_setup_td4 = (struct general_transfer_descriptor*)0x400040;
        ohci_setup_td5 = (struct general_transfer_descriptor*)0x400050;
        ohci_setup_td8 = (struct general_transfer_descriptor*)0x400060;//(struct general_transfer_descriptor*)td1_buffer;
        ohci_setup_td9 = (struct general_transfer_descriptor*)0x400070;
        td1_buffer = (volatile uint8_t*)0x400070;
        td2_buffer = (volatile uint8_t*)0x400080;
        td3_buffer = (volatile uint8_t*)0x400090;
        td4_buffer = (volatile uint8_t*)0x4000a0;
        td5_buffer = (volatile uint8_t*)0x4000b0;
        td8_buffer = (volatile uint8_t*)0x4000c0;
        ohci_setup_ed->endpoint_info = 0x00404000;
        ohci_setup_ed->tail_pointer = (volatile uint32_t)1;
        ohci_setup_ed->head_pointer = (volatile uint32_t)1;
        ohci_setup_ed->next_ed = 0;

        ohci_setup_td1->descriptor_info = 0;
        ohci_setup_td1->cbp = (volatile uint8_t*)td1_buffer;
        ohci_setup_td1->next_td = (volatile uint32_t)ohci_setup_td2;
        ohci_setup_td1->buffer_end = (volatile uint32_t)((volatile uint32_t)td1_buffer + (volatile uint32_t)7);

        ohci_setup_td2->descriptor_info = 0;
        ohci_setup_td2->cbp = (volatile uint8_t*)td2_buffer; // watch out for the buffers here. maybe they should point to td8 or td8+0x10 etc
        ohci_setup_td2->next_td = (volatile uint32_t)ohci_setup_td3;
        ohci_setup_td2->buffer_end = (volatile uint32_t)((volatile uint32_t)td2_buffer + (volatile uint32_t)7);

        /* Commented code: ohci_setup_td2->descriptor_info = 0;
         * ohci_setup_td2->cbp = 0;
         * ohci_setup_td2->next_td = (struct general_transfer_descriptor*)ohci_setup_td3;
         * ohci_setup_td2->buffer_end = 0; 
         */

        ohci_setup_td3->descriptor_info = 0;
        ohci_setup_td3->cbp = (volatile uint8_t*)td3_buffer;
        ohci_setup_td3->next_td = (volatile uint32_t)ohci_setup_td4;
        ohci_setup_td3->buffer_end = (volatile uint32_t)((volatile uint32_t)td3_buffer + (volatile uint32_t)7);

        ohci_setup_td4->descriptor_info = 0;
        ohci_setup_td4->cbp = (volatile uint8_t*)td4_buffer;
	    ohci_setup_td4->next_td = (volatile uint32_t)ohci_setup_td5;
	    ohci_setup_td4->buffer_end = (volatile uint32_t)((volatile uint32_t)td4_buffer + (volatile uint32_t)1);

	    ohci_setup_td5->descriptor_info = 0;
	    ohci_setup_td5->cbp = (volatile uint8_t*)td5_buffer;
	    ohci_setup_td5->next_td = (volatile uint32_t)ohci_setup_td8;
	    ohci_setup_td5->buffer_end = (volatile uint32_t)((volatile uint32_t)td5_buffer + (volatile uint32_t)1);

	    ohci_setup_td8->descriptor_info = 0;
	    ohci_setup_td8->cbp = (volatile uint8_t*)td8_buffer;
	    ohci_setup_td8->next_td = (volatile uint32_t)ohci_setup_td9;
	    ohci_setup_td8->buffer_end = (volatile uint32_t)((volatile uint32_t)td8_buffer + (volatile uint32_t)0);

	    ohci_setup_td9->descriptor_info = 0;
	    ohci_setup_td9->cbp = 0;
	    ohci_setup_td9->next_td = 0;
	    ohci_setup_td9->buffer_end = 0;


	    int_zero1 = (struct endpoint_descriptor*)kmem_4k_allocate();
	    iso_zero = (struct endpoint_descriptor_isochronous*)kmem_4k_allocate();
	    ctrl_zero = (struct endpoint_descriptor*)kmem_4k_allocate();
	    bulk_zero = (struct endpoint_descriptor*)kmem_4k_allocate();

	    iso_td_zero = (struct isochronous_transfer_descriptor*)kmem_4k_allocate();
	    int_td_zero = (struct general_transfer_descriptor*)kmem_4k_allocate();
	    ctrl_td_zero = (struct general_transfer_descriptor*)kmem_4k_allocate();
	    bulk_td_zero = (struct general_transfer_descriptor*)kmem_4k_allocate();
	    ohci_setup_ed_0 = (struct endpoint_descriptor*)kmem_4k_allocate();

	    ohci_bulk_in_ed = (struct endpoint_descriptor*)kmem_4k_allocate();
	    ohci_bulk_out_ed = (struct endpoint_descriptor*)kmem_4k_allocate();


	    ohci_bulk_td2 = (struct general_transfer_descriptor*)kmem_4k_allocate();
	    ohci_bulk_td3 = (struct general_transfer_descriptor*)kmem_4k_allocate();
	    ohci_bulk_td4 = (struct general_transfer_descriptor*)kmem_4k_allocate();


	    ohci_done_gen_td = (struct general_transfer_descriptor*)kmem_4k_allocate();
	    zero_usbms_mem_5((uint8_t*)ohci_done_gen_td);

	    int_zero1->endpoint_info = 0x00404000;
	    int_zero1->tail_pointer = 0;
	    int_zero1->head_pointer = 0;
	    int_zero1->next_ed = (volatile uint32_t)int_zero2;

	    int_zero2->endpoint_info = 0x00404000;
	    int_zero2->tail_pointer = 0;
	    int_zero2->head_pointer = 0;
	    int_zero2->next_ed = (volatile uint32_t)int_zero3;

	    int_zero3->endpoint_info = 0x00404000;
	    int_zero3->tail_pointer = 0;
	    int_zero3->head_pointer = 0;
	    int_zero3->next_ed = (volatile uint32_t)int_zero4;

	    int_zero4->endpoint_info = 0x00404000;
	    int_zero4->tail_pointer = 0;
	    int_zero4->head_pointer = 0;
	    int_zero4->next_ed = (volatile uint32_t)int_zero5;

	    int_zero5->endpoint_info = 0x00404000;
	    int_zero5->tail_pointer = 0;
	    int_zero5->head_pointer = 0;
	    int_zero5->next_ed = (volatile uint32_t)int_zero6;

	    int_zero6->endpoint_info = 0x00404000;
	    int_zero6->tail_pointer = 0;
	    int_zero6->head_pointer = 0;
	    int_zero6->next_ed = (volatile uint32_t)int_zero7;

	    int_zero7->endpoint_info = 0x00404000;
	    int_zero7->tail_pointer = 0;
	    int_zero7->head_pointer = 0;
	    int_zero7->next_ed = (volatile uint32_t)int_zero8;

	    int_zero8->endpoint_info = 0x00404000;
	    int_zero8->tail_pointer = 0;
	    int_zero8->head_pointer = 0;
	    int_zero8->next_ed = (volatile uint32_t)int_zero9;

	    int_zero9->endpoint_info = 0x00404000;
	    int_zero9->tail_pointer = 0;
	    int_zero9->head_pointer = 0;
	    int_zero9->next_ed = (volatile uint32_t)int_zero10;

	    int_zero10->endpoint_info = 0x00404000;
	    int_zero10->tail_pointer = 0;
	    int_zero10->head_pointer = 0;
	    int_zero10->next_ed = (volatile uint32_t)int_zero11;

	    int_zero11->endpoint_info = 0x00404000;
	    int_zero11->tail_pointer = 0;
	    int_zero11->head_pointer = 0;
	    int_zero11->next_ed = (volatile uint32_t)int_zero12;

	    int_zero12->endpoint_info = 0x00404000;
	    int_zero12->tail_pointer = 0;
	    int_zero12->head_pointer = 0;
	    int_zero12->next_ed = (volatile uint32_t)int_zero13;

	    int_zero13->endpoint_info = 0x00404000;
	    int_zero13->tail_pointer = 0;
	    int_zero13->head_pointer = 0;
	    int_zero13->next_ed = (volatile uint32_t)int_zero14;

	    int_zero14->endpoint_info = 0x00404000;
	    int_zero14->tail_pointer = 0;
	    int_zero14->head_pointer = 0;
	    int_zero14->next_ed = (volatile uint32_t)int_zero15;

	    int_zero15->endpoint_info = 0x00404000;
	    int_zero15->tail_pointer = 0;
	    int_zero15->head_pointer = 0;
	    int_zero15->next_ed = (volatile uint32_t)int_zero16;

	    int_zero16->endpoint_info = 0x00404000;
	    int_zero16->tail_pointer = 0;
	    int_zero16->head_pointer = 0;
	    int_zero16->next_ed = (volatile uint32_t)int_zero17;

	    int_zero17->endpoint_info = 0x00404000;
	    int_zero17->tail_pointer = 0;
	    int_zero17->head_pointer = 0;
	    int_zero17->next_ed = (volatile uint32_t)int_zero18;

	    int_zero18->endpoint_info = 0x00404000;
	    int_zero18->tail_pointer = 0;
	    int_zero18->head_pointer = 0;
	    int_zero18->next_ed = (volatile uint32_t)int_zero19;

	    int_zero19->endpoint_info = 0x00404000;
	    int_zero19->tail_pointer = 0;
	    int_zero19->head_pointer = 0;
	    int_zero19->next_ed = (volatile uint32_t)int_zero20;

	    int_zero20->endpoint_info = 0x00404000;
	    int_zero20->tail_pointer = 0;
	    int_zero20->head_pointer = 0;
	    int_zero20->next_ed = (volatile uint32_t)int_zero21;

	    int_zero21->endpoint_info = 0x00404000;
	    int_zero21->tail_pointer = 0;
	    int_zero21->head_pointer = 0;
	    int_zero21->next_ed = (volatile uint32_t)int_zero22;

	    int_zero22->endpoint_info = 0x00404000;
	    int_zero22->tail_pointer = 0;
	    int_zero22->head_pointer = 0;
	    int_zero22->next_ed = (volatile uint32_t)int_zero23;

	    int_zero23->endpoint_info = 0x00404000;
	    int_zero23->tail_pointer = 0;
	    int_zero23->head_pointer = 0;
	    int_zero23->next_ed = (volatile uint32_t)int_zero24;

	    int_zero24->endpoint_info = 0x00404000;
	    int_zero24->tail_pointer = 0;
	    int_zero24->head_pointer = 0;
	    int_zero24->next_ed = (volatile uint32_t)int_zero25;

	    int_zero25->endpoint_info = 0x00404000;
	    int_zero25->tail_pointer = 0;
	    int_zero25->head_pointer = 0;
	    int_zero25->next_ed = (volatile uint32_t)int_zero26;

	    int_zero26->endpoint_info = 0x00404000;
	    int_zero26->tail_pointer = 0;
	    int_zero26->head_pointer = 0;
	    int_zero26->next_ed = (volatile uint32_t)int_zero27;

	    int_zero27->endpoint_info = 0x00404000;
	    int_zero27->tail_pointer = 0;
	    int_zero27->head_pointer = 0;
	    int_zero27->next_ed = (volatile uint32_t)int_zero28;

	    int_zero28->endpoint_info = 0x00404000;
	    int_zero28->tail_pointer = (volatile uint32_t)0;
	    int_zero28->head_pointer = (volatile uint32_t)0;
	    int_zero28->next_ed = (volatile uint32_t)int_zero29;

	    int_zero29->endpoint_info = 0x00404000;
	    int_zero29->tail_pointer = 0;
	    int_zero29->head_pointer = 0;
	    int_zero29->next_ed = (volatile uint32_t)int_zero30;

	    int_zero30->endpoint_info = 0x00404000;
	    int_zero30->tail_pointer = 0;
	    int_zero30->head_pointer = 0;
	    int_zero30->next_ed = (volatile uint32_t)int_zero31;

	    int_zero31->endpoint_info = 0x00404000;
	    int_zero31->tail_pointer = 0;
	    int_zero31->head_pointer = 0;
	    int_zero31->next_ed = (volatile uint32_t)int_zero32;


	    int_zero32->endpoint_info = 0x00404000;
	    int_zero32->tail_pointer = 0;
	    int_zero32->head_pointer = 0;
	    int_zero32->next_ed = (volatile uint32_t)iso_zero;

	    ctrl_zero->endpoint_info = 0;
	    ctrl_zero->tail_pointer = 0;
	    ctrl_zero->head_pointer = 0;
	    ctrl_zero->next_ed = 0;

	    bulk_zero->endpoint_info = 0;
	    bulk_zero->tail_pointer = 0;
	    bulk_zero->head_pointer = 0;
	    bulk_zero->next_ed = 0;

	    iso_zero->endpoint_info = 0x00404000;
	    iso_zero->tail_pointer = (struct isochronous_transfer_descriptor*)0;
	    iso_zero->head_pointer = (struct isochronous_transfer_descriptor*)0;
	    iso_zero->next_ed = (struct endpoint_descriptor_isochronous*)0;


	    iso_td_zero->descriptor_info = 0;
	    iso_td_zero->buffer_page0 = (volatile uint8_t*)0;
	    iso_td_zero->next_td = (struct isochronous_transfer_descriptor*)0;
	    iso_td_zero->buffer_end = (volatile uint8_t*)0;
	    iso_td_zero->psw0_1 = 0;
	    iso_td_zero->psw2_3 = 0;
	    iso_td_zero->psw4_5 = 0;
	    iso_td_zero->psw6_7 = 0;

	    int_td_zero->descriptor_info = 0;
	    int_td_zero->cbp = (volatile uint8_t*)0;
	    int_td_zero->next_td = (volatile uint32_t)0;
	    int_td_zero->buffer_end = (volatile uint32_t)0;

	    ctrl_td_zero->descriptor_info = 0;
	    ctrl_td_zero->cbp = (volatile uint8_t*)0;
	    ctrl_td_zero->next_td = (volatile uint32_t)0;
	    ctrl_td_zero->buffer_end = (volatile uint32_t)0;

	    ctrl_td_zero->descriptor_info = 0;
	    ctrl_td_zero->cbp = (volatile uint8_t*)0;
	    ctrl_td_zero->next_td = (volatile uint32_t)0;
	    ctrl_td_zero->buffer_end = (volatile uint32_t)0;

	    ohci_setup_ed_0->endpoint_info = 0x00404000;
	    ohci_setup_ed_0->tail_pointer = 0;
	    ohci_setup_ed_0->head_pointer = 0;
	    ohci_setup_ed_0->next_ed = (volatile uint32_t)ohci_setup_ed;


	    ohci_bulk_in_ed->endpoint_info = 0;
	    ohci_bulk_in_ed->tail_pointer = 0;
	    ohci_bulk_in_ed->head_pointer = 0;
	    ohci_bulk_in_ed->next_ed = 0;

	    ohci_bulk_out_ed->endpoint_info = 0;
	    ohci_bulk_out_ed->tail_pointer = 0;
	    ohci_bulk_out_ed->head_pointer = 0;
	    ohci_bulk_out_ed->next_ed = 0;

	    bulk_td_zero->descriptor_info = 0;
	    bulk_td_zero->cbp = 0;
	    bulk_td_zero->next_td = 0;
	    bulk_td_zero->buffer_end = 0;

	    ohci_bulk_td1->descriptor_info = 0;
	    ohci_bulk_td1->cbp = 0;
	    ohci_bulk_td1->next_td = 0;
	    ohci_bulk_td1->buffer_end = (volatile uint32_t)0;

	    ohci_bulk_td2->descriptor_info = 0;
	    ohci_bulk_td2->cbp = 0;
	    ohci_bulk_td2->next_td = 0;
	    ohci_bulk_td2->buffer_end = (volatile uint32_t)0;

	    ohci_bulk_td3->descriptor_info = 0;
	    ohci_bulk_td3->cbp = 0;
	    ohci_bulk_td3->next_td = 0;
	    ohci_bulk_td3->buffer_end = (volatile uint32_t)0;

	    ohci_bulk_td4->descriptor_info = 0;
	    ohci_bulk_td4->cbp = 0;
	    ohci_bulk_td4->next_td = 0;
	    ohci_bulk_td4->buffer_end = (volatile uint32_t)0;

}



/* Prints out the HC Revision register
 * of the OHC which is called HcRevision.
 * It is printed out in binary notation.
 */
void read_ohci_hcrevision() 
{
	    uint32_t ohci_hc_rev = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000000);
	    print("\n\nOhci HcRevision: ");
	    dec_to_bin((uint32_t)ohci_hc_rev);
}

/* Prints out the whole Hc Control Register 
 * of the OHC which is called HcControl. 
 * It is printed out in binary notation.
 */
void read_ohci_hccontrol() 
{
	    uint32_t ohci_hc_ctrl = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004);
	    print("\n\nOhci HcControl: ");
	    dec_to_bin((uint32_t)ohci_hc_ctrl);
}

/* Prints out the whole Hc Command and Status register 
 * of the OHC which is called HcCommandStatus. 
 * It is printed out in binary notation.
 */
void read_ohci_hccmd()
{
	    uint32_t ohci_hc_cmd = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000008);
	    print("\n\nOhci HcCommandstatus: ");
	    dec_to_bin((uint32_t)ohci_hc_cmd);
}

/* Prints out the whole Hc Interrupt Status register 
 * of the OHC which is called HcInterruptStatus. 
 * It is printed out in binary notation.
 */
void read_ohci_hcintsts()
{
	    uint32_t ohci_hc_intsts = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c);
	    print("\n\nOhci HcIntStatus: ");
	    dec_to_bin((uint32_t)ohci_hc_intsts);
}

/* Prints out the whole Hc Interrupt Enable register 
 * of the OHC which is called HcInterruptEnable. 
 * It is printed out in binary notation.
 */
void read_ohci_hcintenable()
{
	    uint32_t ohci_hc_intenbl = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000010);
	    print("\n\nOhci HcIntEnable: ");
	    dec_to_bin((uint32_t)ohci_hc_intenbl);
}

/* Prints out the whole Hc Interrupt Disable register 
 * of the OHC which is called HcInterruptDisable. 
 * It is printed out in binary notation.
 */
void read_ohci_hcintdisable()
{
	    uint32_t ohci_hc_intdsbl = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000014);
	    print("\n\nOhci HcIntDisable: ");
	    dec_to_bin((uint32_t)ohci_hc_intdsbl);
}

/* Prints out the whole 
 * Host Controller Communication Area register for 
 * the OHC. It is called HcHCCA.
 * It is printed out in hexadecimal notation.
 */
void read_ohci_hchcca()
{
	    uint32_t ohci_hc_hcca = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000018);
	    print("\n\nOhci HcHcc address: ");
	    print_hex((uint32_t)ohci_hc_hcca);
}

/* Prints out the Hc Periodic Current Endpoint Descriptor
 * register for the OHC. It is called HcPeriodCurrentED.
 * It is printed out in hexadecimal notation.
 */
void read_ohci_hccped()
{
	    uint32_t ohci_hc_cped = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000001c);
	    print("\n\nOhci Hc Current Periodic Endpoint Descriptor address:\n ");
	    print_hex((uint32_t)ohci_hc_cped);
}

/* Prints out the physical address of the first 
 * Endpoint Descriptor of the Control list. for the OHC.
 * It is called HcControlHeadED.
 * It is printed out in hexadecimal notation.
 */
void read_ohci_hcched()
{
	    uint32_t ohci_hc_ched = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000020);
	    print("\n\nOhci Hc Control Head Endpoint Descriptor address:\n ");
	    print_hex((uint32_t)ohci_hc_ched);
}

/* Prints out the HcControlCurrentED register which contains
 * the physical address of the current Endpoint
 * Descriptor of the Control list.
 * It is printed out in hexadecimal notation.
 */
void read_ohci_hccced()
{
	    uint32_t ohci_hc_cced = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000024);
	    print("\n\nOhci HcControlCurrentEd: ");
	    dec_to_bin((uint32_t)ohci_hc_cced);
}

/* Prints out the HcBulkHeadED register which 
 * contains the physical address of the
 * first Endpoint Descriptor of the Bulk list.
 * It is printed out in hexadecimal notation.
 */
void read_ohci_hcbhed()
{
	    uint32_t ohci_hc_bhed = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000028);
	    print("\n\nOhci HcBulkHeadEd: ");
	    print_hex((uint32_t)ohci_hc_bhed);
}

/* Prints out the HcBulkCurrentED register which contains
 * the physical address of the current endpoint of the Bulk
 * list. It is printed out in hexadecimal notation.
 */
void read_ohci_hcbced()
{
	    uint32_t ohci_hc_bced = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000002c);
	    print("\n\nOhci Hc BulkCurrentEd: ");
	    print_hex((uint32_t)ohci_hc_bced);
}

/* Prints out the HcDoneHead register which contains
 * the physical address of the last completed
 * Transfer Descriptor that was added to the Done queue
 * It is printed out in hexadecimal notation.
 */
void read_ohci_hcdonehead()
{
	    uint32_t ohci_hc_donehead = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000030);
	    print("\n\nOhci HcDoneHead: ");
	    print_hex((uint32_t)ohci_hc_donehead);
}

/* Prints out the HcFmInterval Register.
 * It is printed out in binary notation.
 */
void read_ohci_hcfmi()
{
	    uint32_t ohci_hc_fmi = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000034);
	    print("\n\nOhci Hc FmInterval: ");
	    dec_to_bin((uint32_t)ohci_hc_fmi);
}

/* Prints out the HcFmRemaining register.
 * It is printed out in hexadecimal notation.
 */
void read_ohci_hcfmr()
{
	    uint32_t ohci_hc_fmr = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000038);
	    print("\n\nOhci HcFmRemaining: ");
	    print_hex((uint32_t)ohci_hc_fmr);
}

/* Prints out the HcFmNumber register.
 * It is printed out in hexadecimal notation.
 */
void read_ohci_hcfmn()
{
	    uint32_t ohci_hc_fmn = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000003c);
	    print("\n\nOhci Hc FmNumber: ");
	    print_hex((uint32_t)ohci_hc_fmn);
}

/* Prints out the HcPeriodicStart register.
 * It is printed out in hexadecimal notation.
 */
void read_ohci_hcps()
{
	    uint32_t ohci_hc_ps = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000040);
	    print("\n\nOhci Periodic Start: ");
	    print_hex((uint32_t)ohci_hc_ps);
}

/* Prints out The HcLSThreshold register.
 * It is printed out in binary notation.
 */
void read_ohci_hclsth()
{
	    uint32_t ohci_hc_lsth = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000044);
	    print("\n\nOhci Hc LSThreshold: ");
	    dec_to_bin((uint32_t)ohci_hc_lsth);
}

/* Prints out the HcRhDescriptorA register which 
 * is the first register of two describing the
 * characteristics of the Root Hub.
 * It is printed out in binary notation.
 */
void read_ohci_hcrhda()
{
	    uint32_t ohci_hc_rhda = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000048);
	    print("\n\nOhci HcRhDescriptorA: ");
	    dec_to_bin((uint32_t)ohci_hc_rhda);
}

/* Prints out the HcRhDescriptorB register which 
 * is the second register of two describing the
 * characteristics of the Root Hub. 
 * It is printed out in binary notation.
 */
void read_ohci_hcrhdb()
{
	    uint32_t ohci_hc_rhdb = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000004c);
	    print("\n\nOhci HcRhDescriptorB: ");
	    dec_to_bin((uint32_t)ohci_hc_rhdb);
}

/* Prints out the HcRhStatus register which is
 * divided into two parts. The lower word of a
 * Dword represents the Hub Status field and the
 * upper word represents the Hub Status Change field.
 * It is printed out in binary notation.
 */ 
void read_ohci_hcrhstatus()
{
	    uint32_t ohci_hc_rhsts = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000050);
	    print("\n\nOhci HcRhStatus: ");
	    dec_to_bin((uint32_t)ohci_hc_rhsts);
}

/* Prints the port status on port 1
 * in binary notation. 
 */
void read_ohci_port1()
{
	    uint32_t ohci_hc_port1 = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000054);
	    print("\n\nOhci port1: ");
	    dec_to_bin((uint32_t)ohci_hc_port1);
}

/* Prints the port status on port 2
 * in binary notation. 
 */
void read_ohci_port2()
{
	    uint32_t ohci_hc_port2 = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000058);
	    print("\n\nOhci port2: ");
	    dec_to_bin((uint32_t)ohci_hc_port2);
}

/* Prints the port status on port 3
 * in binary notation. 
 */
void read_ohci_port3()
{
	    uint32_t ohci_hc_port3 = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000005c);
	    print("\n\nOhci port3: ");
	    dec_to_bin((uint32_t)ohci_hc_port3);
}

/* Prints the service ratio between Control and Bulk EDs.
 * It's printed in decimal notation.
 */
void read_ctrl_bulk_ratio()
{
	    uint32_t temp = get_hccontrol();
	    temp &= 3;
	    print("\n\nRatio: ");
	    printi(temp);
}

/* Returns the service ratio between Control and Bulk EDs. */
uint32_t get_ctrl_bulk_ratio()
{
	    uint32_t temp = get_hccontrol();
	    temp &= 3;
	    return temp;
}

/* Stores the HcControl register. */
uint32_t get_hccontrol()
{
	    uint32_t hc_control = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004);
	    return hc_control;
}

/* Returns the HcCommandStatus register. */
uint32_t get_hccmd()
{
	    uint32_t hc_cmd = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000008);
	    return hc_cmd;
}

/* Writes the ownership change request bit
 * in the HcCommandStatus register.
 * The OS will request ownership from the BIOS.
 */
void write_ownership_bit()
{
	    uint32_t temp = get_hccmd();
	    temp |= 0x00000008;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)temp);
}

/* Resets the HC and the root hub. */
void reset_usb()
{
	    uint32_t temp = get_hccontrol();
	    if (((temp >> 6) & 0x00000003) != 0) {
			    temp &= (~(1 << 1));
			    temp &= (~(1 << 0));
			    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)temp);
	    }
}

/* Fills the interrupt and isochronous address space with
 * endpoint descriptors.
 */
void fill_hcca_int_iso()
{
	    uint32_t* hcca_addr = (uint32_t*)ohci_hcca;
	    *hcca_addr = (uint32_t)int_zero1;
	    *(hcca_addr + 1) = (uint32_t)int_zero2;
	    *(hcca_addr + 2) = (uint32_t)int_zero3;
	    *(hcca_addr + 3) = (uint32_t)int_zero4;
	    *(hcca_addr + 4) = (uint32_t)int_zero5;
	    *(hcca_addr + 5) = (uint32_t)int_zero6;
	    *(hcca_addr + 6) = (uint32_t)int_zero7;
	    *(hcca_addr + 7) = (uint32_t)int_zero8;
	    *(hcca_addr + 8) = (uint32_t)int_zero9;
	    *(hcca_addr + 9) = (uint32_t)int_zero10;
	    *(hcca_addr + 10) = (uint32_t)int_zero11;
	    *(hcca_addr + 11) = (uint32_t)int_zero12;
	    *(hcca_addr + 12) = (uint32_t)int_zero13;
	    *(hcca_addr + 13) = (uint32_t)int_zero14;
	    *(hcca_addr + 14) = (uint32_t)int_zero15;
	    *(hcca_addr + 15) = (uint32_t)int_zero16;
	    *(hcca_addr + 16) = (uint32_t)int_zero17;
	    *(hcca_addr + 17) = (uint32_t)int_zero18;
	    *(hcca_addr + 18) = (uint32_t)int_zero19;
	    *(hcca_addr + 19) = (uint32_t)int_zero20;
	    *(hcca_addr + 20) = (uint32_t)int_zero21;
	    *(hcca_addr + 21) = (uint32_t)int_zero22;
	    *(hcca_addr + 22) = (uint32_t)int_zero23;
	    *(hcca_addr + 23) = (uint32_t)int_zero24;
	    *(hcca_addr + 24) = (uint32_t)int_zero25;
	    *(hcca_addr + 25) = (uint32_t)int_zero26;
	    *(hcca_addr + 26) = (uint32_t)int_zero27;
	    *(hcca_addr + 27) = (uint32_t)int_zero28;
	    *(hcca_addr + 28) = (uint32_t)int_zero29;
	    *(hcca_addr + 29) = (uint32_t)int_zero30;
	    *(hcca_addr + 30) = (uint32_t)int_zero31;
	    *(hcca_addr + 31) = (uint32_t)int_zero32;
}

/* Adjusts the ControlBulkServiceRatio with
 * a given value.
 */
void write_cb_ratio(uint32_t ratio)
{
	    uint32_t temp = get_hccontrol();
	    temp |= ratio;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)temp);
}


/**********************************************************************************************
 *                                                                                            *
 *  look at start_hc for more detail. assertion of reset (wait a while)                                                         *   
 *  request ownership (eventually)                                                            *
 *  save the value in hcfminterval                                                            *
 *  make a reset by writing a one to the HostControllerReset bit in HcCommandStatus.          *
 *   maximum 10 micro seconds                                                                 *
 *  restore value of hcfminterval                                                             *
 *  usb-suspend state. dont wait more than 2 ms                                               *
 *  init hcca (make terminate all queues)                                                     *
 *  init control and bulk terminate control and make way for bulk, in your case               *
 *  HcBulkHeadED shoul point to the first ED. yipeee                                          *
 *  Set the HcHCCA to the physical address of the HCCA block.                                 *
 *  enable interrupts                                                                         *
 *  select queues in hccontrol (maybe all queues on)                                          *
 *  Set HcPeriodicStart to a value that is 90% of the value in FrameInterval field of the     *
 *  HcFmInterval register.                                                                    *
 *  start the hc: write usboperational in the control register                                *
 *  pausing the processing of EDs. Use skip. And wait for the next frame.                     *
 *  resume processing: clear halt and skip                                                    *
 *  list Head field is initialized to a null list and                                         *
 *  the PhysicalHead field contains the address of the proper list head operational register. *
 *  watch the ControlBulkServiceRatio in the cControl register                                *
 *                                                                                            *  
 **********************************************************************************************/

/* Configures some parameters as a preparation
 * for starting the HC.
 */
void configure_start_hc()
{

	    asm("sti");

        /* check if interrupt routing is 1. In that case it's smi. */
	    uint32_t temp2 = get_hcfminterval();
	    print("\n\nInitial fminteval value:\n");
	    dec_to_bin((uint32_t)temp2);
	    print("\n\nInitial fmremaining value:\n");
	    read_ohci_hcps();
	    uint32_t ir_is_smi = check_interruptrouting();
	    uint32_t temp_ratio = get_ctrl_bulk_ratio();

        /* check if usbreset */
	    if (!ir_is_smi) {
			    uint32_t usb_is_reset = check_hcfs_usbreset();
			    if(usb_is_reset) {
					    timer_ticks = 0;
					    while(timer_ticks < 7);
					    start_hc((const uint32_t) temp2, (const uint32_t)temp_ratio);
	    
	    
			    }
			    else {
					    reset_usb();
					    timer_ticks = 0;
					    while(timer_ticks < 7);
					    start_hc((const uint32_t) temp2, (const uint32_t)temp_ratio);

			    }
	    }

	    else {
                /* Attempts to change ownership to the kernel. */
			    print("\nChanging ownership");
			    write_ownership_bit();
			    timer_ticks = 0;
			    while(timer_ticks < 2);
			    uint32_t temp1 = check_interruptrouting();

	            if(!temp1) {
					    uint32_t usb_is_reset = check_hcfs_usbreset();
					    if(usb_is_reset) {
							    timer_ticks = 0;
							    while(timer_ticks < 7);
							    start_hc((const uint32_t) temp2, (const uint32_t)temp_ratio);
			    
			    
					    }
					    else {
							    reset_usb();
							    timer_ticks = 0;
							    while(timer_ticks < 7);
							    // Commented code: asm("cli");
							    start_hc((const uint32_t) temp2, (const uint32_t)temp_ratio);
		    
					    }
		    
		    
		    
			    }

			    else {
					    print("\n\nError: Could not proceed with ownership change");
					    print("\nError: Could not initialize the ohc.");
			    }

	    }

}

/* Returns the ownership bit value in the HcCommandStatus
 * Register.
 */
uint32_t check_ownership_hccmd()
{
	    uint32_t check_bit = get_hccmd();
	    check_bit >>= 3;
	    check_bit &= 0x00000001;
	    return check_bit;
}

/* Returns a one or a zero. A one indicates that
 * interrupts are routed to the SMI. Else the OS
 * handles the interrupts.
 */
uint8_t check_interruptrouting()
{
	    uint8_t bit_set;
	    uint32_t hc_cmd = get_hccontrol();
	    if(((hc_cmd >> 8) & 0x00000001) == 1)
		        bit_set = 1;
	    else
		        bit_set = 0;

	    return bit_set;
}

/* Returns a one or a zero. A one indicates that functional state */
uint8_t check_hcfs_usbreset()
{
	    uint8_t hc_usb_reset;
	    uint32_t hc_cmd = get_hccontrol();
	    if(((hc_cmd >> 7) & 0x00000001) == 0)
		        hc_usb_reset = 1;
	    else
		        hc_usb_reset = 0;

	    return hc_usb_reset;
}

/* Returns the HCFMInterval register. */
uint32_t get_hcfminterval()
{
	    uint32_t temp = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000034);
	    print("\n\n fminterval value: ");
	    print_hex(temp);
	    return temp;
}

/* Writes a value to the HcFMInterval 
 * for clock synchronizationa among other things.
 */
void write_hcfminterval(const uint32_t value)
{
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000034, (const uint32_t)value);
}

/* Resets the HC without reseting the root hub. */
void reset_hc()
{
	    uint32_t temp = get_hccmd();
	    temp |= 0x00000001;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000008, (const uint32_t)temp);
}


/* Starts the HC after some other things are done. */
void start_hc(const uint32_t interval_value, const uint32_t cb_ratio)
{

	    asm("cli");
	    uint32_t temp1;
	    asm("sti");

        /* must be completed within 10 microseconds. */
	    reset_hc(); 

	    timer_ticks = 0;
	    while(timer_ticks < 8);
	    uint32_t ir_is_smi = check_interruptrouting();
	    if(ir_is_smi) {
			    print("\n\nsmi mode on ohci was on");
			    write_ownership_bit();
			    timer_ticks = 0;
			    while(timer_ticks < 2);
	    }

	    set_ohci_commandreg(); 
	    print("\nframe interval after reset:\n"); 
	    read_ohci_hcfmi();

	    print("\nperiodic start after reset:\n");
	    read_ohci_hcps();
	    print("\nrhda after reset (number of ports at bits 0-7) :\n");
	    read_ohci_hcrhda();

        /* frame number */
	    read_ohci_hcfmn(); 

        /* periodic start */
	    read_ohci_hcps(); 

        /* threshhold */
	    read_ohci_hclsth(); 

        /* write_hcfminterval((const uint32_t) interval_value); // hope it wont stay in usb-suspend state longer than 2 ms */
	    write_fm_interval(); 
	    write_hcperiodicstart();
	    write_cb_ratio((const uint32_t)cb_ratio);

	    disable_int_eds();
	    disable_iso_eds();
	    disable_bulk_eds();
	    enable_ctrl_eds();

	    write_controlheaded_addr();
	    write_hcca();
	    fill_hcca_int_iso();
	    set_ohci_interrupts();

	    disable_ohci_interrupts_reverseset();

        /* then sends sof within 1 ms */
	    write_usb_operational();  
	    timer_ticks = 0;
	    while(timer_ticks < 2);

	    temp1 = get_hccontrol();
	    if((temp1 & 0xc0) != 0x80)
		    print("\nError: usb is not in operational mode."); 


	    print("\nresetting port 2");
	    reset_ohci_port2();
	    timer_ticks = 0;
	    while(timer_ticks < 8);
	    ohci_ports_busy = 1;
	    temp1 = get_ohci_port2();

        /* port reset status change */
	    if((((temp1 >> 20) & 0x00000001) == 1)) 
		    reset_port2_prsc();
	    temp1 = get_ohci_port2();
	    if((((temp1 >> 1) & 0x00000001) == 0))
		    print("\nport2 not enabled after reset");
	    ohci_ports_busy = 0;


	    print("\nresetting port 1");
	    reset_ohci_port1();
	    timer_ticks = 0;
	    while(timer_ticks < 12);
	    ohci_ports_busy = 1;
	    temp1 = get_ohci_port1();

        /* port reset status change */
	    if((((temp1 >> 20) & 0x00000001) == 1)) 
		    reset_port1_prsc();
	    temp1 = get_ohci_port1();
	    if((((temp1 >> 1) & 0x00000001) == 0))
		    print("\nError: port1 not enabled after reset");

	    setenable_ohci_port1();
	    timer_ticks = 0;
	    while(timer_ticks < 8);
		    
	    ohci_ports_busy = 1;
	    Get_Ohci_Port_Status();
	    temp1 = get_ohci_port1();
	    if((((temp1 >> 16) & 0x00000001) == 1))
	    reset_port1_csc();
	    ohci_ports_busy = 0;

	    timer_ticks = 0;
	    while(timer_ticks < 1);
	    read_ohci_port1();
	    print("\nport2: ");
	    read_ohci_port2();
	    
	    asm("cli");
	    
}

/* This starts the OHC. */
void write_usb_operational() 
{
	uint32_t operational = get_hccontrol();

	if(((operational >> 6) & 0x00000003) != 2) {
		    operational &= (~(1 << 6));
		    operational |= 0x80;
		    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)operational);
    }
}

/* Suspends the OHC.*/
void write_usb_suspend() 
{
	uint32_t operational = get_hccontrol();
	/* value for operational should be 10b */
	if(((operational >> 6) & 0x00000003) != 3) {
		    operational |= 0x40;
		    operational |= 0x80;
		    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)operational);
    }
}

/* resets the HC and resets USB */
void stop_ohci()
{
	    asm("cli");

	    timer_install();
	    timer_phase(1000);
	    asm("sti");
	    reset_hc();

	    timer_ticks = 0;
	    while(timer_ticks < 1);
	    asm("cli");
	    timer_phase(18);
	    irq_uninstall_handler(0);
	    zero_usbms_mem_6((uint32_t*)0x400000);
	    reset_usb();
}

/* Writes all 1s to the HcHCCA register.
 * This driver should then read it again.
 */
void read_hcca_limits()
{
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000018, (const uint32_t)0xffffffff);
	    uint32_t temp = get_hchcca();
	    print("\n\nhcca limits: ");
	    print_hex(temp);
}

/* Writes 1s and then reads the register to
 * get the alignment value.
 */
uint32_t get_hcca_limits()
{
	    uint32_t temp4;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000018, (const uint32_t)0xffffffff);
     	temp4 = get_hchcca();
	    return temp4;
}

/* Returns the contents of the HcHCCA register. */
uint32_t get_hchcca()
{
	    uint32_t temp = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000018);
	    return (uint32_t)temp;
}

/* Writes 0s to the interrupt enable register. */
void enable_ohci_interrupts_0()
{
	    uint32_t ohci_int_enable = 0;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000010, (const uint32_t)ohci_int_enable);
}

/* Disables some interrupts. */
void disable_ohci_interrupts()
{
	    /*0-6  30-31 should be set to 1, rest to 0 in hex: C000007F  3221225599 */
	    uint32_t ohci_int_disable = 3221225599;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000014, (const uint32_t)ohci_int_disable);
}

/* Writes 0s to the disable interrupt register. */
void disable_ohci_interrupts_reverseset()
{
	    uint32_t dsint = 0x00000000; 
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000014, (const uint32_t)dsint);
}

/* Sets some interrupts. */
void set_ohci_interrupts()
{
	    uint32_t temp = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000010);
        /* 11.....1111011 All interrupt enabled except 
         * the start of frame interrupt. 
         */
	    temp |= 0xC000007b; 

	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000010, (const uint32_t)temp);
}

/* Sets some interrupts. */
void set_ohci_interrupts2()
{
	    uint32_t temp = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000010);
	    temp |= 0x0000007b; // 10.....1111011
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000010, (const uint32_t)temp);

	    temp = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000010);
	    temp |= 0x80000000;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000010, (const uint32_t)temp);
}

/* Sets a bulk ED at the bulkedhead register. */
void write_bulkheaded_addr()
{
	struct endpoint_descriptor* baddr =  bulk_zero;
	uint32_t addr = (uint32_t)baddr;
	write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000028, (const uint32_t)addr);
}

/* Sets a control ED at the control ed head register. */
void write_controlheaded_addr()
{

    	write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000020, (const uint32_t)0x1400000);
}

/* Disables the periodic list. */
void disable_int_eds()
{
	    uint32_t int_eds = get_hccontrol();
	    int_eds &= (~(1 << 2));
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)int_eds);
}

/* Disables the isochronous processing of EDs. */
void disable_iso_eds()
{
	    uint32_t iso_eds = get_hccontrol();
	    iso_eds &= (~(1 << 3));
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)iso_eds);
}

/* Disables the processing of control EDs. */
void disable_ctrl_eds()
{
	    uint32_t ctrl_eds = get_hccontrol();
	    ctrl_eds &= (~(1 << 4));
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)ctrl_eds);
}

/* Enables the processing of bulk EDs. */
void enable_bulk_eds()
{
	    uint32_t bulk_eds = get_hccontrol();
	    bulk_eds |= 0x00000020;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)bulk_eds);
}

/* Enables the processing of control EDs. */
void enable_ctrl_eds()
{
	    uint32_t ctrl_eds = get_hccontrol();
	    ctrl_eds |= 0x00000010;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)ctrl_eds);
}

/* Disables the processing of bulk EDs. */
void disable_bulk_eds()
{
	    uint32_t bulk_eds = get_hccontrol();
	    bulk_eds &= (~(1 << 5));
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000004, (const uint32_t)bulk_eds);
}

/* Sets the HcPeriodicStart to a typical value. */
void write_hcperiodicstart()
{
	    /* written before: 3E67 15975 */
	    const uint32_t periodicstart = 0x3e67;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000040, periodicstart);
}

/* Writes a memory address for the 
 * Host controller communication area.
 */
void write_hcca()
{
    	write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000018, (const uint32_t)ohci_hcca);
}

/* done_queue_copy is a uint32_t format location of all copies of memory addresses of tds. 
 * done_queue_location is an uint8_t* pointer to the hcca + 84. the queue. 
 */
void copy_done_queue()
{
	    memset2((uint8_t*)(done_queue_copy + done_queue_index), done_queue_location, 4);
	    done_queue_index += 4;
}

/* Prints out the done queue. */
void print_done_queue(uint32_t tds, uint32_t buffer_count)
{
	    uint32_t* tempaddr = (uint32_t*)done_queue_copy;
	    uint32_t** addr = &tempaddr;
	    uint8_t* data_area_addr;
	    
	    for(uint32_t d = 0; d < ((tds*4) + 1); d+=4) {

			    print("\n\ntd number ");
			    printi(d/4);
			    print(": ");
			    for(uint32_t a = 0; a < 4; a++) {
					    dec_to_bin((uint32_t)(**(addr + d + a)));    
					    print("\n");
			    }

			    print("\nBuffer area: ");
			    print("\n\n");
			    data_area_addr = (uint8_t*)(**(addr + d + 1));
			    for (uint32_t b = 0; b < buffer_count; b++) {
					    print_hex_byte((*(data_area_addr + b)));
					    print(" ");
			    }
	    }
	    done_queue_index = 0;
	    uint32_t* temp = (uint32_t*)done_queue_copy;
	    zero_usbms_mem_6(temp);
}

/* Prints out the done queue in other ways. */
void print_done_queue2()
{
	    uint32_t* tempaddr = (uint32_t*)done_queue_copy;
	    print("\n\nTds in done queue: ");
	    print("\n\n");

	    print_hex((uint32_t)(*(tempaddr)));
	    print("\n\ntd value: \n");
	    uint32_t temp = (uint32_t)tempaddr;
	    uint32_t* temp1 = (uint32_t*)temp;
	    dec_to_bin((uint32_t)(*(temp1)));
	    print("\n");
	    print_hex((uint32_t)(*(temp1 + 1)));
	    print("\n");
	    print_hex((uint32_t)(*(temp1 + 2)));
	    print("\n");
	    print_hex((uint32_t)(*(temp1 + 3)));
}

/* Writes a one to the BulkList Field field to indicate
 * that the bulk list is filled.
 */
void write_bulklistfilled()
{
	    uint32_t temp = get_hccmd();
	    temp |= 4;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000008, (const uint32_t)temp);
}
/* Writes a one to the ControlList Field field to indicate
 * that the control list is filled.
 */
void write_ctrllistfilled()
{
	    uint32_t temp = get_hccmd();
	    temp |= 2;
	    write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000008, (const uint32_t)temp);
}

/* Prints out the done queue and focuses on TDs. */
void print_done_queue_td()
{
	    uint32_t temp = get_hchcca();
	    uint32_t* ptr = (uint32_t*)temp;
	    ptr += 33;
	    print("\n\nhcca value: ");
	    print_hex((uint32_t)(*(ptr)));
}

/* Attempts to get a device descriptor. I guess this
 * function uses control TDs and control EDs. 
 */
uint32_t ohci_get_descriptor()
{

	    asm("sti");
	    struct setup_data* device_descriptor = (struct setup_data*)0x400070;


        device_descriptor->RequestType = USB_DEVICE_TO_HOST;
        device_descriptor->Request = USB_GET_DESCRIPTOR;
        device_descriptor->value_low = 0x00;
        device_descriptor->value_high = USB_GET_DESCRIPTOR_DEVICE;
        device_descriptor->index_low = 0x00;
        device_descriptor->index_high = 0x00;
        device_descriptor->length_low = USB_DEVICE_DESCRIPTOR_DEVICE_LENGTH;
        device_descriptor->length_high = 0x00;

        /* ok set up cpbs for each td and the device descriptor */

        /* disable the bulk first (0 to bulklistenable) before changing the lists (below) */


	    ohci_setup_td1->descriptor_info = (volatile uint32_t)(0 | ((7 << 29) | (2 << 24) | (7 << 21) | (0 << 19) | (0 << 18)));
	    ohci_setup_td1->cbp = (volatile uint8_t*)device_descriptor;
	    ohci_setup_td1->next_td = (volatile uint32_t)ohci_setup_td2;
	    ohci_setup_td2->descriptor_info = (volatile uint32_t)(0 | ((7 << 29) | (3 << 24) | (0 << 21) | (2 << 19) | (0 << 18)));
	    ohci_setup_td2->next_td = (volatile uint32_t)ohci_setup_td3;
	    ohci_setup_td3->descriptor_info = (volatile uint32_t)(0 | ((7 << 29) | (2 << 24) | (0 << 21) | (2 << 19) | (0 << 18)));
	    ohci_setup_td3->next_td = (volatile uint32_t)ohci_setup_td4;
	    ohci_setup_td4->descriptor_info = (volatile uint32_t)(0 | ((7 << 29) | (3 << 24) | (0 << 21) | (2 << 19) | (0 << 18)));
	    ohci_setup_td4->next_td = (volatile uint32_t)ohci_setup_td9;
	    ohci_setup_td5->descriptor_info = (volatile uint32_t)(0 | ((7 << 29) | (2 << 24) | (0 << 21) | (2 << 19) | (0 << 18)));
	    ohci_setup_td5->next_td = (volatile uint32_t)ohci_setup_td8;
	    ohci_setup_td8->descriptor_info = (volatile uint32_t)(0 | ((7 << 29) | (3 << 24) | (0 << 21) | (1 << 19) | (0 << 18)));
	    ohci_setup_td8->next_td = (volatile uint32_t)ohci_setup_td9;

	    
	    ohci_setup_ed->tail_pointer = (volatile uint32_t)ohci_setup_td9;
	    ohci_setup_ed->endpoint_info = 0 | ((8 << 16) | (0 << 15) | (0 << 14) | (0 << 13) | (0 << 11) | (0 << 7) | 0); 
	    ohci_setup_ed->head_pointer = (volatile uint32_t)ohci_setup_td1;
	    write_ctrllistfilled();


	    print("\n\nOk first attempt to read data from ohci transfers:\n\n");
	    if((((uint32_t)ohci_setup_ed->head_pointer) & 0x00000001) == 1)
		        print("\n\ntransfer descriptor halted due to an error.");
	    print("\n\ntd1 content: ");
	    dec_to_bin((uint32_t)ohci_setup_td1->descriptor_info);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td1->cbp);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td1->next_td);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td1->buffer_end);
	    ohci_setup_ed->endpoint_info = 0x00404000;

	    if((ohci_setup_td1->descriptor_info & 0x70000000) == 0x40000000) {
			    disable_ctrl_eds();
			    ohci_setup_ed->endpoint_info = 0x00404000;
			    ohci_setup_ed->tail_pointer = 0;
			    ohci_setup_ed->head_pointer = 0;
			    ohci_setup_ed_0->next_ed = 0;
			    print("\n\nStalled...");

			    asm("cli");
			    return 0;
	    } 

	    print("\n\nOk second attempt to read data from ohci transfers:\n\n");
	    if((((uint32_t)ohci_setup_ed->head_pointer) & 0x00000001) == 1)
		        print("\n\ntransfer descriptor halted due to an error.");

	    print("\n\ntd2 content: ");
	    dec_to_bin((uint32_t)ohci_setup_td2->descriptor_info);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td2->cbp);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td2->next_td);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td2->buffer_end);

	    print("\n\ntd3 content: ");
	    dec_to_bin((uint32_t)ohci_setup_td3->descriptor_info);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td3->cbp);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td3->next_td);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td3->buffer_end);

	    print("\n\ntd4 content: ");
	    dec_to_bin((uint32_t)ohci_setup_td4->descriptor_info);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td4->cbp);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td4->next_td);
	    print("\n");
	    print_hex((uint32_t)ohci_setup_td4->buffer_end);


	    print("\n\ndone queue index: ");
	    printi(done_queue_index);
	    print("\n\ndata sent from device:\n\n");

		    volatile uint8_t* pointr;
	    pointr = (volatile uint8_t*)td1_buffer;
	    for(uint32_t a = 0; a < 200; a++) {
			    print_hex_byte((*(pointr + a))); 
			    print(" ");
	    }
	    print("\nTd2 buffer:\n");


	    pointr = (volatile uint8_t*)td2_buffer;
	    for(uint32_t a = 0; a < 8; a++) {
			    print_hex_byte((*(pointr + a))); 
			    print(" ");
	    }
	    print("\nTd3 buffer:\n");


	    pointr = (volatile uint8_t*)td3_buffer;
	    for(uint32_t a = 0; a < 8; a++) {
			    print_hex_byte((*(pointr + a))); 
			    print(" ");
	    } 

	    print("\nTd4 buffer:\n");

	    pointr = (volatile uint8_t*)td4_buffer;
	    for(uint32_t a = 0; a < 8; a++) {
			    print_hex_byte((*(pointr + a))); 
			    print(" ");
	    } 



	    ohci_setup_ed->endpoint_info = 0x00404000;
	    ohci_setup_ed->head_pointer = (volatile uint32_t)1;
	    disable_ctrl_eds();
	    ohci_setup_ed->tail_pointer = (volatile uint32_t)1;
	    ohci_setup_ed_0->next_ed = 0;

	    asm("cli");
	    return 0;

}

/* Returns the HC interrupt status register. */
uint32_t get_ohci_intstatus()
{
	    uint32_t intsts = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000000c);
	    return (uint32_t)intsts;
}

/* Returns the root hub port status of port 1. */
uint32_t get_ohci_port1()
{
	    uint32_t port1 = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000054);
	    return (uint32_t)port1;
}

/* Returns the root hub port status of port 2. */
uint32_t get_ohci_port2()
{
	    uint32_t port2 = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000058);
	    return (uint32_t)port2;
}

/* Returns the root hub port status of port 3. */
uint32_t get_ohci_port3()
{
	    uint32_t port3 = read_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x0000005c);
	    return (uint32_t)port3;
}
