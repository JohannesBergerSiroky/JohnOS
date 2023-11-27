/* The kernel's handling of the PCI Configuration Space that x86 systems
 * have and which constitutes the foundation for making many drivers.
 */


#include "../types.h"
#include "../system.h"
#include "pci.h"
#include "hdd/ahci.h"
#include "../graphics/screen.h"
#include "usb/usb_uhci.h"
#include "usb/usb_ohci.h"
#include "usb/usb_ehci.h"
#include "usb/usb_xhci.h"
#include "network/using_ethernet.h"

/* This code makes the OS list available PCI devices that are on the PCI-bus.
 * It assumes that it has found the valid parameters i.e. the pci_type, the 
 * pci_func and the pci_progif. 
 */
void list_pci_devices(uint16_t pci_type, uint16_t pci_func, uint16_t pci_progif, uint32_t f)
{

        if ((pci_type > 0xff) || (pci_func > 0xff) || (pci_progif > 0xff))
                return;

        if (f > 0) {
                print("\nFunction: ");
                printi((uint32_t)f);
                print("\n");
        }

        switch (pci_type) {
                case 0:
                        if (pci_func == 0)
                                print("          Type: VGA/Non-VGA\n           Function: Non-VGA\n\n");
                        else if (pci_func > 0) {
                                print("pci_func = "); printi((uint32_t)pci_func); print("\n");
                                print("          Type: VGA/Non-VGA\n           Function: VGA compatible device\n\n");

                        }
                        break;

                case 1:
                        if (pci_func == 0) {
                                print("          Type: Mass storage controller\n");
                                print("          Function: SCSI Bus Controller\n\n");
                        }
                        else if (pci_func == 1) {
                                print("          Type: Mass storage controller\n"); 
                                print("          Function: IDE Controller\n\n");
                        }
                        else if (pci_func == 2) {
                                print("          Type: Mass storage controller\n");
                                print("          Function: Floppy Disk Controller\n\n");
                        }
                        else if (pci_func == 3) {
                                print("          Type: Mass storage controller\n");
                                print("          Function: IPI Bus Controller\n\n");
                        }
                        else if (pci_func == 4) {
                                print("          Type: Mass storage controller\n");
                                /* suggestion: check progIF here */
                                print("          Function: RAID Controller\n\n");
                        }
                        else if (pci_func == 5) {
                                if (pci_progif == 0x20) {
                                        print("          Type: Mass storage controller\n");
                                        print("          Function: ATA Controller(Single DMA)\n\n");
                                }
                                else if (pci_progif == 0x30) {
                                        print("          Type: Mass storage controller\n");
                                        print("          Function: ATA Controller(Chained DMA)\n\n");
                                }
                        }
                        else if (pci_func == 6) {
                                if (pci_progif == 0) {
                                        print("          Type: Mass storage controller\n");
                                        print("          Function: Serial ATA (Vendor Specific Interface)\n\n");
                                }
                                else if (pci_progif == 1) {
                                        print("          Type: Mass storage controller\n");
                                        print("          Function: 	Serial ATA (AHCI 1.0)\n\n");
                                }
                        }

                        else if (pci_func == 7) {
                                print("          Type: Mass storage controller\n");
                                print("          Function: Serial Attached SCSI (SAS)\n\n");
                        }
                        else if (pci_func == 0x80) {
                                print("          Type: Mass storage controller\n");
                                print("          Function: Other Mass Storage Controller\n\n");
                        }
                        break;

                case 2:
                        if (pci_func == 0) {
                                print("          Type: Network Controller\n");
                                print("          Function: Ethernet Controller\n\n");
                        }
                        else if (pci_func == 1) {
                                print("          Type: Network Controller\n");
                                print("          Function: Token Ring Controller\n\n");
                        }
                        else if (pci_func == 2) {
                                print("          Type: Network Controller\n");
                                print("          Function: FDDI Controller\n\n");
                        }
                        else if (pci_func == 3) {
                                print("          Type: Network Controller\n");
                                print("          Function: ATM Controller\n\n");
                        }
                        else if (pci_func == 4) {
                                print("          Type: Network Controller\n");
                                print("          Function: ISDN Controller\n\n");
                        }
                        else if (pci_func == 5) {
                                print("          Type: Network Controller\n");
                                print("          Function: WorldFip Controller\n\n");
                        }
                        else if (pci_func == 6) {
                                print("          Type: Network Controller\n");
                                print("          Function: PICMG 2.14 Multi Computing\n\n");
                        }
                        else if (pci_func == 0x80) {
                                print("          Type: Network Controller\n");
                                print("          Function: Other Network Controller\n\n");
                        }
                break;

                case 3:
                        if (pci_func == 0) {
                                if (pci_progif == 0) {
                                        print("          Type: Display Controller\n");
                                        print("          Function: VGA-Compatible Controller\n\n");
                                }
                                else if (pci_progif == 1) {
                                        print("          Type: Display Controller\n");
                                        print("          Function: 8512-Compatible Controller\n\n");
                                }
                        }
                        else if (pci_func == 1) {
                                print("          Type: Display Controller\n");
                                print("          Function: XGA Controller\n\n");
                        }
                        else if (pci_func == 2) {
                                print("          Type: Display Controller\n");
                                print("          Function: 3D Controller (Not VGA-Compatible)\n\n");
                        }
                        else if (pci_func == 0x80) {
                                print("          Type: Display Controller\n");
                                print("          Function: Other Display Controller\n\n");
                        }
                break;

                case 4:
                        if (pci_func == 0) {
                                print("          Type: Multimedia Device\n");
                                print("          Function: Video Device\n\n");
                        }
                        else if (pci_func == 1) {
                                print("          Type: Multimedia Device\n");
                                print("          Function: Audio Device\n\n");
                        }
                        else if (pci_func == 2) {
                                print("          Type: Multimedia Device\n");
                                print("          Function: Computer Telephony Device\n\n");
                        }
                        else if (pci_func == 0x80) {
                                print("          Type: Multimedia Device\n");
                                print("          Function: Other Multimedia Device\n\n");
                        }
                break;

                case 5:
                        if (pci_func == 0) {
                                print("          Type: Memory Controller\n");
                                print("          Function: RAM Controller\n\n");
                        }
                        else if (pci_func == 1) {
                                print("          Type: Memory Controller\n");
                                print("          Function: Flash Controller\n\n");
                        }
                        else if (pci_func == 0x80) {
                                print("          Type: Memory Controller\n");
                                print("          Function: Other Memory Controller\n\n");
                        }
                break;

                case 6:
                        if (pci_func == 0) {
                                print("          Type: Bridge Device\n"); 
                                print("          Function: Host Bridge\n\n");
                        }
                        else if (pci_func == 1) {
                                print("          Type: Bridge Device\n");
                                print("          Function: ISA Bridge\n\n");
                        }
                        else if (pci_func == 2) {
                                print("          Type: Bridge Device\n");
                                print("          Function: EISA Bridge\n\n");
                        }
                        else if (pci_func == 3) {
                                print("          Type: Bridge Device\n");
                                print("          Function: MCA Bridge\n\n");
                        }
                        else if (pci_func == 4) {
                                if (pci_progif == 0) {
                                        print("          Type: Bridge Device\n");
                                        print("          Function: PCI-to-PCI Bridge\n\n");
                                }
                                else if (pci_progif == 1) {
                                        print("          Type: Bridge Device\n");
                                        print("          Function: PCI-to-PCI Bridge (Subtractive Decode)\n\n");
                                }
                        }
                        else if (pci_func == 5) {
                                print("          Type: Bridge Device\n");
                                print("          Function: PCMCIA Bridge\n\n");
                        }
                        else if (pci_func == 6) {
                                print("          Type: Bridge Device\n");
                                print("          Function: NuBus Bridge\n\n");
                        }
                        else if (pci_func == 7) {
                                print("          Type: Bridge Device\n");
                                print("          Function: CardBus Bridge\n\n");
                        }
                        else if (pci_func == 8) {
                                print("          Type: Bridge Device\n");
                                print("          Function: RACEway Bridge\n\n");
                        }
                        else if (pci_func == 9) {
                                if (pci_progif == 0x40) {
                                        print("          Type: Bridge Device\n");
                                        print("          Function: PTP Bridge (Semi-Transparent, Primary)\n\n");
                                }
                                else if (pci_progif == 0x80) {
                                        print("          Type: Bridge Device\n");
                                        print("          Function: PTP Bridge (Semi-Transparent, Secondary)\n\n");
                                }
                        }
                        else if (pci_func == 0xa) {
                                print("          Type: Bridge Device\n");
                                print("          Function: RAM Controller\n\n");
                        }
                        else if (pci_func == 0x80) {
                                print("          Type: Bridge Device\n");
                                print("          Function: Other Bridge Device\n\n");
                        }
                break;

                case 7:
                        if (pci_func == 0) {
                    
                                if (pci_progif == 0) {
                                        print("          Type: 	Simple Communication Controllers\n");
                                        print("          Function: Generic XT-Compatible Serial Controller\n\n");
                                }
                                else if (pci_progif == 1) {
                                        print("          Type: 	Simple Communication Controllers\n");
                                        print("          Function: 16450-Compatible Serial Controller\n\n");
                                }
                                else if (pci_progif == 2) {
                                        print("          Type: 	Simple Communication Controllers\n");
                                        print("          Function: 16550-Compatible Serial Controller\n\n");
                                }
                                else if (pci_progif == 3) {
                                        print("          Type: 	Simple Communication Controllers\n");
                                        print("          Function: 16650-Compatible Serial Controller\n\n");
                                }
                                else if (pci_progif == 4) {
                                        print("          Type: 	Simple Communication Controllers\n");
                                        print("          Function: 16750-Compatible Serial Controller\n\n");
                                }
                                else if (pci_progif == 5) {
                                        print("          Type: 	Simple Communication Controllers\n");
                                        print("          Function: 16850-Compatible Serial Controller\n\n");
                                }
                                else if (pci_progif == 6) {
                                        print("          Type: 	Simple Communication Controllers\n");
                                        print("          Function: 16950-Compatible Serial Controller\n\n");
                                }
                        }

                        else if (pci_func == 1) {
                                if (pci_progif == 0) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: Parallel Port\n\n");
                                }
                                else if (pci_progif == 1) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: Bi-Directional Parallel Port\n\n");
                                }
                                else if (pci_progif == 2) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: ECP 1.X Compliant Parallel Port\n\n");
                                }
                                else if (pci_progif == 3) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: IEEE 1284 Controller\n\n");
                                }
                                else if (pci_progif == 0xfe) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: IEEE 1284 Target Device\n\n");
                                }
                        }
                        else if (pci_func == 2) {
                                print("          Type: Simple Communication Controllers\n");
                                print("          Function: Multiport Serial Controller\n\n");
                        }

                        else if (pci_func == 3) {
                                if (pci_progif == 0) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: Generic Modem\n\n");
                                }
                                else if (pci_progif == 1) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: H. Modem (16450-Compatible Interface)\n\n");
                                }
                                else if (pci_progif == 2) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: H. Modem (16550-Compatible Interface)\n\n");
                                }
                                else if (pci_progif == 3) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: H. Modem (16650-Compatible Interface)\n\n");
                                }
                                else if (pci_progif == 4) {
                                        print("          Type: Simple Communication Controllers\n");
                                        print("          Function: H. Modem (16750-Compatible Interface)\n\n");
                                }
                        }
                        else if (pci_func == 4) {
                                print("          Type: Simple Communication Controllers\n");
                                print("          Function: IEEE 488.1/2 (GPIB) Controller\n\n");
                        }
                        else if (pci_func == 5) {
                                print("          Type: Simple Communication Controllers\n");
                                print("          Function: Smart Card\n\n");
                        }
                        else if (pci_func == 0x80) {
                                print("          Type: Simple Communication Controllers\n");
                                print("          Function: Other Communications Device\n\n");
                        }

                break;

                case 8:
                        if (pci_func == 0) {
                                if (pci_progif == 0) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: Generic 8259 PIC\n\n");
                                }
                                else if (pci_progif == 1) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: ISA PIC\n\n");
                                }
                                else if (pci_progif == 2) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: EISA PIC\n\n");
                                }
                                else if (pci_progif == 0x10) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: I/O APIC Interrupt Controller\n\n");
                                }
                                else if (pci_progif == 0x20) {
                                        print("          Type: 	Base System Peripherals\n");
                                        print("          Function: I/O(x) APIC Interrupt Controller\n\n");
                                }

                        }
                        else if (pci_func == 1) {
                                if (pci_progif == 0) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: Generic 8237 DMA Controller\n\n");
                                }
                                else if (pci_progif == 1) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: ISA DMA Controller\n\n");
                                }
                                else if (pci_progif == 2) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: EISA DMA Controller\n\n");
                                }

                        }
                        else if (pci_func == 2) {
                                if (pci_progif == 0) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: Generic 8254 System Timer\n\n");
                                }
                                else if (pci_progif == 1) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: ISA System Timer\n\n");
                                }
                                else if (pci_progif == 2) {
                                        print("          Type: Base System Peripherals\n");
                                        print("          Function: EISA System Timer\n\n");
                                }

                        }
				        else if (pci_func == 3) {
					        if (pci_progif == 0) {
							        print("          Type: Base System Peripherals\n");
							        print("          Function: Generic RTC Controller\n\n");
					        }
					        else if (pci_progif == 1) {
							        print("          Type: Base System Peripherals\n");
							        print("          Function: ISA RTC Controller\n\n");
					        }


				        }
			            else if (pci_func == 4) {
						            print("          Type: Base System Peripherals\n");
						            print("          Function: Generic PCI Hot-Plug Controller\n\n");
			            }
			            else if (pci_func == 0x80) {
						            print("          Type: Base System Peripherals\n");
						            print("          Function: Other System Peripheral\n\n");
			            }
		        break;

		        case 9:
				        if (pci_func == 0) {
						        print("          Type: Input Devices\n");
						        print("          Function: Keyboard Controller\n\n");
				        }
				        else if (pci_func == 1) {
						        print("          Type: Input Devices\n");
						        print("          Function: Digitizer\n\n");
				        }
				        else if (pci_func == 2) {
						        print("          Type: Input Devices\n");
						        print("          Function: Mouse Controller\n\n");
				        }
				        else if (pci_func == 3) {
						        print("          Type: Input Devices\n");
						        print("          Function: Scanner Controller\n\n");
				        }
				        else if (pci_func == 4) {
						        if (pci_progif == 0x00) {
								        print("          Type: Input Devices\n");
								        print("          Function: Gameport Controller (Generic)\n\n");
						        }
						        else if (pci_progif == 0x10) {
								        print("          Type: Input Devices\n"); 
								        print("          Function: Gameport Controller (Legacy)\n\n");
						        }
				        }
				        else if (pci_func == 0x80) {
						        print("          Type: Input Devices\n");
						        print("          Function: Other Input Controller\n\n");
				        }
		        break;

		        case 0xa:
				        if (pci_func == 0) {
							        print("          Type: Docking Stations\n");
							        print("          Function: Generic Docking Station\n\n");
				        }
				        else if (pci_func == 0x80) {
						        print("          Type: Docking Stations\n");
						        print("          Function: Other Docking Station\n\n");
				        }
				        break;

				        case 0xb:
				        if (pci_func == 0x0) {
							        print("          Type: Processors\n");
							        print("          Function: 386 Processor\n\n");
				        }
				        else if (pci_func == 0x1) {
						        print("          Type: Processors\n");
						        print("          Function: 486 Processor\n\n");
				        }
				        else if (pci_func == 0x2) {
						        print("          Type: Processors\n");
						        print("          Function: Pentium Processor\n\n");
				        }
				        else if (pci_func == 0x10) {
						        print("          Type: Processors\n");
						        print("          Function: Alpha Processor\n\n");
				        }
				        else if (pci_func == 0x20) {
						        print("          Type: Processors\n");
						        print("          Function: PowerPC Processor\n\n");
				        }
				        else if (pci_func == 0x30) {
						        print("          Type: Processors\n");
						        print("          Function: MIPS Processor\n\n");
				        }
				        else if (pci_func == 0x40) {
						        print("          Type: Processors\n");
						        print("          Function: Co-Processor\n\n");
				        }
		        break;

	            case 0xc:
				        if (pci_func == 0) {
						        if (pci_progif == 0x00) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: IEEE 1394 Controller (FireWire)\n\n");
						        }
						        else if (pci_progif == 0x10) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: IEEE 1394 Controller (1394 OHCI Spec)\n\n");
						        }
				        }
				        else if (pci_func == 1) {
						        print("          Type: Serial Bus Controllers\n");
						        print("          Function: ACCESS bus\n\n");
				        }
				        else if (pci_func == 2) {
						        print("          Type: Serial Bus Controllers\n");
						        print("          Function: SSA\n\n");
				        }
				        else if (pci_func == 3) {
						        if (pci_progif == 0) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: USB (UHCI Spec)\n\n");
						        }
						        else if (pci_progif == 0x10) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: USB (OHCI Spec)\n\n");
						        }
						        else if (pci_progif == 0x20) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: USB2 Host Controller (Intel EHCI)\n\n");
						        }
						        else if (pci_progif == 0x30) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: USB3 XHCI Controller\n\n");
						        }
						        else if (pci_progif == 0x80) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: Unspecified USB Controller\n\n");
						        }
						        else if (pci_progif == 0xfe) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: USB (Not Host Controller)\n\n");
						        }
				        }
				        else if (pci_func == 4) {
						        print("          Type: Serial Bus Controllers\n");
						        print("          Function: Fibre Channel\n\n");
				        }
				        else if (pci_func == 5) {
						        print("          Type: Serial Bus Controllers\n");
						        print("          Function: SMBus\n\n");
				        }
				        else if (pci_func == 6) {
						        print("          Type: Serial Bus Controllers\n");
						        print("          Function: InfiniBand\n\n");
				        }
				        else if (pci_func == 7) {
						        if (pci_progif == 0) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: IPMI SMIC Interface\n\n");
						        }
						        else if (pci_progif == 1) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: IPMI Kybd Controller Style Interface\n\n");
						        }
						        else if (pci_progif == 2) {
								        print("          Type: Serial Bus Controllers\n");
								        print("          Function: IPMI Block Transfer Interface\n\n");
						        }
				        }
				        else if (pci_func == 8) {
						        print("          Type: Serial Bus Controllers\n");
						        print("          Function: SERCOS Interface Standard (IEC 61491)\n\n");
				        }
				        else if (pci_func == 9) {
						        print("          Type: Serial Bus Controllers\n");
						        print("          Function: CANbus\n\n");
				        }
		        break;

		        case 0xd:
				        if (pci_func == 0) {
						        print("          Type: Wireless Controllers\n");
						        print("          Function: iRDA Compatible Controller\n\n");
				        }
				        else if (pci_func == 1) {
						        print("          Type: Wireless Controllers\n");
						        print("          Function: Consumer IR Controller\n\n");
				        }
				        else if (pci_func == 0x10) {
						        print("          Type: Wireless Controllers\n");
						        print("          Function: RF Controller\n\n");
				        }
				        else if (pci_func == 0x11) {
						        print("          Type: Wireless Controllers\n");
						        print("          Function: Bluetooth Controller\n\n");
				        }
				        else if (pci_func == 0x12) {
						        print("          Type: Wireless Controllers\n");
						        print("          Function: Broadband Controller\n\n");
				        }
				        else if (pci_func == 0x20) {
						        print("          Type: Wireless Controllers\n");
						        print("          Function: Ethernet Controller (802.11a)\n\n");
				        }
				        else if (pci_func == 0x21) {
						        print("          Type: Wireless Controllers\n");
						        print("          Function: Ethernet Controller (802.11b)\n\n");
				        }
				        else if (pci_func == 0x80) {
						        print("          Type: Wireless Controllers\n");
						        print("          Function: Other Wireless Controller\n\n");
				        }
		        break;

		        case 0xe:
				        if (pci_func == 0) {
						        if (pci_progif == 0) {
								        print("          Type: Intelligent I/O Controllers\n");
								        print("          Function: Message FIFO\n\n");
						        }
				        }
		        break;

		        case 0xf:
				        if (pci_func == 0) {
						        print("          Type: Satellite Communication Controllers\n");
						        print("          Function: TV Controller\n\n");
				        }
				        else if (pci_func == 1) {
						        print("          Type: Satellite Communication Controllers\n");
						        print("          Function: Audio Controller\n\n");
				        }
				        else if (pci_func == 2) {
						        print("          Type: Satellite Communication Controllers\n");
						        print("          Function: Voice Controller\n\n");
				        }
				        else if (pci_func == 3) {
						        print("          Type: Satellite Communication Controllers\n");
						        print("          Function: Data Controller\n\n");
				        }
				        else if (pci_func == 4) {
						        print("          Type: Satellite Communication Controllers\n");
						        print("          Function: Broadband Controller\n\n");
				        }
		        break;

		        case 0x10:
				        if (pci_func == 0x00) {
						        print("          Type: Encryption/Decryption Controllers\n");
						        print("          Function: N and C Encrpytion/Decryption\n\n");
				        }
				        else if (pci_func == 0x10) {
						        print("          Type: Encryption/Decryption Controllers\n");
						        print("          Function: Entertainment Encryption/Decryption\n\n");
				        }
				        else if (pci_func == 0x80) {
						        print("          Type: Encryption/Decryption Controllers\n");
						        print("          Function: Other Encryption/Decryption\n\n");
				        }
		        break;

		        case 0x11:
				        if (pci_func == 0x00) {
						        print("          Type: Data A/S Processing Controllers\n");
						        print("          Function: DPIO Modules\n\n");
				        }
				        else if (pci_func == 0x1) {
						        print("          Type: Data A/S Processing Controllers\n");
						        print("          Function: Performance Counters\n\n");
				        }
				        else if (pci_func == 0x10) {
						        print("          Type: Data A/S Processing Controllers\n");
						        print("          Function: CSPTFTM\n\n");
				        }
				        else if (pci_func == 0x20) {
						        print("          Type: Data A/S Processing Controllers\n");
						        print("          Function: Management Card\n\n");
				        }
				        else if (pci_func == 0x80) {
						        print("          Type: Data A/S Processing Controllers\n");
						        print("          Function: Other Data A/S Processing Controller\n\n");
				        }
		        break;
		        default:
			            if ((pci_type > 0x11) && (pci_type < 0xff))
				            print("Type: Reserved");
			            if (pci_type == 0xff)
				            return;
			            if (pci_type > 0xff)
				            return;
		        break;

        }

}


