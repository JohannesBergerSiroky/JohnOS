#ifndef USB_MASS_STORAGE_H
#define USB_MASS_STORAGE_H

#define USB_BULKSTORAGE_SCSI_READ10                                             0x28
#define USB_BULKSTORAGE_SCSI_LBA                                                0x01
#define USB_BULKSTORAGE_SCSI_CTL                                                0x02
#define USB_BULKSTORAGE_SCSI_INQURY                                             0x12
#define USB_BULKSTORAGE_SCSI_REQUEST_SENSE                                      0x03
#define USB_BULKSTORAGE_SCSI_READ_CAPACITY10                                    0x25



uint8_t init_success_1;
uint8_t init_success_2;

uint32_t usbms_extra_data[2];
struct cbw_read_6 {

        /* 0x43425355 (little endian) */
        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        /* any value */
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2; 
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low; 
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        /* bit 7 = 1 (from device to host). So 0 or 128 (128 in case of 
        device to host) */
        volatile uint8_t flags; 
        /* 0 most of the time, I guess */
        volatile uint8_t lun; 
        /* 0xa. look at the interface subclass just in case */
        volatile uint8_t cbwcw_length; 
        /* 08h for read6 */
        volatile uint8_t operation_code; 
        // rdprotect = 0, dpo = 1, fua/fua_nv = 00. so value should be 16 or 0x10
        volatile uint8_t reserved; 
        /* logical block address = 0 */
        volatile uint8_t lba_high; 
        volatile uint8_t lba_low;
        volatile uint8_t transfer_length;
        /* could be set to zero. */
        volatile uint8_t control_byte; 
        /* zero out the remaining data. 6 bytes. */
        volatile uint8_t zero_1; 
        volatile uint8_t zero_2;
        volatile uint8_t zero_3;
        volatile uint8_t zero_4;
        volatile uint8_t zero_5;
        volatile uint8_t zero_6;
        volatile uint8_t zero_7;
        volatile uint8_t zero_8;
        volatile uint8_t zero_9;
        volatile uint8_t zero_10;


} __attribute__((packed));



struct cbw_read_12 {


        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2; 
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low; 
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        volatile uint8_t flags; 
        /* 0 */
        volatile uint8_t lun; 
        volatile uint8_t cbwcw_length; 
        /* a8h for read 12 */
        volatile uint8_t operation_code; 
        /* rdprotect = 0, dpo = 1, fua/fua_nv = 00. so value should be 16 or 0x10 */
        volatile uint8_t meta_data; 
        volatile uint8_t lba_high_word_low; 
        volatile uint8_t lba_high_word_high;
        volatile uint8_t lba_low_word_low;
        volatile uint8_t lba_low_word_high;
        volatile uint8_t transfer_length_high_high; 
        volatile uint8_t transfer_length_high_low;
        volatile uint8_t transfer_length_low_high;
        volatile uint8_t transfer_length_low_low;
        volatile uint8_t group_number; 
        volatile uint8_t control_byte; 
        volatile uint8_t zero_1; 
        volatile uint8_t zero_2;
        volatile uint8_t zero_3;
        volatile uint8_t zero_4;


} __attribute__((packed));

struct cbw_read_16 {


        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2; 
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low; 
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        /* bit 7 = 1 (from device to host). So 0 or 128 (128 in this case) */
        volatile uint8_t flags; 
        /* 0 */
        volatile uint8_t lun; 
        volatile uint8_t cbwcw_length; 
        /* 88h for read16 */
        volatile uint8_t operation_code; 
        /* rdprotect = 0, dpo = 1, fua/fua_nv = 00. so value should be 16 or 0x10 */
        volatile uint8_t meta_data; 
        volatile uint8_t lba_high_dword_low; 
        volatile uint8_t lba_high_dword_low2;
        volatile uint8_t lba_high_dword_low3;
        volatile uint8_t lba_high_dword_high;
        volatile uint8_t lba_low_dword_low;
        volatile uint8_t lba_low_dword_low2;
        volatile uint8_t lba_low_dword_low3;
        volatile uint8_t lba_low_dword_high;
        /* zero */
        volatile uint8_t group_number; 
        volatile uint8_t transfer_length_high_word_low; 
        volatile uint8_t transfer_length_high_word_high;
        volatile uint8_t transfer_length_low_word_low; 
        volatile uint8_t transfer_length_low_word_high;
        volatile uint8_t control_byte; 


} __attribute__((packed));

