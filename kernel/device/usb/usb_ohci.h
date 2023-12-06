#ifndef USB_OHCI_H
#define USB_OHCI_H


#define OHCI_BULK_LIST_ENABLE(x)                          (x |= (1 << 5))
#define CHECK_OHCI_BULK_LIST_ENABLED(x)                    (x & (1 << 5))
#define CHECK_OHCI_HC_RESUME_STATE(x)             (((x >> 6) & 0x3) == 1)
#define CHECK_OHCI_HC_OPERATIONAL_STATE(x)        (((x >> 6) & 0x3) == 2)
#define CHECK_OHCI_HC_SUSPENDED_STATE(x)          (((x >> 6) & 0x3) == 3)
#define CHECK_OHCI_HC_RESET_STATE(x)              (((x >> 6) & 0x3) == 0)
#define OHCI_HARD_RESET(x)                             (x &= (~(1 << 0)))
#define OHCI_FILL_BULK_LIST(x)                            (x |= (1 << 2))
#define OHCI_OCR(x)                                       (x |= (1 << 3))
#define CHECK_OHCI_OC(x)                  (((x >> 30) & 0x00000001) == 1)
#define OHCI_WRITE_IE_FIELD(x) ((x = 0x00000000 | (1 << 30) | (1 << 6) | \
                                                               (1 << 31))


#define OHCI_ED_SETUP_EDINFO 0 | ((8 << 16) | (0 << 15) | (0 << 14) | \
                                 (0 << 13) | (0 << 11) | (0 << 7) | 0)

#define OHCI_TD_SETUP_INFO1 (0 | ((7 << 29) | (2 << 24) | (7 << 21) | \
                                               (0 << 19) | (0 << 18)))
#define OHCI_TD_SETUP_INFO2 (0 | ((7 << 29) | (3 << 24) | (0 << 21) | \
                                              (1 << 19) | (0 << 18)))
#define OHCI_TD_SETUP_INFO3 (0 | ((7 << 29) | (3 << 24) | (0 << 21) | \
                                              (2 << 19) | (0 << 18)))
#define OHCI_TD_SETUP_INFO4 (0 | ((7 << 29) | (2 << 24) | (0 << 21) | \
                                              (2 << 19) | (0 << 18)))


#define OHCI_ED_FUNCTION_ADDRESS(x)                             (x << 0)
#define OHCI_ED_ENDPOINT_NUMBER(x)                              (x << 7)
#define OHCI_ED_DIR_FROM_TD                                     (0 << 11)
#define OHCI_ED_DIR_OUT                                         (1 << 11)
#define OHCI_ED_DIR_IN                                          (2 << 11)

#define OHCI_ED_FULL_SPEED                                      (0 << 13)
#define OHCI_ED_NOSKIP                                          (0 << 14)
#define OHCI_ED_FORMAT_BLKCTLIR                                 (0 << 15)
#define OHCI_ED_MAX_PACKET_SIZE(x)                              (x << 16)

#define OHCI_GTD_BUF_ROUNDING                                   (0 << 18)
#define OHCI_GTD_TYPE_SETUP                                     (0 << 19)
#define OHCI_GTD_TYPE_OUT                                       (1 << 18)
#define OHCI_GTD_TYPE_IN                                        (2 << 18)
#define OHCI_GTD_NODELAYIR                                      (7 << 21)
#define OHCI_GTD_DATA_TOGGLE0                                   (2 << 24)
#define OHCI_GTD_DATA_TOGGLE1                                   (3 << 24)
#define OHCI_GTD_MAX_ERR_COUNT                                  (3 << 26)


volatile uint32_t found_ohci_bus;
volatile uint32_t found_ohci_device;
volatile uint32_t found_ohci_function;
volatile uint32_t ohci_ports_busy;

uint32_t ohci_hcca;
uint8_t* done_queue_location;
uint32_t done_queue_copy;
uint32_t done_queue_index;
uint32_t frbaseaddr;

volatile uint32_t ohci_mem_address;

void read_ohci_hcrevision();
void read_ohci_hccontrol();
void read_ohci_hccmd();
void read_ohci_hcintsts();
void read_ohci_hcintenable();
void read_ohci_hcintdisable();
void read_ohci_hchcca();
void read_ohci_hccped();
void read_ohci_hcched();
void read_ohci_hcrhda();
void read_ohci_port1();
void read_ohci_port2();
void read_ohci_port3();

volatile uint8_t* td1_buffer;
volatile uint8_t* td2_buffer;
volatile uint8_t* td3_buffer;
volatile uint8_t* td4_buffer;
volatile uint8_t* td5_buffer;
volatile uint8_t* td8_buffer;




void set_ohci_commandreg();
void write_cb_ratio(uint32_t ratio);
uint32_t get_hccontrol();
uint32_t get_hccmd();
void write_ownership_bit();
uint8_t check_interruptrouting();
uint8_t check_hcfs_usbreset();
void read_hcca_limits();
void read_ctrl_bulk_ratio();
uint32_t get_ctrl_bulk_ratio();
uint32_t get_hcfminterval();
uint32_t get_hchcca();
void configure_start_hc();
void start_hc(const uint32_t interval_value, const uint32_t cb_ratio);
void reset_hc();
uint32_t check_ownership_hccmd();
void write_usb_operational();
void write_usb_suspend();
uint32_t get_hcca_limits();
void enable_ohci_interrupts_0();
void disable_ohci_interrupts();
void disable_ohci_interrupts_reverseset();
void write_bulkheaded_addr();
void write_controlheaded_addr();
void disable_int_eds();
void disable_iso_eds();
void disable_ctrl_eds();
void disable_bulk_eds();
void enable_bulk_eds();
void enable_ctrl_eds();
void write_hcperiodicstart();
void set_ohci_interrupts();
void set_ohci_interrupts2();
uint32_t get_ohci_intstatus();
void write_hcca();
void fill_hcca_int_iso();
void copy_done_queue();
void print_done_queue(uint32_t tds, uint32_t buffer_count);
void print_done_queue2();
void stop_ohci();
void write_fm_interval();
void toggle_fm_interval();

