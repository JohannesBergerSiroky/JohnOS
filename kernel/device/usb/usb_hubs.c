#include "../../types.h"
#include "../../isr.h"
#include "../../irq.h"
#include "../kb.h"
#include "usb_ehci.h"
#include "../../graphics/screen.h"
#include "../../mem.h"
#include "usb_ohci.h"
#include "usb_uhci.h"
#include "usb_xhci.h"
#include "usb.h"
#include "usb_hubs.h"
#include "../../timer.h"


uint32_t get_xhci_max_ports()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000004);
        xbaddr5 >>= 24;
        return xbaddr5;
}

void print_xhci_max_ports()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000004);
        xbaddr5 >>= 24;
        print_ehc_info_2((uint32_t)xbaddr5);
}


void print_xhci_port_status_ctrl(uint32_t port_offset)
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)(0x00000400 + port_offset));
        xbaddr5 >>= 24;
        print_ehc_info_2((uint32_t)xbaddr5);
}

void print_xhci_port_power(uint32_t port_offset)
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)(0x00000400 + port_offset + 0x00000004));
        xbaddr5 >>= 24;
        print_ehc_info_2((uint32_t)xbaddr5);
}

void print_xhci_port_link(uint32_t port_offset)
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)(0x00000400 + port_offset + 0x00000008));
        xbaddr5 >>= 24;
        print_ehc_info_2((uint32_t)xbaddr5);
}

void print_xhci_port_hardwarelpm(uint32_t port_offset)
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)(0x00000400 + port_offset + 0x0000000c));
        xbaddr5 >>= 24;
        print_ehc_info_2((uint32_t)xbaddr5);
}

void store_xhci_maxports()
{
        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000004);
        xbaddr5 >>= 24;
        xhci_max_ports = xbaddr5;     
}

uint32_t get_xhci_port(uint32_t portx)
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)(0x00000400 + (portx*0x10)));
        return xbaddr5;
}


void disable_port(uint32_t port_to_disable)
{
        uint32_t port_value = get_xhci_port(port_to_disable-1);
        port_value |= (1 << 1); // clear port enabled
        port_value &= (~(1 << 4)); // clear port reset
        write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000400 + ((port_to_disable-1)*0x10), (const uint32_t)port_value);
}

void print_port(uint32_t port)
{
        uint32_t port_value = get_xhci_port(port-1);
        print("\nport ");
        printi((uint32_t)port);
        print(" has value:\n");
        dec_to_bin((uint32_t)port_value);
}

void write_xhci_port(uint32_t porty, uint32_t tempy)
{
        write_dword((const uint32_t)hci_operational_base, (const uint32_t)0x00000400 + (porty*0x10), (const uint32_t)tempy);
}

void write_port(uint32_t usb_port, uint32_t value)
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)0x00000400 + (usb_port*16));
        xbaddr5 |= value;
        write_dword((const uint32_t)hci_operational_base,(const uint32_t)(0x00000400 + (usb_port*16)), (const uint32_t)value);
}

void reset_port(uint32_t port_to_be_reset)
{
        write_port((port_to_be_reset-1), (1 << 4));
}

void write_port_power(uint32_t port_to_be_powered)
{
        uint32_t xbaddr5 = read_dword((const uint32_t)hci_operational_base,(const uint32_t)(0x00000400 + ((port_to_be_powered-1)*16)));
        if(!(xbaddr5 & (1 << 9))) {

                xbaddr5 |= (1 << 9);
                write_dword((const uint32_t)hci_operational_base,(const uint32_t)(0x00000400 + ((port_to_be_powered-1)*16)), (const uint32_t)xbaddr5);
        }
}


/* first the kernel needs to do port enumeration and check if a device is connected. 
 * this can be done via msi interrupts but that will be done later on. after a device 
 * is connected and a port is enabled, either via a usb2 port or via a
 * usb3 port, we can go on with assigning a device context for our newly connected usb device. 
 * a mechanism can be inplemented to check whether we are using a usb2 connector or a usb 3 connector. 
 */