struct inquiry {


        volatile uint8_t signature_low;
        volatile uint8_t signature_mid_low;
        volatile uint8_t signature_low_high;
        volatile uint8_t signature_mid_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low1;
        volatile uint8_t tag_low2;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low;
        volatile uint8_t data_transfer_length_low1; 
        volatile uint8_t data_transfer_length_low2; 
        volatile uint8_t data_transfer_length_high; 

        volatile uint8_t flags; 
        /* 0 */
        volatile uint8_t lun; 
        volatile uint8_t cbwcw_length; 
        /*12h */
        volatile uint8_t operation_code;
        /* zero */ 
        volatile uint8_t meta_data; 
        /* 0 */
        volatile uint8_t page_code; 
        volatile uint8_t allocation_length_low; 
        volatile uint8_t allocation_length_high;
        volatile uint8_t control;
        volatile uint8_t zero_1;
        volatile uint8_t zero_2;
        volatile uint8_t zero_3;
        volatile uint8_t zero_4;
        volatile uint8_t zero_5;
        volatile uint8_t zero_6;
        volatile uint8_t zero_7;
        volatile uint8_t zero_8;
        volatile uint8_t zero_9;
        volatile uint8_t zero_10;


} __attribute__((packed));

struct request_sense {


        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2; 
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low; 
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        /* bit 7 = 1 (from device to host). So 0 or 128 (128 in this case) */
        volatile uint8_t flags; 
        /* 0 */
        volatile uint8_t lun; 
        volatile uint8_t cbwcw_length; 
        volatile uint8_t operation_code; 
        volatile uint8_t desc;
        volatile uint8_t reserved1; 
        volatile uint8_t reserved2;
        volatile uint8_t allocation_length; 
        volatile uint8_t control;
        volatile uint8_t zero_1;
        volatile uint8_t zero_2;
        volatile uint8_t zero_3;
        volatile uint8_t zero_4;
        volatile uint8_t zero_5;
        volatile uint8_t zero_6;
        volatile uint8_t zero_7;
        volatile uint8_t zero_8;
        volatile uint8_t zero_9;
        volatile uint8_t zero_10;


} __attribute__((packed));

struct read_capacity_10 {


        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2; 
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low; 
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        volatile uint8_t flags; 
        volatile uint8_t lun; 
        volatile uint8_t cbwcw_length; 
        volatile uint8_t operation_code; 
        volatile uint8_t reserved;
        volatile uint8_t lba_low;
        volatile uint8_t lba_low_1;
        volatile uint8_t lba_low_2;
        volatile uint8_t lba_high;
        volatile uint8_t reserved2;
        volatile uint8_t reserved3;
        volatile uint8_t pmi; 
        volatile uint8_t control; 
        volatile uint8_t zero_1;
        volatile uint8_t zero_2;
        volatile uint8_t zero_3;
        volatile uint8_t zero_4;
        volatile uint8_t zero_5;
        volatile uint8_t zero_6;


} __attribute__((packed));

struct read_capacity_16 {


        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2;
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low;
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        volatile uint8_t flags; 
        volatile uint8_t lun; 
        volatile uint8_t cbwcw_length; 
        volatile uint8_t operation_code; 
        volatile uint8_t service_action; 
        volatile uint8_t lba_low; 
        volatile uint8_t lba_low_1;
        volatile uint8_t lba_low_2;
        volatile uint8_t lba_low_3;
        volatile uint8_t lba_low_4; 
        volatile uint8_t lba_low_5;
        volatile uint8_t lba_low_6;
        volatile uint8_t lba_high;
        volatile uint8_t allocation_length_low; 
        volatile uint8_t allocation_length_low1;
        volatile uint8_t allocation_length_low2;
        volatile uint8_t allocation_length_high; 
        /* 0 */
        volatile uint8_t pmi; 
        volatile uint8_t control; 


} __attribute__((packed));

