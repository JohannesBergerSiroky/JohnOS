/* This is the driver for UHCI for this kernel. */

#include "../../types.h"
#include "../pci.h"
#include "usb_uhci.h"
#include "../../system.h"
#include "../../mem.h"
#include "../../graphics/screen.h"
#include "../../timer.h"
#include "usb_hubs.h"
#include "usb.h"



/*reset procedure:
 *
 * from a post in osdev forum:
 *
 * First, I write 0x8F00 to the device's legacy register (PCI register C0). I then read the device's command register (PCI register 04), followed by ORing it with 5h and writing the resulting value back to the command register.
 *
 * So I have attempted to disable SMIs, enable I/O access to the device, and allow the device to be a bus master.
 *
 * Now, I begin by clearing the host controller status register (at base + 2) with a write of 0xFFFF. I then clear the least significant bit of the host controller command register to halt it.
 *
 * Once the host controller status register indicates that the host controller has halted (bit 5 set), I reset the host controller by setting bit 1 (and making sure that bit 0 remains cleared, just to be extra safe).
 *
 * The host controller is supposed to clear bit 1 once it has finished resetting, so I wait for the least two significant bits to be zero.
 *
 * Next, I set bit 2 for a global reset. After waiting 15 milliseconds (1.5 times the minimum amount/duration required), 
 * I clear the three least significant bits (again, ensuring that bit 0 remains cleared, as well as bit 1, just to be safe).
 *
 * Then, I set bits 4 and 3. I am uncertain about the necessity of this, but Intel's UHCI 1.1 document mentions clearing bit 3 when bit 4 is cleared (or afterwards). So, I have been setting them together, then clearing them together.
 *
 * Following a 30 millisecond delay (again, 1.5 times the minimum), I completely clear the host controller command register. Once I confirm that the register is 0x0000, I continue.
 *
 * Now, I clear the interrupt register (at base + 4) and the frame number register (at base + 6). I set the frame list base address via the register at base + 8.
 *
 * I have checked what BIOS sets the frame modify register (base + C) to. It turns out to be the default of 0x40 on this computer. 
 * So, it is at this point that I ensure that this register is set to that value (though it should be following the resets).
 *
 * To reset the ports, I write 0x020A to both. After confirming that bit 9 (the reset bit) has been set, I wait. Earlier, I was waiting for 10 milliseconds (actually, 1.5 times that, just to be safe). 
 * I have since increased the wait to 75 milliseconds (the USB 1.1 specification mentions something about 50 milliseconds, though I cannot remember to what exact cases that applies). Regardless, my delay should more than cover me.
 *
 * After the delay, I clear the reset bit by writing 0x000A. To be certain that the bit becomes cleared, I loop until it is. Then, I wait for another 75 milliseconds. (Uncertain about how important the timing of things is, I do not want * 
 * anything regrettable like insufficient waiting to interfere with my results.)
 *
 * Now, I write 0x000F to the port at the front of my desktop computer's case. (I have something plugged into it.) After testing that the enable bit has been set, I continue.
 *
 * After clearing the host controller status register again (0xFFFF, just to be safe), I set the host controller command register's least significant bit.
 *
 * A read of the host controller status register will now yield 0x0030, indicating that a process error has occurred and that the host controller has been halted as a result.
 *
 * At the moment, my frame list contains 0x00007D62 for all 1024 entries. At that address, this QH exists: 0x00007DA200007DA2. At that address, this QH exists: 0x00007D6200007D62. So they infinitely point to one another.
 *
 * Of course, on my floppy disk, all values have be written as little-endian. So each entry in the frame list actually looks like this on the disk: A2 7D 00 00. I am very used to writing values in little-endian. 
 */


/* from osdev:
 * 1. Set reset bit.
 * 2. Wait 58ms.
 * 3. Clear reset bit.
 * 4. Wait for connect status change.
 * 5. Set enable bit.
 * 6. Wait for enable bit to actually change.
 * 7. Wait 24ms.
 * 8. Proceed with enumeration.
 */

/* Sets some parameters in the command register for the UHC.
 * The parameters mean that SERR is enabled, and that it has access
 * to i/o and memory space and that it acts as a bus master. 
 */
void set_uhci_commandreg()
{
		uint16_t temp_cmd = PCI_Config_RW(found_uhci_bus,found_uhci_device,found_uhci_function,4);

		uint16_t temp_cmd2 = PCI_Config_RW(found_uhci_bus,found_uhci_device,found_uhci_function,6);

		uint32_t temp_cmd3 = (uint32_t)(temp_cmd2*65536);

		uint32_t temp_cmd4 = (uint32_t)temp_cmd;

		uint32_t temp_cmd5 = temp_cmd3 + temp_cmd4;

		temp_cmd5 &= (~(1 << 2));

		temp_cmd5 |= 0x107;


		uint32_t address = (uint32_t)((found_uhci_bus << 16) | (found_uhci_device << 11) | (found_uhci_function << 8) | (4 & 0xfc) | ((uint32_t)0x80000000));

		outportl(0xCF8, address);

		outportl(0xCFC, temp_cmd5);
}