/* USB topology: default value is 1 */
void xhci_portx_enumeration(uint32_t x_usb_topology)
{

        /* port power - device connected - disabled (if usb 3) -> training - enabled - u0 */

        /* elseif usb2 -> disabled - do port reset - enabled - u0 or maybe u2 */


        uint32_t i = 0;
        uint32_t temp = 0;

        print("\nchecking port power");

        uint32_t xbaddr5 = read_dword((const uint32_t)xhci_mem_address_attr_doorb,(const uint32_t)0x00000010);
        if(xbaddr5 & (1 << 3)) {
                print("\nports have their own port power switches. can apply port power,\nthe ppflag reports correctly");
        }


        write_port_power(1);
        kernel_delay_100(50);
        write_port_power(2);
        kernel_delay_100(50);
        write_port_power(3);
        kernel_delay_100(50);
        write_port_power(4);
        kernel_delay_100(50);
        print_port(1);
        print_port(2);
        print_port(3);
        print_port(4);

        for(; i < xhci_max_ports; i++) {

                if ((temp & 0x00000001)) {

                        print("\n Port ");
                        printi((uint32_t)i+1);
                        print(" has a device connected to it");
                        temp = get_xhci_port(i);
                        if((temp & (1 << 17))) {

                                print("\n Port ");
                                printi((uint32_t)i+1);
                                print("has connect status change still on. clearing..");
                                temp |= (1 << 17);
                                write_xhci_port(i, temp);
                                kernel_delay_100(5);

                                temp = get_xhci_port(i);
                                kernel_delay_100(50);
                            }

                        temp = get_xhci_port(i);
                        if((temp & (1 << 1))) {

                                print("\nPort ");
                                printi((uint32_t)i+1);
                                print(" is enabled");
                                /* typically what happens when a usb 3 port is used */
                                xport_speed = (temp >> 10) & 0x0000000f;
                                temp >>= 5;
                                temp &= 0x0000000f;
                                print("\nValue from pls is (should be 0): ");
                                printi((uint32_t)temp);
                                xport_num = i+1;
                                xroute_string = x_usb_topology*xport_num;

                            }

                        else {
                                /* probably a usb 2 port. if so then a port reset must be done */
                                print("Port ");
                                printi((uint32_t)i+1);
                                print(" is disabled. This might be a USB2 port.");
                        }

                }

                else {

                        print("\n Port ");
                        printi((uint32_t)i+1);
                        print(" has no device connected to it");
                }

        }

}



void init_port_variables()
{
	    port1 = get_ehci_port1();
	    port2 = get_ehci_port2();
	    port3 = get_ehci_port3();
	    port4 = get_ehci_port4();
	    port5 = get_ehci_port5();
	    port6 = get_ehci_port6();
	
}

               
/* the functions below should not be there because how many ports there are depends on the host controller */
uint32_t get_ehci_port1()
{
        const uint32_t get_port_status = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000064);
        return get_port_status;
}
uint32_t get_ehci_port2()
{
        const uint32_t get_port_status = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000068);
        return get_port_status;
}
uint32_t get_ehci_port3()
{
        const uint32_t get_port_status = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x0000006c);
        return get_port_status;
}
uint32_t get_ehci_port4()
{
        const uint32_t get_port_status = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000070);
        return get_port_status;
}
uint32_t get_ehci_port5()
{
        const uint32_t get_port_status = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000074);
        return get_port_status;
}
uint32_t get_ehci_port6()
{
        const uint32_t get_port_status = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000078);
        return get_port_status;
}
uint32_t get_ehci_port7()
{
        const uint32_t get_port_status = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x0000007c);
        return get_port_status;
}
uint32_t get_ehci_port8()
{
        const uint32_t get_port_status = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000080);
        return get_port_status;
}


uint32_t get_ehci_usbsts()
{
        uint32_t get_status = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)0x00000024);
        return get_status;
}




void GetUsbPortStatus()
{
        uint32_t usb_status = get_ehci_usbsts(); // read from the usb status register
        if ((usb_status & (1 << 2))) {
                Get_Port_Status();
        }
}
void port_enum()
{
        print("hi");
}

