#ifndef USB_HUBS_H
#define USB_HUBS_H


extern volatile uint32_t gps_flag;
void assign_ehci_port_functions();
void port_enum();
void GetUsbPortStatus();
void treat_connection_change(volatile uint32_t temp3, uint32_t a, 
                             uint32_t usb_port);
void treat_overcurrent(volatile unsigned temp3, uint32_t usb_port);
uint32_t check_port(uint32_t port, volatile uint32_t get_port);


/* xhci */
void write_port(uint32_t usb_port, uint32_t value);
void reset_port(uint32_t port_to_be_reset);
void Get_Port_Status();
uint32_t get_ehci_usbsts();
void write_ehci_usbsts();
void resetpc_ehci_usbsts();
void init_port_variables();

uint32_t port1;
uint32_t port2;
uint32_t port3;
uint32_t port4;
uint32_t port5;
uint32_t port6;


/* ehci */

uint32_t get_ehci_port1();
uint32_t get_ehci_port2();
uint32_t get_ehci_port3();
uint32_t get_ehci_port4();
uint32_t get_ehci_port5();
uint32_t get_ehci_port6();
uint32_t get_ehci_port7();
uint32_t get_ehci_port8();


/* ohci */
void Get_Ohci_Port_Status();
void reset_port1_csc();
void reset_port1_pesc();
void reset_port1_pssc();
void reset_port1_ocic();
void reset_port1_prsc();

void reset_port2_csc();
void reset_port2_pesc();
void reset_port2_pssc();
void reset_port2_ocic();
void reset_port2_prsc();


void reset_ohci_port1();
void reset_ohci_port2();
void setenable_ohci_port1();
void setenable_ohci_port2();


void uhci_port_reset();


void check_transfer10noprint_data_toggles();
uint32_t async_advance();
uint32_t usb_mass_storage_reset();


#endif