/* Reads a word from a memory address. */
uint16_t read_uhci_word(const uint16_t base_addr, const uint16_t offset)
{
		volatile uint16_t word = *((uint16_t*)(base_addr + offset));
		return word;
}

/* Writes a word to a memory address. */
void write_uhci_word(const uint16_t base_addr, const uint16_t offset, const uint16_t value)
{
		*((uint16_t*)(base_addr + offset)) = value;
}

/* Reads a word when using ports. */
uint16_t read_word2(const uint16_t base_addr, const uint16_t offset)
{

		volatile uint16_t word = inportw((uint16_t)(base_addr + offset)); 
		return (uint16_t)word;
}

/* Reads and returns a dword when using ports. */
uint32_t read_dword3(const uint16_t base_addr, const uint16_t offset)
{
		volatile uint32_t word = inportl((uint16_t)(base_addr + offset)); 
		return (uint32_t)word;
}

/* Reads a port and returns a byte from that port. */
uint8_t read_byte2(const uint16_t base_addr, const uint16_t offset)
{
		volatile uint8_t pbyte = inportb((uint16_t)(base_addr + offset)); 
		return (uint8_t)pbyte;
}

/* Reads a byte from a memory address and returns that byte. */
uint8_t read_bbyte(const uint32_t base_addr, const uint32_t offset)
{
		volatile uint8_t bbyte = *((uint8_t*)(base_addr + offset)); 
		return (uint8_t)bbyte;
}

/* Writes a word to a port. */
void write_word2(const uint16_t base_addr, const uint16_t offset, const uint16_t value)
{
		outportw((uint16_t)(base_addr + offset), (uint16_t)value); // how to get the port?
}

/* Writes a dword to a port. */
void write_dword3(const uint16_t base_addr, const uint16_t offset, const uint32_t value)
{
		outportl((uint16_t)(base_addr + offset), (uint32_t)value); // how to get the port?
}

/* Writes a byte to a port. */
void write_byte(const uint16_t base_addr, const uint16_t offset, const uint8_t value)
{
		outportb((uint16_t)(base_addr + offset), (uint8_t)value); // how to get the port?
}

/* Returns an address as a word, based on the address' base and offset
 * using pointers. 
 */ 
uint16_t * read_word_addr(const uint16_t base, const uint16_t offset)
{
		uint16_t * word = ((uint16_t*) (base + offset));
		return word;
}

/* Returns an address as a word, based on the address' base and offset
 * without using pointers.
 */ 
uint16_t read_word_addr2(const uint16_t base, const uint16_t offset)
{
		uint16_t word = ((uint16_t) (base + offset));
		return word;
}

/* Writes a word to an address. */
void write_word(const uint32_t base, const uint32_t offset, const uint16_t value)
{
		*((uint16_t *) (base + offset)) = value;		
}

/* Returns a dword value from a modifies address value. */
uint32_t read_dword_modify(uint32_t base_addr, uint32_t modification_value, uint32_t offset)
{
		base_addr = (base_addr & modification_value);
		base_addr += offset;
		const uint32_t base_addr2 = base_addr;
		volatile uint32_t dword = *((uint32_t*)base_addr2);
		return dword;
}

/* Prints and address as a word. */
void print_uhc_info(uint16_t addr)
{
	    print("\n");
	    print("Value: ");
	    printword((uint16_t)addr);
	    print("\n\ndone.");
}

/* Prints some addresses and addresses of pointers 
 * in deciman and i hexadecimal notation. 
 * An address is also printed in binary.
 */
void print_uhc_info2(uint32_t addr, uint32_t* addr2)
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
	    print("\n\ndone.");
}

/* An address of a pointer is printed in decimal notation
 * and hexadecimal notation.
 */
void print_uhc_info3(uint32_t* addr)
{
	    print("\n");
	    print("Base address: ");
	    printi((uint32_t)&addr);
	    print("\n");
	    print("In hex: ");
	    print_hex((uint32_t)&addr); 
	    print("\n\ndone.");
}

/* And address is printed in decimal, 
 * hexadecimal and binary notation.
 */
void print_uhc_info4(uint32_t addr)
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
	    print("\n\ndone.");
}

