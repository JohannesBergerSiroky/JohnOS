#include "types.h"
#include "multiboot.h"
#include "system.h"
#include "graphics/screen.h"
#include "gdt.h"
#include "isr.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "mem.h"
#include "kernel_heap_alloc_nopaging.h"
#include "test_kernel_heap.h"

#include "device/device.h"
#include "device/pci.h"
#include "device/kb.h"
#include "device/usb/usb.h"
#include "device/usb/usb_ehci.h"
#include "device/usb/usb_mass_storage.h"
#include "fs/fat16.h"
#include "power/using_acpi.h"
#include "power/check_shutdown_acpi.h"
#include "power/check_enable_acpi.h"



/* The multiboot structure gives the kernel's main function
 *some low level information. 
 */

int main(multiboot_info_t *mbr, unsigned int magic)
{
        //Commented code: uint8_t temp2; 
        /* Some code for setting the cursor shape and setting the screen buffer */
        /* Register value 10 is for setting the cursor shape start position */
        /*
        Commented code: outportb(0x3d4, 10); 
        temp2 = inportb(0x3d5); 
        temp2 &= 0xf0;
        temp2 |= 6;
        outportb(0x3d5, temp2);
        */
        /* Register value 11 is the register for cursor shape end position */
        /* Commented code: outportb(0x3d4, 11);
         * temp2 = inportb(0x3d5); 	
         * temp2 &= 0xf0;
         * temp2 |= 8;					
         * outportb(0x3d5, temp2);
         */

       /* Now let's initialize the screen buffers for a 80*25
        * screen resolution and clear the screen. 
        */

        init_screen_driver();
        init_screenbuffers();
        clearScreen();

        /* Set up the GDT, ISR, IDT and the IRQ, ps/2 keyboard, 
         * and start the timer among other things 
         */

        gdt_install();
        idt_install();
        isr_install();

        irq_install();
        set_kbvariables_zero();
        __asm__ __volatile__ ("sti");
        keyboard_install();
        timer_install();

        timer_phase(18);
        set_timer_variable(); 	
        irq_uninstall_handler(0); 
        asm("cli");

        /* Some code for initializing the memory handling
         * functions. The multiboot structure gives us some
         * memory information. 
         */

        init_memoryhandler();

        print("Lower memory size: ");
        printi(mbr->mem_lower);
        print("KB\nUpper memory size: ");


        printi(mbr->mem_upper); 
        max_mem_address = mbr->mem_upper;
        

        print("KB");
        print("\nMemory map address: "); 
        print_hex(mbr->mmap_addr);

        memorymap_addr = mbr->mmap_addr;

        /* USB and FAT16 code. USB 2 is supported in this one.
         * This can be changed so that the kernel looks for USB 1.0 and 1.1 
         * and also USB 3. All USB code is for USB mass storage only.
         * Those are the USB pen drives with the SCSI protocol, which
         * most pen drives support, I guess. 
         */

        asm("sti");
        timer_install();
        asm("cli");
        hci_in_use = 0;
        init_temporary_device_as_usbms();
        uint32_t bpbsector_location = 0;
        /* this assumes that the computer has an Enhanced Host Controller for USB */
        find_device(0x0c,0x03,0x20); 
        irq_uninstall_handler(0); 

        /* find the EHCI interrupt line */

        volatile int device_irq = (volatile int)read_device_intline(found_ehci_bus,found_ehci_device,found_ehci_function); 
        if (device_irq <= 15) {
                /* install ehci interrupt */
                irq_install_handler(device_irq, ehci_usb_handler); 
                /* EHCI */
                hci_in_use = HCI_EHCI;
        }
        /* This OS only supports the old interrupt controller, that is the PIC */
        else {
                print("\nError: Could not install usb ehci irq. Os supports the old pic, not apic."); 
        }

        /* Preparing shutdown code using ACPI  */
        find_rsdp();
        set_acpi_structs();
        read_rsdp();
        test_read_rsdt();
        print_fadt_struct_info();
        init_acpi_variables();
        enable_acpi();
        check_shutdown_code();
        print("\n");
        print("\n");

        // Commented code: init_memallocarrays_nopaging();

        // Commented code: test_kernel_heap_func();

        /* OK. Now, test if this USB Mass storage code works and then initialize the fat 16 */

        asm("sti");
        init_fs_variables();
        set_fs_clusters();
        //get_bpb_struct();

        init_usb_setup();
        init_scsi_structs();
        init_queue_heads(); 
        asm("cli"); 
        init_ehci(QHhead19);
        usb_ehci_get_descriptor(USB_GET_DESCRIPTOR_DEVICE, USB_DEVICE_DESCRIPTOR_DEVICE_LENGTH);
        usb_ehci_get_descriptor(USB_GET_DESCRIPTOR_CONFIGURATION, USB_DEVICE_DESCRIPTOR_CONFIGURATION_LENGTH);
        

        uint8_t temp_seq = sequence_1(); 
        if (temp_seq == 1) {
                asm("sti"); 
                print("\nUSB ready for transfers");
                print("\nPlease wait..");
                /* This initializes the FAT16. It assumes that it uses the MBR. */
                bpbsector_location = get_bpb_sector();
                get_bpb_struct(bpbsector_location);
                scan_all_directories();                
        }
        else {
                print("\n\nError: Could not initialize the Usb Flash Memory");
                print("Usb mass storage device reset to be inplemented later (I hope).");
                asm("cli");
                asm("hlt");
        }

        /* Print welcome message */
        asm("sti");
        /* Commented code: init_memallocarrays_nopaging();
         * test_kernel_heap_func();
         */
        print("\nFinished initializing\n");
        print("\n\nWelcome to this operating system!\nPlease enter a command or press 'help' for a list of commands.\n");
        print(csptr);
        // Commented code: stop_ehci();
        // Commented code: reset_ehci();
        while (1);
return 0xDEADBABA;
}
        