struct test_unit_ready {


        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2; 
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low; 
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        volatile uint8_t flags; 
        volatile uint8_t lun;
        volatile uint8_t cbwcw_length; 
        volatile uint8_t operation_code; 
        volatile uint8_t reserved1;
        volatile uint8_t reserved2;
        volatile uint8_t reserved3;
        volatile uint8_t reserved4;
        volatile uint8_t control;
        volatile uint8_t zero_1;
        volatile uint8_t zero_2;
        volatile uint8_t zero_3;
        volatile uint8_t zero_4;
        volatile uint8_t zero_5;
        volatile uint8_t zero_6;
        volatile uint8_t zero_7;
        volatile uint8_t zero_8;
        volatile uint8_t zero_9;
        volatile uint8_t zero_10;


} __attribute__((packed));

struct identify {
        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2;
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low; 
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        /* bit 7 = 1 (from device to host). So 0 or 128 (128 in this case) */
        volatile uint8_t flags; 
        volatile uint8_t lun; 
        /* 0xa. look at the interface subclass just in case */
        volatile uint8_t cbwcw_length; 
        /* 80h */
        volatile uint8_t idtf_message; 
        volatile uint8_t zero_pad1;
        volatile uint8_t zero_pad2;
        volatile uint8_t zero_pad3;
        volatile uint8_t zero_pad4;
        volatile uint8_t zero_pad5;
        volatile uint8_t zero_1;
        volatile uint8_t zero_2;
        volatile uint8_t zero_3;
        volatile uint8_t zero_4;
        volatile uint8_t zero_5;
        volatile uint8_t zero_6;
        volatile uint8_t zero_7;
        volatile uint8_t zero_8;
        volatile uint8_t zero_9;
        volatile uint8_t zero_10;


} __attribute__((packed));


struct cbw_write_10  {


        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2; 
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low;
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        /* It's a write operation, so set the flags to 0 */
        volatile uint8_t flags; 
        volatile uint8_t lun; 
        /* 0xa */
        volatile uint8_t cbwcw_length; 
        // 0x2a
        volatile uint8_t operation_code; 
        /* set to 0 */
        volatile uint8_t meta_data; 
        volatile uint8_t lba_high_word_low; 
        volatile uint8_t lba_high_word_high;
        volatile uint8_t lba_low_word_low;
        volatile uint8_t lba_low_word_high;
        /* zero */
        volatile uint8_t group_number; 
        volatile uint8_t transfer_length_low_byte; 
        volatile uint8_t transfer_length_high_byte;
        /* 0 */
        volatile uint8_t control_byte; 
        volatile uint8_t zero_1;
        volatile uint8_t zero_2;
        volatile uint8_t zero_3;
        volatile uint8_t zero_4;
        volatile uint8_t zero_5;
        volatile uint8_t zero_6;


} __attribute__((packed));

struct cbw_read_10 {


        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2; 
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low; 
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        volatile uint8_t flags; 
        volatile uint8_t lun; 
        volatile uint8_t cbwcw_length; 
        /* 28h for read10 */
        volatile uint8_t operation_code; 
        volatile uint8_t meta_data; 
        volatile uint8_t lba_high_word_low;
        volatile uint8_t lba_high_word_high;
        volatile uint8_t lba_low_word_low;
        volatile uint8_t lba_low_word_high;
        /* zero */
        volatile uint8_t group_number; 
        volatile uint8_t transfer_length_low_byte; 
        volatile uint8_t transfer_length_high_byte;
        /* could be set to zero. */
        volatile uint8_t control_byte; 
        /* zero out the remaining data. 6 bytes. */
        volatile uint8_t zero_1; 
        volatile uint8_t zero_2;
        volatile uint8_t zero_3;
        volatile uint8_t zero_4;
        volatile uint8_t zero_5;
        volatile uint8_t zero_6;


} __attribute__((packed));

struct usb_storage_data {
        volatile uint8_t signature_low; 
        volatile uint8_t signature_low2;
        volatile uint8_t signature_low3;
        volatile uint8_t signature_high;
        volatile uint8_t tag_low;
        volatile uint8_t tag_low2; 
        volatile uint8_t tag_low3;
        volatile uint8_t tag_high;
        volatile uint8_t data_transfer_length_low; 
        volatile uint8_t data_transfer_length_low2;
        volatile uint8_t data_transfer_length_low3;
        volatile uint8_t data_transfer_length_high;
        volatile uint8_t flags; 
        volatile uint8_t lun; 
        volatile uint8_t cbwcw_length; 
        /* 28h for read10 */
        volatile uint8_t operation_code; 
        /* for read10: extra_data[4] is lba_low_low, and extra_data[3] is lba_low_high
         * extra_data[7] are 512 bytes sets for length of the transfer
         */
        volatile uint8_t extra_data[15];
} __attribute__((packed));


