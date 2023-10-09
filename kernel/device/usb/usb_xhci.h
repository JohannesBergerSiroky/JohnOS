#ifndef USB_XHCI_H
#define USB_XHCI_H


#define XHCI_SLOT_CONTEXT_ENTRIES(x)                        (x << 27)
#define XHCI_ENDPOINT_CONTEXT_MAX_ERRCOUNT                   (3 << 1)
#define XHCI_ENDPOINT_CONTEXT_MAX_BURSTSIZE                 (15 << 8)
#define XHCI_ENDPOINT_CONTEXT_MAX_PACKETSIZE(x)             (x << 16)
#define XHCI_ENDPOINT_CONTEXT_AVERAGETRBSIZE(x)              (x << 0)

#define XHCI_SET_CONFIGURATION_1                                 0x80
#define XHCI_SET_CONFIGURATION_0                                0x800
#define XHCI_GET_CONFIGURATION                                 0x8000
#define XHCI_CLEAR_FEATURE                                     0x4000
#define XHCI_MASS_STORAGE_RESET                                   0x8
#define XHCI_GET_MAX_LUN                                          0x4

#define XHCI_SETUP_TRB_INDATA                               (3 << 16)
#define XHCI_SETUP_TRB_OUTDATA                              (2 << 16)
#define XHCI_SETUP_TRB_NODATA                               (0 << 16)
#define XHCI_SETUP_TRB_SETUP                                (2 << 10)
#define XHCI_SETUP_TRB_STATUS                               (4 << 10)
#define XHCI_SETUP_TRB_SETUPDATA_LENGTH                      (8 << 0)
#define XHCI_SETUP_TRB_IOC                                   (1 << 5)
#define XHCI_SETUP_TRB_IMMEDIATE_DATA                        (1 << 6)
#define XHCI_SETUP_TRB_CYCLE_BIT(x)                          (x << 0)
#define XHCI_STATUS_TRB_DIRECTION_IN                        (1 << 16)
#define XHCI_STATUS_TRB_DIRECTION_OUT                       (0 << 16)

#define XHCI_COMMAND_ENABLE_SLOT                                    9
#define XHCI_COMMAND_NOOP                                           8
#define XHCI_COMMAND_DISABLE_SLOT                                  10
#define XHCI_COMMAND_ADDRESS_DEVICE                                11
#define XHCI_COMMAND_CONFIGURE_ENDPOINT                            12
#define XHCI_COMMAND_RESET_ENDPOINT                                14
#define XHCI_COMMAND_STOP_ENDPOINT                                 15
#define XHCI_COMMAND_RESET_DEVICE                                  17
#define XHCI_TAG_LINK_TRB                                   (6 << 10)
#define XHCI_LINK_TOGGLE_CYCLE(x)                            (x << 1)


volatile uint32_t found_xhci_bus;
volatile uint32_t found_xhci_device;
volatile uint32_t found_xhci_function;
volatile uint32_t xhci_mem_address_attr_doorb;
volatile uint32_t xhci_crctrl;
volatile uint32_t xhci_dcbaap;

uint32_t hci_mem_resources[4096];
volatile uint32_t input_contexts[256];

/* prints */
void print_xhci_pci_control_status();
void print_xhci_capl_version();
void print_xhci_hciparams1();
void print_xhci_hciparams2();
void print_xhci_hciparams3();
void print_xhci_hccparams1();
void print_xhci_doorbelloffset();
void print_xhci_rtsoffset();
void print_xhci_hccparams2();
void print_xhci_usbcmd();
void print_xhci_usbsts();
void print_xhci_pagesize();
void print_xhci_dvcnctrl();
void print_xhci_crctrl();
void print_xhci_dcbaap();
void print_xhci_configure();
void print_xhci_max_ports();
void print_spba();
void print_hci_scratchpad_size();
void print_event_ring_table();
void print_command_ring_base();
void print_check_xhci_64_bit_capable();
void print_first_and_second_and_third_dca();
void print_event_ring_zero(uint32_t ev_offset);
void print_command_ring();
void print_base_doorbell_offset();
void print_command_ring_control_address();
void print_port(uint32_t port);


/* reads */
uint32_t get_xhci_usbcmd();
uint32_t get_xhci_usbsts();
uint32_t get_xhci_max_ports();
uint32_t get_xhci_cnr();
void get_xhci_commandringbase();
uint8_t read_byte(const uint32_t base_addr, const uint32_t offset);


