#ifndef USING_ETHERNET_H
#define USING_ETHERNET_H


/* These definitions are used in if-statements and before read and write operations */

#define ETH_CTL_START_LINK(x) (x |= (1 << 6))

#define ETH_8254_CHECK_MDICTL_ERROR_CLEARED(x)          ((x & (1 << 30)) == 0)
#define ETH_8254_CHECK_MDICTL_READYBIT_SET(x)                  (x & (1 << 28))
#define ETH_8254_MDICTL_READYBIT_CLEAR(x)                  (x &= (~(1 << 28)))
#define ETH_8254_MDICTL_ERRORBIT_CLEAR(x)                  (x &= (~(1 << 30)))
#define ETH_8254_MDICTL_START_READ_PHYADDR(x)   (x |= ((1 << 21) | (2 << 26)))


#define ETH_8254_EEPROM_READ_DONE_ANDMASK(x)                    (x & (1 << 4))
#define ETH_8254_EEPROM_READ_DONE(x)             (x = ((x >> 4) & 0x00000001)) 
#define ETH_8254_EEPROM_GET_DATA(x)             (x = ((x >> 16) & 0x0000FFFF))
#define ETH_8254_EEPROM_READ_START(x, y)          (x |= ((y << 8) | (1 << 0)))



volatile uint32_t found_ethernetc_bus;
volatile uint32_t found_ethernetc_device;
volatile uint32_t found_ethernetc_function;
volatile uint32_t ethernetc_io_address;
volatile uint32_t ethernetc_mem_address;
volatile uint32_t ethernetc_flash_address;


uint32_t e1000_phy_location;
uint32_t e1000_eeprom_read_addr;

/* the mac address as 6 bytes */
uint8_t mac[6];

uint32_t phy_reg_flag;
uint32_t phy;

uint32_t current_rx;
uint32_t handled_rx;
uint32_t current_tx;
uint32_t handled_tx;

uint32_t netmem;

struct e1000_rx_descriptor {


	    volatile uint32_t addr_low;
	    volatile uint32_t addr_high;
	    volatile uint16_t length;
	    volatile uint16_t checksum;
	    volatile uint8_t status;
	    volatile uint8_t errors;
	    volatile uint16_t special;


} __attribute__((packed));

struct e1000_rx_descriptor* rx[32];


struct e1000_tx_descriptor {


	    volatile uint32_t addr_low;
	    volatile uint32_t addr_high;
	    volatile uint16_t length;
	    volatile uint8_t cso; 
	    volatile uint8_t cmd;
	    volatile uint8_t status;
	    volatile uint8_t css;
	    volatile uint16_t special;


} __attribute__((packed));

struct e1000_tx_descriptor* tx[8];




/* Reads and prints. */

void read_ethernetc_deviceinfo();
void read_ethernetc_vendorinfo();
void read_ethernetc_cmd();
void read_ethernetc_sts();
void read_ethernetc_eeprom();
void read_ethernetc_eeprom_read();
void read_ethernetc_cmdext();
void read_ethernetc_fcal();
void read_ethernetc_fcah();
void read_ethernetc_rcfgw();
void read_ethernetc_tcfgw();
void read_ethernetc_rctrl();
void read_ethernetc_tctrl();
void read_ethernetc_tipg();

void read_ethernetc_rdbl();
void read_ethernetc_rdbh();
void read_ethernetc_rdl();
void read_ethernetc_rdh();
void read_ethernetc_rdt();
void read_ethernetc_tdbl();
void read_ethernetc_tdbh();
void read_ethernetc_tdl();
void read_ethernetc_tdh();
void read_ethernetc_tdt();
void read_ethernetc_tdc();

void read_ethernetc_ism();
void read_ethernetc_icm();
/* interrupt cause set */
void read_ethernetc_ics(); 
/* interrupt clear */
void read_ethernetc_ic(); 
/* should be at spot 0, 1 and 2 in the eeprom */
void read_mac_address(); 


uint32_t get_ethernetc_int();
uint32_t get_ethernetc_intmask();
uint32_t get_ethernetc_intclear();
uint32_t get_ethernetc_intmaskclear();
uint32_t get_ethernetc_mdictl();
uint32_t get_ethernetc_cmd();
uint32_t get_ethernetc_sts();
uint32_t get_ethernetc_eeprom();
uint32_t get_ethernetc_eeprom_read();
uint32_t get_ethernetc_cmdext();
/* flow control address low. value should be c28001h */
uint32_t get_ethernetc_fcal(); 
/* flow control address high. value should be 0100h */
uint32_t get_ethernetc_fcah(); 
uint32_t get_ethernetc_rdbl();
uint32_t get_ethernetc_rdbh();
uint32_t get_ethernetc_rdl();
uint32_t get_ethernetc_rdh();
uint32_t get_ethernetc_rdt();
uint32_t get_ethernetc_rdc();

uint32_t get_ethernetc_tdbl();
uint32_t get_ethernetc_tdbh();
uint32_t get_ethernetc_tdl();
uint32_t get_ethernetc_tdh();
uint32_t get_ethernetc_tdt();
uint32_t get_ethernetc_tdc();
uint32_t get_eeprom_read_data(uint32_t eeprom_address);


/* writes */
void set_ethernetc_commandreg();
void write_ethernetc_eepromcmd();
void write_ethernetc_int();
void write_ethernetc_intmask();
void write_ethernetc_intmaskclear();
void write_mdac_interruptenable();
uint32_t read_mdictl_phyreg_data(uint32_t phy_reg);
void allow_read_eepromcmd();
void disallow_read_eepromcmd();
void start_eeprom_read();
void set_eeprom_read(uint32_t address);
void toggle_eeprom();
void write_mta();
void start_link();


/* interrupt handling */
void ethernetc_e1000_handler();
void writeclear_ethernetc_mdac();
void writeclear_ethernetc_goodthreshold();
void writeclear_ethernetc_packetreceived();
void writeclear_ethernetc_linkstatuschange();
void writeclear_ethernetc_rse();
void writeclear_ethernetc_rfifooverrun();
void writeclear_ethernetc_rcorderedset();
void writeclear_ethernetc_phyinterrupt();
void writeclear_ethernetc_gpint();
void writeclear_ethernetc_txgoodthreshold();
void writeclear_ethernetc_rshortpcktdetect();


/* inits */

void init_mac_and_variables_netmem();
void init_phy_location();
void rd_init();
void td_init();
void init_ethernetc_controller();
void send_packet(uint32_t tx_addr_low, uint16_t len);


#endif