/* Initializes some queue heads and 
 * some descriptors for the UHC. 
 * These are currently for test purposes and
 * they are hard coded to fixed memory addresses.
 */
void init_uhci_descriptors()
{

	    /* try the td_zero to point the interrupt qh first, if this doesnt work */
	    zero_usbms_mem_6((uint32_t*)0x1400000);
	    zero_usbms_mem_6((uint32_t*)0x1401000);
	    zero_usbms_mem_6((uint32_t*)0x1402000);
	    zero_usbms_mem_6((uint32_t*)0x1403000);
	    td_null = (struct uhci_transfer_descriptor*)0x1403000;
	    uhci_qh_zero = (struct uhci_queue_head*)0x1402000;
	    uhci_ctrl_1 = (struct uhci_queue_head*)0x1401000;

	    td_1 = (struct uhci_transfer_descriptor*)0x1401010; 
	    td_2 = (struct uhci_transfer_descriptor*)0x1401030;
	    td_3 = (struct uhci_transfer_descriptor*)0x1401050;
	    td_4 = (struct uhci_transfer_descriptor*)0x1401070;
	    td_5 = (struct uhci_transfer_descriptor*)0x1401090;
	    td_6 = (struct uhci_transfer_descriptor*)0x14010b0;
	    td_7 = (struct uhci_transfer_descriptor*)0x14010d0;
	    td_8 = (struct uhci_transfer_descriptor*)0x14010f0;

	    u_td_buffer1 = (volatile uint32_t)0x1401110;
	    u_td_buffer2 = (volatile uint32_t)0x1401150;
	    u_td_buffer3 = (volatile uint32_t)0x1401190;
	    u_td_buffer4 = (volatile uint32_t)0x14011d0;
	    u_td_buffer5 = (volatile uint32_t)0x1401110;
	    u_td_buffer6 = (volatile uint32_t)0x1401150;
	    u_td_buffer7 = (volatile uint32_t)0x1401190;
	    u_td_buffer8 = (volatile uint32_t)0x14011d0; 


	    td_null->link_pointer = 0x1402006;
	    td_null->td_ctrl_sts = 0;
	    td_null->td_token = 0;
	    td_null->u_td_buffer = u_td_buffer2;
	    td_null->sw_dword1 = 0;
	    td_null->sw_dword2 = 0;
	    td_null->sw_dword3 = 0;
	    td_null->sw_dword4 = 0;

	    uhci_ctrl_1->qh_head_next = 1; 
	    uhci_ctrl_1->qh_emnt_next = 1;
	    uhci_ctrl_1->zero_1 = 0;
	    uhci_ctrl_1->zero_2 = 0;

	    uhci_qh_zero->qh_head_next = 1; 
	    uhci_qh_zero->qh_emnt_next = 1;
	    uhci_qh_zero->zero_1 = 0;
	    uhci_qh_zero->zero_2 = 0;

	    td_1->link_pointer = 1;
	    td_1->td_ctrl_sts = 0;
	    td_1->td_token = 0;
	    td_1->u_td_buffer = u_td_buffer1;
	    td_1->sw_dword1 = 0;
	    td_1->sw_dword2 = 0;
	    td_1->sw_dword3 = 0;
	    td_1->sw_dword4 = 0; 

	    td_2->link_pointer = 1;
	    td_2->td_ctrl_sts = 0;
	    td_2->td_token = 0;
	    td_2->u_td_buffer = u_td_buffer2;
	    td_2->sw_dword1 = 0;
	    td_2->sw_dword2 = 0;
	    td_2->sw_dword3 = 0;
	    td_2->sw_dword4 = 0;

	    td_3->link_pointer = 1;
	    td_3->td_ctrl_sts = 0;
	    td_3->td_token = 0;
	    td_3->u_td_buffer = u_td_buffer3;
	    td_3->sw_dword1 = 0;
	    td_3->sw_dword2 = 0;
	    td_3->sw_dword3 = 0;
	    td_3->sw_dword4 = 0;

	    td_4->link_pointer = 1;
	    td_4->td_ctrl_sts = 0;
	    td_4->td_token = 0;
	    td_4->u_td_buffer = u_td_buffer4;
	    td_4->sw_dword1 = 0;
	    td_4->sw_dword2 = 0;
	    td_4->sw_dword3 = 0;
	    td_4->sw_dword4 = 0;

	    td_5->link_pointer = 1;
	    td_5->td_ctrl_sts = 0;
	    td_5->td_token = 0;
	    td_5->u_td_buffer = u_td_buffer5;
	    td_5->sw_dword1 = 0;
	    td_5->sw_dword2 = 0;
	    td_5->sw_dword3 = 0;
	    td_5->sw_dword4 = 0;

	    td_6->link_pointer = 1;
	    td_6->td_ctrl_sts = 0;
	    td_6->td_token = 0;
	    td_6->u_td_buffer = u_td_buffer6;
	    td_6->sw_dword1 = 0;
	    td_6->sw_dword2 = 0;
	    td_6->sw_dword3 = 0;
	    td_6->sw_dword4 = 0;

	    td_7->link_pointer = 1;
	    td_7->td_ctrl_sts = 0;
	    td_7->td_token = 0;
	    td_7->u_td_buffer = u_td_buffer7;
	    td_7->sw_dword1 = 0;
	    td_7->sw_dword2 = 0;
	    td_7->sw_dword3 = 0;
	    td_7->sw_dword4 = 0;

	    td_8->link_pointer = 1;
	    td_8->td_ctrl_sts = 0;
	    td_8->td_token = 0;
	    td_8->u_td_buffer = u_td_buffer8;
	    td_8->sw_dword1 = 0;
	    td_8->sw_dword2 = 0;
	    td_8->sw_dword3 = 0;
	    td_8->sw_dword4 = 0; 

}


