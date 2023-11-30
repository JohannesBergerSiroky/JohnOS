/* This is the kernel's network card driver for
 * the Intel 8254x series of cards.
 */

#include "../../types.h"
#include "../pci.h"
#include "using_ethernet.h"
#include "../../system.h"
#include "../../graphics/screen.h"
#include "../../mem.h"
#include "../../timer.h"
#include "../../isr.h"
#include "../../irq.h"

/* Sets the card as bus master and access to i/o and memory 
 * space. It is set on the PCI Configuration Space.
 */
void set_ethernetc_commandreg()
{
        uint16_t temp_cmd = PCI_Config_RW(found_ethernetc_bus,found_ethernetc_device,found_ethernetc_function,4);
        uint16_t temp_cmd2 = PCI_Config_RW(found_ethernetc_bus,found_ethernetc_device,found_ethernetc_function,6);
        uint32_t temp_cmd3 = (uint32_t)(temp_cmd2*65536);
        uint32_t temp_cmd4 = (uint32_t)temp_cmd;

        uint32_t temp_cmd5 = temp_cmd3 + temp_cmd4;
        temp_cmd5 &= (~(1));
        temp_cmd5 |= 0x107;

        uint32_t address = (uint32_t)((found_ethernetc_bus << 16) | (found_ethernetc_device << 11) | (found_ethernetc_function << 8) | (4 & 0xfc) | ((uint32_t)0x80000000));
        outportl(0xCF8, address);
        outportl(0xCFC, temp_cmd5);
}


/****************************************************************************** init data *************************************************************************** */

/* Initializes some memory regions. */
void init_phy_location()
{
        e1000_phy_location = kmem_4k_allocate();
        zero_usbms_mem_6((uint32_t*)e1000_phy_location);
        e1000_eeprom_read_addr = kmem_4k_allocate();
        zero_usbms_mem_6((uint32_t*)e1000_eeprom_read_addr);
}

/* Initializes some variables. */
void init_mac_and_variables_netmem()
{
        for(uint32_t a = 0;a < 6;a++) {
                mac[a] = 0;
        }

        current_rx = 0;
        handled_rx = 0;
        current_tx = 0;
        handled_tx = 0;
        netmem = kmem_4k_allocate();
        zero_usbms_mem_6((uint32_t*)netmem);
}



/* ******************************************************************************* reads ******************************************************************************* */

/* Prints the device info on the PCI Configuration Space.
 * It is printed in hexadecimal notation.
 */
void read_ethernetc_deviceinfo()
{
        uint32_t temp = PCI_Config_RW(found_ethernetc_bus, found_ethernetc_device, found_ethernetc_function, 2);
        print("\n\nEthernet device info:\n");
        print_hex((uint32_t)temp);
}

/* Prints the vendor info on the PCI Configuration Space.
 * It is printed in hexadecimal notation.
 */
void read_ethernetc_vendorinfo()
{
        uint32_t temp = PCI_Config_RW(found_ethernetc_bus, found_ethernetc_device, found_ethernetc_function, 0);
        print("\n\nEthernet vendor info:\n");
        print_hex((uint32_t)temp);
}

/* Prints the device control register. It
 * is printed in binary notation.
 */
