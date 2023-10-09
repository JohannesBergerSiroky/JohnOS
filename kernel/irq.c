#include "types.h"
#include "isr.h"
#include "irq.h"
#include "idt.h"
#include "system.h"



/*  These are own ISRs that point to our special IRQ handler
 *  instead of the regular 'fault_handler' function 
 */


/*  This array is actually an array of void function pointers. We use
 *  this to handle custom IRQ handlers for a given IRQ 
 */


void* irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* This installs a custom IRQ handler for the given IRQ */

void irq_install_handler(int32_t irq, void (*handler)(struct regs *r))
{
        irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */

void irq_uninstall_handler(int32_t irq)
{
        irq_routines[irq] = 0;
}


/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */


/* initialize the two Programmable Interrupt Controllers */
void irq_remap(void) 
{
        /* First pulse (ICW 1). There are 2 PICs. 0x20 = the primary PIC. 0x11 = 00010001 = code (setting bits for different states) for initializing the PIC */
        outportb(0x20, 0x11); 

        /* Still on the first pulse (ICW 1). 0xA0 = secondary PIC. 00010001 = initialize the second PIC */
        outportb(0xA0, 0x11); 

        /* Second pulse (ICW 2, it uses a diff register). To the PICs data registers (0x21 and 0xA1). 0x20 = 32 (decimal) = set address for irq0-7 to 32-39 (irq0-7 are in the primary PIC) */
        outportb(0x21, 0x20); 

        /* still ICW 2 = the second pulse. set irq8-15 to address 40-47 (irq8-15 are in the secondary PIC) */
        outportb(0xA1, 0x28);

        /* Third pulse (ICW 3) = letting the PiCs know what pins to communicate with each other. Primary to secondary and vice versa: the 4th = bit number 2 = the second IR line */
	    outportb(0x21, 0x04);
     
        /* ICW 3 the secondary PIC take and sends data to the PIC. the 4th = bit number 2 so uses IR line 2 to communicate */
        outportb(0xA1, 0x02);
     
        /* ICW 4: this control word sets different states. For example if a buffer is used. set to 1 for x86 mode. rest are ignored */
        outportb(0x21, 0x01); 
        outportb(0xA1, 0x01);

        /* and finally null out all registers */
        outportb(0x21, 0x0); 
        outportb(0xA1, 0x0);
}

/*  We first remap the interrupt controllers, and then we install
 *  the appropriate ISRs to the correct entries in the IDT. This
 *  is just like installing the exception handlers 
 */

void irq_install()
{
        irq_remap();
        set_idt(32, (uint32_t)irq0, 0x08, 0x8E);
        set_idt(33, (uint32_t)irq1, 0x08, 0x8E);
        set_idt(34, (uint32_t)irq2, 0x08, 0x8E);
        set_idt(35, (uint32_t)irq3, 0x08, 0x8E);
        set_idt(36, (uint32_t)irq4, 0x08, 0x8E);
        set_idt(37, (uint32_t)irq5, 0x08, 0x8E);
        set_idt(38, (uint32_t)irq6, 0x08, 0x8E);
        set_idt(39, (uint32_t)irq7, 0x08, 0x8E);
        set_idt(40, (uint32_t)irq8, 0x08, 0x8E);
        set_idt(41, (uint32_t)irq9, 0x08, 0x8E);
        set_idt(42, (uint32_t)irq10, 0x08, 0x8E);
        set_idt(43, (uint32_t)irq11, 0x08, 0x8E);
        set_idt(44, (uint32_t)irq12, 0x08, 0x8E);
        set_idt(45, (uint32_t)irq13, 0x08, 0x8E);
        set_idt(46, (uint32_t)irq14, 0x08, 0x8E);
        set_idt(47, (uint32_t)irq15, 0x08, 0x8E);
}

/*  Each of the IRQ ISRs point to this function, rather than
 *  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
 *  to be told when you are done servicing them, so you need
 *  to send them an "End of Interrupt" command (0x20). There
 *  are two 8259 chips: The first exists at 0x20, the second
 *  exists at 0xA0. If the second controller (an IRQ from 8 to
 *  15) gets an interrupt, you need to acknowledge the
 *  interrupt at BOTH controllers, otherwise, you only send
 *  an EOI command to the first controller. If you don't send
 *  an EOI, you won't raise any more IRQs 
 */

void irq_handler(struct regs *r)
{
        /* This is a blank function pointer */
        void (*handler)(struct regs *r);

        /* Find out if we have a custom handler to run for this
        *  IRQ, and then finally, run it */
        handler = irq_routines[r->int_no - 32]; // points to an array of functions

        /* execute function */ 
        if (handler) {
                handler(r); 
        }

        /* If the IDT entry that was invoked was greater than 40
        *  (meaning IRQ8 - 15), then we need to send an EOI to
        *  the slave controller */
        if (r->int_no >= 40) {
                outportb(0xA0, 0x20);
        }

        /* In either case, we need to send an EOI to the master
        *  interrupt controller too */
        outportb(0x20, 0x20);
}