/* Prints the command register for the UHC 
 * which is called USBCMD.
 * It is printed in binary notation.
 */
void read_uhci_usbcmd()  
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0000);
	    print("\n\nusbcmd content: ");
	    dec_to_bin((uint32_t)uhci_cmd);
}

/* Returns the command register for the UHC 
 * which is called USBCMD.
 */
uint16_t get_uhci_usbcmd()  
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0000);
	    return uhci_cmd;
}

/* Returns the interrupt enable register for the UHC 
 * which is called USBINTR.
 */
uint32_t get_uhci_usbintr()
{
	    uint16_t ebaddr = read_word2((const uint16_t)uhci_mem_address,(const uint16_t)0x0004);
	    return (uint32_t)ebaddr;
}

/* Returns the USB status register for the UHC 
 * which is called USBSTS.
 */
uint16_t get_uhci_usbsts()  
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0002);
	    return uhci_cmd;
}

/* Returns the frame number register for the UHC 
 * which is called FRNUM.
 */
uint16_t get_uhci_frnum()  
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0006);
	    return uhci_cmd;
}

/* Returns the frame list base address register for the UHC 
 * which is called FRBASEADD.
 */
uint32_t get_uhci_frbaseaddr()
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0008);
	    return (uint16_t)uhci_cmd;
}

/* Returns the start of frame modify register for the UHC 
 * which is called SOFMOD.
 */
uint8_t get_uhci_sofmod()  
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x000c);
	    return uhci_cmd;
}

/* Returns the status and control register for port 1
 * for the UHC which is called PORTSC1.
 */
uint16_t get_uhci_port1()  
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0010);
	    return uhci_cmd;
}

/* Returns the status and control register for port 2
 * for the UHC which is called PORTSC2.
 */
uint16_t get_uhci_port2()  
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0012);
	    return uhci_cmd;
}

/* Sets the clocks so that 1 frame is 1 ms.
 * Note: before this value was set to 63.
 */
void write_uhci_sofmod(uint8_t sfmod)
{
    	write_byte((const uint16_t)uhci_mem_address,(const uint16_t)0x000c, (const uint8_t)64); 
}

/* Writes a zero in the frame number register. */
void write_uhci_frnum()
{
    	write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0006, (const uint16_t)0x0000);
}

/* Sets the max packet to 64 bytes. */
void write_uhci_maxpacket()
{
	    uint16_t temp = get_uhci_usbcmd();
	    temp |= (0 | (1 << 7));
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0000, (const uint16_t)temp);

}

/* This is an unfinished function. */
void write_uhci_usbintr()
{
	print("hi");
}

/* Sets the base address for the TDs to 0x01400000. */
void write_uhci_frbaseaddr(uint32_t fr_baseaddr) 
{
	    zero_usbms_mem_6((uint32_t*)0x1400000);
	    write_dword3((const uint16_t)uhci_mem_address, (const uint16_t)0x0008, (const uint32_t)0x01400000);
}

/* Prints the status register for the UHC 
 * which is called USBSTS.
 * It is printed in binary notation.
 */
void read_uhci_usbsts()
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0002);
	    print("\n\nusbsts content: ");
	    dec_to_bin((uint32_t)uhci_cmd);
}

/* Prints the interrupt enable register for the UHC 
 * which is called USBINTR.
 * It is printed in binary notation.
 */
void read_uhci_usbintr() 
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0004);
	    print("\n\nusbintr content: ");
	    dec_to_bin((uint32_t)uhci_cmd);
}