void read_ethernetc_cmd()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000000);
        print("\n\nEthernet cmd register:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the device status register. It
 * is printed in binary notation.
 */
void read_ethernetc_sts()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000008);
        print("\n\nEthernet status register:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the EEPROM/Flash Control/Data register.
 * It is printed in binary notation.
 */
void read_ethernetc_eeprom()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000010);
        print("\n\nEthernet eeprom:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the EEPROM Read register.
 * It is printed in binary notation.
 */
void read_ethernetc_eeprom_read()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000014);
        print("\n\nEthernet eeprom read:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the extended device control register.
 * It is printed in binary notation.
 */
void read_ethernetc_cmdext()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000018);
        print("\n\nEthernet cmd ext:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Flow control address low register. 
 * The value should be c28001h.
 * It is printed in binary notation. 
 */
void read_ethernetc_fcal() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000028);
        print("\n\nEthernet fcal:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Flow control address high register. 
 * The value should be 0100h.
 * It is printed in binary notation 
 */
void read_ethernetc_fcah() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x0000002c);
        print("\n\nEthernet fcah:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Receive configuration word register.
 * It is printed in binary notation
 */
void read_ethernetc_rcfgw() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000180);
        print("\n\nEthernet receive control:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Transmit configuration word register.
 * It is printed in binary notation.
 */
void read_ethernetc_tcfgw() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000178);
        print("\n\nEthernet transmit control:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Receive control register.
 * It is printed in binary notation
 */
void read_ethernetc_rctrl() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000100);
        print("\n\nEthernet receive control:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Transmit control register.
 * It is printed in binary notation.
 */
void read_ethernetc_tctrl() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000400);
        print("\n\nEthernet transmit control:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Transmit IPG register.
 * It is printed in binary notation.
 */
void read_ethernetc_tipg() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000410);
        print("\n\nEthernet transmit ipg:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Receive Descriptor Base Low register.
 * It is printed in binary notation.
 */
void read_ethernetc_rdbl() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002800);
        print("\n\nEthernet receive descriptor base low:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Receive Descriptor Base High register.
 * It is printed in binary notation.
 */
void read_ethernetc_rdbh() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002804);
        print("\n\nEthernet receive descriptor base high:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Receive Descriptor Length register.
 * It is printed in binary notation.
 */
void read_ethernetc_rdl() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002808);
        print("\n\nEthernet receive descriptor length:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Receive Descriptor Head register.
 * It is printed in binary notation.
 */
void read_ethernetc_rdh() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002810);
        print("\n\nEthernet receive descriptor head:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Receive Descriptor Tail register.
 * It is printed in binary notation.
 */
void read_ethernetc_rdt() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002818);
        print("\n\nEthernet receive descriptor tail:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Transmit Descriptor Base Low register.
 * It is printed in binary notation.
 */
void read_ethernetc_tdbl() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003800);
        print("\n\nEthernet transmit descriptor base low:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Transmit Descriptor Base High register.
 * It is printed in binary notation.
 */
void read_ethernetc_tdbh() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003804);
        print("\n\nEthernet transmit descriptor base high:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Transmit Descriptor Length register.
 * It is printed in binary notation.
 */
void read_ethernetc_tdl() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003808);
        print("\n\nEthernet transmit descriptor length:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Transmit Descriptor Head register.
 * It is printed in binary notation.
 */
void read_ethernetc_tdh() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003810);
        print("\n\nEthernet transmit descriptor head:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Transmit Descriptor Tail register.
 * It is printed in binary notation.
 */
void read_ethernetc_tdt() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003818);
        print("\n\nEthernet transmit descriptor tail:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the Transmit Descriptor Control register.
 * It is printed in binary notation.
 */
void read_ethernetc_tdc() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003828);
        print("\n\nEthernet transmit descriptor control:\n");
        dec_to_bin((uint32_t)temp);
}

/* Attempts to read and print the 
 * Mac Adress from EEPROM. 
 */
void read_mac_address()
{
        timer_install();
        asm("sti");
        uint32_t temp;
        temp = get_eeprom_read_data(0);
        mac[0] = (uint8_t)(temp & 0xff);
        mac[1] = (uint8_t)(temp >> 8);

        temp = get_eeprom_read_data(1);
        mac[2] = (uint8_t)(temp & 0xff);
        mac[3] = (uint8_t)(temp >> 8);

        temp = get_eeprom_read_data(2);
        mac[4] = (uint8_t)(temp & 0xff);
        mac[5] = (uint8_t)(temp >> 8);

        print("\n\nMac Address: ");
        print_hex_byte(mac[0]);
        print("-");
        print_hex_byte(mac[1]);
        print("-");
        print_hex_byte(mac[2]);
        print("-");
        print_hex_byte(mac[3]);
        print("-");
        print_hex_byte(mac[4]);
        print("-");
        print_hex_byte(mac[5]);
        asm("cli");
        irq_uninstall_handler(0);
        print("\n\n");

}


