/* This code is EHCI code. An EHC is an Enhanced
 * Host Controller usually for USB 2.
 */

#include "../../types.h"
#include "../../system.h"
#include "../pci.h"
#include "usb_ehci.h"
#include "usb_hubs.h"
#include "usb.h"
#include "../../graphics/screen.h"
#include "../../mem.h"
#include "../../timer.h"
#include "../../isr.h"
#include "../../irq.h"


/* Prints the enhanced host controller's base address */
void read_ehci_baddr2()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)PCI_Config_RW2(0,4,0,16),(const uint32_t)0x00000000);
        uint32_t ebaddr = read_dword((const uint32_t)PCI_Config_RW2(0,4,0,16),(const uint32_t)0x00000000);
        print("\n");
        print("Base address: ");
        printi((uint32_t)&ebaddr2);
        print("\n");
        print("In hex: ");
        print_hex((uint32_t)&ebaddr2);
        print("\n");
        print("Value: ");
        printi((uint32_t)ebaddr);
        print("\n");
        print("In hex: ");
        print_hex((uint32_t)ebaddr);
        print("\n");
        print("In binary: ");
        dec_to_bin((uint32_t)ebaddr);
}

/* Initalizes some variables. */
void init_ehci_stuff()
{
        check_ports_flag = 0;
        ehci_port_busy = 0;
        async_int_success = 0;
        ehci_data_toggle1 = 0;
        ehci_data_toggle2 = 1;
        ehci_64bit_e = read_ehci_64bit_enabled();
}


/* A linked list implementation with fixed list nodes which means it does not dynamically allocate new ones.
 * but hey, it's a start. somewhere in this folder is code for linked lists which are dynamically allocated. almost finished as of 2 feb 2019
 * these lists will hold usb transfers. uhci, ohci, and starting with ehci, below.
 *
 * for ehci:
 * each QHhead list will hold a queue_transfer_desctiptor list among other things.
 * the QHhead list can in abstract ways be looked at as horizontal. and the queue transfer list as vertical
 * a host controller (a chip on the motherboard which communicates with the USB device and the USB system) will first check the memory address of the first QH.
 * This first QH can be a empty QH with a pointer to the next QH.
 * This next QH is checked and the endpoint infos etc. are being checked. Also the pointer to a queue transfer descriptor is being followed and in that queue transfer
 * descriptor is the actual transfer info for the transfer.
 * each queue transfer descriptor can be linked to yet another queue transfer descriptor, and that next one can be linked to yet another one, and so on. for more transfers in that endpoint.
 * each queue transfer descriptor usually represents a maximum of 512 bytes of data to be transfered.
 * now an endpoint ( a functionality of the usb device with for example a certain data direction) is represented by a QH head.
 * when all queue transfer descriptors have been taken care of, the host controller will check at the pointer to the next QH (next endpoint/functionality).
 * let's say that there are no more QHs for now (which is usually not the case, but whatever). The last QH that was checked will now point to the first QH.
 * And then the host contoller notices that the transfers will end for now. Well, until the driver activates the linked lists again.
 *
 *
 * The last QH's (or the first QH's, I don't remember right now) pointer to the current queue transfer descriptor (qtd) , or the next qtd, can be the last qtd that was handled.
 * If the driver will make the first QH point to itself again (so that it becomes an infinite loop for the host controller, i.e. it stops handling the data you want to send, and stays on the empty QH) , and then set the other QH's to 
 * point to their first qtd's or new qtd's, that's when the driver inserts new lists, in a controlled manner. When all QHs are in order, then to start again, the driver makes the first empty QH point to the next QH, and voila, the
 * transfers are on again. 
 */
