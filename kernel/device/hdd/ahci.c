/* This is the kernel's AHCI driver. */

#include "../../types.h"
#include "ahci.h"
#include "../../mem.h"
#include "../../timer.h"

/* Initializes some variables. */
void init_some_values()
{
        ahci_cap_regs = ahci_base_address;
        ahci_ghc_regs = ahci_base_address + (volatile uint32_t)4;
        ahci_pi_regs = ahci_base_address + (volatile uint32_t)0xc;
        ahci_ports_regs = ahci_base_address + (volatile uint32_t)AHCI_PORTS_REGS_OFFSET;
}

/* This is an unfinished function. */
uint8_t reset_ahci_hostcontroller()
{
        return 5;
}

/* Starts a specific port. */
void start_ahci_port(uint32_t port)
{
        volatile uint32_t ahci_port_address = ahci_ports_regs + (volatile uint32_t)(port*(uint32_t)AHCI_PORT_REGS_LENGTH);
        uint32_t address = read_dword((const uint32_t)ahci_port_address, (const uint32_t)AHCI_PORT_CMD);
        address |= 1;
        write_dword((const uint32_t)ahci_port_address, (const uint32_t)AHCI_PORT_CMD, (const uint32_t)address);
}

/* Resets a specific port. */
void reset_ahci_port(uint32_t ahci_port)
{
        volatile uint32_t ahci_port_address = ahci_ports_regs + (volatile uint32_t)(ahci_port*(uint32_t)AHCI_PORT_REGS_LENGTH);
        uint32_t address = read_dword((const uint32_t)ahci_port_address, (const uint32_t)AHCI_PORT_STCTL);
        address |= 1;
        write_dword((const uint32_t)ahci_port_address, (const uint32_t)AHCI_PORT_STCTL, (const uint32_t)address);
        kernel_delay_100(1);
}

/* Checks if the SATA controller supports AHCI mode only.
 * If it is so then it returns 1. Otherwise it returns 0. 
*/
uint8_t get_ahci_mode_enable()
{
        uint32_t cap_regs_values = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000000);
        cap_regs_values >>= 18;
        cap_regs_values &= 0x00000001;
        uint8_t ame = (uint8_t)cap_regs_values;
        return ame;
}

/* Checks if communication to the HBA shall be via
 * AHCI mechanisms. If it is so then it returns 1.
 * Otherwise it returns 0.
 */
uint8_t get_ahci_mode_activated()
{
        uint32_t ghc_regs_values = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000004);
        ghc_regs_values >>= 31;
        ghc_regs_values &= 0x00000001;
        uint8_t ama = (uint8_t)ghc_regs_values;
        return ama;
}

/* Returns the number of ports. */
uint32_t get_ahci_num_ports()
{
        uint32_t cap_regs_values = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000000);
        cap_regs_values &= 0x0000003F;
        return cap_regs_values;
}

/* Returns the number of ports which are exposed to the HBA. */
uint32_t get_ahci_ports_implemented()
{
        uint32_t pi_regs_values = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x0000000c);
        return pi_regs_values;
}

/* Checks if a port multiplier is enables. if it is so
 * then this function returns a one. Otherwise this function
 * returns a zero.
 */
uint32_t get_ahci_port_multiplier_enabled()
{
        uint32_t cap_regs_values = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000000);
        cap_regs_values >>= 17;

        cap_regs_values &= 0x00000001;
        return cap_regs_values;
}

/* Makes the BIOS hand over the controller to the kernel. */
void perform_bios_handoff()
{
        uint32_t cap_regs_values = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000028);
        cap_regs_values |=2;
        write_dword((const uint32_t)ahci_base_address, (const uint32_t)0x28, (const uint32_t)cap_regs_values);
}

/* Checks if the kernel has control over the HBA.
 * If it does then this function returns a one.
 * Otherwise this function returns a zero.
 */
uint8_t check_os_owned_semaphore()
{
        uint32_t cap_regs_values = read_dword((const uint32_t)ahci_base_address, (const uint32_t)0x00000028);
        cap_regs_values &= (uint32_t)0x00000003;

        if (cap_regs_values == 2)
                return (uint8_t)1;
        
        return (uint8_t)0;
}

/* Checks if a device is attached to a specific port.
 * If it is so then this function returns a one.
 * Otherwise this function returns a zero.
 */
uint8_t check_ahci_attached_device_at_port(uint32_t ahci_port)
{
        volatile uint32_t address = (volatile uint32_t)(ahci_ports_regs + (volatile uint32_t)(ahci_port*AHCI_PORT_REGS_LENGTH));
        uint32_t port_regs_values = read_dword((const uint32_t)address, (const uint32_t)AHCI_PORT_STSTS);
        port_regs_values &= 0x0000000f;
        if (port_regs_values == AHCI_DEVICECONNECTION_EST)
                return (uint8_t)1;
        else
                return (uint8_t)0;
}

/* Checks if a device is powered at a specific port.
 * If it is so then this function returns a one.
 * Otherwise this function returns a zero.
 */
uint8_t check_ahci_device_powered_at_port(uint32_t ahci_port)
{
        volatile uint32_t address = (volatile uint32_t)(ahci_ports_regs + (volatile uint32_t)(ahci_port*AHCI_PORT_REGS_LENGTH));
        uint32_t port_regs_values = read_dword((const uint32_t)address, (const uint32_t)AHCI_PORT_STSTS);
        port_regs_values >>= 8;
        port_regs_values &= 0x0000000f;
        if (port_regs_values == AHCI_DEVICECONNEST_AND_POWERED)
                return (uint8_t)1;
        else
                return (uint8_t)0;

}