/* Prints the interrupt cause set register.
 * It is printed in binary notation.
 */ 
void read_ethernetc_ics() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0);
        print("\n\nEthernet interrupt causes set:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the interrupt clear register.
 * It is printed in binary notation.
 */ 
void read_ethernetc_ic() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c8);
        print("\n\nEthernet interrupt clear:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the interrupt set mask register.
 * It is printed in binary notation.
 */
void read_ethernetc_ism() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000d0);
        print("\n\nEthernet interrupt mask set:\n");
        dec_to_bin((uint32_t)temp);
}

/* Prints the interrupt mask clear register.
 * It is printed in binary notation.
 */
void read_ethernetc_icm() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000d8);
        print("\n\nEthernet interrupt mask clear:\n");
        dec_to_bin((uint32_t)temp);
}


/* ******************************************************************************************* gets **************************************************************************** */

/* Returns the device control register. */
uint32_t get_ethernetc_cmd()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000000);
        return (uint32_t)temp;
}

/* Returns the device status register. */
uint32_t get_ethernetc_sts()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000008);
        return (uint32_t)temp;
}

/* Returns the EEPROM/Flash Control/Data register. */
uint32_t get_ethernetc_eeprom()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000010);
        return (uint32_t)temp;
}

/* Returns the EEPROM read register. */
uint32_t get_ethernetc_eeprom_read()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000014);
        return (uint32_t)temp;
}

/* Returns the extended device control register. */
uint32_t get_ethernetc_cmdext()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000018);
        return (uint32_t)temp;
}

/* Returns the flow control address low register. 
 * value should be c28001h
 */
uint32_t get_ethernetc_fcal() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000028);
        return (uint32_t)temp;
}

/* Returns the flow control address high register. 
 * value should be 0100h
 */
uint32_t get_ethernetc_fcah() 
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x0000002c);
        return (uint32_t)temp;
}

/* Returns the Receive Descriptor Base Low register. */
uint32_t get_ethernetc_rdbl()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002800);
        return (uint32_t)temp;
}

/* Returns the Receive Descriptor Base High register. */
uint32_t get_ethernetc_rdbh()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002804);
        return (uint32_t)temp;
}

/* Returns the Receive Descriptor Length register. */
uint32_t get_ethernetc_rdl()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002808);
        return (uint32_t)temp;
}

/* Returns the Receive Descriptor Head register. */
uint32_t get_ethernetc_rdh()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002810);
        return (uint32_t)temp;
}

/* Returns the Receive Descriptor Tail register. */
uint32_t get_ethernetc_rdt()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002818);
        return (uint32_t)temp;
}

/* Returns the Transmit Descriptor Base Low register. */
uint32_t get_ethernetc_tdbl()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003800);
        return (uint32_t)temp;
}

/* Returns the Transmit Descriptor Base High register. */
uint32_t get_ethernetc_tdbh()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003804);
        return (uint32_t)temp;
}

/* Returns the Transmit Descriptor Length register. */
uint32_t get_ethernetc_tdl()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003808);
        return (uint32_t)temp;
}

/* Returns the Transmit Descriptor Head register. */
uint32_t get_ethernetc_tdh()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003810);
        return (uint32_t)temp;
}

/* Returns the Transmit Descriptor Tail register. */
uint32_t get_ethernetc_tdt()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003818);
        return (uint32_t)temp;
}

/* Returns the Transmit Descriptor Control register. */
uint32_t get_ethernetc_tdc()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003828);
        return (uint32_t)temp;
}