/***********************************************************************************
 * all PCI-devices have  256 bytes of config registers that enables software to    *
 * configure the device                                                            *
 *                                                                                 *
 *256 bytes = 64 registers, 32bit each                                             *
 *                                                                                 *
 *processor uses address lines 2 to 7 (6 bits) for the 64 registers. Register 0-63.*
 *                                                                                 *
 *                                                                                 *
 *configuration address space contains these registers I guess                     *
 *                                                                                 *
 *I guess that we are going to use the host bridge = host (system) to pci bridge   *
 *                                                                                 *
 *Two 32-bit I/O memory locations are used, the first location (PCI_CONFIG_ADDRESS)*
 *is named CONFIG_ADDRESS, and the second (PCI_CONFIG_DATA) is called CONFIG_DATA  *
 *                                                                                 * 
 *config_data generates the configuration access and tranfers data to or from the  *
 *configuration data register                                                      *
 *                                                                                 *
 *CONFIG_ADDRESS address space (PCI_CONFIG_ADDRESS, 32 bit):                       *  
************************************************************************************/

/***********************************************************************************************
  31			30 - 24      23 - 16	 	15 - 11	       10 - 8	      7 - 2	       1 - 0   *
                                                                                               *
  Enable Bit	Reserved	pci Bus Number	Device Number	Function Number	Register Number	00 *
***********************************************************************************************/



