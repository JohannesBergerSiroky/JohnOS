#ifndef USB_UHCI_H
#define USB_UHCI_H


#define UHCI_HC_START                                   (1 << 0)
#define UHCI_HC_RESET                                   (1 << 1)
#define UHCI_GLOBAL_RESET                               (1 << 2)
#define UHCI_TD_PIDSETUP                             (0x2d << 0)


#define UHCI_TD_PIDIN                                (0x69 << 0)
#define UHCI_TD_PIDOUT                               (0xE1 << 0)
#define UHCI_TD_DEVICE_ADDRESS(x)                       (x << 8)
#define UHCI_TD_EPNUM(x)                               (x << 15)


#define UHCI_TD_DATATOGGLE(x)                          (x << 19)
#define UHCI_TD_MAXLENGTH(x)                           (x << 21)
#define UHCI_TD_MAX_ERR_COUNT                          (3 << 27)


#define UHCI_TD_SET_ACTIVE                             (1 << 23)
#define UHCI_TD_FULLSPEED_DEVICE                       (0 << 26)
#define UHCI_TD_DISABLE_SHORT_PACKET_DETECT            (0 << 29)



volatile uint32_t found_uhci_bus;
volatile uint32_t found_uhci_device;
volatile uint32_t found_uhci_function;
volatile uint32_t uhci_mem_address;
void set_uhci_commandreg();


uint16_t read_uhci_word(volatile uint16_t base_addr, 
                        volatile uint16_t offset);
void write_uhci_word(const uint16_t base_addr, const uint16_t offset,
                     const uint16_t value);
uint16_t * read_word_addr(const uint16_t base, const uint16_t offset);
uint16_t read_word_addr2(const uint16_t base, const uint16_t offset);
uint16_t read_word2(const uint16_t base_addr, const uint16_t offset);
void write_word(const uint32_t base, const uint32_t offset, 
                const uint16_t value);


uint32_t read_dword_modify(uint32_t base_addr, uint32_t modification_value, 
                           uint32_t offset);
void write_word2(const uint16_t base_addr, const uint16_t offset, 
                  const uint16_t value);
uint32_t read_dword3(const uint16_t base_addr, const uint16_t offset);
uint8_t read_byte2(const uint16_t base_addr, const uint16_t offset);
void write_dword3(const uint16_t base_addr, const uint16_t offset, 
                  const uint32_t value);
void write_byte(const uint16_t base_addr, const uint16_t offset, 
                  const uint8_t value);


uint8_t read_bbyte(const uint32_t base_addr, const uint32_t offset);
void print_uhc_info(uint16_t addr);
void print_uhc_info2(uint32_t addr, uint32_t* addr2);
void print_uhc_info3(uint32_t* addr);

void read_uhci_usbcmd();
void read_uhci_usbsts();
void read_uhci_usbintr();
void read_uhci_frnum();
void read_uhci_frbaseaddr();
void read_uhci_sofmod();
void read_uhci_usbport1();
void read_uhci_usbport2();
void read_uhci_pcicommand();
void read_uhci_legsup();

uint16_t get_uhci_usbcmd();
uint16_t get_uhci_usbsts();
uint32_t get_uhci_usbintr();
uint16_t get_uhci_frnum();
uint32_t get_uhci_frbaseaddr();
uint16_t get_uhci_port1();
uint16_t get_uhci_port2();


void write_uhci_frbaseaddr(uint32_t fr_baseaddr);
void write_uhci_usbintr();
void write_uhci_frnum();
void write_uhci_maxpacket();

void halt_uhci();
void start_uhci();
void uhci_globalreset();
void uhci_hcreset();
void init_uhci_hc();

volatile uint32_t u_td_buffer1;
volatile uint32_t u_td_buffer2;
volatile uint32_t u_td_buffer3;
volatile uint32_t u_td_buffer4;
volatile uint32_t u_td_buffer5;
volatile uint32_t u_td_buffer6;
volatile uint32_t u_td_buffer7;
volatile uint32_t u_td_buffer8;

/* resume signal */
void resetpc_uhci_rs(); 
/* host system error */
void resetpc_uhci_hserr(); 
void resetpc_uhci_hcperr();
void uhci_usb_handler();


void init_uhci_descriptors();
void init_uhci_framepointers(uint32_t uhci_fr_addr);
void get_uhci_descriptor();

struct uhci_transfer_descriptor {


	    volatile uint32_t link_pointer;
	    volatile uint32_t td_ctrl_sts;
	    volatile uint32_t td_token;
        /* u stands for uhci. mem addr 31:0 */
	    volatile uint32_t u_td_buffer; 
        /* 128 bytes for software usage */
	    volatile uint32_t sw_dword1; 
	    volatile uint32_t sw_dword2; 
	    volatile uint32_t sw_dword3; 
	    volatile uint32_t sw_dword4;


} __attribute__((packed)); 





struct uhci_transfer_descriptor* td_null;
struct uhci_transfer_descriptor* td_1;
struct uhci_transfer_descriptor* td_2;
struct uhci_transfer_descriptor* td_3;
struct uhci_transfer_descriptor* td_4;
struct uhci_transfer_descriptor* td_5;
struct uhci_transfer_descriptor* td_6;
struct uhci_transfer_descriptor* td_7;
struct uhci_transfer_descriptor* td_8;


struct uhci_queue_head {


	    volatile uint32_t qh_head_next;
	    volatile uint32_t qh_emnt_next;
	    volatile uint32_t zero_1; 
	    volatile uint32_t zero_2;


} __attribute__((packed));


struct uhci_queue_head* uhci_qh_zero;
struct uhci_queue_head* uhci_ctrl_1;


#endif