void Get_Port_Status()
{

       /* this is probably done correctly now. however it can be conflict if for example a program is writing something to a port, and then this interrupt routine is called. ports can be corrupted.
        so whenever a port is written to, by a program, then this interrupt routine should not be called but instead be put on a queue.

        however this queue could also be corrupted. maybe a message function to the queue container could let the queue "know" how a port has been changed. */


        if(ehci_port_busy == 1) {
                ehci_int_unhandled = 1; // make an array with a queue instead
                return;
        }

        uint32_t temp = 0;
        uint32_t temp3 = 0;
        uint32_t check_ports_flag = 0;
        uint32_t usb_port = 0x00000060;


        temp = ehci_port_num; // check this one
        for(uint32_t i = 0; (i < temp) && (check_ports_flag = 0); i++) {
                usb_port += 4;
                temp3 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port);
                if (temp3 & (1 << 4))
                        check_ports_flag = 1; 
        }

        /* overcurrent */
        if (temp3 & (1 << 4)) {

                timer_install();
                asm("sti");

                print("over current on port ");
                printi((usb_port - 60) / 4);
                print("!\n\n");
                if (temp3 & (1 << 5)) {

                        temp3 |= (1 << 5);
                        /* clear port overcurrent change */
                        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port, (const uint32_t)temp3);
                }

                temp3 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port);
                /* if change port enable = 1 */
                if((temp3 & (1 << 3)))  {
                        temp3 |= (1 << 3);
                        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port, (const uint32_t)temp3);
                }

                temp3 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port);
                /* if overcurrent active and port enabled */
                if((((temp3 & (1 << 4)))) && ((temp3 & (1 << 2))))  {
                        /* enable port suspend */
                        temp3 |= (1 << 7); 
                        /* suspend */
                        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port, (const uint32_t)temp3);
                }

                print("\nPlease wait a little..");
                timer_ticks = 0;
                while(timer_ticks < 4);


                temp3 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port);

                /* if overcurrent stopped */
                if ((temp3 & (1 << 4)) == 0) {
                        /* check if over-current changed */
                        if ((temp3 & (1 << 5)))  {
                                temp3 |= (1 << 5);
                                /* clear overcurrent change */
                                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port, (const uint32_t)temp3); 
                                /* force resume */
                                temp3 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port);
                                /* if port reports itself suspended and is not resumed, is enabled and overcurrent stopped */
                                if(((temp3 & (1 << 7))) && (((temp3 & (1 << 6)) == 0)) && (((temp3 & (1 << 2)))) && (((temp3 & (1 << 4)) == 0)))  {
                                        /* force port resume */
                                        temp3 |= (1 << 6); 
                                        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port, (const uint32_t)temp3);
                                        timer_ticks = 0;
                                        while(timer_ticks < 2);
                                }
                        }
                }

                temp3 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port);
                /* if overcurrent stopped */
                if (((((temp3 & (1 << 4)) == 0))) && ((temp3 & (1 << 5)))) {
                        gps_flag = 1;
                        temp3 |= (1 << 5);
                        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port, (const uint32_t)temp3); // clear port overcurrent change
                }
                asm("cli");
                irq_uninstall_handler(0);
        }


       temp3 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port);


        if (temp3 & (1 << 1)) {

                if(temp3 & 0x00000001) {
                        print("\nDevice connected to port ");
                        printi((usb_port - 60) / 4);
                        print("\n\n");

                }
                else if ((temp3 & 0x00000001) == 0) {
                        print("Device removed from port ");
                        printi((usb_port - 60) / 4);
                        print("\n\n");

                }
        }
        temp3 = (uint32_t)read_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port);
        if(temp3 & 0x00000002) {								
                temp3 |= 0x2;
                write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port, (const uint32_t)temp3); // clear port connect change
        }


        set_kbvariables_zero();

}


void Get_Ohci_Port_Status()
{

        uint32_t temp1 = get_ohci_port1();

        /* port connect change */
        /* csc = connect status change */
        if((((temp1 >> 16) & 0x00000001) == 1)) 
                reset_port1_csc();

        /* port enabled change */
        /* pesc = port enable status change */
        if((((temp1 >> 17) & 0x00000001) == 1)) 
                reset_port1_pesc();

        /* port suspend status change */
        /* pssc = port suspend status change */
        if((((temp1 >> 18) & 0x00000001) == 1)) 
                reset_port1_pssc();

        /* port over current indicator change */
        /* ocic = overcurrent indicator change */
        if((((temp1 >> 19) & 0x00000001) == 1)) 
                reset_port1_ocic();

        /* port reset status change */
        /* prsc = port reset status change */
        if((((temp1 >> 20) & 0x00000001) == 1)) 
                reset_port1_prsc();

        temp1 = get_ohci_port2();
        if((((temp1 >> 16) & 0x00000001) == 1))
                reset_port2_csc();

        if((((temp1 >> 17) & 0x00000001) == 1))
                reset_port2_pesc();

        if((((temp1 >> 18) & 0x00000001) == 1))
                reset_port2_pssc();

        if((((temp1 >> 19) & 0x00000001) == 1))
                reset_port2_ocic();

        if((((temp1 >> 20) & 0x00000001) == 1))
                reset_port2_prsc();

}