/***************************************************************************
*16 registers for header type 0:                                           *
*                                                                          * 
*register (offset)	bits 31-24	bits 23-16	bits 15-8	bits 7-0           * 
*	                              Device ID	            Vendor ID        00*
*	                              Status	             Command         04*
*	                Class code	Subclass    Prog IF     Revision ID      08*
*	                BIST	   Header type	Latency Time Cache Line Size 12*
*	Base address #0 (BAR0)                                               16*
*	Base address #1 (BAR1)                                               20*
*	Base address #2 (BAR2)                                               24*
*	Base address #3 (BAR3)                                               28*
*	Base address #4 (BAR4)                                               32*
*	Base address #5 (BAR5)                                               36*
*	Cardbus CIS Pointer                                                  40*
*	Subsystem ID	Subsystem Vendor ID                                  44*
*	Expansion ROM base address                                           48*
*	Reserved	Capabilities Pointer                                     52*
*	Reserved                                                             56*
*   Max latency	 Min Grant	Interrupt PIN	Interrupt Line               60*
***************************************************************************/

/* Returns a kind of useless value I guess. */
uint32_t PCI_Config_RW3(uint32_t p_bus, uint32_t p_device, uint32_t p_func, uint16_t p_offset)
{
		uint32_t address;
		address = (uint32_t)(PCI_BUS(p_bus) | (PCI_DEVICE(p_device)) | PCI_FUNCTION(p_func) | (p_offset & 0xfc) | ((uint32_t)PCI_ENABLE));
		return address;
}