/* Returns the Receive Descriptor Control register. */
uint32_t get_ethernetc_rdc()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002828);
        return (uint32_t)temp;
}

/* Returns the Interrupt Cause Read register. */
uint32_t get_ethernetc_int()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0);
        return (uint32_t)temp;
}

/* Returns the Interrupt Cause Set register. */
uint32_t get_ethernetc_intclear()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c8);
        return (uint32_t)temp;
}

/* Returns the Interrupt Mask Set/Read register. */
uint32_t get_ethernetc_intmask()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000d0);
        return (uint32_t)temp;
}

/* Returns the Interrupt Mask Clear register. */
uint32_t get_ethernetc_intmaskclear()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000d8);
        return (uint32_t)temp;
}

/* Returns the Management Data Interface Control register. */
uint32_t get_ethernetc_mdictl()
{
        uint32_t temp = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000020);
        return (uint32_t)temp;
}

/* This function reads the EEPROM register at offset 0x14.
 * An address in the EEPROM must be specified
 * which in this case is handed to this function through  
 * the eeprom_address parameter. 
*/
uint32_t get_eeprom_read_data(uint32_t eeprom_address)
{
        uint32_t temp2;
        uint32_t read_complete = 0;
        set_eeprom_read(eeprom_address);

        /* poll until read is complete */
        while(!read_complete) { 
                temp2 = get_ethernetc_eeprom_read();
                if((temp2 & ETH_8254_EEPROM_READ_DONE_ANDMASK(temp2)))
                        read_complete = 1;
        }

        temp2 = get_ethernetc_eeprom_read();
        ETH_8254_EEPROM_GET_DATA(temp2);
        return temp2;
}


/* ******************************************************************************************* writes **************************************************************************** */

/* Allows overall access to EEPROM. */
void allow_read_eepromcmd()
{
        timer_install();

        uint32_t temp2 = get_ethernetc_eeprom();
        temp2 |= (1 << 7);
        timer_phase(1000);
        asm("sti");
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000010, (const uint32_t)temp2);
        timer_ticks = 0;
        while(timer_ticks < 2);
        asm("cli");
        timer_phase(18);
        irq_uninstall_handler(0);
}

/* Disallow access to EEPROM. */
void disallow_read_eepromcmd()
{
        timer_install();
        uint32_t temp2 = get_ethernetc_eeprom();
        temp2 &= (~(1 | 2 | (1 << 2) | (1 << 6)));
        temp2 &= (~(1 << 7));
        timer_phase(1000);
        asm("sti");
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000010, (const uint32_t)temp2);
        timer_ticks = 0;
        while(timer_ticks < 2);
        asm("cli");
        timer_phase(18);
        irq_uninstall_handler(0);
}

/* Toggles a value usually after a EEPROM read operation. */
void toggle_eeprom()
{
        uint32_t temp2 = get_ethernetc_eeprom();


        if((temp2 & 0x40))
                temp2 &= (~(0x40));
        else
                temp2 |= 0x40;

        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000010, (const uint32_t)temp2);
}


/* the index of the word to read. size of network eeprom is 1024 bytes on qemu */

/* Starts the EEPROM read operation. */
void start_eeprom_read() 
{
        uint32_t temp2 = get_ethernetc_eeprom_read();
        temp2 |= 1;
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000014, (const uint32_t)temp2);
}

/* Sets the address of the EEPROM that is to be read
 * or written to.
 */
void set_eeprom_read(uint32_t address)
{
        uint32_t temp2 = get_ethernetc_eeprom_read();
        temp2 &= 0x000000ff;
        ETH_8254_EEPROM_READ_START(temp2, address);
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000014, (const uint32_t)temp2);
}

/* Writes data to the Interrupt Mask Set/Read Register */
void write_ethernetc_intmask()
{
        uint32_t temp2 = get_ethernetc_intmask();
        temp2 |= 0x1f6dc;
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000d0, (const uint32_t)temp2);
}