/* clear connect status change */
void reset_port1_csc() 
{
        uint32_t temp2 = get_ohci_port1();
        temp2 |= (1 << 16);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000054, (const uint32_t)temp2);
}
/* port enable status change */
void reset_port1_pesc() 
{
        uint32_t temp2 = get_ohci_port1();
        temp2 |= (1 << 17);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000054, (const uint32_t)temp2);
}
/* port suspend status change */
void reset_port1_pssc() 
{
        uint32_t temp2 = get_ohci_port1();
        temp2 |= (1 << 18);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000054, (const uint32_t)temp2);
}
/* over current indicator change */
void reset_port1_ocic() 
{
        uint32_t temp2 = get_ohci_port1();
        temp2 |= (1 << 19);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000054, (const uint32_t)temp2);
}
/* port reset status change */
void reset_port1_prsc() 
{
        uint32_t temp2 = get_ohci_port1();
        temp2 |= (1 << 20);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000054, (const uint32_t)temp2);
}
/* clear connect status change */
void reset_port2_csc() 
{
        uint32_t temp2 = get_ohci_port2();
        temp2 |= (1 << 16);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000058, (const uint32_t)temp2);
}
/* port enable status change */
void reset_port2_pesc() 
{
        uint32_t temp2 = get_ohci_port2();
        temp2 |= (1 << 17);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000058, (const uint32_t)temp2);
}
/* port suspend status change */
void reset_port2_pssc() 
{
        uint32_t temp2 = get_ohci_port2();
        temp2 |= (1 << 18);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000058, (const uint32_t)temp2);
}
/* over current indicator change */
void reset_port2_ocic() 
{
        uint32_t temp2 = get_ohci_port2();
        temp2 |= (1 << 19);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000058, (const uint32_t)temp2);
}
/* port reset status change */
void reset_port2_prsc() 
{
        uint32_t temp2 = get_ohci_port2();
        temp2 |= (1 << 20);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000058, (const uint32_t)temp2);
}


void reset_ohci_port1()
{
        ohci_ports_busy = 1;
        uint32_t temp2 = get_ohci_port1();
        temp2 |= (1 << 4);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000054, (const uint32_t)temp2);
        ohci_ports_busy = 0;
}

void reset_ohci_port2()
{
        ohci_ports_busy = 1;
        uint32_t temp2 = get_ohci_port2();
        temp2 |= (1 << 4);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000058, (const uint32_t)temp2);
        ohci_ports_busy = 0;
}



void setenable_ohci_port1()
{
        uint32_t temp = get_ohci_port1();
        temp |= (1 << 1);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000054, (const uint32_t)temp);
        ohci_ports_busy = 0;
}

void setenable_ohci_port2()
{
        uint32_t temp = get_ohci_port2();
        temp |= (1 << 1);
        write_dword((const uint32_t)ohci_mem_address, (const uint32_t)0x00000058, (const uint32_t)temp);
}


void uhci_port_reset()
{

        print("\n\nport1 after hc reset:");
        read_uhci_usbport1();

        uint16_t temp1 = get_uhci_port1();
        if ((temp1 & (1 << 1))) {
                temp1 |= (1 << 1);
                write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0010, (const uint16_t)temp1);
        }
        if ((temp1 & (1 << 3))) {
                temp1 |= (1 << 3);
                write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0010, (const uint16_t)temp1);
        }
        print("\n\nport1 after clearing the bits:");
        read_uhci_usbport1();

        /* reset port 1 */
        temp1 = get_uhci_port1();
        if ((temp1 & (1 << 9)) != (1 << 9)) {
                temp1 |= (1 << 9);
                timer_phase(1000);
                asm("sti");
                timer_ticks = 0;
                while(timer_ticks < 58);
                asm("cli");
                print("\n1");
                temp1 = get_uhci_port1();
                /* reset */
                if ((temp1 & (1 << 9))) {
                        temp1 &= (~(1 << 9));
                        write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0010, (const uint16_t)temp1);

                }

                temp1 = get_uhci_port1();
                print("\n3");
                /* port enable change */
                if((temp1 & (1 << 3)))  {
                        temp1 |=(1 << 3);
                        write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0010, (const uint16_t)temp1);
                }
                /* port enable. in this case disabled */
                if((temp1 & (1 << 2)) == 0)  {
                        temp1 |= (1 << 2);
                        write_word2((const uint16_t)uhci_mem_address, (const uint16_t)0x0010, (const uint16_t)temp1);

                }
                print("\n6 finished");

                asm("sti");
                timer_ticks = 0;
                while(timer_ticks < 24);
                asm("cli");
                timer_phase(18);
                temp1 = get_uhci_port1();
                /* port not enabled */ 
                if((temp1 & (1 << 2)) != (1 << 2)) {
                        print("\n\nError: port1 not enabled after reset. Resetting hc.");
                        uhci_hcreset();
                }

        }
        print("\nuhci port1 after trying to reset and enable:");
        read_uhci_usbport1();

}



void treat_connection_change(volatile uint32_t temp3, uint32_t a, uint32_t usb_port)
{
        if (((temp3 & (1 << 1)))) {
                if((temp3 & 0x00000001)) {
                        print("\nDevice Connected to port ");
                        printi(a);

                }
                if((temp3 & 0x00000001) == 0) {
                        print("\nDevice removed from port ");
                        printi(a);

                }
                if(((temp3 & (1 << 1)))) {								
                        temp3 |= (1 << 1);
                        /* clear port connect change */
                        write_dword((const uint32_t)ehci_mem_address, (const uint32_t)usb_port, (const uint32_t)temp3); 
                }
        }
}