struct cbw_read_16* cbw_16_1;
struct inquiry* inq;
struct request_sense* r_s;
struct read_capacity_10* read_capacity;
struct read_capacity_16* r_capacity_16;
struct cbw_read_6* cbw_6_1;
struct cbw_read_10* cbw_10_1;
struct cbw_read_12* cbw_12_1;
struct test_unit_ready* tur;
struct cbw_write_10* cbw_w_10_1;
struct identify* idtf;


struct request_sense* set_rs();
struct read_capacity_10* set_read_capacity();
struct test_unit_ready* set_tur();
struct inquiry* set_inquiry();
struct identify* set_identify();
struct read_capacity_16* set_read_capacity_16();
volatile uint8_t* prepare_usb_storage_bulk_transfer(uint8_t transfer_type, uint8_t direction, uint8_t cbwlun, uint32_t length, uint32_t* usd_extra_data);

struct cbw_write_10* set_cbw_w_10_data(volatile uint8_t lba_low_low, 
                                       volatile uint8_t lba_low_high);
struct cbw_read_12* set_cbw_12_data(volatile uint8_t lba_low_low, 
                                    volatile uint8_t lba_low_high);
struct cbw_read_6* set_cbw_6_data(volatile uint8_t lba_low_low, 
                                  volatile uint8_t lba_low_high);
struct cbw_read_16* set_cbw_16_data(uint8_t lba_low1, 
                                    uint8_t lba_low2);
struct cbw_read_10* set_cbw_10_data(volatile uint8_t lba_low_low, 
                                    volatile uint8_t lba_low_high, volatile uint32_t length);

void init_scsi_structs();
void print_bulk_data_buffer(uint8_t* buffer, uint32_t buffer_length);
uint32_t init_xhci_transfer(uint32_t slot_index, uint32_t transfer_type, 
                            uint32_t transfer_length, 
                            volatile uint8_t lba_low, 
                            volatile uint8_t lba_high);


uint8_t inquiry(struct inquiry* i_data, uint32_t data_toggle_1, 
                uint32_t data_toggle_2);
uint8_t request_sense(struct request_sense* rs, uint32_t data_toggle_1, 
                      uint32_t data_toggle_2);

uint8_t read_capacity_10(struct read_capacity_10* read_capacity, 
                         uint32_t data_toggle_1, 
                         uint32_t data_toggle_2);

uint8_t transfer_data_12(struct cbw_read_12* cbw_data_12, 
                         volatile uint32_t data_toggle_1, 
                         volatile uint32_t data_toggle_2);


uint8_t transfer_data_10(struct cbw_read_10* cbw_data_10, 
                         volatile uint32_t data_toggle_1, 
                         volatile uint32_t data_toggle_2);

uint8_t transfer_data_10_w(struct cbw_write_10* cbw_data_w_10, 
                           volatile uint32_t data_toggle_1, 
                           volatile uint32_t data_toggle_2, 
                           volatile uint8_t* transfer_buffer);


volatile uint8_t* ehci_transfer_bulkstorage_data(struct usb_storage_data* data);


uint8_t test_unit_ready(struct test_unit_ready* t_unit_r, 
                        uint32_t data_toggle_1, 
                        uint32_t data_toggle_2);

void transfer_data_6(struct cbw_read_6* cbw_data_6, 
                     volatile uint32_t data_toggle_1, 
                     volatile uint32_t data_toggle_2);

uint8_t read_capacity_16(struct read_capacity_16* read_capacity, 
                         uint32_t data_toggle_1, 
                         uint32_t data_toggle_2);

uint8_t transfer_data_16(struct cbw_read_16* trdata, 
                         volatile uint32_t data_toggle_1, 
                         volatile uint32_t data_toggle_2); 


uint8_t set_addr();
uint8_t set_config();
uint8_t confirm_data();
void init_queue_heads();



void usbms_get_max_lun(uint32_t usbms_slot_index);
void usbms_mass_storage_reset(uint32_t usbms_slot_index);
uint8_t sequence_1();

uint8_t select_lba_low(uint32_t l);
uint8_t select_lba_low2(uint32_t l);
uint8_t select_lba_low3(uint32_t l);
uint8_t select_lba_high(uint32_t l);


void print_mr_error();




#endif