void init_queue_heads()
{

        QHhead19 = (struct queue_head*)kmem_4k_allocate();
        QHhead20 = (struct queue_head*)kmem_4k_allocate();
        QHhead21 = (struct queue_head*)kmem_4k_allocate();
        QHhead22 = (struct queue_head*)kmem_4k_allocate();

        qtd29 = (struct queue_transfer_descriptor*)kmem_4k_allocate();
        qtd30 = (struct queue_transfer_descriptor*)kmem_4k_allocate();
        qtd31 = (struct queue_transfer_descriptor*)kmem_4k_allocate();
        qtd32 = (struct queue_transfer_descriptor*)kmem_4k_allocate();
        qtd33 = (struct queue_transfer_descriptor*)kmem_4k_allocate();
        qtd34 = (struct queue_transfer_descriptor*)kmem_4k_allocate();

        /* so we got 4 QHs and 5 qts to work with */

        /* pointing to itself to start with so that it becomes useless. */
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | 2); 
        QHhead19->qhcurrent_qtd_ptr = (struct queue_transfer_descriptor*)0;
        QHhead19->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)1;
        /* 1 means that the host controller ignores it */
        QHhead19->qhalternate_qtd_ptr = (struct queue_transfer_descriptor*)1; 
        QHhead19->endpointch1 = (volatile uint32_t)(0 | (1 << 15));
        QHhead19->endpointch2 = (volatile uint32_t)0;
        QHhead19->qhtransfer_info = (volatile uint32_t)0;
        QHhead19->qhbuffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead19->qhbuffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead19->qhbuffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead19->qhbuffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead19->qhbuffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();

        zero_usbms_mem_1(QHhead19->qhbuffer_ptr0, QHhead19->qhbuffer_ptr1, QHhead19->qhbuffer_ptr2, QHhead19->qhbuffer_ptr3, QHhead19->qhbuffer_ptr4);


        QHhead20->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | 2);
        QHhead20->qhcurrent_qtd_ptr = (struct queue_transfer_descriptor*)0;
        QHhead20->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)1;
        QHhead20->qhalternate_qtd_ptr = (struct queue_transfer_descriptor *)((15 << 1) | 1);
        QHhead20->endpointch1 = (volatile uint32_t)0;
        QHhead20->endpointch2 = (volatile uint32_t)(0 | (1 << 30));
        QHhead20->qhtransfer_info = (volatile uint32_t)0;
        QHhead20->qhbuffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead20->qhbuffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead20->qhbuffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead20->qhbuffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead20->qhbuffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();

        QHhead20->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | 2); 


        zero_usbms_mem_1(QHhead20->qhbuffer_ptr0, QHhead20->qhbuffer_ptr1, QHhead20->qhbuffer_ptr2, QHhead20->qhbuffer_ptr3, QHhead20->qhbuffer_ptr4);

        QHhead20->endpointch1 = (volatile uint32_t)(0x00000000 | (0 << 27) | EHCI_QH_MAX_NAK_COUNTER | EHCI_QH_USE_QTD_TOGGLE | EHCI_QH_MAX_PACKET_SIZE(64) | (2 << 12) | EHCI_QH_DEVICE_ADDRESS(0));
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | 2);

        //EHCI_QH_USE_QTD_TOGGLE | 


        QHhead21->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead21 | 2);
        QHhead21->qhcurrent_qtd_ptr = (struct queue_transfer_descriptor*)0;
        QHhead21->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)1;
        QHhead21->qhalternate_qtd_ptr = (struct queue_transfer_descriptor *)((15 << 1) | 1);
        QHhead21->endpointch1 = (volatile uint32_t)0;
        QHhead21->endpointch2 = (volatile uint32_t)(0 | (1 << 30));
        QHhead21->qhtransfer_info = (volatile uint32_t)0;
        QHhead21->qhbuffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead21->qhbuffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead21->qhbuffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead21->qhbuffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead21->qhbuffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();


        zero_usbms_mem_1(QHhead21->qhbuffer_ptr0, QHhead21->qhbuffer_ptr1, QHhead21->qhbuffer_ptr2, QHhead21->qhbuffer_ptr3, QHhead21->qhbuffer_ptr4);


        QHhead22->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead22 | 2);
        QHhead22->qhcurrent_qtd_ptr = (struct queue_transfer_descriptor*)0;
        QHhead22->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)1;
        QHhead22->qhalternate_qtd_ptr = (struct queue_transfer_descriptor *)((15 << 1) | 1);
        QHhead22->endpointch1 = (volatile uint32_t)0;
        QHhead22->endpointch2 = (volatile uint32_t)(0 | (1 << 30));
        QHhead22->qhtransfer_info = (volatile uint32_t)0;
        QHhead22->qhbuffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead22->qhbuffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead22->qhbuffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead22->qhbuffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
        QHhead22->qhbuffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();


        zero_usbms_mem_1(QHhead22->qhbuffer_ptr0, QHhead22->qhbuffer_ptr1, QHhead22->qhbuffer_ptr2, QHhead22->qhbuffer_ptr3, QHhead22->qhbuffer_ptr4);


        qtd29->next_qtd_ptr = (struct queue_transfer_descriptor *)1; 
        qtd29->alternate_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd29->transfer_info = (volatile uint32_t)0;
        qtd29->buffer_ptr0 = (uint8_t*)kmem_4k_allocate();
        qtd29->buffer_ptr1 = (uint8_t*)kmem_4k_allocate();
        qtd29->buffer_ptr2 = (uint8_t*)kmem_4k_allocate();
        qtd29->buffer_ptr3 = (uint8_t*)kmem_4k_allocate();
        qtd29->buffer_ptr4 = (uint8_t*)kmem_4k_allocate();

        qtd30->next_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd30->alternate_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd30->transfer_info = (volatile uint32_t)0;
        qtd30->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
        qtd30->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
        qtd30->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
        qtd30->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
        qtd30->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();


        zero_usbms_mem_1(qtd30->buffer_ptr0, qtd30->buffer_ptr1, qtd30->buffer_ptr2, qtd30->buffer_ptr3, qtd30->buffer_ptr4);


        qtd31->next_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd31->alternate_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd31->transfer_info = (volatile uint32_t)0;
        qtd31->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
        qtd31->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
        qtd31->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
        qtd31->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
        qtd31->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();


        zero_usbms_mem_1(qtd31->buffer_ptr0, qtd31->buffer_ptr1, qtd31->buffer_ptr2, qtd31->buffer_ptr3, qtd31->buffer_ptr4);


        qtd32->next_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd32->alternate_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd32->transfer_info = (volatile uint32_t)0;
        qtd32->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
        qtd32->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
        qtd32->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
        qtd32->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
        qtd32->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();


        zero_usbms_mem_1(qtd32->buffer_ptr0, qtd32->buffer_ptr1, qtd32->buffer_ptr2, qtd32->buffer_ptr3, qtd32->buffer_ptr4);


        qtd33->next_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd33->alternate_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd33->transfer_info = (volatile uint32_t)0;
        qtd33->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
        qtd33->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
        qtd33->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
        qtd33->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
        qtd33->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();

        qtd34->next_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd34->alternate_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd34->transfer_info = (volatile uint32_t)0;
        qtd34->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
        qtd34->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
        qtd34->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
        qtd34->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
        qtd34->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();


        zero_usbms_mem_1(qtd33->buffer_ptr0, qtd33->buffer_ptr1, qtd33->buffer_ptr2, qtd33->buffer_ptr3, qtd33->buffer_ptr4);

}