/* Writes data to the Interrupt Mask Clear Register. */
void write_ethernetc_intmaskclear()
{
        uint32_t temp2 = get_ethernetc_intmaskclear();
        temp2 = (0xFFFFFFFF & (~0x1f6dc));
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000d8, (const uint32_t)temp2);
}

/* Enables interrupt on the MDI control register. */
void write_mdac_interruptenable()
{
        uint32_t temp2 = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000020);
        temp2 |= (1 << 29);
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000020, (const uint32_t)temp2);
}


/* 
 * Reads the MDI ctl, MDI control register to access 
 * phy addresses and phy data locations.
 * It reads the data returned for it or them.
 * The macros are for if-statements 
 * and preparing for read and writes.
 */
uint32_t read_mdctl_phyreg_data(uint32_t phy_reg)
{
        /* get data from the mdictl register and set the read_complete variable to zero */
        uint32_t temp2 = get_ethernetc_mdictl();
        uint32_t read_complete = 0;

        /* check if the error bit is cleared. if not then clear it */
        if (ETH_8254_CHECK_MDICTL_ERROR_CLEARED(temp2)) {
                goto read_mdictl_phy_continue;
        } 
        else {
                ETH_8254_MDICTL_ERRORBIT_CLEAR(temp2);
        }

        read_mdictl_phy_continue:

        /* Check if the ready bit is set if it's set then clear it. */
        if(ETH_8254_CHECK_MDICTL_READYBIT_SET(temp2)) {
                uint32_t temp3 = temp2;
                ETH_8254_MDICTL_READYBIT_CLEAR(temp3);
                write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)20, (const uint32_t)temp3);
        }

        /* set the bits to make ready for a read */
        temp2 = get_ethernetc_mdictl();
        ETH_8254_MDICTL_START_READ_PHYADDR(temp2);

        /* write to indicate that you're ready to read */
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)20, (const uint32_t)temp2);

        /* polling , waiting for the ready bit */
        while(!read_complete) {
                temp2 = get_ethernetc_mdictl();
                if(ETH_8254_CHECK_MDICTL_READYBIT_SET(temp2))
                        read_complete = 1;
        }

        /* go and get the data read from the phy data locations and and mask it */
        temp2 = read_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)20);
        temp2 &= 0x0000FFFF;

        return temp2;
}

/* Writes zeroes to the Multicast Table Array */

void write_mta() 
{
        for(uint32_t i = 0; i < 0x80; i++) {
                write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00005200 + (const uint32_t)i, (const uint32_t)0x00000000);
        }
}


/* Switches on the network card's ability to transfer data. Link up. */
void start_link() 
{
        /* get the total value from the card's command register (memory mapped hardware register). command register is at offset 0 from the card's base memory addr (memory mapped) */
        uint32_t temp2 = get_ethernetc_cmd(); 

        /* sets bit number 6's value to 1. the other bits are unchanged. */
        ETH_CTL_START_LINK(temp2); 

        /* write back to the command register. Again, the command register is at offset 0 from the card's base mem address in the card's address range */
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000000, (const uint32_t)temp2);
}

