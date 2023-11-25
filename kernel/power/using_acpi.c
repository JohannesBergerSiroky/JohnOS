/* This is the ACPI driver of the kernel. */

#include "../types.h"
#include "using_acpi.h"
#include "../mem.h"
#include "../system.h"
#include "../graphics/screen.h"
#include "../string.h"


/* This function attempts to find the
 * Root Table Descriptor Pointer.
 */
void find_rsdp() 
{
        char* find_rsdp = (char*) 0xe0000;
        char temp_str[9];
        char* ch_to_ptr;
        ch_to_ptr = temp_str;
        uint32_t rsdp_found = 0;

        for(uint32_t a = 0xe0000; (a < 0x100000) && (rsdp_found == 0); a+=16) {
                for(uint32_t b = 0; b < 8; b++)
                        (*(ch_to_ptr + b)) = (*(find_rsdp + b));
                (*(ch_to_ptr + 8)) = '\0';

                if(strEql(ch_to_ptr, "RSD PTR "))
                        rsdp_found = 1;
                find_rsdp +=16;
        }

        if(rsdp_found == 1) {
                find_rsdp -=16;
                rsdp_address = (uint32_t)find_rsdp;
        }
        else
                print("\n\nError, could not find the rsdp");
}

/* Sets some memory regions and writes a zero
 * to some of them.
 */
void set_acpi_structs()
{
        rsd_ptr = (struct root_system_description_ptr*)kmem_4k_allocate();
        rsd_tbl = (struct root_system_description_tbl*)kmem_4k_allocate();
        fadt = (struct fixed_acpi_description_table*)kmem_4k_allocate();
        madt = (struct m_apic_description_table*)kmem_4k_allocate();

        ssdt = (struct s_system_descriptor_table*)kmem_4k_allocate();
        dsdt = (struct d_system_descriptor_table*)kmem_4k_allocate();
        dsdt_code_ptr = dsdt_code;
        ssdt_code_ptr = ssdt_code;

        for(uint32_t a = 0; a < 16000; a++) {
                (*(dsdt_code_ptr + a)) = 0x00;
                (*(ssdt_code_ptr + a)) = 0x00;
        }
        for(uint32_t a = 0; a < 512; a++) {
                (*(madt_array + a)) = 0x00;
        }
        for(uint32_t a = 0; a < 128; a++) {
                (*(madt_array_pointers + a)) = 0x00000000;
                (*(rsdt_array + a)) = 0x0000000000;
        }
}

/* Reads from a memory region and copies it's contents into
 * the Root Table Descriptor Pointer structure.
 */
void read_rsdp()
{
        uint32_t rsdp_addr = rsdp_address;
        uint32_t temp_size = sizeof(struct root_system_description_ptr);

        memset2((uint8_t*)rsd_ptr, (uint8_t*)rsdp_addr, temp_size);
}

/* Prints the contents of the Root Table Descriptor Pointer
 * structure.
 */
void read_rsdp_2()
{
	    uint32_t rsdp_addr = rsdp_address;
	    uint32_t temp_size = sizeof(struct root_system_description_ptr) - 3;
	    print("\nsize of rsdp struct: ");
	    print_hex(temp_size);

	    uint8_t* rsd = (uint8_t*)rsdp_addr;
	    print("rsdp byte by byte:");
	    print("\n\n");
        for(uint32_t a = 0; a < temp_size; a++) {
                if((a < 8) || ((a > 8) && (a < 15)))
                        printch((char)(*(rsd + a)), 0);
                else
                        print_hex_byte((*(rsd + a)));
        }
}

/* Reads from a memory region and copies it's contents into
 * the Root Table Descriptor Table structure.
 */
void test_read_rsdt()
{
        uint32_t rsdt_addr = rsd_ptr->rsdt_address;
        uint8_t* rsdt_ptr = (uint8_t*)rsdt_addr;
        uint32_t temp_size = sizeof(struct root_system_description_tbl);
        memset2((uint8_t*)rsd_tbl, rsdt_ptr, temp_size);

        rsdt_entries = (rsd_tbl->length - temp_size)/(uint32_t)4;

        uint32_t* rsdt_ptr_int = (uint32_t*)(rsdt_addr + temp_size);
        for(uint32_t a = 0; a < rsdt_entries; a++)
                *(rsdt_array + a) = *(rsdt_ptr_int + a);
}
/* Prints the Root Table Descriptor Table from an array.
 * It works after reading the rsdt. so now maybe this is 
 * unnecessary.
 */
