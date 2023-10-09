/* This file contains AHCI functions. These functions
 * print values and we can use this file partly for
 * debugging purposes.
 */

#include "../../types.h"
#include "ahci.h"
#include "../../graphics/screen.h"
#include "../../mem.h"


/* Checks if the SATA controller supports AHCI mode only.
 * If it is so then it prints a one. Otherwise it 
 * prints a zero. 
*/
void print_ahci_mode_enable()
{
        uint32_t temp = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000000);
        temp >>= (uint32_t)18;
        temp &= (uint32_t)0x00000001;
        uint8_t ame = (uint8_t)temp;
        print("Ahci enable bit is set to: ");
        printbyte(ame);
        print("\n");
}

/* Checks if communication to the HBA shall be via
 * AHCI mechanisms. If it is so then it prints a one.
 * Otherwise it prints a zero.
 */
void print_ahci_mode_activated()
{
        uint32_t temp = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000004); 
        temp >>= (uint32_t)31;
        temp &= (uint32_t)0x00000001;
        uint8_t ama = (uint8_t)temp;
        print("Ahci activated bit is set to: ");
        printbyte(ama);
        print("\n");
}

/* Prints the number of ports. */
void print_ahci_num_ports()
{
        uint32_t temp = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000000);
        temp &= (uint32_t)0x0000001F;
        print("Ahci number of ports: ");
        printi((uint32_t)temp);
        print("\n");
}

/* Prints the number of ports which are exposed to the HBA. */
void print_ports_implemented()
{
        uint32_t temp = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x0000000c);
        print("number of ports implemented, for software to use: ");
        printi((uint32_t)temp);
        print("\n");
}

/* Checks if a port multiplier is enables. if it is so
 * then this function prints a one. Otherwise this function
 * prints a zero.
 */ 
void print_ahci_port_multiplier_enabled()
{
        uint32_t cap_regs_values = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000000);
        cap_regs_values >>= (uint32_t)17;
        cap_regs_values &= (uint32_t)0x00000001;
        uint8_t pme = (uint8_t)cap_regs_values;
        print("port multiplier enable bit is set to: ");
        printbyte(pme);
        print("\n");
}