/* If the parameters will result in a valid space for a valid device
 * then preturn will return an offset within a part of the
 * configuration space for that device.
 */
uint32_t PCI_Config_RW2(uint32_t p_bus, uint32_t p_device, uint32_t p_func, uint16_t p_offset)
{
        uint32_t address;
        uint32_t preturn;

        address = (uint32_t)(PCI_BUS(p_bus) | (PCI_DEVICE(p_device)) | PCI_FUNCTION(p_func) | (p_offset & 0xfc) | ((uint32_t)PCI_ENABLE));

        outportl(PCI_CONFIG_ADDRESS, address);
        preturn = inportl(PCI_CONFIG_DATA);

        return preturn;
}



/* If the parameters will result in a valid space for a valid device
 * then preturn will return an offset within a part of the
 * configuration space for that device. This function returns
 * a short value instead.
 */
uint16_t PCI_Config_RW(uint32_t p_bus, uint32_t p_device, uint32_t p_func, uint16_t p_offset)
{
        uint32_t address;
        uint16_t temp;
        uint32_t preturn;

        address = (uint32_t)(PCI_BUS(p_bus) | (PCI_DEVICE(p_device)) | PCI_FUNCTION(p_func) | (p_offset & 0xfc) | ((uint32_t)PCI_ENABLE));

        outportl(PCI_CONFIG_ADDRESS, address);
        preturn = inportl(PCI_CONFIG_DATA);


        temp = (uint16_t)((preturn >> ((p_offset & 2) * 8)) & 0x0000ffff); 
        return temp;
}