/* Prints the frame number register for the UHC 
 * which is called FRNUM.
 * It is printed in binary notation.
 */
void read_uhci_frnum()
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0006);
	    print("\n\nusbfrnum content: ");
	    dec_to_bin((uint32_t)uhci_cmd);
}

/* Prints the frame list base address register
 * for the UHC which is called FRBASEADD.
 * It is printed in decimal, hexadecimal and
 * binary notation.
 */
void read_uhci_frbaseaddr()
{
	    uint32_t uhci_cmd = read_dword3((const uint16_t)uhci_mem_address, (const uint16_t)0x0008);
	    print("\n\nuhci frbaseaddr: ");
	    printi((uint32_t)uhci_cmd);
	    print("\n");
	    print_hex((uint32_t)uhci_cmd);
	    print("\n");
	    dec_to_bin((uint32_t)uhci_cmd);
}

/* Prints the start of frame modify register
 * for the UHC which is called SOFMOD.
 * It is printed in binary notation.
 */
void read_uhci_sofmod()
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x000c);
	    uint8_t uhci_cmd2 = (uint8_t)(uhci_cmd & 0x00ff);
	    print("\n\nuhci sofmod content: ");
	    dec_to_bin((uint32_t)uhci_cmd2);
}

/* Prints the status and control register for port 1
 * for the UHC which is called PORTSC1.
 * It is printed in binary notation.
 */
void read_uhci_usbport1()
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0010);
	    print("\n\nusbport1 content: ");
	    dec_to_bin((uint32_t)uhci_cmd);
}

/* Prints the status and control register for port 2
 * for the UHC which is called PORTSC2.
 * It is printed in binary notation.
 */
void read_uhci_usbport2() 
{
	    uint16_t uhci_cmd = read_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0012);
	    print("\n\nusbport2 content: ");
	    dec_to_bin((uint32_t)uhci_cmd);
}

/* Prints the command register for the UHC
 * at the PCI Configuration Space.
 * It is printed in binary notation.
 */
void read_uhci_pcicommand()
{
	    uint16_t uhci_cmd = read_uhci_word((const uint16_t)PCI_Config_RW_Uhci(found_uhci_bus,found_uhci_device,found_uhci_function,4), (const uint16_t)0x0000);
	    print("\n\nuhci command content: ");
	    dec_to_bin((uint32_t)uhci_cmd);
}

/* Prints the legsup register for the UHC
 * at the PCI Configuration Space.
 * It is printed in binary notation.
 */
void read_uhci_legsup()
{
	    uint16_t uhci_cmd = read_uhci_word((const uint16_t)PCI_Config_RW_Uhci(found_uhci_bus,found_uhci_device,found_uhci_function,0xc0), (const uint16_t)0x0000);
	    print("\n\nuhci legsup content: ");
	    dec_to_bin((uint32_t)uhci_cmd);
}

/* Disables interrupts for the UHC. */
void disable_uhci_intr()
{
	    uint16_t d_uhci_int = ((get_uhci_usbintr()) & (uint16_t)0xfff0);
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0004, (const uint16_t)d_uhci_int);
}

/* Halts the UHC. */
void halt_uhci()
{
	    uint16_t temp = get_uhci_usbcmd();
	    temp &= (~1);
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0000, (const uint16_t)temp);
}

/* Starts the UHC. */
void start_uhci()
{
	    uint16_t temp = get_uhci_usbcmd();
	    temp |= UHCI_HC_START;
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0000, (const uint16_t)temp);

}

/* Performs a host controller reset. */
void uhci_hcreset()
{
	uint16_t temp = get_uhci_usbcmd();
	if((temp & UHCI_HC_RESET) != 2) {
			temp |=UHCI_HC_RESET;
			write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0000, (const uint16_t)temp);
	}
	else
		    print("\n\nError: uhc is already in reset mode");
}

/* from osdev:
 * 1. Set reset bit.
 * 2. Wait 58ms.
 * 3. Clear reset bit.
 * 4. Wait for connect status change.
 * 5. Set enable bit.
 * 6. Wait for enable bit to actually change.
 * 7. Wait 24ms.
 * 8. Proceed with enumeration. 
 */

/* Performs a global reset. */
void uhci_globalreset()
{
	    uint16_t temp = get_uhci_usbcmd();
	    temp |=UHCI_GLOBAL_RESET;
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0000, (const uint16_t)temp);
	    timer_ticks = 0;
	    while(timer_ticks < 1);
	    temp = get_uhci_usbcmd();
	    temp |=UHCI_GLOBAL_RESET;
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0000, (const uint16_t)temp);
}