/* Sets the operational offset variable. Consult the
 * EHCI specification for details about what the
 * operational offset is.
 */
void set_ehci_operationaloffset()
{
         uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)0x00000000);
         ebaddr &= 0x000000ff;
         operational_offset = ebaddr;
}

/* Prints the memory offset of the operational offset */
void print_ehci_operationaloffset()
{
         print("\nehci operation regs are at offset: ");
         printi(operational_offset);
         print("\n");
}

/* Prints the HCCParams register. Bit number 0 indicates wheher the 
 * host controller are capable of handling 64 bits of data.
 */
void print_ehci_64bit_enabled()
{
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)0x00000008);
        print("\n64 bit enabled at bit 0:\n");
        dec_to_bin(ebaddr);
        print("\n");
}

/* Bit number zero of the HCCParams register, i.e.
 * the 64 bit indicator, is stored in a variable.
 */
uint8_t read_ehci_64bit_enabled()
{
        uint32_t ehci_addr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)0x00000008);
        ehci_addr &= 0x00000001;
        return (uint8_t)ehci_addr;
}

/* Prints a register in decimal, hex and binary notation */
void print_ehc_info(uint32_t addr, uint32_t* addr2)
{
        print("\n");
        print("Base address: ");
        printi((uint32_t)&addr2);
        print("\n");
        print("In hex: ");
        print_hex((uint32_t)&addr2);
        print("\n");
        print("Value: ");
        printi((uint32_t)addr);
        print("\n");
        print("In hex: ");
        print_hex((uint32_t)addr);
        print("\n");
        print("In binary: ");
        dec_to_bin((uint32_t)addr);	
}

/* Prints addresses. */
void print_ehc_info_2(uint32_t addr)
{
        print("\n");
        print("Value: ");
        printi((uint32_t)addr);
        print("\n");
        print("In hex: ");
        print_hex((uint32_t)addr);
        print("\n");
        print("In binary: ");
        dec_to_bin((uint32_t)addr);	
}

/* Toggles some data. */
void check_transfer10noprint_data_toggles()
{
        if(ehci_data_toggle1 == 0)
                ehci_data_toggle1 = 1;
        else if (ehci_data_toggle1 == 1)
                ehci_data_toggle1 = 0;
}

/* Sets the ehc interrput unhandled variable to zero. */
void set_ehci_int_unhandled()
{
        ehci_int_unhandled = 0;
}

/* Prints the HCSParams register */
void read_ehci_hcparams()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)0x00000004);
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)0x00000004);
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints the HCCParams register */
void read_ehci_hccparams()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)0x00000008);
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)0x00000008);
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints the USB CMD register */
void read_ehci_usbcmd()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000000));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000000));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints the USB Status register */
void print_ehci_usbsts()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000004));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000004));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Returns the USB Status register */
uint32_t read_ehci_usbsts()
{
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000004));
        return ebaddr;
}