/* Handles the data from preturn so that it will suit the memory
 * space for the Uhci's sport in the configuration space.
 */
uint16_t PCI_Config_RW_Uhci(uint32_t p_bus, uint32_t p_device, uint32_t p_func, uint16_t p_offset)
{
        uint32_t address;
        uint16_t temp;
        uint32_t preturn;

        address = (uint32_t)(PCI_BUS(p_bus) | (PCI_DEVICE(p_device)) | PCI_FUNCTION(p_func) | (p_offset & 0xfc) | ((uint32_t)PCI_ENABLE));

        outportl(PCI_CONFIG_ADDRESS, address);
        preturn = inportl(PCI_CONFIG_DATA);

        if ((p_offset > 15) && (p_offset < 37)) {
	            if((preturn & 0x00000001) == 1) {
			            temp = (uint16_t)(preturn & (uint16_t)0xffe0);
	            }
	            else 
		                temp = (uint16_t)((preturn >> ((p_offset & 2) * 8)) & (uint16_t)0xffff);
        }

        else
	        temp = (uint16_t)((preturn >> ((p_offset & 2) * 8)) & (uint16_t)0xffff);
        return temp;
}



/* Handles the data from preturn so that it will suit the memory
 * space for the Uhci's sport in the configuration space. Handles the 
 * data a bit differently from PCI_Config_RW_Uhci.
 */