/* Resets the resume signal bit in the USB status register. */
void resetpc_uhci_rs() 
{
	    uint16_t uhci_intsts = get_uhci_usbsts();
	    uhci_intsts |= 4;
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0002, (const uint16_t)uhci_intsts);
}

/* Resets the host system error bit in the USB status register. */
void resetpc_uhci_hserr() 
{
	    uint16_t uhci_intsts = get_uhci_usbsts();
	    uhci_intsts |= 8;
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0002, (const uint16_t)uhci_intsts);
}		

/* Resets the host controller process error bit in the
 * USB status register.
 */
void resetpc_uhci_hcperr() 
{
	    uint16_t uhci_intsts = get_uhci_usbsts();
	    uhci_intsts |= 16;
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0002, (const uint16_t)uhci_intsts);	
}

/* This function acts as the UHC general
 * interrupt handler. 
 */
void uhci_usb_handler()
{
	    uint16_t uhci_intstatus = get_uhci_usbsts();
	    if((uhci_intstatus & 4) == 4)
		    resetpc_uhci_rs();
	    if((uhci_intstatus & 8) == 8) {
			    resetpc_uhci_hserr();
			    print("\n\nHost system error.");
			    asm("hlt");
	    }
	    if((uhci_intstatus & 16) == 16) {
			    resetpc_uhci_hcperr();
			    print("\n\nHost Controller Process Error.");
	    }
}

/* This probably makes sure that the HC gets
 * 9 ms to handle a test transfer descriptor
 * at the memory address of 0x1401002.
 */
void init_uhci_framepointers(uint32_t uhci_fr_addr)
{
	    uint32_t* temp1 = (uint32_t*)uhci_fr_addr;
	    *temp1 = 0x1401002;
	    (*(temp1 + 1)) = 0x1401002;
	    (*(temp1 + 2)) = 0x1401002;
	    (*(temp1 + 3)) = 0x1401002;
	    (*(temp1 + 4)) = 0x1401002; 
	    (*(temp1 + 5)) = 0x1401002;
	    (*(temp1 + 6)) = 0x1401002;
	    (*(temp1 + 7)) = 0x1401002;
	    (*(temp1 + 8)) = 0x1401002; 
	    // first fill with 1s (terminated pointers)
	    for(uint32_t a = 9; a < 1024 ; a +=1) {

			    (*(temp1 + a)) = 1;
	    }
	    
        /* then start with 1 frame and see if you can get a descriptor within that frame */
}


/* Must be done before any usb transactions on a UHC */
void init_uhci_hc() 
{

        /* 1000 hz. 1/1000 seconds. To go over that is kind of unnecessary and may cause problems if too much */
	    timer_phase(1000); 

        /* get from the bios */
	    uint8_t sofmod = get_uhci_sofmod(); 

        /* activate timer */
	    asm("sti"); 
	    uhci_globalreset();
	    timer_ticks = 0;
        /* 58 ms */
	    while (timer_ticks < 58); 
	    write_uhci_sofmod(sofmod);
        /* deactivate timer */
	    asm("cli"); 
	    timer_phase(18);

	    uhci_port_reset();
	    write_uhci_maxpacket();
	    disable_uhci_intr();
        /* The three lines of code below can probably be discarded. */
	    uint32_t temp1 = get_uhci_usbcmd();
	    temp1 &= (~(1 << 13));
	    write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0000, (const uint16_t)temp1);
	    asm("sti");
        /* Frame number starts at zero. */
	    write_uhci_frnum(); 
        /* Set the address of the TDs at 0x1400000. */
	    write_uhci_frbaseaddr(0x1400000);
	    timer_ticks = 0;
	    while(timer_ticks < 1);
	    init_uhci_framepointers(0x1400000);

	    timer_ticks = 0;
	    while(timer_ticks < 1);

	    read_uhci_frbaseaddr();
	    timer_ticks = 0;
	    while(timer_ticks < 1);
	    print("\n\nKernel debug: first frame pointer value (should be 0x1401002):\n");
	    uint32_t* frame_temp1 = (uint32_t*)0x1400000;
	    print_hex(*frame_temp1);
	    print("\n\nkernel debug: fr num register value (should be 0):\n");
	    read_uhci_frnum();
	    print("\n\nAnd after this, set timer_ticks to over 1 second");
        uhci_port_reset();
	    print("\n\nUhc initialized. Starting...");
	    asm("cli");

}

/* Tests to see if we can get a device descriptor 
 * from a device.
 */
