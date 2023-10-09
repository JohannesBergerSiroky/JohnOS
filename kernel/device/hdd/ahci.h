#ifndef AHCI_H
#define AHCI_H

#define AHCI_PORTS_REGS_OFFSET          0x100
#define AHCI_PORT_CMD                    0x18
#define AHCI_PORT_STSTS                  0x28
#define AHCI_PORT_STCTL                  0x2c
#define AHCI_PORT_REGS_LENGTH            0x80
#define AHCI_DEVICECONNECTION_EST           3
#define AHCI_DEVICECONNEST_AND_POWERED      1


volatile uint32_t found_ahci_bus;
volatile uint32_t found_ahci_device;
volatile uint32_t found_ahci_function;
volatile uint32_t ahci_base_address;


/* prints */
void print_ahci_mode_enable();
void print_ahci_mode_activated();
void init_some_values();
void print_ahci_num_ports();
void print_ports_implemented();
void print_ahci_port_multiplier_enabled();

uint8_t reset_ahci_hostcontroller();
void reset_ahci_port(uint32_t ahci_port);

volatile uint32_t ahci_cap_regs;
/* global host control */
volatile uint32_t ahci_ghc_regs; 
/* ports implemented */
volatile uint32_t ahci_pi_regs; 
volatile uint32_t ahci_ports_regs; 


/* reads */
uint8_t get_ahci_mode_enable();
uint8_t get_ahci_mode_activated();
uint32_t get_ahci_num_ports();
uint32_t get_ahci_ports_implemented();
uint32_t get_ahci_port_multiplier_enabled();
uint8_t check_ahci_attached_device_at_port(uint32_t ahci_port);
uint8_t check_ahci_device_powered_at_port(uint32_t ahci_port);


void perform_bios_handoff();
uint8_t check_os_owned_semaphore();


#endif