/* Prints the USB Interrupt register */
void read_ehci_usbint()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000008));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000008));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints the USB frame index register */
void read_ehci_frindex()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x0000000c));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x0000000c));
        print_ehc_info(ebaddr, ebaddr2);
}
/* Prints the Control Segment Register */
void read_ehci_ctrldssegment()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000010));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000010));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints the Periodic List Base Address Register */
void read_ehci_periodiclistbase()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000014));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000014));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints the Current Asynchronous List Address Register */
void read_ehci_asynclistaddr()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000018));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000018));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints the Configure Flag Register */
void read_ehci_configflag()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000040));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000040));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints Port 1 */
void read_ehci_port1()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000044));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000044));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints Port 2 */
void read_ehci_port2()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000048));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset+ 0x00000048));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints Port 3 */
void read_ehci_port3()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x0000004c));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x0000004c));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints Port 4 */
void read_ehci_port4()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000050));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000050));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints Port 5 */
void read_ehci_port5()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000054));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000054));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints Port 6 */
void read_ehci_port6()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000058));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000058));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints Port 7 */
void read_ehci_port7()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x0000005c));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x0000005c));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Prints Port 8 */
void read_ehci_port8()
{
        uint32_t * ebaddr2 = read_dword_addr((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000060));
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000060));
        print_ehc_info(ebaddr, ebaddr2);
}

/* Resests the 0 bit in the USB Status Register after a completed
 * USB transaction. A TD with IOC enabled will be retired and
 * this should also be set when a short packet is detected.
 * Writing a 1 will set the bit to 0.
 */
void resetpc_ehci_usbstscmplt() 
{
        uint32_t pc = get_ehci_usbsts();
        pc |= 0x1;
        write_dword((const uint32_t)PCI_Config_RW2(found_ehci_bus,found_ehci_device,found_ehci_function,16), (const uint32_t)(operational_offset + 0x00000004), (const uint32_t)pc);
}

/* Resets the port change detection bit */
void resetpc_ehci_usbsts()
{
        uint32_t pc = get_ehci_usbsts();
        pc |= 0x4;
        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000004), (const uint32_t)pc);
}

/* Resets a bit when an error condition regarding a packet
 * is detected.
 */
void resetpc_ehci_usbstserr()
{
        uint32_t pc = get_ehci_usbsts();
        pc |= 0x2;
        write_dword((const uint32_t)PCI_Config_RW2(found_ehci_bus,found_ehci_device,found_ehci_function,16), (const uint32_t)(operational_offset + 0x00000004), (const uint32_t)pc);
}

/* Resets a bit when a host system error is detected. */
void resetpc_ehci_usbstshosterr()
{
        uint32_t pc = get_ehci_usbsts();
        pc |= 0x10;
        write_dword((const uint32_t)PCI_Config_RW2(found_ehci_bus,found_ehci_device,found_ehci_function,16), (const uint32_t)(operational_offset + 0x00000004), (const uint32_t)pc);
}

/* Resets a bit when the async schedule is being advanced. */
void resetpc_ehci_usbstsasyncadvance() 
{
        uint32_t pc = get_ehci_usbsts();
        pc |= 0x20;
        write_dword((const uint32_t)PCI_Config_RW2(found_ehci_bus,found_ehci_device,found_ehci_function,16), (const uint32_t)(operational_offset + 0x00000004), (const uint32_t)pc);
        async_int_success = 0;
}

/* Acts as the general EHCI interrupt handler. Some of the
 * functions above are included in this function. 
 */
void ehci_usb_handler()
{
        uint32_t usbsts = get_ehci_usbsts();
        if (((usbsts & 0x10) >> 4) == 1) {
                resetpc_ehci_usbstshosterr();
                stop_ehci();
                reset_ehci();		
                print("Host system error. Resetting the ehc.\n\n");
                print(csptr);
        }
        if (((usbsts & 0x2) >> 1) == 1) {
                resetpc_ehci_usbstserr();		
                print("Error during transaction\n\n");
                print(csptr);
	    }
        if (((usbsts & 0x4) >> 2) == 1) {
                resetpc_ehci_usbsts();
                Get_Port_Status();
        }
        if ((usbsts & 0x1) == 1) {
                resetpc_ehci_usbstscmplt();

        }
        if (((usbsts & 0x20) >> 5) == 1) {

                resetpc_ehci_usbstsasyncadvance();
                print(csptr);
        }
}

/* Reads the intpin of the EHC from the PCI Configuration Space */
void read_ehci_intpin(uint32_t bus, uint32_t device, uint32_t funct)
{
        uint16_t vendor;
        uint16_t ebaddr;
        if ((vendor = PCI_Config_RW(bus,device,0,0)) != 0xFFFF) 
                ebaddr = ((PCI_Config_RW(bus,device,funct,0x3d) >> 8) & 0x00ff);
        else
                ebaddr = 512;

        print_ehc_info_2((uint32_t)ebaddr);
}

/* gets the address of the extra cap register from hccparams */
uint32_t get_ehci_extracap()
{
        uint32_t addr = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000008); 
        addr >>= 8;
        addr &= 0x000000ff;
        return addr;
}

/* Gets the amount of ports from hcsparams */
void set_ehci_port_n()
{
        uint32_t addr = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000004);
        addr &= 0x0000000f;
        ehci_port_num = addr;
}


/* Prints the extra cap register from hccparams. */
void print_ehci_extracap()
{
        uint32_t addr = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000000); 
        print_ehc_info_2((uint32_t)addr);
}

/* Prints the EHC's command register from 
 * the PCI Configuration Space. 
 */
