/* This is the kernel's keyboard driver for ps/2 keyboards. */

#include "../types.h"
#include "../isr.h"
#include "../irq.h"
#include "../system.h"
#include "../timer.h"
#include "../fs/fat16.h"
#include "../string.h"
#include "../graphics/screen.h"
#include "../mem.h"
#include "kb.h"
#include "usb/usb_ehci.h"
#include "usb/usb_uhci.h"
#include "usb/usb_ohci.h"
#include "pci.h"
#include "../power/check_shutdown_acpi.h"
#include "../power/using_acpi.h"
#include "usb/usb_mass_storage.h"
#include "usb/usb.h"


uint32_t table[69] = {0, 27, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61, 8, 9, 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93, 13, 0, 97, 115, 100, 102, 103, 104, 106, 107, 108, 59, 
                      39, 96, 0, 92, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, 0, 42, 0, 32, 0, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68}; 
uint32_t scan_to_value[12] = {0,0,1,2,3,4,5,6,7,8,9,0};
uint32_t scan_to_value2[49] = {0,0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,10,0,13,15,0,0,0,0,0,0,0,0,0,0,0,0,12,0,11};
uint32_t table3[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/* translates keyboard scancode to ascii */
void keyboard_install()
{
        /* Installs 'keyboard_handler' to IRQ1 */
        irq_install_handler(1, keyboard_handler);
}

void set_kbvariables_zero()
{
        buffstr = (int8_t*)kmem_4k_allocate();
        zero_usbms_mem_7(buffstr);
        buffstr2 = (int8_t*)kmem_4k_allocate();
        zero_usbms_mem_7(buffstr2);
        input = 0x00000000;
        kb_i = 0; 
        kb_temp_dir = (int8_t*)kmem_4k_allocate();
        zero_usbms_mem_7(kb_temp_dir);

        kb_temp1 = 0;
        kb_temp2 = 0;
}

void keyboard_handler(struct regs* r)
{
        check_root = 0;
        uint32_t fs_test_counter2 = 0;
        uint32_t fs_test_counter3 = 0;
        fs_string_match = 0;
        kb_temp_dir = (int8_t*)kmem_4k_allocate();
        zero_usbms_mem_7(kb_temp_dir);

        kb_temp1 = 0;
        kb_temp2 = 0;
        reading = 1;
        /* Stores value of keyboard input (for example 28) in the
         * memoryaddress (probably in the stack) represented by 
         * input, or copy the input value and store it in input
         */
        input = inportb(0x60);
 
        switch (input) {
                /* if user presses enter, the keyboard input reading is finished */
                case 28:
                        *(buffstr + kb_i) = '\0'; 
                        *(buffstr2 + kb_i) = '\0'; 
                        reading = 0;
                break;
                /* backspace */
                case 14: 
                        printch('\b', kb_i);

                        if (kb_i > 0) 
                                kb_i--;

                        *(buffstr + kb_i) = 0;
                        *(buffstr2 + kb_i) = 0;
                        break;
                case 0x48: 
                        scrollDown();
                        input = 0x00000000;
                        break;
                case 0x50:
                        scrollUp(1);
                        input = 0x00000000;
                        break;
                default:

                if (input < 69 && kb_i < 127) {
                        printch((int8_t)table[input], 0);
                        *(buffstr + kb_i) = (int8_t)table[input];
                        *(buffstr2 + kb_i) = (int8_t)table[input];
                        kb_i++;
                }

		        break;
        }

        if (reading == 0) {
                fs_a = 0;
                fs_string_match = 0;
                kb_temp_dir = (int8_t*)kmem_4k_allocate();
                zero_usbms_mem_7(kb_temp_dir);
                kb_temp1 = 0;
                kb_temp2 = 0;
                fs_c = 0;
                uint16_t kb_str_length;

                fs_b = 0;
                kb_temp1 = 0;
                kb_temp2 = 0;
                uint16_t fs_p;
                uint32_t fs_q;
                int8_t* ch = buffstr;
                int8_t* ch2 = buffstr2;

                /* go to where all the files are for that directory */		    
                while(!strEql(fs_current_directory_ptr, (fs_filenames + (fs_test_counter2*10))) && (fs_test_counter2 < 455))
                        fs_test_counter2++;

                /* at the right dir now */
                fs_test_counter3 = fs_test_counter2;
	            fs_p = strlength(ch2);
                fs_q = 0;

                for(; fs_q < (uint32_t)fs_p; fs_q++) {
                        if(((*(ch2 + fs_q)) > 0x60) && ((*(ch2 + fs_q)) < 0x7b))
                                (*(ch2 + fs_q)) -= 0x20;
                }

                (*(ch2 + fs_q)) = '\0';

                while ((!strEql((fs_filenames + (fs_test_counter2*10)), "eod")) && (fs_test_counter2 < 409) && (check_root == 0) && (!strEql((fs_filenames + (fs_test_counter2*10)), ch2))) {
                        fs_test_counter2++;

                        if(strEql((fs_filenames + (fs_test_counter2*10)), ch2))
                                check_root = 1;
                }

                if (check_root == 1) {
                        if((*(fs_filenames + (fs_test_counter2*10) + 8)) != 't')
                                print("\nError: could not open file. Not a text file.");
                        else {
                                (*(fs_filenames + (fs_test_counter2*10) + 8)) = '\0';
                                uint32_t test1 = (*(fs_filelocation_cluster_ptr + fs_test_counter2));
                                uint32_t test2 = (*(fs_filelength + fs_test_counter2));
                                read_fat16_file(test1, test2);
                                (*(fs_filenames + (fs_test_counter2*10) + 8)) = 't';
                                kb_i = 0;
                        }

                        print("\n\n");
                        print(csptr);
                }

                if(check_root == 0) {	
                        if (strEql(ch, "help")) {
                                print("\n\nList of available commands:\n");
                                print("clear : Clears the screen.\nred : Changes textcolor.\n");
                                print("Supported textcolors: green, blue, red \n");
                                print("yellow, violet, magenta, light grey (type grey)\n");
                                print("light green and white\n");
                                print("\nshow memmap: Shows the memory map.\nType show memmap -e for a memory map without explaination.\n");
                                print("list pci: Lists PCI Compatible Devices where \"bus\" means a PCI-bus.\n");
                                print("ls: Looks at the available directories and files. \ncd: Changes directory. \"cd r\" or \"cd root\": Goes back to root directory.\n");
                                print("cd b: Goes back to the former directory.\n");
                                print("shutdown: Attempts to shut down the computer.\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "clear")) {
                                clearScreen();
                                print(csptr);
                        }
                        else if (strEql(ch, "shutdown")) {
                                check_shutdown_code();
                        }
                        /* sc stands for screencolor and is defined in screen.c */
                        else if (strEql(ch, "green")) {
                                sc = 0x02; 
                                print("\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "magenta")) {
                                sc = 0x05; 
                                print("\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "light green")) {
                                sc = 0xA; 
                                print("\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "grey")) {
                                sc = 0x07;
                                print("\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "blue")) {
                                sc = 0x01;
                                print("\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "red")) {
                                sc = 0x04;
                                print("\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "violet")) {
                                sc = 0x05;
                                print("\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "yellow")) {
                                sc = 0xE;
                                print("\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "white")) {
                                sc = 0xF;
                                print("\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "show memmap")) {
                                show_memmap(1);
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "show memmap -e")) {
                                show_memmap(0);
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "list pci")) {
                                pci_bscan_devices();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "stop uhci")) {
                                uhci_hcreset();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "stop ohci")) {
                                timer_install();
                                stop_ohci();
                                irq_uninstall_handler(0);
                                asm("sti");
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci port1")) {
                                read_ehci_port1();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "reset ehci port1")) {
                                uint32_t* kbport = port_reset();
                                print_hex(*kbport);
                                print_hex(*(kbport + 1));
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci port2")) {
                                read_ehci_port2();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci port3")) {
                                read_ehci_port3();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci port4")) {
                                read_ehci_port4();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci port5")) {
                                read_ehci_port5();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci port6")) {
                                read_ehci_port6();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci port7")) {
                                read_ehci_port7();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci port8")) {
                                read_ehci_port8();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "stop ehci")) {
                                stop_ehci();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "start ehci")) {
                                start_ehci();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "reset ehci")) {
                                reset_ehci();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci command register")) { 
                                read_ehci_usbcmd();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ehci status register")) {
                                print_ehci_usbsts();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read fadt")) {
                                print_fadt_struct_info();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read madt")) {
                                print_madt_struct_info();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read dsdt")) {
                                read_dsdt();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read ssdt")) {
                                read_ssdt();
                                print("\n\n");
                                print(csptr);
                        }
                        else if (strEql(ch, "read signatures")) {
                                read_h_f_rsdt();
                                print("\n\n");
                                print(csptr);
                        } 

                        else if (strEql(ch, "shutdown")) {
                                shutdown();
                        }
                        else if (strEql(ch, "ls")) {
                                /* look for the current directory ptr */
                                while((!strEql((fs_filenames_dirinfo + fs_c), fs_current_directory_ptr)) && (fs_c < 456)) {
                                        /* goes through the first dir */
                                        while((*(fs_filenames_dirinfo + fs_c)) && (fs_c < 456))
                                                fs_c++;
                                        /* goes through the zeroes */
                                        while((!(*(fs_filenames_dirinfo + fs_c))) && (fs_c < 456))
                                                fs_c++;
                                        /* now dirinfo contains a letter */
                                }

                                /* now fs_dir_info point32_ts to the same as current ptr.
                                 * now let's look for a % 
                                 */
                                if(fs_c > 455) {
                                        print("\nInvalid command.");
                                        kb_i = 0;
                                        return;
                                }

                                /* goes through the first dir (root) */
                                while((*(fs_filenames_dirinfo + fs_c)) && (fs_c < 456))
                                        fs_c++;

                                /* after this it reaches a space between boot and the next thing */
                                if(fs_c > 455) {
                                        print("\nInvalid command.");
                                        kb_i = 0;
                                        return;
                                }

                                while((!(*(fs_filenames_dirinfo + fs_c))) && (fs_c < 456)) { 
                                        /* goes through the zeroes */
                                        fs_c++; 
                                        /* after this it reaches a letter. for example e for eod. */
                                }

                                if(fs_c > 455) {
                                        print("\nInvalid command.");
                                        kb_i = 0;
                                        return;
                                }

                                if ((!strEql((fs_filenames_dirinfo + fs_c), "%"))) {
                                        while((!strEql((fs_filenames_dirinfo + fs_c), fs_current_directory_ptr)) && (fs_c < 456)) {
                                                while((*(fs_filenames_dirinfo + fs_c)) && (fs_c < 456))
                                                        fs_c++;
                                                while((!(*(fs_filenames_dirinfo + fs_c))) && (fs_c < 456))
                                                        fs_c++;
                                        }

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        while((*(fs_filenames_dirinfo + fs_c)) && (fs_c < 456))
                                                fs_c++; 

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        while((!(*(fs_filenames_dirinfo + fs_c))) && (fs_c < 456))
                                                fs_c++; 

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        if (!strEql((fs_filenames_dirinfo + fs_c), "%")) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        fs_c +=9;

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        fs_c /=9;
                                        print("\n\nDIRECTORIES:\n");

                                        while((!(strEql((fs_filenames_dirinfo  + (fs_c*9)), (int8_t*)"eod"))) && (fs_c < 456)) {
                                                print("\n");
                                                print((fs_filenames_dirinfo  + (fs_c*9)));
                                                fs_c++;
                                        }
                                }
                                else if (fs_c == 9) {
                                        fs_c += 9;
                                        fs_c /= 9;					
                                        print("\n\nDIRECTORIES:\n");

                                        while((!(strEql((fs_filenames_dirinfo  + (fs_c*9)), (int8_t*)"eod"))) && (fs_c < 456)) {
                                                print("\n");
                                                print((fs_filenames_dirinfo  + (fs_c*9)));
                                                fs_c++;
                                        }
                                }
                                else {
                                        print("\nKb: Unexptected value of the return value of StrEql or fs_c. Halting.");
                                        asm("hlt");
                                }

                                print("\n\nFILES:\n");
                                fs_test_counter3++;

                                while((!(strEql((fs_filenames + (fs_test_counter3*10)), (int8_t*)"eod"))) && (fs_test_counter3 < 456)) {
                                        print("\n");
                                        print((fs_filenames + (fs_test_counter3*10)));

                                        if((*(fs_filenames + (fs_test_counter3*10) + 8)) == 't') {
                                                print(".TXT");
                                        }

                                        fs_test_counter3++;
                                }

                                print("\n\n");
                                print(csptr);
                        } 
                        else if (strEql(ch, "writetest")) {
                                test_read_write();
                        }
                        else if (strEql(ch, "cd b")) {
                                kb_str_length = strlength(fs_current_directory_ptr);

                                if (strEql(fs_current_directory_ptr, "ROOT")) {
                                        print("\n");
                                        print(csptr);
                                        kb_i = 0;
                                        return;
                                }

                                /* look for the current directory ptr */
                                while((!strEql((fs_filenames_dirinfo + fs_c), fs_current_directory_ptr)) && (fs_c < 456)) {
                                        /* goes through the first dir */
                                        while((*(fs_filenames_dirinfo + fs_c)) && (fs_c < 456))
                                                fs_c++;
                                        /* goes through the zeroes */ 
                                        while((!(*(fs_filenames_dirinfo + fs_c))) && (fs_c < 456))
                                                fs_c++;
                                }

                                /* go to current directory name and check for a % */
                                if(fs_c > 455) {
                                        print("\nInvalid command.");
                                        kb_i = 0;
                                        return;
                                }

                                while((*(fs_filenames_dirinfo + fs_c)) && (fs_c < 456)) 
                                        fs_c++; 

                                if(fs_c > 455) {
                                        print("\nInvalid command.");
                                        kb_i = 0;
                                        return;
                                }

                                while((!(*(fs_filenames_dirinfo + fs_c))) && (fs_c < 456)) 
                                        fs_c++; 

                                if(fs_c > 455) {
                                        print("\nInvalid command.");
                                        kb_i = 0;
                                        return;
                                }

                                if ((!strEql((fs_filenames_dirinfo + fs_c), "%"))) {
                                        while((!strEql((fs_filenames_dirinfo + fs_c), fs_current_directory_ptr)) && (fs_c < 456)) {
                                                while((*(fs_filenames_dirinfo + fs_c)) && (fs_c < 456))
                                                        fs_c++;

                                                while((!(*(fs_filenames_dirinfo + fs_c))) && (fs_c < 456))
                                                        fs_c++;
                                        }

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        /* goes through the actual current directory dir */
                                        while((*(fs_filenames_dirinfo + fs_c)) && (fs_c < 456))
                                                fs_c++;

                                        /* after this it reaches a space between cd and the next thing */
                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        while((!(*(fs_filenames_dirinfo + fs_c))) && (fs_c < 456))
                                                fs_c++;

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        /* goes to the current dir and the % after */
                                        if (!strEql((fs_filenames_dirinfo + fs_c), "%")) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        /* now its time to go back to the current dir name but without a % mark */
                                        fs_c -= 18; 
                                        /* now we are at the current dir name */
                                        while((!strEql((fs_filenames_dirinfo + fs_c), fs_current_directory_ptr)) && (fs_c >= 1)) {
                                                fs_c -= 9;

                                        }
                                        if(fs_c == 0) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }


                                        /* we are now at the current directory but with no % after */
                                        while((!strEql((fs_filenames_dirinfo + fs_c), "%")) && (fs_c >= 1)) {
                                                fs_c -= 9;
                                        }

                                        if(fs_c == 0) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        /* now we are at the % just go back one step now. */
                                        fs_c -= 9;
                                        fs_b = 0;

                                        while((*(fs_filenames_dirinfo + fs_c)) && (fs_b < 8)) {
                                                (*(fs_current_directory_ptr + fs_b)) = (*(fs_filenames_dirinfo + fs_c));
                                                fs_b++;
                                                fs_c++;
                                        }

                                        for(;fs_b < 9; fs_b++)
                                                (*(fs_current_directory_ptr + fs_b)) = (int8_t)'\0';

                                        fs_b = 0;
                                        for(;(*(csptr + fs_b)) && (fs_b < 128);fs_b++);
                                        fs_b -=2;

                                        /* at the $ */
                                        if (fs_b > kb_str_length)
                                                fs_b -= (uint32_t)kb_str_length;

                                        else {
                                                kb_i = 0;
                                                print("Erroneous string operation");
                                                return;
                                        }

                                        if(fs_b > 0)
                                                fs_b-=1;
                                        /* at the / in front of t */

                                        if(fs_b < 125) {
                                                (*(csptr + fs_b)) = '$';
                                                (*(csptr + fs_b + 1)) = ' ';
                                                (*(csptr + fs_b + 2)) = '\0';
                                        }

                                        print("\n\n");
                                        print(csptr);
                                }
                        }
                        else if (strEql(ch, "cd r") ||  strEql(ch, "cd root")) {
                                (*(fs_current_directory_ptr)) = 'R';
                                (*(fs_current_directory_ptr + 1)) = 'O';
                                (*(fs_current_directory_ptr + 2)) = 'O';
                                (*(fs_current_directory_ptr + 3)) = 'T';
                                (*(fs_current_directory_ptr + 4)) = '\0';
                                (*(fs_current_directory_ptr + 5)) = '\0';
                                (*(fs_current_directory_ptr + 6)) = '\0';
                                (*(fs_current_directory_ptr + 7)) = '\0';
                                (*(fs_current_directory_ptr + 8)) = '\0';
                                csptr[0] = '~';
                                csptr[1] = 'R';
                                csptr[2] = 'o';
                                csptr[3] = 'o';
                                csptr[4] = 't';

                                csptr[5] = '$'; 
                                csptr[6] = ' ';
                                csptr[7] = '\0';
                                print("\n\n");
                                print(csptr);
                        }

                        /* This is for if you wrote "cd docsb" or "cd boot" or something. 
                         * Now we take docsa as an example.
                         * This block of code will go to the current directory and look
                         * for a "%" which indicates that the following directories are
                         * child directories. 
                         * From there the block of code will determine if you can jump
                         * to that next directory and set that directory as the current
                         * directory.           
                         * This supports only fat16 and directories must have been
                         * scanned and put to RAM using a driver. see fat16.c for the
                         * information about scanning of directories.   
                         */
                        else if (strEql2(ch, "cd ")) {
                                if(stringlength(ch) == 3) {
                                        print("\nInvalid command.");
                                        kb_i = 0;
                                        return;
                                }

                                fs_c = 0;
                                fs_b = 0;
                                fs_a = 0;

                                /* look for the current directory ptr */
                                while((!strEql((fs_filenames_dirinfo + (fs_c*9)), fs_current_directory_ptr)) && (fs_c < 456)) {
                                        fs_c++;
				                }

                                /* now fs_dir_info point32_ts to the same as current ptr.
                                 * now let's look for a % 
                                 */
                                if(fs_c > 455) {
                                        print("\nInvalid command.");
                                        kb_i = 0;
                                        return;
                                }

                                fs_c++; 

                                if(fs_c > 455) {
                                        print("\nInvalid command.");
                                        kb_i = 0;
                                        return;
                                }

                                /* if looking for current directory boot for example then the first find of boot should not have % */
                                if ((!strEql((fs_filenames_dirinfo + (fs_c*9)), "%"))) { 
                                        /* now its time to look for the current dir again. and a % in front of it */
                                        while((!strEql((fs_filenames_dirinfo + (fs_c*9)), fs_current_directory_ptr)) && (fs_c < 456)) {

                                                fs_c++;
                                        }

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        fs_c++;

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        /* now look for a "%" */
                                        if (!strEql((fs_filenames_dirinfo + (fs_c*9)), "%")) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        fs_c++;

                                        /* now we should be at the next dir after the % which was after the current directory */
                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        zero_usbms_mem_7(kb_temp_dir);
                                        kb_temp1 = 0;

                                        while((!strEql((fs_filenames_dirinfo + (fs_c*9)), "eod")) && (fs_c < 456)) {
                                                kb_temp2 = 0;
                                                fs_b = 0;

                                                for(;kb_temp2 < 8; kb_temp2++) {
                                                        (*(kb_temp_dir + (kb_temp1*9) + kb_temp2)) = (*(fs_filenames_dirinfo + (fs_c*9) + fs_b));
                                                        fs_b++;
                                                }

                                                fs_c+=1;
                                                kb_temp1++;
                                        }

                                        (*(kb_temp_dir + (kb_temp1*9) + kb_temp2)) = '\0'; 

                                        /* now we have filled the temp string array with a copy of the directories after the current dir. an eod should be put there */
                                        if((kb_temp1*9) < 4090) {
                                                (*(kb_temp_dir + (kb_temp1*9))) = 'e';
                                                (*(kb_temp_dir + (kb_temp1*9) + 1)) = 'o';
                                                (*(kb_temp_dir + (kb_temp1*9) + 2)) = 'd';
                                                (*(kb_temp_dir + (kb_temp1*9) + 3)) = '\0';
                                        }

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        fs_b = 0;
                                        while((*(ch + 3 + fs_b))) {
                                                /* a to z */
                                                if(((*(ch + 3 + fs_b)) > 0x60) && ((*(ch + 3 + fs_b)) < 0x7b)) {
                                                        (*(ch + 3 + fs_b)) = (*(ch + 3 + fs_b)) - 0x20;
                                                }
                                                fs_b++;
                                        }

                                        (*(ch + 3 + fs_b)) = '\0';
                                        fs_b = 0;

                                        while(!strEql((ch + 3), (kb_temp_dir + fs_b)) && (!strEql((kb_temp_dir + fs_b), "eod")))
                                                fs_b += 9;

                                        if(strEql((ch + 3), (kb_temp_dir + fs_b))) {
                                                fs_a = 0;
                                                fs_string_match = 1;

                                                while((*(kb_temp_dir + fs_b + fs_a)) && (fs_a < 8)) {
                                                        (*(fs_current_directory_ptr + fs_a)) = (*(kb_temp_dir + fs_b + fs_a));
                                                        fs_a++;
                                                }

                                                (*(fs_current_directory_ptr + fs_a)) = '\0';

                                        }

                                        if(fs_string_match == 0) {
                                                fs_c = 0;
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        fs_b = 0;

                                        for(;(*(csptr + fs_b)) && (fs_b < 125);fs_b++);

                                        fs_b -=2;
                                        (*(csptr + fs_b)) = '/';
                                        fs_b++;
                                        fs_a = 0;

                                        for(;((*(fs_current_directory_ptr + fs_a)) && (fs_a < 9) && (fs_b < 128)); fs_a++) {
                                                if(((*(fs_current_directory_ptr + fs_a)) > 0x40) && ((*(fs_current_directory_ptr + fs_a)) < 0x5b) && (fs_a != 0))
                                                        (*(csptr + fs_b)) = (*(fs_current_directory_ptr + fs_a)) + 0x20;
                                                else
                                                        (*(csptr + fs_b)) = (*(fs_current_directory_ptr + fs_a));
                                                fs_b++;
                                        }

                                        if(fs_b < 125) {
                                                (*(csptr + fs_b)) = '$';
                                                (*(csptr + fs_b + 1)) = ' ';
                                                (*(csptr + fs_b + 2)) = '\0';
                                        }

                                }
                                /* if we are at root */
                                else if (fs_c == 1) {
                                        fs_b = 0;
                                        fs_c += 1; 
                                        /* now we arrive at the first dir after root. if any. should be boot here. */
                                        if ((fs_c > 455) || (strEql((fs_filenames_dirinfo + (fs_c*9)), "eod"))) {
                                                /* but if eod instead of a dir: */
                                                print("\nInvalid command");
                                                kb_i = 0;
                                                return;
                                        }

                                        /* will fill kb_temp_dir with coorect dirs until eod */
                                        zero_usbms_mem_7(kb_temp_dir);
                                        kb_temp1 = 0;

                                        while((!strEql((fs_filenames_dirinfo + (fs_c*9)), "eod")) && (fs_c < 456)) {
                                                kb_temp2 = 0;
                                                fs_b = 0;	
								                
                                                for(;kb_temp2 < 8; kb_temp2++) {
                                                        (*(kb_temp_dir + (kb_temp1*9) + kb_temp2)) = (*(fs_filenames_dirinfo + (fs_c*9) + fs_b)); 
                                                        fs_b++;
                                                }

                                                fs_c+=1;
                                                kb_temp1++;
                                        }

                                        if((kb_temp1*9) < 4090) {
                                                (*(kb_temp_dir + (kb_temp1*9))) = 'e';
                                                (*(kb_temp_dir + (kb_temp1*9) + 1)) = 'o';
                                                (*(kb_temp_dir + (kb_temp1*9) + 2)) = 'd';
                                                (*(kb_temp_dir + (kb_temp1*9) + 3)) = '\0';
                                        }

                                        if(fs_c > 455) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }
                                        
                                        if (!(*(ch + 3))) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        fs_b = 0;

                                        /* a to z */
                                        while((*(ch + 3 + fs_b)) && (fs_b < 456)) {

                                                if(((*(ch + 3 + fs_b)) > 0x60) && ((*(ch + 3 + fs_b)) < 0x7b))
                                                        (*(ch + 3 + fs_b)) = (*(ch + 3 + fs_b)) - 0x20; 
                                                fs_b++;
                                        } 

                                        /* small letters become big letters to fit info fat16 */
                                        (*(ch + 3 + fs_b)) = '\0';
                                        fs_b = 0;

                                        while(!strEql((ch + 3), (kb_temp_dir + fs_b)) && (!strEql((kb_temp_dir + fs_b), "eod")) && (fs_b < 456)) 
                                                fs_b += 9;

                                        /* if the strings (and hopefully name of dirs) match */
                                        if(strEql((ch + 3), (kb_temp_dir + fs_b))) {
                                                fs_a = 0;
                                                fs_string_match = 1;

                                                while((*(kb_temp_dir + fs_b + fs_a)) && (fs_a < 8)) {
                                                        (*(fs_current_directory_ptr + fs_a)) = (*(kb_temp_dir + fs_b + fs_a)); 
                                                        fs_a++;
                                                }

                                                (*(fs_current_directory_ptr + fs_a)) = '\0';
                                        }

                                        if(fs_string_match == 0) {
                                                print("\nInvalid command.");
                                                kb_i = 0;
                                                return;
                                        }

                                        fs_b = 0;

                                        /* go to the end of the csptr */
                                        for(;(*(csptr + fs_b)) && (fs_b < 128);fs_b++); 

                                        fs_b -=2; 
                                        /* we are now at the $ 
                                         * below we put a "/" instead of a "$" 
                                         */
                                        (*(csptr + fs_b)) = '/'; 
                                        fs_b++;
                                        fs_a = 0;

                                        for(;((*(fs_current_directory_ptr + fs_a)) && (fs_a < 9) && (fs_b < 128)); fs_a++) {
                                                /* now fill the csptr with the new dir */
                                                if(((*(fs_current_directory_ptr + fs_a)) > 0x40) && ((*(fs_current_directory_ptr + fs_a)) < 0x5b) && (fs_a != 0)) {
                                                        /* fill it with small letters. */
                                                        (*(csptr + fs_b)) = (*(fs_current_directory_ptr + fs_a)) + 0x20;
                                                }
                                                else {
                                                        (*(csptr + fs_b)) = (*(fs_current_directory_ptr + fs_a)); 
                                                        /* except the first letter and if all other signs are shown */
                                                }

                                                fs_b++;
                                        }

                                        if (fs_b < 125) {
                                                (*(csptr + fs_b)) = '$';
                                                (*(csptr + fs_b + 1)) = ' ';
                                                (*(csptr + fs_b + 2)) = '\0';
                                        }
                                }
                                else {
                                        print("\nKb: Unexptected value of the return value of StrEql or fs_c. Halting.");
                                        asm("hlt");
                                }

                        print("\n\n");
                        print(csptr);
                        } 
                        else {
                                print("Invalid command.");
                        }

                        kb_i = 0;
                }
        }
}  

/* Attempts to read from the USB mass storage device
 * with a specified address from keyboard input.
 */
void keyboard_handler2(struct regs* r)
{
        uint32_t reading2 = 1;
        input2 = inportb(0x60);
        switch (input2) {
                case 28:
                        /* Commented code: print("\ni: ");
                         * printi((uint32_t)i); 
                         */
                        /* if user presses enter, the keyboard input reading is finished. */
                        *(buffstr2 + kb_i) = '\0';
                        // Commented code: buffstr2[i+1] = '\0';
                        buffstr3[kb_i] = '\0';
                        // Commented code: buffstr3[i+1] = '\0';
                        reading2 = 0;
                        break;
               case 14: // backspace
                        printch('\b', kb_i);
                        if (kb_i > 0) 
                                kb_i--;
                        *(buffstr2 + kb_i) = 0;
                        buffstr3[kb_i] = 0;
                        break;
              default:
                      if (((input2 > 1) && (input2 < 12))  && (kb_i < 5)) {
                              printch((int8_t)table[input2], 0);
                              *(buffstr2 + kb_i) = (int8_t)table[input2];
                              buffstr3[kb_i] = scan_to_value[input2];
                              /* Commented code: print("\nbuffstr3[i]: "); 
                               * printi(buffstr3[i]);
                               */
                              kb_i++;
                      }
                      break;
        }
        if (reading2 == 0) {
                int8_t* ch2 = buffstr3;
                uint8_t i2 = kb_i - 1;
                mem_addr_result = read_mem_address((uint32_t*)ch2,i2);

                if (mem_addr_result < 43008) {
                        usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_LBA;
                        usbms_extra_data[1] = mem_addr_result;
                        volatile uint8_t* rma = prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_READ10, USB_DEVICE_TO_HOST, 0, 512, usbms_extra_data);

                        if (rma == (volatile uint8_t*)0x600000)
                                goto exit;

                        print("\n");

                        for (uint32_t a = 0; a < 512;a++) {
                                print_hex_byte((uint8_t)(*(rma + a)));
                                print(" ");
                                if((a > 0) && (a % 25 == 0))
                                        print("\n");
                        }
                }
                else {
                        print("\nInvalid logical block address");
                }
                exit:
                irq_install_handler(1, keyboard_handler);
                print("\n\n");
                print(csptr);
                kb_i = 0;
        }
}

/* Attempts to read a memory address specified by
 * keyboard input. 
 */
void keyboard_handler3(struct regs* r)
{
        uint32_t reading3 = 1;
        input3 = inportb(0x60);
        switch (input3) {
                case 28:
                        buffstr4[kb_i] = '\0'; 
                        buffstr5[kb_i] = '\0';
                        reading3 = 0;
                        break;
               case 14: 
                        printch('\b', kb_i);
                        if (kb_i > 0) 
                                kb_i--;
                        buffstr4[kb_i] = 0;
                        buffstr5[kb_i] = 0;
                        break;
              default:
                      if ((((input3 > 1) && (input3 < 12)) || (input3 == 18) || (input3 == 30) || ((input3 > 31) && (input3 < 34)) || (input3 == 46) || (input3 == 48)) && (kb_i < 8)) {
                              printch((int8_t)table[input3], 0);
                              buffstr4[kb_i] = (int8_t)table[input3];
                              buffstr5[kb_i] = scan_to_value[input3];
                              kb_i++;
                      }

                      break;
        }

        if ((reading3 == 0) && (kb_i == 8)) {
                int8_t* ch2 = buffstr5;
                mem_addr_offset = hex_to_dec((uint32_t*)ch2);
                result = read_dword((const uint32_t)PCI_Config_RW2(0,6,0,16), (const uint32_t)mem_addr_offset);
                result_addr = read_dword_addr((const uint32_t)PCI_Config_RW2(0,6,0,16), (const uint32_t)mem_addr_offset);
                print("\nMemory address: ");
                printi((uint32_t)result_addr);
                print("\nAddress in hex: ");
                print_hex((uint32_t)result_addr);
                print("\nValue: ");
                printi((uint32_t)result);
                print("\nValue in hex: ");
                print_hex((uint32_t)result);
                irq_install_handler(1, keyboard_handler);
                print("\n\n");
                print(csptr);
                kb_i = 0;
        }
}