uint32_t get_ohci_port1();
uint32_t get_ohci_port2();
uint32_t get_ohci_port3();

void resetpc_ohci_so();
void resetpc_ohci_wdh();
void resetpc_ohci_sf();
void resetpc_ohci_rd();
void resetpc_ohci_ue();
void resetpc_ohci_fno();
void resetpc_ohci_rhsc();
void resetpc_ohci_oc();

void ohci_usb_handler();

void init_ohci_descriptors();
uint32_t ohci_get_descriptor2();
void write_bulklistfilled();
void write_ctrllistfilled();
void print_done_queue_td();


struct general_transfer_descriptor {


        volatile uint32_t descriptor_info;
        volatile uint8_t* cbp;
        volatile uint32_t next_td;
        volatile uint32_t buffer_end;


} __attribute__((packed));

struct isochronous_transfer_descriptor {


        volatile uint32_t descriptor_info;
        volatile uint8_t* buffer_page0;
        struct isochronous_transfer_descriptor* next_td;
        volatile uint8_t* buffer_end;
        volatile uint32_t psw0_1;
        volatile uint32_t psw2_3;
        volatile uint32_t psw4_5;
        volatile uint32_t psw6_7;


} __attribute__((packed));

struct endpoint_descriptor {


        volatile uint32_t endpoint_info;
        volatile uint32_t tail_pointer;
        volatile uint32_t head_pointer;
        volatile uint32_t next_ed;


} __attribute__((packed));

struct endpoint_descriptor_isochronous {


        volatile uint32_t endpoint_info;
        struct isochronous_transfer_descriptor* tail_pointer;
        struct isochronous_transfer_descriptor* head_pointer;
        struct endpoint_descriptor_isochronous* next_ed;


} __attribute__((packed));



struct endpoint_descriptor* int_zero1;
struct endpoint_descriptor* int_zero2;
struct endpoint_descriptor* int_zero3;
struct endpoint_descriptor* int_zero4;
struct endpoint_descriptor* int_zero5;
struct endpoint_descriptor* int_zero6;
struct endpoint_descriptor* int_zero7;
struct endpoint_descriptor* int_zero8;
struct endpoint_descriptor* int_zero9;
struct endpoint_descriptor* int_zero10;
struct endpoint_descriptor* int_zero11;
struct endpoint_descriptor* int_zero12;
struct endpoint_descriptor* int_zero13;
struct endpoint_descriptor* int_zero14;
struct endpoint_descriptor* int_zero15;
struct endpoint_descriptor* int_zero16;
struct endpoint_descriptor* int_zero17;
struct endpoint_descriptor* int_zero18;
struct endpoint_descriptor* int_zero19;
struct endpoint_descriptor* int_zero20;
struct endpoint_descriptor* int_zero21;
struct endpoint_descriptor* int_zero22;
struct endpoint_descriptor* int_zero23;
struct endpoint_descriptor* int_zero24;
struct endpoint_descriptor* int_zero25;
struct endpoint_descriptor* int_zero26;
struct endpoint_descriptor* int_zero27;
struct endpoint_descriptor* int_zero28;
struct endpoint_descriptor* int_zero29;
struct endpoint_descriptor* int_zero30;
struct endpoint_descriptor* int_zero31;
struct endpoint_descriptor* int_zero32;
struct endpoint_descriptor_isochronous* iso_zero;
struct endpoint_descriptor* ctrl_zero;
struct endpoint_descriptor* bulk_zero;
struct isochronous_transfer_descriptor* iso_td_zero;
struct general_transfer_descriptor* int_td_zero;
struct general_transfer_descriptor* ctrl_td_zero;
struct general_transfer_descriptor* bulk_td_zero;
struct general_transfer_descriptor* ohci_done_gen_td;
struct endpoint_descriptor* ohci_setup_ed_0;


/* normally used data structures (bulk and setup) */


struct endpoint_descriptor* ohci_setup_ed;
struct endpoint_descriptor* ohci_bulk_in_ed;
struct endpoint_descriptor* ohci_bulk_out_ed;


struct general_transfer_descriptor* ohci_setup_td1;
struct general_transfer_descriptor* ohci_setup_td2;
struct general_transfer_descriptor* ohci_setup_td3;
struct general_transfer_descriptor* ohci_setup_td4;
struct general_transfer_descriptor* ohci_setup_td5;
struct general_transfer_descriptor* ohci_setup_td8;
struct general_transfer_descriptor* ohci_setup_td9;
struct general_transfer_descriptor* ohci_bulk_td1;
struct general_transfer_descriptor* ohci_bulk_td2;
struct general_transfer_descriptor* ohci_bulk_td3;
struct general_transfer_descriptor* ohci_bulk_td4;


#endif
