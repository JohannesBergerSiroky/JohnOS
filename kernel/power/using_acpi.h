#ifndef USING_ACPI_H
#define USING_ACPI_H


#define ACPI_PM1_CTL_BLK_SET_SLP_EN                     (1 << 13)
#define ACPI_PM1_CTL_BLK_IGN_SLP_TYPX(x)      (x &= (~(7 << 10)))
#define ACPI_PM1_CTL_BLK_SET_SLP_TYPX(x, y)      (x |= (y << 10))
#define ACPI_PM1_CTL_BLK_SET(x, y) (x |= ((y << 10) | (1 << 13)))


uint32_t rsdp_address;
uint32_t rsdt_entries;
uint32_t madt_entries;
uint32_t dsdt_code_length;
uint32_t ssdt_code_length;


volatile uint32_t using_acpi_pm1a_cnt_blk;
volatile uint32_t using_acpi_pm1b_cnt_blk;
volatile uint16_t using_acpi_smi_cmd;
volatile uint32_t pm1a_ignore;
volatile uint32_t pm1b_ignore;
volatile uint32_t using_acpi_sci_en_a;
volatile uint32_t using_acpi_sci_en_b;
volatile uint32_t using_acpi_slp_typ_a;
volatile uint32_t using_acpi_slp_typ_b;
volatile uint8_t using_acpi_enable;


uint8_t dsdt_code[16000];
uint8_t ssdt_code[16000];
uint8_t* dsdt_code_ptr;
uint8_t* ssdt_code_ptr;

uint8_t madt_array[512];
uint32_t madt_array_pointers[128];
uint32_t rsdt_array[128];

struct root_system_description_ptr {


	    char signature[8];
	    uint8_t checksum;
	    char oemid[6];
	    uint8_t revision;
	    uint32_t rsdt_address;
	    uint32_t length;
	    uint32_t xsdt_addr_high;
	    uint32_t xsdt_addr_low;
	    uint8_t ext_checksum;
	    uint16_t reserved_1;
	    uint8_t reserved_2;


} __attribute__((packed));

struct root_system_description_tbl {


	    char signature[4];
	    uint32_t length;
	    uint8_t revision;
	    uint8_t checksum;
	    char oemid[6];
	    char oemtblid[8];
	    char oemrevision[4];
	    char engineerid[4];
	    char engineer_revision[4];


} __attribute__((packed));

struct fixed_acpi_description_table {