uint32_t PCI_Config_RW_Uhci2(uint32_t p_bus, uint32_t p_device, uint32_t p_func, uint16_t p_offset)
{
	    uint32_t address;
	    uint32_t preturn;

	    address = (uint32_t)(PCI_BUS(p_bus) | (PCI_DEVICE(p_device)) | PCI_FUNCTION(p_func) | (p_offset & 0xfc) | ((uint32_t)PCI_ENABLE));

	    outportl(PCI_CONFIG_ADDRESS, address);
	    preturn = inportl(PCI_CONFIG_DATA);

	    if ((p_offset > 15) && (p_offset < 37)) {
		        if((preturn & 0x00000001) == 1) {

            			preturn &= (uint32_t)0xffffffe0;
		        }
	    }

	    return preturn;
}



/* Handles the data from preturn so that it will suit the memory
 * space for the ethernet e1000's sport in the configuration space.
 */
void PCI_Config_RW_ethernetc_io(uint32_t p_bus, uint32_t p_device, uint32_t p_func, uint16_t p_offset)
{
	    uint32_t address;
	    uint32_t preturn;
	    address = (uint32_t)(PCI_BUS(p_bus) | (PCI_DEVICE(p_device)) | PCI_FUNCTION(p_func) | (p_offset & 0xfc) | ((uint32_t)PCI_ENABLE));

	    outportl(PCI_CONFIG_ADDRESS, address);
	    preturn = inportl(PCI_CONFIG_DATA);

	    if ((p_offset > 15) && (p_offset < 37)) {
		        if((preturn & 0x00000001) == 1) {
				        print("Kernel debug: bit 0 is set to 1, io is used.\n\n");
            			preturn &= (uint32_t)0xfffffff8;
				        ethernetc_io_address = (const uint32_t)preturn;
                }
		        else
			        print("Bit 0 not set to 1. io is not used.\n\n");
	    }
}

/* Returns offset 0, that is the vendor information of the device. */
uint16_t get_pci_VD(uint32_t bus, uint32_t device, uint32_t funct)
{
        uint16_t vendor;
        vendor = (PCI_Config_RW(bus,device,funct,0) != 0xFFFF);
        return vendor;
}

/* This function is not used, and it looks unfinished. */
void check_pci_devices(uint8_t bus, uint8_t device)
{

        uint16_t VendorID;
        uint16_t pci_header_type;
        uint16_t function;
        pci_header_type = get_pci_HeaderType(bus, device,0);
        if ((VendorID = PCI_Config_RW(bus,device,0,0)) != 0xFFFF && (pci_header_type == 0)) {

                if( (pci_header_type & 0x80) != 0) {
                         /* It is a multi-function device, so check remaining functions */
                         for(function = 1; function < 8; function++) {
                            	if(get_pci_VD(bus, device,0) != 0xFFFF)
                                 	    check_pci_function(bus, device,0);
                  		 }
                }

           

        }
}

/* Reads the subclass of a device */
uint16_t check_pci_function(uint32_t bus, uint32_t device, uint32_t funct)
{
        uint16_t func;
        uint16_t vendor;
        /*functions: class ID, subclass ID, progIF. subclass = function */
        if ((vendor = PCI_Config_RW(bus,device,0,0)) != 0xFFFF)
	            func = (PCI_Config_RW(bus,device,funct,10) & 0x00ff);
		    
	    else
		        func = 512;
	    
        return func;
}