void print_ehci_commandreg()
{
        uint16_t temp_cmd = PCI_Config_RW(found_ehci_bus,found_ehci_device,found_ehci_function,4);
        uint32_t ehci_pci_cmd = (uint32_t)temp_cmd;
        print("\n\n ehci pci commandreg low word");
        print_ehc_info_2((uint32_t)ehci_pci_cmd);

        print("\n\n ehci pci commandreg high word");
        temp_cmd = PCI_Config_RW(found_ehci_bus,found_ehci_device,found_ehci_function,6);
        ehci_pci_cmd = (uint32_t)temp_cmd;
        print_ehc_info_2((uint32_t)ehci_pci_cmd);
}


/* Sets some parameters in the command register for the EHC.
 * The parameters mean that SERR is enabled, and that it has access
 * to i/o and memory space and that it acts as a bus master. 
 */
void set_ehci_commandreg()
{
        /* command */
        uint16_t temp_cmd = PCI_Config_RW(found_ehci_bus,found_ehci_device,found_ehci_function,4); 
        /* status */
        uint16_t temp_cmd2 = PCI_Config_RW(found_ehci_bus,found_ehci_device,found_ehci_function,6); 
        uint32_t temp_cmd3 = (uint32_t)(temp_cmd2*65536);

        uint32_t temp_cmd4 = (uint32_t)temp_cmd;
        uint32_t temp_cmd5 = temp_cmd3 + temp_cmd4;
        temp_cmd5 |= 0x107;


        uint32_t address = (uint32_t)((0 << 16) | (4 << 11) | (0 << 8) | ((1 << 2) & 0xfc) | ((uint32_t)0x80000000));
        outportl(0xCF8, address);
        outportl(0xCFC, temp_cmd5);
}


/* Writes a zero to the USB Legacy Support Control/Status
 * register for the EHC located at the PCI Configuration Space.
 */
void disable_ehci_legacy()
{
        /* get extra cap register from hccparams */
        uint16_t ehci_ext_cap_offset = (uint16_t)get_ehci_extracap(); 
        ehci_ext_cap_offset += 4;
        uint32_t address = (uint32_t)((0 << 16) | (4 << 11) | (0 << 8) | (ehci_ext_cap_offset & 0xfc) | ((uint32_t)0x80000000));

        outportl(0xCF8, address);
        outportl(0xCFC, (const uint32_t)0x00000000);
}

/* Prints the legacy register of the EHC */
void print_ehci_legacy()
{
        /* hccparams */
        uint32_t addr = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000008); 
        addr >>= 8;
        addr &= 0x000000ff;
        uint16_t addr2 = (uint16_t)addr;

        addr2 += 4;

        uint16_t ebaddr = PCI_Config_RW(found_ehci_bus,found_ehci_device,found_ehci_function,addr2);		
        print("\n\nlegacy register low word");
        print_ehc_info_2((uint32_t)ebaddr);
        addr2 += 2;
        ebaddr = PCI_Config_RW(found_ehci_bus,found_ehci_device,found_ehci_function,addr2);
        print("\n\nlegacy register high word");
        print_ehc_info_2((uint32_t)ebaddr);
        print("\n");
}


/* Lets the OS take control of the EHC. */
void bios_hand_off()
{
        /* get extra cap register from hccparams */
        uint16_t ehci_ext_cap_offset = (uint16_t)get_ehci_extracap(); 
        uint16_t temp_hand_off = PCI_Config_RW(found_ehci_bus,found_ehci_device,found_ehci_function,ehci_ext_cap_offset);
        ehci_ext_cap_offset += 2;

        uint16_t temp_hand_off2 = PCI_Config_RW(found_ehci_bus,found_ehci_device,found_ehci_function,ehci_ext_cap_offset); 
        uint32_t temp_hand_off3 = (uint32_t)(temp_hand_off2*65536);
        uint32_t temp_hand_off4 = (uint32_t)temp_hand_off;
        uint32_t temp_hand_off5 = temp_hand_off3 + temp_hand_off4; 

        /* and write 32 bits to the config space */
        temp_hand_off5 |= (1 << 24);
        ehci_ext_cap_offset -= 2;
        uint32_t address = (uint32_t)((0 << 16) | (4 << 11) | (0 << 8) | (ehci_ext_cap_offset & 0xfc) | ((uint32_t)0x80000000));
        outportl(0xCF8, address);
        outportl(0xCFC, temp_hand_off5);
}


/* Returns the contents of the USB CMD register. */
uint32_t get_ehci_usbcmd()
{
        uint32_t ebaddr = read_dword((const uint32_t)ehci_mem_address,(const uint32_t)(operational_offset + 0x00000000));
        return ebaddr;
}


