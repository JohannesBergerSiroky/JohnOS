#ifndef USB_EHCI_H
#define USB_EHCI_H


#define EHCI_QH_TYPE_QH                                          (1 << 1)
#define EHCI_QH_DEVICE_ADDRESS(x)                                (x << 0)
#define EHCI_QH_EP_NUM(x)                                        (x << 8)
#define EHCI_QH_HIGH_SPEED                                      (2 << 12)
#define EHCI_QH_USE_QTD_TOGGLE                                  (1 << 14)
#define EHCI_QH_MAX_PACKET_SIZE(x)                              (x << 16)
#define EHCI_QH_MAX_NAK_COUNTER                                (15 << 28)


#define EHCI_QTD_SET_ACTIVE                                      (1 << 7)
#define EHCI_QTD_SET_INACTIVE(x)    x &= (~((volatile uint32_t)(1 << 7)))
#define EHCI_QTD_SET_OUT_TOKEN                                   (0 << 8)
#define EHCI_QTD_SET_IN_TOKEN                                    (1 << 8)
#define EHCI_QTD_SET_SETUP_TOKEN                                 (2 << 8)
#define EHCI_QTD_ERRORS_COUNT(x)                                (x << 10)
#define EHCI_QTD_CURRENT_PAGE(x)                                (x << 12)
#define EHCI_QTD_SET_INTC                                       (1 << 15)
#define EHCI_QTD_BYTES_TTRANSFER(x)                             (x << 16)
#define EHCI_QTD_DATA_TOGGLE(x)                                 (x << 31)


volatile uint32_t found_ehci_bus;
volatile uint32_t found_ehci_device;
volatile uint32_t found_ehci_function;
volatile uint32_t async_int_success;
uint32_t ehci_mem_address;
uint32_t operational_offset;
uint8_t ehci_64bit_e;


uint32_t ehci_port_array[2];

void resetpc_ehci_usbstscmplt();
void resetpc_ehci_usbstserr();
void resetpc_ehci_usbstsasyncadvance();
void resetpc_ehci_usbstshosterr();
void ehci_usb_handler();



struct queue_head {
        struct queue_head * Horizontalpointer;
        volatile uint32_t endpointch1;
        volatile uint32_t endpointch2;
        struct queue_transfer_descriptor* qhcurrent_qtd_ptr;
        struct queue_transfer_descriptor * qhnext_qtd_ptr;
        struct queue_transfer_descriptor * qhalternate_qtd_ptr;
        volatile uint32_t qhtransfer_info;
        volatile uint8_t* qhbuffer_ptr0;
        volatile uint8_t* qhbuffer_ptr1;
        volatile uint8_t* qhbuffer_ptr2;
        volatile uint8_t* qhbuffer_ptr3;
        volatile uint8_t* qhbuffer_ptr4;
        volatile uint32_t qh_extended_buffers[5];
        struct queue_head* p_back;
        struct queue_transfer_descriptor* qtd_middle;
        volatile uint32_t amount_of_qts;
} __attribute__((packed));


struct queue_transfer_descriptor {
        struct queue_transfer_descriptor * next_qtd_ptr;
        struct queue_transfer_descriptor * alternate_qtd_ptr;
        volatile uint32_t transfer_info;
        volatile uint8_t* buffer_ptr0;
        volatile uint8_t* buffer_ptr1;
        volatile uint8_t* buffer_ptr2;
        volatile uint8_t* buffer_ptr3;
        volatile uint8_t* buffer_ptr4;
        volatile uint32_t qt_extended_buffers[5];
        uint32_t zero;
        struct queue_transfer_descriptor* qtd_back;
} __attribute__((packed));


struct queue_head* QHhead19;
/* default control pipe */
struct queue_head* QHhead20; 
/* in. from the device */
struct queue_head* QHhead21; 
/* out. to the device */
struct queue_head* QHhead22; 


struct queue_transfer_descriptor* qtd29;
struct queue_transfer_descriptor* qtd30;
struct queue_transfer_descriptor* qtd31;
struct queue_transfer_descriptor* qtd32;
struct queue_transfer_descriptor* qtd33;
struct queue_transfer_descriptor* qtd34;

void init_queue_heads();
void init_ehci_stuff();
void set_ehci_operationaloffset();
void print_ehci_operationaloffset();
void print_ehci_64bit_enabled();
uint8_t read_ehci_64bit_enabled();


void print_ehc_info(uint32_t addr, uint32_t* addr2);
void print_ehc_info_2(uint32_t addr);
void write_ehci_usbcmd2();


uint8_t init_transfer();
void start_ehci();
uint32_t check_port();
uint8_t init_ehci(struct queue_head * head);


void run_schedule();

void stop_ehci();
uint8_t reset_ehci();
void read_ehci_hcparams();
void read_ehci_hccparams();



uint32_t get_ehci_extracap();
void print_ehci_extracap();
void print_ehci_extracap_addr();
void bios_hand_off();


void print_ehci_commandreg();
void set_ehci_commandreg();
void disable_ehci_legacy();
void print_ehci_legacy();

uint32_t get_reclamation_bit();
void enable_async();
void disable_async();
void write_async_addr(struct queue_head* q);

volatile uint32_t ehci_data_toggle1;
volatile uint32_t ehci_data_toggle2;


void set_ehci_port_n();
void read_ehci_port1();
void read_ehci_port2();
void read_ehci_port3();
void read_ehci_port4();
void read_ehci_port5();
void read_ehci_port6();
void read_ehci_port7();
void read_ehci_port8();


void read_ehci_configflag();
void read_ehci_asynclistaddr();
void read_ehci_periodiclistbase();
void read_ehci_ctrldssegment();

void read_ehci_baddr2();
void read_ehci_frindex();
void read_ehci_usbint();
uint32_t get_ehci_usbcmd();
uint32_t read_ehci_usbsts();
void read_ehci_usbcmd();


/* writes */
void write_ehci_periodicbase();
void write_ehci_usbcmd();
void write_ehci_configflag();
void write_ehci_usbint();
void write_ehci_ctrldssegment();
void reset_ehci_2();
void fill_ehci_periodiclistbase();
void write_ehci_async_interrupt();

void print_ehci_usbsts();

void set_port_busy();
void set_ehci_int_unhandled();
void read_ehci_class(uint32_t bus, uint32_t device, uint32_t funct);
void read_ehci_progif(uint32_t bus, uint32_t device, uint32_t funct);
void read_ehci_subclass(uint32_t bus, uint32_t device, uint32_t funct);
void read_ehci_intpin(uint32_t bus, uint32_t device, uint32_t funct);



uint32_t reset_address();
uint32_t* port_reset();

void usb_ehci_get_descriptor(volatile uint8_t descriptor_type, volatile uint8_t usb_length_low);

uint32_t ehci_port_num;
uint32_t ehci_port_busy;
uint32_t ehci_int_unhandled;
uint32_t check_ports_flag;



#endif