/* Initializes the abilities to receive packets. */
void rd_init()
{
        uint32_t rx_mem = kmem_4k_allocate();
        uint32_t rx_addr = kmem_4k_allocate();
        uint32_t mem_bit;
        zero_usbms_mem_6((uint32_t*)rx_mem);
        mem_bit = (rx_addr - 0x300000)/4096;
        for(uint32_t i = 0; i < 32; i++) {

                rx[i] = (struct e1000_rx_descriptor*)(rx_mem + (i*16));
                rx[i]->addr_low = (rx_addr + (i*8192) + (i*16));
                set_bit(mem_bit + i);
                set_bit(mem_bit + 32 + i);
                zero_usbms_mem_6((uint32_t*)rx[i]->addr_low);
                zero_usbms_mem_6((uint32_t*)(rx[i]->addr_low + 4096));
                zero_usbms_mem_6((uint32_t*)(rx[i]->addr_low + 8192));
                rx[i]->addr_high = 0;
                rx[i]->status = 0;

        }
                set_bit(mem_bit + 64);

        /* rx addr low */
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002800, (const uint32_t)rx_mem); 

        /* rx addr high */
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002804, (const uint32_t)0x00000000); 

        /* rx len */
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002808, (const uint32_t)(32*16)); 

        /* rx head */
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002810, (const uint32_t)0x00000000);

        /* rx tail */
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002818, (const uint32_t)31); 

        uint32_t temp = (0 | ((1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 15) | (1 << 26)));

        /* Writes to the receive control register. */
        write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000100, (const uint32_t)temp); 
}

/* Initializes the abilities to transmit packets. */
void td_init()
{
        uint32_t tx_mem = kmem_4k_allocate();
	    zero_usbms_mem_6((uint32_t*)tx_mem);

	    for(uint32_t i = 0; i < 8; i++) {

			    tx[i] = (struct e1000_tx_descriptor*)(tx_mem + (i*16));
			    tx[i]->addr_low = 0;
			    tx[i]->addr_high = 0;
			    tx[i]->cmd = 0;
			    tx[i]->status = 1;

	    }

	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003800, (const uint32_t)tx_mem);
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003804, (const uint32_t)0x00000000);

        /* tx len */
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003808, (const uint32_t)(8*16)); 

        /* tx head */
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003810, (const uint32_t)0x00000000);
 
        /* tx tail */
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003818, (const uint32_t)0x00000000); 

	    uint32_t temp = (0 | ((1 << 1) | (1 << 3) | (15 << 4) | (64 << 12) | (1 << 24)));

        /* Writes to the transmit control register. */
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00000400, (const uint32_t)temp); 
}

/* Initializes the network card by using some
 * of the functions in this file.
 */
void init_ethernetc_controller()
{
	    print("\nInitializing ethernet controller.");
	    read_mac_address();
	    start_link();
	    write_mta();
	    write_ethernetc_intmask();
	    write_ethernetc_intmaskclear();
	    rd_init();
	    td_init();
	    print("\n\nCard initialized.");
}

/* Attempts to send a packet. */
void send_packet(uint32_t tx_addr_low, uint16_t len)
{
	    tx[current_tx]->addr_low = tx_addr_low;
	    tx[current_tx]->length = len;
	    tx[current_tx]->cmd = (0 | (1 | 2 | (1 << 3) | (1 << 4)));
	    tx[current_tx]->status = 0;
	    handled_tx = current_tx;
	    current_tx = ((current_tx + 1) % 8);
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00003818, (const uint32_t)current_tx); // tx tail
	    timer_install();
	    asm("sti");
	    timer_ticks = 0;
	    while((!(tx[handled_tx]->status & 0xff)) && (timer_ticks < 36));
	    if(!(tx[handled_tx]->status & 0xff))
		        print("\n\nError: time out after sending network packet.");
	    irq_uninstall_handler(0);
	    asm("cli");
}

/* ************************************************************************************** interrupt handling ****************************************************************************** */

/* This function acts as the general interrupt handler
 * for a Intel 8254x network card. It uses some of the
 * functions in this file.
 */
