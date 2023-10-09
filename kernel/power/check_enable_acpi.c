#include "../types.h"
#include "check_enable_acpi.h"
#include "using_acpi.h"
#include "../mem.h"
#include "../system.h"
#include "../graphics/screen.h"
#include "../timer.h"
#include "../isr.h"
#include "../irq.h"


/* Checks if ACPI is enabled. */
uint8_t check_acpi_enabled()
{
        if(using_acpi_sci_en_a) {

                return (uint8_t)1;
        }    
        return (uint8_t)0;
}

/* Attempts to enable ACPI using the FADT. */
uint32_t enable_acpi()
{
   
        uint16_t enable_flag = 0;
        uint16_t enable_flag_b = 0;
        uint32_t enable_timeout = 0;
        uint8_t enable = fadt->acpi_enable;
        timer_install();
        asm("sti");

        /* Trying to enable ACPI. */
        outportb((uint16_t)using_acpi_smi_cmd, enable);
        print("\nEnabling acpi.....");
        timer_ticks = 0;

        while((!enable_flag) && (timer_ticks < 300)) {


                while((timer_ticks == 0) || ((timer_ticks % 5) > 0));
                enable_flag = (inportw((uint16_t)using_acpi_pm1a_cnt_blk) & (uint16_t)0x0001);

        }

        /* Commented code: if(timer_ticks >= 300) {
                enable_timeout = 1;
                print("\ntimed out");
        } */

        asm("cli");
        timer_ticks = 0;

        /* Commented code: print("\nenable flag (hopefully 1): ");
        printi((uint32_t)enable_flag);
        print("\n"); */


        /* Commented code: if(enable_timeout) {
                print("\n?????\n");
                return 0;

        } */

        /* If pm1b_cnt_blk is used then we probably have to do this too. */
        if(using_acpi_pm1b_cnt_blk) {

                if(enable_flag) {
                        print("\nthe code is here\n");
                        enable_timeout = 0;
                        asm("sti");

                        timer_ticks = 0;
                        while((!enable_flag_b) && (timer_ticks < 300)) {
                                while((timer_ticks == 0) || ((timer_ticks % 5) > 0));
                                enable_flag_b = (inportw((uint16_t)using_acpi_pm1b_cnt_blk) & (uint16_t)0x0001);
                            }

                        if(timer_ticks >= 300)
                                enable_timeout = 1;

                        asm("cli");
                        timer_ticks = 0;
                        irq_uninstall_handler(0);
                    

                        if(enable_timeout) {
                                return 2;

                        }
                        if(enable_flag_b)
                                return 1;
                        else {
                                return 2;
                        }
                }

                else {
                        return 0;
                }

        }

        else {
                if(enable_flag) {
                        return 1;
                }

        }
        return 0;
        
}