/* variables for storing regarding the registers and other locations */ 
volatile uint32_t hci_mem_resources_index;
volatile uint32_t hci_operational_base;
volatile uint32_t hci_max_interrupters;
volatile uint32_t xhci_eventregsbase;
volatile uint32_t xhci_runtime_base;
volatile uint32_t* xhci_dcbaap_base;
volatile uint32_t xhci_doorbell_base;
volatile uint32_t* hci_dcaba;
volatile uint32_t xhci_doorbell_base_offset;
volatile uint32_t* xhci_cmdring_ctrl_base;
volatile uint32_t xhci_64bit_capable;
volatile uint32_t* xhci_commandring_base;
volatile uint32_t xhci_runtime_base;
volatile uint32_t xhci_max_ports;
volatile uint32_t xhci_max_device_slots;
volatile uint32_t hci_max_erst;
volatile uint32_t xhci_max_er_segment_table_slots;
volatile uint32_t hci_er_dq_info;
volatile uint32_t hci_er_segment_table_ba;
volatile uint32_t* hci_er_segment_table_dq_reg;
volatile uint32_t hci_er_dq_ptr;
volatile uint32_t hci_er; // first er
volatile uint32_t xhci_dcbaap_base_max_index;
volatile uint32_t xhci_dcbaap_base_current_index;
volatile uint32_t* x_device_context_array;
volatile uint32_t** x_device_context_array_ptr;
volatile uint32_t hci_scratchpad_pagesize;
volatile uint32_t hci_scratchpad_size;
volatile uint32_t hci_ext_cap_ptr;
volatile uint32_t device_context_array_start;
volatile uint32_t hci_er_segment_table_size;
volatile uint32_t hci_cic;
volatile uint32_t hci_cie;
volatile uint32_t actual_xdevice_slot;
volatile uint32_t* hci_scratchpad_table_base;
volatile uint32_t xroute_string;
volatile uint32_t xport_num;
volatile uint32_t xport_speed;
volatile uint8_t xhci_interrupt_status;
volatile uint32_t hci_context_size;
volatile uint32_t hci_er_start;
uint32_t xhci_get_descriptor_first_time_used;
uint32_t* trb_address_field;
volatile uint32_t* trb_address_bulk_out;
volatile uint32_t* trb_address_bulk_in;
uint32_t slot_error;
uint32_t link_trb_flag;


/* some structs */
struct input_contexts {


        uint32_t* ica_address;
        struct input_contexts* ica_next;

};

struct input_contexts * ica_list;
struct input_contexts* ica_list_start;

struct trb {


        volatile uint32_t field_0;
        volatile uint32_t field_1;
        volatile uint32_t field_2;
        volatile uint32_t field_3;


};

struct xhci_default_endpoint {


        uint32_t xhci_max_packet_size;
        volatile uint32_t dequeue_cycle_state;
        volatile uint32_t* endpoint_trb_address;
        struct trb* setup_trb;
        struct trb* data_stage_trb;
        struct trb* status_stage_trb; 
        uint32_t* ep0_trb_address;
        uint32_t* data_stage_buffer;       


};

struct xhci_endpoint {


        uint32_t data_direction;
        volatile uint32_t endpoint_max_packet_size;
        volatile uint32_t dcs;
        volatile uint32_t* trb_address;
        struct trb* transfer_trb;
        volatile uint32_t* data_buffer;
        volatile uint32_t maxburst_size;
        volatile uint32_t transfer_data_protocol;
        volatile uint32_t ep_address;
        struct xhci_endpoint* xhci_endpoint_next;


};

struct xhci_endpoint* slot_endpoint_start;

struct xhci_usb_device_slot {


        volatile uint32_t port_speed;
        volatile uint32_t port_number;
        uint32_t configuration_value; 
        uint32_t configured;

        volatile uint32_t* device_slot_address;
        struct xhci_endpoint* slot_endpoint;
        struct xhci_default_endpoint* default_ep;
        struct xhci_usb_device_slot* slot_next;
        uint32_t slot_num_endpoints;
        uint8_t slot_endpoints[31];



};

struct xhci_usb_device_slot* usb_slot_start;