	    char signature[4];
	    uint32_t length;
	    uint8_t fadt_major_version;
	    uint8_t checksum;
	    char oemid[6];
	    char oem_table_id[8];
	    char oem_revision[4];
	    char engineerid[4];
	    char engineer_revision[4];
	    uint32_t facs_addr;
	    uint32_t dsdt_addr;
	    uint8_t might_be_reserved;
	    uint8_t preferred_pm_profile; 
	    uint16_t sci_int;
	    uint32_t smi_cmd; 
	    uint8_t acpi_enable;
	    uint8_t acpi_disable;
	    uint8_t s4biosreq; 
	    uint8_t pstate_cnt; 
	    uint32_t pm1a_evt_blk;
	    uint32_t pm1b_evt_blk;
	    uint32_t pm1a_cnt_blk; 
	    uint32_t pm1b_cnt_blk;
	    uint32_t pm2_cnt_blk; 
	    uint32_t pm_tmr_blk;
	    uint32_t gpe0_blk; 
	    uint32_t gpe1_blk;
	    uint8_t pm1_evt_len;
	    uint8_t pm1_cnt_len;
	    uint8_t pm2_cnt_len;
	    uint8_t pm_tmr_len; 
	    uint8_t gpe0_blk_len;
	    uint8_t gpe1_blk_len;
	    uint8_t gpe1_base; 
	    uint8_t cst_cnt;
	    uint16_t p_lvl2_lat;
	    uint16_t p_lvl3_lat;
	    uint16_t flush_size; 
	    uint16_t flush_stride;
	    uint8_t duty_offset;
	    uint8_t duty_width;
	    uint8_t day_alarm;
	    uint8_t mon_alarm;
	    uint8_t century;
	    uint16_t iapc_boot_arch;
	    uint8_t reserved;
	    uint32_t flags;
	    uint32_t reset_reg_high_high;
	    uint32_t reset_reg_high_low;
	    uint32_t reset_reg_low;
	    uint8_t reset_value;
	    uint16_t arm_boot_arch;
	    uint8_t fadt_minor_version;
	    uint32_t ext_facs_addr_high;
	    uint32_t ext_facs_addr_low;
	    uint32_t ext_dsdt_addr_high;
	    uint32_t ext_dsdt_addr_low;
	    uint32_t ext_pm1a_evt_blk_high_high;
	    uint32_t ext_pm1a_evt_blk_high_low;
	    uint32_t ext_pm1a_evt_blk_low;
	    uint32_t ext_pm1b_evt_blk_high_high;
	    uint32_t ext_pm1b_evt_blk_high_low;
	    uint32_t ext_pm1b_evt_blk_low;
	    uint32_t ext_pm1a_cnt_blk_high_high;
	    uint32_t ext_pm1a_cnt_blk_high_low;
	    uint32_t ext_pm1a_cnt_blk_low;
	    uint32_t ext_pm1b_cnt_blk_high_high;
	    uint32_t ext_pm1b_cnt_blk_high_low;
	    uint32_t ext_pm1b_cnt_blk_low;
	    uint32_t ext_pm2_cnt_blk_high_high;
	    uint32_t ext_pm2_cnt_blk_high_low;
	    uint32_t ext_pm2_cnt_blk_low;
	    uint32_t ext_pm_tmr_blk_high_high;
	    uint32_t ext_pm_tmr_blk_high_low;
	    uint32_t ext_pm_tmr_blk_low;
	    uint32_t ext_gpe0_blk_high_high;
	    uint32_t ext_gpe0_blk_high_low;
	    uint32_t ext_gpe0_blk_low;
	    uint32_t ext_gpe1_blk_high_high;
	    uint32_t ext_gpe1_blk_high_low;
	    uint32_t ext_gpe1_blk_low;
	    uint32_t sleep_control_reg_high_high;
	    uint32_t sleep_control_reg_high_low;
	    uint32_t sleep_control_reg_low;
	    uint32_t sleep_status_reg_high_high;
	    uint32_t sleep_status_reg_high_low;
	    uint32_t sleep_status_reg_low;
	    uint32_t hypervisor_vendor_identity_high;
	    uint32_t hypervisor_vendor_identity_low;


} __attribute__((packed));

struct m_apic_description_table {


	    char signature[4];
	    uint32_t length;
	    uint8_t revision;
	    uint8_t checksum;
	    char oemid[6];
	    char oem_table_id[8];
	    char oem_revision[4];
	    char engineer_id[4];
	    char engineer_revision[4];
	    uint32_t local_interrupt_controller_address;
	    uint32_t flags;


} __attribute__((packed));

struct hpet_acpi_table {


	    char signature[4];
	    uint32_t length;
	    uint8_t Revision;
	    uint8_t checksum;
	    char oemid[6];
	    char oem_table_id[8];
	    char oem_revision[4];
	    char engineerid[4];
	    char engineer_revision[4];
	    uint32_t event_timer_block_id;
	    uint32_t baddress_32low_high_high;
	    uint32_t baddress_32low_high_low;
	    uint32_t baddress_32_low;
	    uint8_t hpet_number;
	    uint16_t min_clock_tick_counter_pmode;
	    uint8_t page_protection_oem_attribute;


} __attribute__((packed));

struct s_system_descriptor_table {


	    char signature[4];
	    uint32_t length;
	    uint8_t revision;
	    uint8_t checksum;
	    char oemid[6];
	    char oem_table_id[8];
	    char oem_revision[4];
	    char engineer_id[4];
	    char engineer_revision[4];


} __attribute__((packed));

struct d_system_descriptor_table {


	    char signature[4];
	    uint32_t length;
	    uint8_t revision;
	    uint8_t checksum;
	    char oemid[6];
	    char oem_table_id[8];
	    char oem_revision[4];
	    char engineer_id[4];
	    char engineer_revision[4];


} __attribute__((packed));

struct root_system_description_ptr* rsd_ptr;
struct root_system_description_tbl* rsd_tbl;
struct fixed_acpi_description_table* fadt;
struct m_apic_description_table* madt;
struct s_system_descriptor_table* ssdt;
struct hpet_acpi_table* hpet;
struct d_system_descriptor_table* dsdt;

void find_rsdp();
void set_acpi_structs();
void read_rsdp();
void read_rsdp_2();
void test_read_rsdt();
void test_read_from_array();
void read_h_f_rsdt();
void print_madt_struct_info();
void print_fadt_struct_info();
void read_dsdt();
void read_ssdt();

void init_acpi_variables();


#endif