void ethernetc_e1000_handler()
{
	    uint32_t good_threshold = 0;
	    uint32_t temp1 = get_ethernetc_int();

	    if ((temp1 & 0x4) == 0x4) {
			    writeclear_ethernetc_linkstatuschange();
			    print("\nWarning: link status changed. An error could have occurred.");
	    }
	    if ((temp1 & 0x8) == 0x8) {
			    writeclear_ethernetc_rse();
			    print("\nReceive sequence error.");
	    }

	    if ((temp1 & 0x10) == 0x10) {
			    writeclear_ethernetc_goodthreshold();
			    good_threshold = 1;
	    }

	    if ((temp1 & 0x40) == 0x40) {
			    writeclear_ethernetc_rfifooverrun();
			    print("\nReceive fifo overrun.");
	    }

	    if ((temp1 & 0x80) == 0x80) {

			    writeclear_ethernetc_packetreceived();
			    print("\n\nPacket received interrupt.");

			    if(good_threshold && (rx[current_rx]->status & 0x1)) {

					    uint8_t* net_addr = (uint8_t*)rx[current_rx]->addr_low;
					    memset2(net_addr, (uint8_t*)netmem, 4096);
					    handled_rx = current_rx;
					    rx[handled_rx]->status = 0;
					    current_rx = ((current_rx + 1) % 32);
					    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x00002818, (const uint32_t)handled_rx); // tail
			    }

			    else if(!good_threshold)
				        print("\n\nError: No good threshold.");
			    else
				        print("\n\nError: rx status is not set.");
	    }



	    if ((temp1 & 0x200) == 0x200) {
			    writeclear_ethernetc_mdac();
			    if(phy_reg_flag)
				        read_mdctl_phyreg_data(phy);
	    }

	    if ((temp1 & 0x400) == 0x400) {
			    writeclear_ethernetc_rcorderedset();
			    print("\nReceive ordered sets.");
	    }

	    if ((temp1 & 0x1000) == 0x1000) {
			    writeclear_ethernetc_phyinterrupt();
			    print("\nPhy interrupt.");
	    }

	    if ((temp1 & 0x2000) == 0x2000) {
			    writeclear_ethernetc_gpint();
			    print("\nGeneral purpose interrupt.");
	    }

	    if ((temp1 & 0x8000) == 0x8000) {
			    writeclear_ethernetc_txgoodthreshold();
			    // Commented code: tx_good_threshold = 1;
			    print("\ntx good threshold.");
	    }

	    if ((temp1 & 0x10000) == 0x10000) {
			    writeclear_ethernetc_rshortpcktdetect();
			    print("\nReceived short packet.");
	    }
}

/* Clears the interrupt notification caused by
 * a MDI/O Access Complete.
 */
void writeclear_ethernetc_mdac()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x200;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification caused by
 * a Receive Descriptor Minimum Threshold Reached change.
 */
void writeclear_ethernetc_goodthreshold()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x10;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification caused by
 * an event when the receiver timer expires.
 */
void writeclear_ethernetc_packetreceived()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x80;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification caused by
 * a link status change.
 */
void writeclear_ethernetc_linkstatuschange()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x4;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification caused by
 * a receive sequence error.
 */
void writeclear_ethernetc_rse() 
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x8;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification caused by
 * a Receiver Overrun.
 */
void writeclear_ethernetc_rfifooverrun()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x40;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification which should happen
 * when the hardware receives configuration symbols (/C/
 * codes.
 */
void writeclear_ethernetc_rcorderedset()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x400;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification caused when
 * when the PHY generates an interrupt.
 */
void writeclear_ethernetc_phyinterrupt()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x1000;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification caused by
 * a General Purpose Interrupt on SDP6.
 */
void writeclear_ethernetc_gpint()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x2000;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification caused when
 * the descriptor ring has reached the threshold
 * specified in the Transmit Descriptor Control
 * register.
 */
void writeclear_ethernetc_txgoodthreshold()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x8000;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}

/* Clears the interrupt notification caused when
 * a packet of size <= RSRPD.SIZE register has been
 * detected and transferred to host memory.
 */
void writeclear_ethernetc_rshortpcktdetect()
{
	    uint32_t temp2 = get_ethernetc_int();
	    temp2 |= 0x10000;
	    write_dword((const uint32_t)ethernetc_mem_address, (const uint32_t)0x000000c0, (const uint32_t)temp2);
}



