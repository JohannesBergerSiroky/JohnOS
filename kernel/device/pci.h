#ifndef PCI_H
#define PCI_H


#define PCI_CONFIG_ADDRESS                                            0xCF8
#define PCI_CONFIG_DATA                                               0xCFC
#define PCI_BUS(x)                                                (x << 16)
#define PCI_DEVICE(x)                                             (x << 11)
#define PCI_FUNCTION(x)                                           (x <<  8)
#define PCI_ENABLE                                                (1 << 31)


volatile uint32_t pci_dvc_bus;
volatile uint32_t pci_dvc_device;
volatile uint32_t pci_dvc_function;


uint32_t pci_device_array[3];
uint16_t PCI_Config_RW(uint32_t bus, uint32_t device, uint32_t func, 
                       uint16_t offset);
uint16_t get_pci_VD(uint32_t bus, uint32_t device, uint32_t funct);
void check_pci_devices(uint8_t bus, uint8_t device);


uint16_t check_pci_function(uint32_t bus, uint32_t device, uint32_t funct);
uint16_t get_pci_HeaderType(uint32_t bus, uint32_t device, uint32_t funct);
void pci_bscan_devices();
uint16_t check_pci_type(uint32_t bus, uint32_t device, uint32_t funct);
uint16_t check_pci_progif(uint32_t bus, uint32_t device, uint32_t funct);


void list_pci_devices(uint16_t pci_type, uint16_t pci_func, 
                      uint16_t pci_progif, uint32_t f);
uint32_t PCI_Config_RW2(uint32_t p_bus, uint32_t p_device, 
                        uint32_t p_func, uint16_t p_offset);
uint32_t PCI_Config_RW3(uint32_t p_bus, uint32_t p_device, 
                        uint32_t p_func, uint16_t p_offset);


uint16_t PCI_Config_RW_Uhci(uint32_t p_bus, uint32_t p_device, 
                            uint32_t p_func, uint16_t p_offset);
uint32_t PCI_Config_RW_Uhci2(uint32_t p_bus, uint32_t p_device, 
                             uint32_t p_func, uint16_t p_offset);
void PCI_Config_RW_ethernetc_io(uint32_t p_bus, uint32_t p_device, 
                                uint32_t p_func, uint16_t p_offset);
uint32_t* find_device(uint16_t f_class, uint16_t f_subclass, 
                      uint16_t f_pprogif);


void set_device(uint32_t deviceinfo_1, uint32_t deviceinfo_2, uint32_t deviceinfo_3, 
                uint16_t pci_class, uint16_t pci_func, uint16_t pci_pprogif);
int32_t read_device_intline(uint32_t bus, uint32_t device, uint32_t funct);



#endif
