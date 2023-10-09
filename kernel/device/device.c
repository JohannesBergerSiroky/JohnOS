#include "../types.h"
#include "device.h"
#include "../mem.h"

/* Allocates memory to a device structure pointer. */
void init_temporary_device_as_usbms()
{
        usbdev = (struct device*)kmem_4k_allocate();
        memsetint((uint32_t*)usbdev, 0, 4096);
        usbdev->for_later = 1;
        usbdev->current_device_type = USB_MASS_STORAGE;
}
