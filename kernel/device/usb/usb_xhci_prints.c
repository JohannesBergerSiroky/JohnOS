/* This file is for debugging by printing some 
 * register values and so on.
 */

#include "../../types.h"
#include "usb_xhci.h"
#include "usb_ehci.h"
#include "../pci.h"
#include "../../graphics/screen.h"
#include "../../mem.h"

/* Prints the command register, for the XHC, on the
 * PCI Configuration Space.
 */
void print_xhci_commandreg()
{
        uint16_t temp_cmd = PCI_Config_RW(found_xhci_bus,found_xhci_device,found_xhci_function,4);
        uint32_t xhci_pci_cmd = (uint32_t)temp_cmd;
        print("\n\n xhci pci commandreg low word");
        print_ehc_info_2((uint32_t)xhci_pci_cmd);
        print("\n\n xhci pci commandreg high word");
        temp_cmd = PCI_Config_RW(found_xhci_bus,found_xhci_device,found_xhci_function,6);
        xhci_pci_cmd = (uint32_t)temp_cmd;
        print_ehc_info_2((uint32_t)xhci_pci_cmd);
}

/* Prints the control and status register, for the XHC, on the
 * PCI Configuration Space.
 */
void print_xhci_pci_control_status()
{
        uint32_t temp = PCI_Config_RW2(found_xhci_bus,found_xhci_device,found_xhci_function,0x00000004);
        print("\nhci pci control status\n\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the caplength and the version of the XHC. */
void print_xhci_capl_version()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000000);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the HCSPARAMS1 register. */
void print_xhci_hciparams1()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000004);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the HCSPARAMS2 register. */
void print_xhci_hciparams2()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000008);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the HCSPARAMS3 register. */
void print_xhci_hciparams3()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x0000000c);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the HCCPARAMS1 register. */
void print_xhci_hccparams1()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000010);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the doorbell offset. */
void print_xhci_doorbelloffset()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000014);
        print_ehc_info_2((uint32_t)xbaddr5);

}

/* Prints the runtime register space offset.
 * It contains info about interrupters and 
 * event rings 
 */
void print_xhci_rtsoffset()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000018); 
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the HCCPARAMS2 register. */
void print_xhci_hccparams2()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x0000001c);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the XHC's USB cmd register. */
void print_xhci_usbcmd()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000000);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the page size register. */
void print_xhci_pagesize()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000008);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the device notification control register. */
void print_xhci_dvcnctrl()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000014);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the command ring control register. */
void print_xhci_crctrl()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000018);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the command ring pointer. */
void print_command_ring_control_address()
{
        uint32_t* xbaddr5 = read_dword_addr((const uint32_t)hci_operational_base,(const uint32_t)0x00000018);
        print("\ncommand ring control address:\n");
        print_hex((uint32_t)xbaddr5);
}

/* Prints the device context base address array 
 * pointer register.
 */
void print_xhci_dcbaap()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000030);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the configure register. */
void print_xhci_configure()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000038);
        print_ehc_info_2((uint32_t)xbaddr5);
}

/* Prints the doorbell offset and attempts to 
 * print the whole address of the doorbell.
 */
void print_base_doorbell_offset()
{
        volatile uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000014);
        print("\nthe offset is: ");
        print_hex((uint32_t)xbaddr5);
        print("\nthe base is: ");
        print_hex((uint32_t)xhci_mem_address_attr_doorb);

        volatile uint32_t doorbell_base = (volatile uint32_t)xbaddr5 + (volatile uint32_t)xhci_mem_address_attr_doorb;

        print("\nthe doorbell base is: ");
        print_hex((uint32_t)doorbell_base);    
}


/* Prints the scratchpad buffer array address */
void print_spba() 
{
        uint32_t temp = (* ((uint32_t*)hci_dcaba));
        print("scratchpad buffer array address: ");
        print_hex((uint32_t)temp);
}

/* Prints the addresses of the first, second
 * and third device context array
 */
void print_first_and_second_and_third_dca()
{
        volatile uint32_t* temp_ptr = (volatile uint32_t*)hci_dcaba;
        /* first array */
        print("\naddress of first device context array (scratchpad): ");
        print_hex((uint32_t)*temp_ptr);
        print("\n");
        temp_ptr+=2;

        print("\naddress of second device context array: ");
        print_hex((uint32_t)*temp_ptr);
        print("\n");
        temp_ptr+=2;
        print("\naddress of third device context array: ");
        print_hex((uint32_t)*temp_ptr);
        print("\n");
}

/* Prints whether the controller uses 64 bit address
 * memory pointers.
 */
void print_check_xhci_64_bit_capable()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000010);
        xbaddr5 &= 0x00000001;  
        if(xbaddr5) {
                print("\nhci 64 bit capable\n");
        }
        else {
                print("\nhci not 64 bit capable\n");
        } 
}

/* Prints the event ring segment table base address. */
void print_event_ring_segment_table_base()
{
        volatile uint32_t xbaddr5 = read_dword((const uint32_t)xhci_runtime_base,(const uint32_t)0x00000030);
        print("\nevent ring segment table base:\n");
        print_hex((uint32_t)xbaddr5);        
}

/* Prints the command ring in binary */
void print_command_ring()
{
        print("\ncommand ring address according to usage: ");
        print_hex((uint32_t)xhci_commandring_base);
        volatile uint32_t* cmdring_address_ptr = xhci_commandring_base;

        print("\ncommand ring:");
        dec_to_bin((uint32_t)(*cmdring_address_ptr));
        print("\n");
        dec_to_bin((uint32_t)(*(cmdring_address_ptr + 1)));
        print("\n");
        dec_to_bin((uint32_t)(*(cmdring_address_ptr + 2)));
        print("\n");
        dec_to_bin((uint32_t)(*(cmdring_address_ptr + 3)));
        print("\n");
}

/* Prints extra cap registers in binary. */
void print_first_hci_ext_cap_reg()
{
        print("\nprinting ext cap\n");
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_ext_cap_ptr,(const uint32_t)0x00000000);
        dec_to_bin((uint32_t)xbaddr5);
        print("\nnext\n\n");

        xbaddr5 = read_dword((const uint32_t)hci_ext_cap_ptr,(const uint32_t)0x00000024);
        dec_to_bin((uint32_t)xbaddr5);
        print("\n\n");

        xbaddr5 = read_dword((const uint32_t)hci_ext_cap_ptr,(const uint32_t)0x00000030);
        dec_to_bin((uint32_t)xbaddr5);
        print("\n\n");
}