void test_read_from_array() 
{
        print("\n\n");
        for(uint32_t b = 0; b < rsdt_entries; b++) {
                print_hex((uint32_t)(*(rsdt_array + b)));
                print(" ");
        }
}

/* Sets signatures in headers pointed to by the RSDT
 * and also prints the signatures in headers pointed
 * to by the RSDT
 */
void read_h_f_rsdt() 
{
        char* read_acpi_signatures;
        for(uint32_t a = 0; a < rsdt_entries; a++) {
                read_acpi_signatures = (char*)(*(rsdt_array + a));
                for(uint32_t b = 0; b < 4; b++)
                        printch((*(read_acpi_signatures + b)), 0);
        }

}

/* Finds the MADT structure and sets the values in
 * the structs appointed to it and also prints the
 * values in it.
 */
void print_madt_struct_info()
{

	    /* find the madt */
		char read_acpi_signatures[(rsdt_entries*5) + 1];
		read_acpi_signatures[(rsdt_entries*5)] = '\0';
		char* read_acpi_signatures_ptr = read_acpi_signatures;

		char* read_sign;
		uint32_t index_value = 0;
		uint32_t found_flag = 0;

	    for(uint32_t a = 0; ((a < rsdt_entries) && (found_flag == 0)); a++) {

			        read_sign = (char*)(*(rsdt_array + a));

                    /* rsdt_array contains mem addresses in int format */
				    for(uint32_t b = 0; b < 4 ; b++) {

            
						    (*(read_acpi_signatures_ptr + (5*a) + b)) = (*(read_sign + b));
				    }

				    (*(read_acpi_signatures_ptr + (5*a) + 4)) = '\0';
				    if(strEql((read_acpi_signatures_ptr + (5*a)), "APIC")) {
						    found_flag = 1;
						    index_value = a;
				    }
				    
	    }

	    if(found_flag == 1) {

			    uint32_t m_size = sizeof(struct m_apic_description_table);
			    memset2((uint8_t*)madt, (uint8_t*)(*(rsdt_array + index_value)), m_size);
			    uint8_t* madt_ptr_2 = (uint8_t*)(*(rsdt_array + index_value));

			    madt_entries = madt->length - m_size;
			    uint8_t* madt_array_ptr = madt_array;
			    print("\n\nMADT structures byte by byte: ");
			    print("\n");
			    for(uint32_t a = 0; a < madt_entries; a++) {
					    (*(madt_array_ptr + a)) = (*(madt_ptr_2 + m_size + a));

			    }

			    for(uint32_t b = 0; b < madt_entries; b++) {
					    print_hex_byte((uint8_t)(*(madt_array_ptr + b)));
					    print(" ");
					    if((b % 24 == 0) && (b != 0))
						        print("\n");
			    }
			    print("\n\nLength of the APIC structure: ");
			    print_hex(madt->length);
	    }

	    else {
			    print("\n\nError: could not read the madt structure"); 
			    print("\n\n Index value: ");
			    print_hex(index_value);

	    }

}

/* Finds the FADT memory region and copies it into
 * the FADT structure.
 */
void print_fadt_struct_info()
{

		/* find the madt */
		char read_acpi_signatures[(rsdt_entries*5) + 1];
		read_acpi_signatures[(rsdt_entries*5)] = '\0';
		char* read_acpi_signatures_ptr = read_acpi_signatures;

		char* read_sign;
		uint32_t index_value = 0;
		uint32_t found_flag = 0;

	    for(uint32_t a = 0; ((a < rsdt_entries) && (found_flag == 0)); a++) {
			read_sign = (char*)(*(rsdt_array + a));

                /* rsdt_array contains mem addresses in int format */
				for(uint32_t b = 0; b < 4 ; b++) {

						(*(read_acpi_signatures_ptr + (5*a) + b)) = (*(read_sign + b));
				}

				(*(read_acpi_signatures_ptr + (5*a) + 4)) = '\0';
				if(strEql((read_acpi_signatures_ptr + (5*a)), "FACP")) {
						found_flag = 1;
						index_value = a;
				}
				

		}

	    if(found_flag == 1) {
			    uint32_t m_size = sizeof(struct fixed_acpi_description_table);
			    memset2((uint8_t*)fadt, (uint8_t*)(*(rsdt_array + index_value)), m_size);

	    }
	    else {
			    print("\n\nError: could not read the fadt structure"); 
			    print("\n\n Index value: ");
			    print_hex(index_value);

	    }


}

