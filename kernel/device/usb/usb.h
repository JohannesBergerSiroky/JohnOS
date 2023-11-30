#ifndef USB_H
#define USB_H


#define HCI_UHCI                                             1
#define HCI_OHCI                                             2
#define HCI_EHCI                                             3
#define HCI_XHCI                                             4


#define USB_SET_ADDRESS                                   0x05
#define USB_GET_DESCRIPTOR                                0x06
#define USB_GET_CONFIGURATION                             0x08
#define USB_SET_CONFIGURATION                             0x09
#define USB_DEVICE_TO_HOST                                0x80
#define USB_HOST_TO_DEVICE                                0x00
#define USB_DEVICE_DESCRIPTOR_DEVICE_LENGTH               0x12
#define USB_DEVICE_DESCRIPTOR_CONFIGURATION_LENGTH          37

#define USB_GET_DESCRIPTOR_DEVICE                         0x01
#define USB_GET_DESCRIPTOR_CONFIGURATION                  0x02


#define USBF_BMREQUESTTYPE_D2H                        (1 << 7)
#define USBF_BMREQUESTTYPE_H2D                        (0 << 7)
#define USBF_BMREQUESTTYPE_GET_DESCRIPTOR             (6 << 8)
#define USBF_BMREQUESTTYPE_SET_CONFIGURATION          (9 << 8)
#define USBF_BMREQUESTTYPE_GET_CONFIGURATION          (8 << 8)
#define USBF_BMREQUESTTYPE_CLEAR_FEATURE              (1 << 8)
#define USBF_BMREQUESTTYPE_MS_RESET                 (255 << 8)
#define USBF_BMREQUESTTYPE_GET_MAX_LUN              (254 << 8)


uint32_t hci_in_use;


struct setup_data {


        volatile uint8_t RequestType;
        volatile uint8_t Request;
        volatile uint8_t value_low;
        volatile uint8_t value_high;
        volatile uint8_t index_low;
        volatile uint8_t index_high;
        volatile uint8_t length_low;
        volatile uint8_t length_high;


} __attribute__((packed));

struct setup_data* sup_data;

struct setup_data* fill_usb_setup_data(uint8_t requesttype, uint8_t req,
                                       uint8_t valuelow, uint8_t valuehigh, 
                                       uint8_t indexlow, uint8_t indexhigh, 
                                       uint8_t lengthlow, uint8_t lengthhigh);

void init_usb_setup();
void clear_feature(uint32_t usb_slot_index, uint32_t usb_type,
                   uint32_t usb_value);

void zero_device_descriptor(struct setup_data* dscrptr);


#endif