/* Checks the header type of a device. */
uint16_t get_pci_HeaderType(uint32_t bus, uint32_t device, uint32_t funct)
{
	    uint16_t pci_header_type;
	    uint16_t vendor;
	    if ((vendor = PCI_Config_RW(bus,device,0,0)) != 0xFFFF)
                /* The information about the header type is located at offset 14. */
		        pci_header_type = (PCI_Config_RW(bus,device,funct,14) & 0x00ff);
		    
	    else
		        pci_header_type = 512;

	    return pci_header_type;
}

/* Scans all available pci devices and display their type and function.
 * The first function in this file is used a lot, here. 
 */
void pci_bscan_devices() 
{
        uint16_t vendor;
        uint16_t ptype;
        uint16_t pfunc;
        uint16_t pprogif;
        uint16_t headerType;
        uint32_t function = 0;

        for (uint32_t pbus = 0;pbus < 256; pbus++) {

		        for (uint32_t pdevice = 0;pdevice < 32; pdevice++) {

				        if ((vendor = PCI_Config_RW(pbus,pdevice,0,0)) != 0xFFFF) {
					        
						        headerType= get_pci_HeaderType(pbus, pdevice,0);
					        
						        if( (headerType & 0x80) != 0) {
								        
								        print("\nBus: "); printi(pbus);
								        print("\nDevice: "); printi(pdevice);
								        print("\nMultifunction device");
								        print("\n");
							            for(function = 1; function < 8; function++) {
                     							ptype = check_pci_type(pbus, pdevice, function);
									            pprogif = check_pci_progif(pbus, pdevice, function);
                         						pfunc = check_pci_function(pbus, pdevice, function);
									            list_pci_devices(ptype, pfunc, pprogif, function);
							            }
							            function = 0;
						        }
					        
						        else {
								        print("\nBus: "); printi(pbus);
								        print("\nDevice: "); printi(pdevice);
								        print("\n");

								        ptype = check_pci_type(pbus, pdevice, (uint32_t)0);

								        pfunc = check_pci_function(pbus, pdevice, (uint32_t)0);
								        pprogif = check_pci_progif(pbus, pdevice, (uint32_t)0);

								        list_pci_devices(ptype, pfunc, pprogif, function);
						        }
				        }
		        }
			        
        }

        print("done");
}

/* Checks if a desired device exists on the computer.
 * The desired class, subclass and progif is checked every time
 * it finds a device. If they match, it returns an array with the 
 * bus, device and the function parameters of that device.
 * Also the memory locations of the device's programmable registers
 * or controllers is gathered using the function set_device.
 */
uint32_t* find_device(uint16_t f_class, uint16_t f_subclass, uint16_t f_pprogif)
{

		uint16_t vendor;
		uint16_t headerType;
		uint16_t ptype;
		uint16_t pfunc;
		uint16_t pprogif;
		pci_device_array[0] = 0;
		pci_device_array[1] = 1;
		pci_device_array[2] = 0;
		uint32_t function = 0;
		
        for (uint32_t pbus = 0;pbus < 256; pbus++) {

		        for (uint32_t pdevice = 0;pdevice < 32; pdevice++) {

				        if ((vendor = PCI_Config_RW(pbus,pdevice,0,0)) != 0xFFFF) {
					        
						        headerType= get_pci_HeaderType(pbus, pdevice,0);
					        
						        if( (headerType & 0x80) != 0) {

							            for(function = 1; function < 8; function++) {
                     							ptype = check_pci_type(pbus, pdevice, function);
									            pprogif = check_pci_progif(pbus, pdevice, function);
                         						pfunc = check_pci_function(pbus, pdevice, function);

									            if ((ptype == f_class) && (pfunc == f_subclass) && (pprogif == f_pprogif)) {
											            pci_device_array[0] = pbus;
											            pci_device_array[1] = pdevice;
											            pci_device_array[2] = function;
											            pci_dvc_bus = pci_device_array[0];
											            pci_dvc_device = pci_device_array[1];
											            pci_dvc_function = pci_device_array[2];
											            set_device(pci_dvc_bus, pci_dvc_device, pci_dvc_function, ptype, pfunc, pprogif);
											            return pci_device_array;
									            }
							            }
							            function = 0;
						        }
					        
						        else {


								        ptype = check_pci_type(pbus, pdevice, (uint32_t)0);

								        pfunc = check_pci_function(pbus, pdevice, (uint32_t)0);
								        pprogif = check_pci_progif(pbus, pdevice, (uint32_t)0);
								        if ((ptype == f_class) && (pfunc == f_subclass) && (pprogif == f_pprogif)) {
										        pci_device_array[0] = pbus;
										        pci_device_array[1] = pdevice;
										        pci_device_array[2] = (uint32_t)0;
										        pci_dvc_bus = pci_device_array[0];
										        pci_dvc_device = pci_device_array[1];
										        pci_dvc_function = pci_device_array[2];
										        set_device(pci_dvc_bus, pci_dvc_device, pci_dvc_function, ptype, pfunc, pprogif);
										        return pci_device_array;
								        }

					            }
			            }
	            }
			        
        }
	    print("\nError: No such pci device.");
	    print("\n");
	    return 0;

}

/* Gets some memory locations from specific devices. These locations
 * are typically the start addresses of the programmable areas
 * of host controllers i.e. where you would make a driver or drivers
 * for a device.
 */
