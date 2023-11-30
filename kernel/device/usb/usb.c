#include "../../types.h"
#include "usb.h"
#include "../pci.h"
#include "usb_xhci.h"
#include "usb_ehci.h"
#include "usb_ohci.h"
#include "usb_uhci.h"
#include "usb_hubs.h"
#include "../../graphics/screen.h"
#include "../../mem.h"

void init_usb_setup()
{
        sup_data = (struct setup_data*)kmem_4k_allocate();
        memset((void *)sup_data, 0x00, 4096);
}

struct setup_data* fill_usb_setup_data(uint8_t requesttype, uint8_t req,
                                  uint8_t valuelow, uint8_t valuehigh,
                                  uint8_t indexlow, uint8_t indexhigh, 
                                  uint8_t lengthlow, uint8_t lengthhigh)
{
        struct setup_data* sdata = sup_data;
        sdata->RequestType = (volatile uint8_t)requesttype;
        sdata->Request = (volatile uint8_t)req;
        sdata->value_low = (volatile uint8_t)valuelow;
        sdata->value_high = (volatile uint8_t)valuehigh;
        sdata->index_low = (volatile uint8_t)indexlow;
        sdata->index_high = (volatile uint8_t)indexhigh;
        sdata->length_low = (volatile uint8_t)lengthlow;
        sdata->length_high = (volatile uint8_t)lengthhigh;
        return sdata;
}

void zero_device_descriptor(struct setup_data* dscrptr)
{
        uint32_t counter;
        uint8_t* dsc_pointer = (uint8_t*)dscrptr;
        for(counter = 8; counter < 4096; counter++)
                *(dsc_pointer + counter) = 0x00;

}

void clear_feature(uint32_t usb_slot_index, uint32_t usb_type, uint32_t usb_value) 
{
        if(hci_in_use == HCI_XHCI) {

                xhci_clear_feature(usb_slot_index, usb_type, usb_value);
        }

        else {

                print("\nclear feature: unsupported hci function");
        }
}