/* Initializes the EHC by writing to the asynchronous schedule address
 * register and other things. It uses many functions in this file.
*/
uint8_t init_ehci(struct queue_head * head)
{
        asm("sti");
        uint32_t* temp_1 = (uint32_t*)0x600000;

        irq_uninstall_handler(0);
        init_ehci_stuff();
        set_ehci_operationaloffset();
        set_ehci_port_n();
        asm("cli");
        timer_install();
        asm("sti");
        bios_hand_off();
        stop_ehci();
        timer_ticks = 0;
        while(timer_ticks < 1);
        reset_ehci();
        timer_ticks = 0;
        while(timer_ticks < 1);
        uint32_t tempinit = read_ehci_usbsts();

        if (((tempinit & 0x4) >> 2) == 1) {
                tempinit |= 0x4;
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000004), (const uint32_t)tempinit);
        }

        write_ehci_ctrldssegment();
        write_ehci_usbint();
        write_ehci_usbcmd2();
        write_ehci_configflag();

        timer_ticks = 0;
        while(timer_ticks < 1);
        irq_uninstall_handler(0);
        asm("cli");

        temp_1 = port_reset();

        if(!(*(temp_1))) {

                print("Error: could not enable port ");
                printi((uint32_t)(*(temp_1 + 1)));
                print(". Ehc stopped. Reset your computer.");
                stop_ehci();
                reset_ehci();
                return 0;

        }

        ehci_port_busy = 0;
        /* Commented code: if (ehci_int_unhandled == 1) {
                ehci_int_unhandled = 0; 
                Get_Port_Status();
        } */
        timer_install();
        asm("sti");
        irq_uninstall_handler(0);
        asm("cli");
        asm("sti");

        write_async_addr(head);
        enable_async();
        print("\n");
        asm("cli");
        return 1;
}

/* This function should not be in use, I guess. */
void write_ehci_ctrldssegment()
{
        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000010), (const uint32_t)0x00000000);
}

/* Enables some interrupts. Interrupt on async advance is excluded */
void write_ehci_usbint()
{
        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000008), (const uint32_t)0x00000017);
}

/* periodicframebase = 0xf00000, base for itds are f02000 */
void write_ehci_periodicbase()
{
        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000014), (const uint32_t)0x01000000); 

}

/* Sets the configuration flag to 1 */
void write_ehci_configflag()
{
        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000040), (const uint32_t)0x00000001);

}

/* This function is unfinished. */
void fill_ehci_periodiclistbase()
{
        print("hi 1");
}

/* Disables the periodic list and the asynchronous schedule */
void write_ehci_usbcmd2()
{
        uint32_t tmp = get_ehci_usbcmd();
        tmp &= (~(1 << 4));
        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)tmp);
        tmp = get_ehci_usbcmd();
        tmp &= (~(1 << 5));
        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)tmp);
}

/* Sets the maximum interrupt interval to 8 micro-frames */
void write_ehci_usbcmd()
{
        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)0x00080020);
}

/* Writes a one to the doorbell bit regarding the asynchronous schedule */
void write_ehci_async_interrupt()
{
        uint32_t temp2 = get_ehci_usbsts();

        /* if async enabled */
        if (((temp2  & (1 << 15)) >> 15) == 1) { 
                uint32_t temp = get_ehci_usbcmd();
                temp |= (1 << 6);
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)temp);
        }
        else
                print("\n\nError: Could not enable async interrupt on usbcmd register.");
}

/* Halts the EHC. */
void stop_ehci()
{
        uint32_t temp;
        uint32_t temp2;
        temp = get_ehci_usbsts();

        /* ehc halted = 0 = running */
        if (((temp & (1 << 12)) >> 12) == 0) {
                temp2 = get_ehci_usbcmd();
                temp2 &= ~(1);
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)temp2);
        }
}

/* Resets the EHC. */
uint8_t reset_ehci()
{
        uint8_t reset_error = 0;
        uint32_t temp;
        temp = get_ehci_usbcmd();
        uint32_t temp2;
        temp2 = get_ehci_usbsts();

        /* if halted is 1. */
        if (((temp2 & (1 << 12)) >> 12) == 1) {
                temp |= 2;
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)temp);
        }
        else {
                print("\n\nError: Could not reset the ehc.\n\n");
                reset_error++;
        }

        return reset_error;
}

/* Resets the EHC. */
void reset_ehci_2()
{
        uint32_t temp;
        temp = get_ehci_usbcmd();
        uint32_t temp2;
        temp2 = get_ehci_usbsts();

        /* if halted is 1. */
        if (((temp2 & (1 << 12)) >> 12) == 1) {
                temp |= 2;
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)temp);
        }
		else {
                print("\n\nError: Could not reset the ehc.\n\n");
        }
}

/* Starts the EHC. */
void start_ehci()
{
        uint32_t checksts;
        uint32_t checkcmd = get_ehci_usbcmd();
        checksts = read_ehci_usbsts();

        if ( (((checksts & (1 << 12)) >> 12) == 1) && ((checkcmd & 0x00000001) == 0)) {
                checkcmd = get_ehci_usbcmd();
                checkcmd |= 1;
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)checkcmd);
        }
}


