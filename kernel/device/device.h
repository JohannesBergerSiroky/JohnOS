#ifndef DEVICE_H
#define DEVICE_H


#define USB_MASS_STORAGE                52


struct device {
        uint32_t current_device_type;
        uint32_t for_later;
};

struct device* usbdev;

void init_temporary_device_as_usbms();


#endif
