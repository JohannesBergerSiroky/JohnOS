#include "types.h"
#include "mem.h"
#include "idt.h"

struct idt_gate idt[256];

struct idt_ptr iptr;

/* Sets an isr to the processor's internal register
 * so that an interrupt points to an isr handler.
 */
void set_idt(uint8_t n, uint32_t offset, uint16_t sel, uint8_t flags)
{
        idt[n].low_offset = offset & 0xFFFF;
        idt[n].selector = sel;
        idt[n].always0 = 0x0;
        idt[n].flags = flags;
        idt[n].high_offset = (offset >> 16) & 0xFFFF;
}

/* Makes the kernel have interrupt handling. */
void idt_install()
{
        /* Clear out the entire IDT, initializing it to zeros */
           memset(&idt, 0, sizeof(struct idt_gate) * 256);
        iptr.size = ((sizeof(struct idt_gate) * 256) - 1);
        iptr.base = (uint32_t) &idt;

        /* Add any new ISRs to the IDT here using idt_set_gate
         * Points the processor's internal register to the new IDT
         */
        idt_flush();
}