/* Checks if the host controller has stopped traversing
 * the async schedule.
 */
uint32_t get_reclamation_bit() 
{
        uint32_t return_value;
        uint32_t tmp = get_ehci_usbsts();

        if(((tmp >> 13) & 0x00000001) == 1)
                return_value = 1;
        else
                return_value = 0;

        return return_value;
}

/* Enables the async schedule. The async schedule can be
 * used for USB flash drive transfers, for example.
 */
void enable_async() 
{
        uint32_t temp_async = get_ehci_usbsts();
        uint32_t temp_async2 = get_ehci_usbcmd();

        /* if async is disabled */
        if ((((temp_async >> 15) & 0x00000001) == 0) && (((temp_async2 >> 5) & 0x00000001) == 0)) {
                uint32_t tmp = get_ehci_usbcmd();
                tmp |= (1 << 5);
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)tmp);
        }
        else
                print("\nCould not enable async or async already enabled");
}

/* Disables the async schedule */
void disable_async()
{
        uint32_t temp_async = get_ehci_usbsts();

        /* if async is enabled */
        if (((temp_async >> 15) & 0x00000001) == 1) {
                uint32_t tmp = get_ehci_usbcmd();
                tmp &= (~(1 << 5));
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000000), (const uint32_t)tmp);
        }
}



/* Writes the address of a Queue Head as the first async schedule
 * data structure.
 */
void write_async_addr(struct queue_head* q) 
{
        timer_install();
        asm("sti");
        disable_async();
        timer_ticks = 0;
        while(timer_ticks < 1);
        uint32_t temp_async = get_ehci_usbsts();

        /* if async is disabled */
        if (((temp_async >> 15) & 0x00000001) == 0) {
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)(operational_offset + 0x00000018), (const uint32_t)q);

        }
        else
                print("\nCould not write qh to async addr.");

        irq_uninstall_handler(0);
        asm("cli");
} 

/* This is a test function to test whether the async schedule
 * did advance or not.
 */
uint32_t async_advance()
{
        timer_install();
        asm("sti");
        write_ehci_async_interrupt();
        uint32_t count = 0;
        while(count++ < 2000);


        if (async_int_success == 0) {
                print("\n\nasync advanced\n\n");
                irq_uninstall_handler(0);
                asm("cli");
                return 1;
        }
        else {
                print("Error: time out. Async did not advance.");
                irq_uninstall_handler(0);
                asm("cli");
                return 0;
        }

        irq_uninstall_handler(0);
        asm("cli");
        return 1;
}

/* Makes an USB transaction on the asynchronous schedule
 * getting the device descriptor and the configuration descriptor
 */
void usb_ehci_get_descriptor(volatile uint8_t descriptor_type, volatile uint8_t usb_length_low)
{

        volatile uint8_t* pointr;
        volatile uint32_t temp;
        /* Commented code: struct setup_data* device_descriptor = kmem_4k_allocate();
         * device_descriptor->RequestType = (volatile uint8_t)USB_DEVICE_TO_HOST;
         * device_descriptor->Request = (volatile uint8_t)USB_GET_DESCRIPTOR;
         * device_descriptor->value_low = (volatile uint8_t)0x00;
         * device_descriptor->value_high = (volatile uint8_t)descriptor_type;
         * device_descriptor->index_low = (volatile uint8_t)0x00;
         * device_descriptor->index_high = (volatile uint8_t)0x00;
         * device_descriptor->length_low = usb_length_low;
         * device_descriptor->length_high = (volatile uint8_t)0x00;
        */
        struct setup_data* device_descriptor = fill_usb_setup_data(USB_DEVICE_TO_HOST, USB_GET_DESCRIPTOR, 0x00, descriptor_type, 0x00, 0x00, usb_length_low, 0x00);

        pointr = (volatile uint8_t*)device_descriptor;

        if (ehci_64bit_e == 0) {
                temp = (volatile uint32_t)qtd31->buffer_ptr0;
                temp &= 0xfffff000;
                qtd31->buffer_ptr0 = (volatile uint8_t*)temp;
                qtd30->buffer_ptr0 = pointr;
        }
        else {
                temp = (volatile uint32_t)qtd31->qt_extended_buffers[0];
                temp &= 0xfffff000;
                qtd31->qt_extended_buffers[0] = (volatile uint32_t)temp;
                qtd30->qt_extended_buffers[0] = (volatile uint32_t)pointr;
        }


        qtd30->next_qtd_ptr = (struct queue_transfer_descriptor *)qtd31;
        qtd30->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(1) | EHCI_QTD_BYTES_TTRANSFER(8) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(0) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_SETUP_TOKEN | EHCI_QTD_SET_ACTIVE));

        qtd31->next_qtd_ptr = (struct queue_transfer_descriptor *)qtd32;
        qtd31->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(0) | EHCI_QTD_BYTES_TTRANSFER(usb_length_low) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(1) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));

        qtd32->next_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd32->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(0) | EHCI_QTD_BYTES_TTRANSFER(18) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(2) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_OUT_TOKEN | EHCI_QTD_SET_ACTIVE));

        qtd33->next_qtd_ptr = (struct queue_transfer_descriptor *)qtd34;
        qtd33->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(1) | EHCI_QTD_BYTES_TTRANSFER(8) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(0) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));

        qtd34->next_qtd_ptr = (struct queue_transfer_descriptor *)1;
        qtd34->transfer_info = (volatile uint32_t)(0x00000000 | (EHCI_QTD_DATA_TOGGLE(0) | EHCI_QTD_BYTES_TTRANSFER(2) | EHCI_QTD_SET_INTC | EHCI_QTD_CURRENT_PAGE(0) | EHCI_QTD_ERRORS_COUNT(3) | EHCI_QTD_SET_IN_TOKEN | EHCI_QTD_SET_ACTIVE));


        QHhead20->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)qtd30;
        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead20 | EHCI_QH_TYPE_QH);
        /* Now the USB transaction begins. */


        kernel_delay_100(25);



        pointr = (volatile uint8_t*)device_descriptor;
        free_mem_uint((uint32_t)pointr);

        QHhead19->Horizontalpointer = (struct queue_head*)((uint32_t)QHhead19 | EHCI_QH_TYPE_QH);


}