/* Finds the dsdt address by reading the FADT structure
 * and then prints the signature of the DSDT and prints
 * the length of the DSDT.
 */
void read_dsdt()
{
	    uint8_t* dsdt_byte_b_byte = (uint8_t*)fadt->dsdt_addr;
	    uint32_t m_size = sizeof(struct d_system_descriptor_table);
	    memset2((uint8_t*)dsdt, dsdt_byte_b_byte, m_size);

	    print("\n\nSignature: ");
	    for(uint32_t c = 0; c < 4; c++) {

                printch(dsdt->signature[c], 0);
	    }

	    dsdt_code_length = dsdt->length - m_size;
	    print("\n\nLength of dsdt code: ");
	    print_hex(dsdt_code_length);
}

/* Finds the SSDT memory region and copies it into
 * the SSDT structure.
 */
void read_ssdt()
{

        /* find the madt */
		char read_acpi_signatures[(rsdt_entries*5) + 1];
		read_acpi_signatures[(rsdt_entries*5)] = '\0';
		char* read_acpi_signatures_ptr = read_acpi_signatures;

		char* read_sign;
		uint32_t index_value = 0;
		uint32_t found_flag = 0;

	    for(uint32_t a = 0; ((a < rsdt_entries) && (found_flag == 0)); a++) {

			    read_sign = (char*)(*(rsdt_array + a));

                    /* rsdt_array contains mem addresses in int format */
				    for(uint32_t b = 0; b < 4 ; b++)
					    {

						        (*(read_acpi_signatures_ptr + (5*a) + b)) = (*(read_sign + b));
					    }
				    (*(read_acpi_signatures_ptr + (5*a) + 4)) = '\0';
				    if(strEql((read_acpi_signatures_ptr + (5*a)), "SSDT")) {
						    found_flag = 1;
						    index_value = a;
				    }
				    
	    }

	    if(found_flag == 1) {

			    uint32_t m_size = sizeof(struct s_system_descriptor_table);
			    memset2((uint8_t*)ssdt, (uint8_t*)(*(rsdt_array + index_value)), m_size);
			    print("\n\nSignature: ");

			    for(uint32_t c = 0; c < 4; c++) {

					    printch(ssdt->signature[c], 0);

			    }

			    print("\n\nLength of the SSDT structure: ");
			    print_hex(ssdt->length);
			    ssdt_code_length = ssdt->length - m_size;

			    print("\n\nLength of the ssdt code: ");
			    print_hex(ssdt_code_length);
		    }

	    else {
			    print("\n\nError: could not read the ssdt structure"); 
			    print("\n\n Index value: ");
			    print_hex(index_value);
	    }

}

/* Initializes some variables by reading some values in
 * the FADT structure. This is partly for shutting down 
 * the computer using ACPI.
 */
void init_acpi_variables()
{

	    if(fadt->ext_pm1a_cnt_blk_high_high | fadt->ext_pm1a_cnt_blk_high_low | fadt->ext_pm1a_cnt_blk_low) {

                pm1a_ignore = 1;
        }
        else {

                pm1a_ignore = 0;

        }


	    if(fadt->ext_pm1b_cnt_blk_high_high | fadt->ext_pm1b_cnt_blk_high_low | fadt->ext_pm1b_cnt_blk_low) {

                pm1b_ignore = 1;

        }
        else {

                pm1b_ignore = 0;

        }


        if(!pm1a_ignore) {

                using_acpi_pm1a_cnt_blk = fadt->pm1a_cnt_blk;

        }
        else {

                using_acpi_pm1a_cnt_blk = 0;

        }


        if(!pm1b_ignore) {

                using_acpi_pm1b_cnt_blk = fadt->pm1b_cnt_blk;

        }
        else {

                using_acpi_pm1b_cnt_blk = 0;
                print("\nacpi: pm1b_cnt_blk was ignored");

        }

        using_acpi_smi_cmd = fadt->smi_cmd;
        using_acpi_enable = fadt->acpi_enable;

        if(using_acpi_pm1a_cnt_blk) {

                using_acpi_sci_en_a = (volatile uint32_t)(inportw((uint16_t)using_acpi_pm1a_cnt_blk) & (uint16_t)0x0001);

        }

        if(using_acpi_pm1b_cnt_blk) {

                using_acpi_sci_en_b = (volatile uint32_t)(inportw((uint16_t)using_acpi_pm1b_cnt_blk) & (uint16_t)0x0001);

        }

}