void set_device(uint32_t deviceinfo_1, uint32_t deviceinfo_2, uint32_t deviceinfo_3, uint16_t pci_class, uint16_t pci_func, uint16_t pci_pprogif)
{

	    asm("sti");
	    volatile uint32_t temp;
         /* uhci */
	    if((pci_class == 0xc) && (pci_func == 0x3) && (pci_pprogif == 0)) {
			    found_uhci_bus = deviceinfo_1;
			    found_uhci_device = deviceinfo_2;
			    found_uhci_function = deviceinfo_3;
                temp = PCI_Config_RW_Uhci(found_uhci_bus,found_uhci_device,found_uhci_function,32);
                uhci_mem_address = (volatile uint32_t)temp;
	    }

        /* xhci */
	    if((pci_class == 0xc) && (pci_func == 0x3) && (pci_pprogif == 0x30)) {
			    found_xhci_bus = deviceinfo_1;
			    found_xhci_device = deviceinfo_2;
			    found_xhci_function = deviceinfo_3;
                temp = PCI_Config_RW2(found_xhci_bus,found_xhci_device,found_xhci_function,0x10); 
                temp &= 0xfffffff0;
                xhci_mem_address_attr_doorb = temp;

	    }

        /* ehci */
	    else if((pci_class == 0x0c) && (pci_func == 0x03) && (pci_pprogif == 0x20)) {
			    found_ehci_bus = deviceinfo_1;
			    found_ehci_device = deviceinfo_2;
			    found_ehci_function = deviceinfo_3;
			    temp = PCI_Config_RW2(found_ehci_bus,found_ehci_device,found_ehci_function,16);
			    ehci_mem_address = (uint32_t)temp;
	    }

        /* ohci */
	    else if((pci_class == 0xc) && (pci_func == 0x3) && (pci_pprogif == 0x10)) {
			    found_ohci_bus = deviceinfo_1;
			    found_ohci_device = deviceinfo_2;
			    found_ohci_function = deviceinfo_3;
                temp = PCI_Config_RW2(found_ohci_bus,found_ohci_device,found_ohci_function,16);
                ohci_mem_address = (volatile uint32_t)temp;
	    }
        /* ethernet e1000 */
	    else if((pci_class == 2) && (pci_func == 0) && (pci_pprogif == 0)) {

			    found_ethernetc_bus = deviceinfo_1;
			    found_ethernetc_device = deviceinfo_2;
			    found_ethernetc_function = deviceinfo_3;

			    temp = PCI_Config_RW2(found_ethernetc_bus,found_ethernetc_device,found_ethernetc_function,16);
			    ethernetc_mem_address = (volatile uint32_t)temp;



			    temp = PCI_Config_RW2(found_ethernetc_bus,found_ethernetc_device,found_ethernetc_function,20);
			    if ((temp & 0x00000001) == 1)
				    ethernetc_flash_address = (volatile uint32_t)(temp - 1);
			    else
				    ethernetc_flash_address = (volatile uint32_t)temp;
			    PCI_Config_RW_ethernetc_io(found_ethernetc_bus,found_ethernetc_device,found_ethernetc_function,24);
			    temp = PCI_Config_RW2(found_ethernetc_bus,found_ethernetc_device,found_ethernetc_function,0x30);
			    if ((temp & 0x00000001) == 1)
				    print("Ethernet: Expansion rom address enabled!\n\n");
			    else
				    print("Ethernet: Expansion rom address not enabled.\n\n");
	    }

        /* ahci */
	    else if((pci_class == 0x1) && (pci_func == 0x6) && (pci_pprogif == 0x1)) {
			    found_ahci_bus = deviceinfo_1;
			    found_ahci_device = deviceinfo_2;
			    found_ahci_function = deviceinfo_3;
                temp = (volatile uint32_t)PCI_Config_RW2(found_ahci_bus,found_ahci_device,found_ahci_function,0x24);
                temp &= (volatile uint32_t)0xfffffff0;
                ahci_base_address = temp;

	    }

	    else
		        print("Error: could not find such a PCI device. \nMaybe some devices like USB will not work at this point.\n\n");

	    asm("cli");

}

/* Checks the class of a device */
uint16_t check_pci_type(uint32_t bus, uint32_t device, uint32_t funct)
{
	    uint16_t vendor;
	    uint16_t type;
	    if ((vendor = PCI_Config_RW(bus,device,0,0)) != 0xFFFF) 
		        type = ((PCI_Config_RW(bus,device,funct,11) >> 8) & 0x00ff);
	    else
		        type = 512;
	    return type;
}

/* Checks the progif of the device */
uint16_t check_pci_progif(uint32_t bus, uint32_t device, uint32_t funct)
{
	    uint16_t vendor;
	    uint16_t progif;

	    if ((vendor = PCI_Config_RW(bus,device,0,0)) != 0xFFFF)
		        progif = ((PCI_Config_RW(bus,device,funct,9) >> 8) & 0x00ff);
	    else
		        progif = 512;

	    return progif;
}	

/* Reads an eventual interrupt line of a device. This assumes that
 * the old PIC is present and usable on the computer.
 */
int32_t read_device_intline(uint32_t bus, uint32_t device, uint32_t funct)
{
	    uint16_t vendor;
	    uint16_t ebaddr;
	    if ((vendor = PCI_Config_RW(bus,device,0,0)) != 0xFFFF) 
		        ebaddr = (PCI_Config_RW(bus,device,funct,0x3c) & 0x00ff);
	    else
		        ebaddr = 512;
	    return (int)ebaddr;
}