void get_uhci_descriptor()
{

	    timer_phase(20);
	    asm("sti");
	    volatile uint8_t* pointr;
	    struct setup_data* device_descriptor = (struct setup_data*)0x1401110;

        /* The setup packet should indicate that we
         * are requesting a device descriptor.
         */
        device_descriptor->RequestType = USB_DEVICE_TO_HOST;
        device_descriptor->Request = USB_GET_DESCRIPTOR;
        device_descriptor->value_low = 0x00;
        device_descriptor->value_high = USB_GET_DESCRIPTOR_DEVICE;
        device_descriptor->index_low = 0x00;
        device_descriptor->index_high = 0x00;
        device_descriptor->length_low = USB_DEVICE_DESCRIPTOR_DEVICE_LENGTH;
        device_descriptor->length_high = 0x00;

        /* One TD for every 8 byte. */
	    td_1->td_ctrl_sts = (0 | (UHCI_TD_DISABLE_SHORT_PACKET_DETECT | UHCI_TD_MAX_ERR_COUNT | UHCI_TD_FULLSPEED_DEVICE | UHCI_TD_SET_ACTIVE));
	    td_1->td_token = (0 | (UHCI_TD_MAXLENGTH(7) | UHCI_TD_DATATOGGLE(0) | UHCI_TD_EPNUM(0) | UHCI_TD_DEVICE_ADDRESS(0) | UHCI_TD_PIDSETUP)); 
	    
	    td_2->td_ctrl_sts = (0 | (UHCI_TD_DISABLE_SHORT_PACKET_DETECT | UHCI_TD_MAX_ERR_COUNT | UHCI_TD_FULLSPEED_DEVICE | UHCI_TD_SET_ACTIVE));
	    td_2->td_token = (0 | (UHCI_TD_MAXLENGTH(7) | UHCI_TD_DATATOGGLE(1) | UHCI_TD_EPNUM(0) | UHCI_TD_DEVICE_ADDRESS(0) | UHCI_TD_PIDIN)); 

	    td_3->td_ctrl_sts = (0 | (UHCI_TD_DISABLE_SHORT_PACKET_DETECT | UHCI_TD_MAX_ERR_COUNT | UHCI_TD_FULLSPEED_DEVICE | UHCI_TD_SET_ACTIVE));
	    td_3->td_token = (0 | (UHCI_TD_MAXLENGTH(7) | UHCI_TD_DATATOGGLE(0) | UHCI_TD_EPNUM(0) | UHCI_TD_DEVICE_ADDRESS(0) | UHCI_TD_PIDIN)); 

	    td_4->td_ctrl_sts = (0 | (UHCI_TD_DISABLE_SHORT_PACKET_DETECT | UHCI_TD_MAX_ERR_COUNT | UHCI_TD_FULLSPEED_DEVICE | UHCI_TD_SET_ACTIVE));
	    td_4->td_token = (0 | (UHCI_TD_MAXLENGTH(1) | UHCI_TD_DATATOGGLE(1) | UHCI_TD_EPNUM(0) | UHCI_TD_DEVICE_ADDRESS(0) | UHCI_TD_PIDIN)); 

	    td_5->td_ctrl_sts = (0 | (UHCI_TD_DISABLE_SHORT_PACKET_DETECT | UHCI_TD_MAX_ERR_COUNT | UHCI_TD_FULLSPEED_DEVICE | UHCI_TD_SET_ACTIVE));
	    td_5->td_token = (0 | (UHCI_TD_MAXLENGTH(0) | UHCI_TD_DATATOGGLE(1) | UHCI_TD_EPNUM(0) | UHCI_TD_DEVICE_ADDRESS(0) | UHCI_TD_PIDOUT)); 

	    td_6->td_ctrl_sts = (0 | (UHCI_TD_DISABLE_SHORT_PACKET_DETECT | UHCI_TD_MAX_ERR_COUNT | UHCI_TD_FULLSPEED_DEVICE | UHCI_TD_SET_ACTIVE));
	    td_6->td_token = (0 | (UHCI_TD_MAXLENGTH(0) | UHCI_TD_DATATOGGLE(1) | UHCI_TD_EPNUM(0) | UHCI_TD_DEVICE_ADDRESS(0) | UHCI_TD_PIDOUT)); 

	    td_7->td_ctrl_sts = (0 | (UHCI_TD_DISABLE_SHORT_PACKET_DETECT | UHCI_TD_MAX_ERR_COUNT | UHCI_TD_FULLSPEED_DEVICE | UHCI_TD_SET_ACTIVE));
	    td_7->td_token = (0 | (UHCI_TD_MAXLENGTH(1) | UHCI_TD_DATATOGGLE(1) | UHCI_TD_EPNUM(0) | UHCI_TD_DEVICE_ADDRESS(0) | UHCI_TD_PIDOUT)); 

	    td_8->td_ctrl_sts = (0 | (UHCI_TD_DISABLE_SHORT_PACKET_DETECT | UHCI_TD_MAX_ERR_COUNT | UHCI_TD_FULLSPEED_DEVICE | UHCI_TD_SET_ACTIVE));
	    td_8->td_token = (0 | (UHCI_TD_MAXLENGTH(0) | UHCI_TD_DATATOGGLE(1) | UHCI_TD_EPNUM(0) | UHCI_TD_DEVICE_ADDRESS(0) | UHCI_TD_PIDOUT)); 


	    td_1->link_pointer = (volatile uint32_t)0x1401030;
	    td_2->link_pointer = (volatile uint32_t)0x1401050;
	    td_3->link_pointer = (volatile uint32_t)0x1401070;
	    td_4->link_pointer = (volatile uint32_t)0x1401090;
	    td_5->link_pointer = (volatile uint32_t)0x14010b1;
	    td_6->link_pointer = (volatile uint32_t)0x14010d1;
	    td_7->link_pointer = (volatile uint32_t)0x14010f1;




	    uhci_ctrl_1->qh_emnt_next = (volatile uint32_t)0x1401010; // Td1
	    uhci_qh_zero->qh_head_next = (volatile uint32_t)0x1401002; // terminated pointers
	    timer_ticks = 0;
	    while(timer_ticks < 1);


	    start_uhci();
	    
	    timer_ticks = 0;
	    while(timer_ticks < 2);


	    /* Display results and some status reports. */
	    print("\n\nResults of get descriptor:");

	    print("\n\nTd1 status:\n");
	    dec_to_bin((uint32_t)td_1->td_ctrl_sts);
	    print("\n\nTd2 status:\n");
	    dec_to_bin((uint32_t)td_2->td_ctrl_sts);
	    print("\n\nTd3 status:\n");
	    dec_to_bin((uint32_t)td_3->td_ctrl_sts);
	    print("\n\nTd4 status:\n");
	    dec_to_bin((uint32_t)td_4->td_ctrl_sts);
	    print("\n\nTd5 status:\n");
	    dec_to_bin((uint32_t)td_5->td_ctrl_sts);
	    print("\n\nTd6 status:\n");
	    dec_to_bin((uint32_t)td_6->td_ctrl_sts);
	    print("\n\nTd7 status:\n");
	    dec_to_bin((uint32_t)td_7->td_ctrl_sts);
	    print("\n\nTd8 status:\n");
	    dec_to_bin((uint32_t)td_8->td_ctrl_sts);
	    
	    print("\n\nTd1:\n");
	    pointr = (volatile uint8_t*)0x1401110;
	    for(uint32_t a = 0; a < 8; a++)
		        print_hex_byte((uint8_t)(*(pointr + a)));
	    print("\n\nTd2:\n");
	    pointr = (volatile uint8_t*)0x1401150;
	    for(uint32_t a = 0; a < 8; a++)
		        print_hex_byte((uint8_t)(*(pointr + a)));
	    print("\n\nTd3:\n");
	    pointr = (volatile uint8_t*)0x1401190;
	    for(uint32_t a = 0; a < 8; a++)
		        print_hex_byte((uint8_t)(*(pointr + a)));
	    print("\n\nTd4:\n");
	    pointr = (volatile uint8_t*)0x14011d0;
	    for(uint32_t a = 0; a < 8; a++)
		        print_hex_byte((uint8_t)(*(pointr + a)));
	    print("\n\nTd5:\n");
	    pointr = (volatile uint8_t*)0x1401210;
	    for(uint32_t a = 0; a < 8; a++)
		        print_hex_byte((uint8_t)(*(pointr + a)));
	    print("\n\nTd6:\n");
	    pointr = (volatile uint8_t*)0x1401250;
	    for(uint32_t a = 0; a < 8; a++)
		        print_hex_byte((uint8_t)(*(pointr + a)));
	    print("\n\nTd7:\n");
	    pointr = (volatile uint8_t*)0x1401290;
	    for(uint32_t a = 0; a < 8; a++)
		        print_hex_byte((uint8_t)(*(pointr + a)));
	    print("\n\nTd8:\n");
	    pointr = (volatile uint8_t*)0x14012d0;
	    for(uint32_t a = 0; a < 8; a++)
		        print_hex_byte((uint8_t)(*(pointr + a)));

	    halt_uhci();


	    td_1->link_pointer = 1;
	    td_2->link_pointer = 1;
	    td_3->link_pointer = 1;


	    uhci_ctrl_1->qh_emnt_next = 1;
        /* terminated pointers */
	    uhci_qh_zero->qh_head_next = 1; 

	    asm("cli");
	    timer_phase(18);

}





