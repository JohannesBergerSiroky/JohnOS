#include "../types.h"
#include "check_shutdown_acpi.h"
#include "using_acpi.h"
#include "../mem.h"
#include "../graphics/screen.h"
#include "../system.h"
#include "../string.h"

/* Attempts to find the S5 in the DSDT. */
void check_shutdown_code()
{
        uint32_t flag_set = 0;
        char temp_holder[5];
        temp_holder[4] = '\0';
        char* dsdt_string_byte = temp_holder;
        uint32_t i = 0;
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

        dsdt_byte_b_byte += m_size;

        while ((i < dsdt_code_length) && (flag_set == 0)) {

                memset2((uint8_t*)dsdt_string_byte, dsdt_byte_b_byte, 3);

                if(strEql(dsdt_string_byte, "_S5")) {
                        
                        if((*(dsdt_byte_b_byte + 3)) == '_')
                                dsdt_byte_b_byte ++;

                        dsdt_byte_b_byte += 3;
                        using_acpi_slp_typ_a = (volatile uint32_t)(*dsdt_byte_b_byte);
                        dsdt_byte_b_byte++;
                        using_acpi_slp_typ_b = (volatile uint32_t)(*dsdt_byte_b_byte);    
                        flag_set = 1;                 
                }


                i++;
                dsdt_byte_b_byte++;                    
        }
}

/* Attempts to shut down the computer by using ACPI. */
void shutdown()
{
        uint16_t temp;
        uint16_t temp2 = 0;
        temp = inportw((uint16_t)using_acpi_pm1a_cnt_blk);
        if(using_acpi_pm1b_cnt_blk) {
                temp2 = inportw((uint16_t)using_acpi_pm1b_cnt_blk);

        }

        /* ignore the slp_typx values */
        ACPI_PM1_CTL_BLK_IGN_SLP_TYPX(temp); 
        outportw(using_acpi_pm1a_cnt_blk, temp);

        if(using_acpi_pm1b_cnt_blk) {
                ACPI_PM1_CTL_BLK_IGN_SLP_TYPX(temp2);              
                ACPI_PM1_CTL_BLK_SET_SLP_TYPX(temp2, using_acpi_slp_typ_b);
                outportw(using_acpi_pm1b_cnt_blk, temp2);
        }

        ACPI_PM1_CTL_BLK_SET(temp, using_acpi_slp_typ_a);
        outportw(using_acpi_pm1a_cnt_blk, temp);
}