/* the stores and a print. maybe some prints */
void store_xhci_max_interrupters();
void store_xhci_max_er_segment_table_slots();
void store_xhci_max_device_slots();
void store_hci_operational_base();
void store_hci_ext_cap_ptr();
void print_first_hci_ext_cap_reg();
void store_xhci_eventregsbase();
void store_xhci_runtimebase();
void store_xhci_dcbaap();
void store_xhci_doorbellbase();
void store_xhci_commandringctrl_base();
void store_check_xhci_64_bit_capable();
void write_store_xhci_commandringbase();
void store_xhci_maxports();
void store_hci_dcaba();
void store_hci_max_erst();
void store_event_ring_segment_table_size();
void store_hci_er();
void store_hci_scratchpad_pagesize();
void store_hci_scratchpad_size();
void store_hci_cic();
void store_hci_cie();
void write_store_config_register(const uint32_t hci_config_value);
void store_hci_context_size();
void request_bios_handsoff();

void xhci_abort_command();
uint32_t write_cmd_on_hc_command_ring(volatile uint32_t command, 
                                      volatile uint32_t xslot_id, 
                                      volatile uint32_t xendpoint_id, 
                                      volatile uint32_t ep_configuration_value, 
                                      volatile uint32_t ctrb_flags, 
                                      volatile uint32_t ctrb_ep_flags, 
                                      uint32_t input_context_address);

uint32_t init_hci_command(volatile uint32_t hci_cmdring_command, 
                          volatile uint32_t device_slot, 
                          volatile uint32_t endpoint_id, 
                          volatile uint32_t cmd_flags, 
                          volatile uint32_t ep_flags, 
                          uint32_t er_counter, uint32_t ic_address);
uint32_t hci_enable_slot();
uint32_t xhci_no_op();
uint32_t hci_configure_slot(uint32_t slot_to_be_configured, 
                            volatile uint32_t deconfigure_flag, 
                            volatile uint32_t input_context_address);
uint32_t reset_slot(uint32_t slot_to_be_reset);
uint32_t reset_endpoint (uint32_t slot_to_be_handled, 
                         uint32_t endpoint_to_be_reset);
uint32_t hci_disable_slot(uint32_t slot);
uint32_t hci_set_address(uint32_t slot_to_be_addressed, 
                         volatile uint32_t block_flag, 
                         volatile uint32_t i_context_address);
uint8_t check_command_ring_running();
void print_event_ring_segment_table_base();
void hci_reset_sequence(uint32_t d_slot_index);

void xhci_trb_to_address(volatile uint32_t* address_field, 
                         struct trb * trb_to_transfer);
void print_xhci_buffer(uint8_t* buffer_address, uint32_t buffer_length);
void xhci_get_descriptor(uint32_t slot_index, uint32_t desc_type, 
                         uint32_t transf_length, 
                         uint32_t control_transfer_flags);
void configure_slot(uint32_t slot_index);
void configure_slot_set_configuration(uint32_t d_slot_index);
void xhci_clear_feature(uint32_t slot_index, uint32_t type, uint32_t value);
void xhci_get_max_lun(uint32_t slot_index);
void xhci_mass_storage_reset(uint32_t slot_index);

void disable_port(uint32_t port_to_disable);
void print_xhci_commandreg();
uint32_t* init_and_send_input_context(uint32_t device_context_index, 
                                      uint32_t hci_port_num, 
                                      uint32_t hci_port_speed);
void init_device_context(uint32_t context, uint32_t port_speed);


/* other functions */
void stop_xhci();
void start_xhci();
void reset_xhci();
void clear_sts_eint();
void disable_hci_interrupts();
void init_hci_mem_resources();
void xhci_portx_enumeration();
void write_xhci_port(uint32_t porty, uint32_t tempy);
uint32_t* copy_device_context_to_input_context(uint32_t dca_index);

void enable_interrupts_and_interrupter();
void hci_clear_interrupts();
void write_interrupter_not_busy();
uint32_t write_event_dequeue_pointer(uint32_t pointer_to_add);
uint32_t handle_event(uint8_t interrupt_handling);
void handle_ev_non_dq(uint8_t interrupt_handl);
void store_event_ring_table_and_rings();

uint32_t* get_hci_event_ring(uint32_t hci_ert_offset, uint32_t hci_er_offset);
void ring_doorbell(uint32_t offset, uint32_t value);
uint32_t test_init_hci();
void free_hci_mem_resources();
void xhci_usb_handler();


#endif