/* Resets a port so that it becomes useful for transactions */
uint32_t* port_reset() 
{
        /* suggestion: use pointer to a function (get_ehci_port1) and the 0x000000074, for example, as arguments. with control statements */
        ehci_port_busy = 1;
        uint32_t* temp_ptr = ehci_port_array;
        timer_install();
        asm("sti");
        uint32_t attached_to_usb2_port = 0;
        /* uint32_t temp = 0; */
        uint32_t temp = 0;
        uint32_t temp2 = 0;
        uint32_t tempsts2 = 0;
        uint32_t usb_port_n = operational_offset + 0x00000040;

        temp = ehci_port_num;

        for(uint32_t i = 0; (i < temp) && (attached_to_usb2_port == 0); i++) {
                usb_port_n += 4;
                temp2 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port_n);
                if ((temp2 & 0x00000001))
                        attached_to_usb2_port = 1; 

        } 
        print("\nDevice attached to port ");
        printi((uint32_t)((usb_port_n - (operational_offset + 0x00000040)) / 4));
        /* Commented code: temp2 = (uint32_t)read_dword((const uint32_t)
                           ehci_mem_address, (const uint32_t)usb_port_n);
                           if((temp2 & 0x00000001) == 0) {
                                   irq_uninstall_handler(0);
                                   asm("cli");
                                   *(temp_ptr) = 0;
                                   *(temp_ptr + 1) = ((usb_port_n - (operational_offset + 0x00000040)) / 4);
                                   return temp_ptr;
                           } */
        temp = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port_n);
        temp &= 0x00000004;
        if(temp != 4) {

                /* attempt to reset the port */
                tempsts2 = read_ehci_usbsts();
                if(tempsts2 & (1 << 12)) {
                        start_ehci();
                        timer_ticks = 0;
                        while(timer_ticks < 1);
                }

                tempsts2 = read_ehci_usbsts();
                /* HCHalted must be zero = running */
                if((tempsts2 & (1 << 12)) == 0)  {
                        print("\nAttempting to enable the port..\n");
                        temp2 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port_n);
                        /* reset and enable change */
                        temp2 |= ((1 << 8) | (1 << 3)); 
                        /* reset and port enable change */
                        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port_n, (const uint32_t)temp2); 
                        timer_ticks = 0;

                        while(timer_ticks < 8); 		        
                        temp2 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port_n);

                        if(temp2 & (1 << 8)) {

                                /* write a zero to bit 8 = port reset */
                                temp2 &= (~(1 << 8)); 
                                /* port reset */
                                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port_n, (const uint32_t)temp2); 
                                kernel_delay_100(25);
                        }

                        temp2 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port_n);
                        /* if port enable change detect = 1 and port reset = 0 */
                        if((temp2 & (1 << 3)) && (((temp2 & (1 << 8)) == 0)))  {
                                /* write clear. writing to it causes it to go to zero */
                                temp2 |= (1 << 3); 
                                /* write zero to port enable change detect */
                                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port_n, (const uint32_t)temp2); 
                                kernel_delay_100(10);
                        }

                }


                temp2 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port_n);
                if((temp2 & (1 << 2))) {
                        irq_uninstall_handler(0);
                        asm("cli");
                        *(temp_ptr) = 1;
                        *(temp_ptr + 1) = ((usb_port_n - (operational_offset + 0x00000040)) / 4);
                        return temp_ptr;

                }

        }

        irq_uninstall_handler(0);
        asm("cli");
        *(temp_ptr) = 0;
        *(temp_ptr + 1) = ((usb_port_n - (operational_offset + 0x00000040)) / 4);
        return temp_ptr;

}

