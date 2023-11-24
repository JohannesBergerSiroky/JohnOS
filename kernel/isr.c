#include "types.h"
#include "isr.h"
#include "idt.h"
#include "graphics/screen.h"

/* Prints an error message. The fault handler function calls
 * this function.
 */
int8_t* exception_messages[] = {
        "Division By Zero",
        "Debug",
        "Non-maskable Interrupt",
        "Breakpoint",
        "Into Detected Overflow",
        "Out Of Bounds",
        "Invalid Opcode",
        "No Co-processor",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Bad Tss",
        "Segment Not Present",
        "Stack Fault",
        "General Protection Fault",
        "Page Fault",
        "Unknown Interrupt",
        "Coprocessor Fault",
        "Alignment Check Exception",
        "Machine Check Exception",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved"
};

/* Sets the isr functions to the interrupt placeholders.
 * The isr functions are located in start_assorted.asm
 */
void isr_install()
{
        set_idt(0, (uint32_t)isr0, 0x08, 0x8E);
        set_idt(1, (uint32_t)isr1, 0x08, 0x8E);
        set_idt(2, (uint32_t)isr2, 0x08, 0x8E);
        set_idt(3, (uint32_t)isr3, 0x08, 0x8E);
        set_idt(4, (uint32_t)isr4, 0x08, 0x8E);
        set_idt(5, (uint32_t)isr5, 0x08, 0x8E);
        set_idt(6, (uint32_t)isr6, 0x08, 0x8E);
        set_idt(7, (uint32_t)isr7, 0x08, 0x8E);
        set_idt(8, (uint32_t)isr8, 0x08, 0x8E);
        set_idt(9, (uint32_t)isr9, 0x08, 0x8E);
        set_idt(10, (uint32_t)isr10, 0x08, 0x8E);
        set_idt(11, (uint32_t)isr11, 0x08, 0x8E);
        set_idt(12, (uint32_t)isr12, 0x08, 0x8E);
        set_idt(13, (uint32_t)isr13, 0x08, 0x8E);
        set_idt(14, (uint32_t)isr14, 0x08, 0x8E);
        set_idt(15, (uint32_t)isr15, 0x08, 0x8E);
        set_idt(16, (uint32_t)isr16, 0x08, 0x8E);
        set_idt(17, (uint32_t)isr17, 0x08, 0x8E);
        set_idt(18, (uint32_t)isr18, 0x08, 0x8E);
        set_idt(19, (uint32_t)isr19, 0x08, 0x8E);
        set_idt(20, (uint32_t)isr20, 0x08, 0x8E);
        set_idt(21, (uint32_t)isr21, 0x08, 0x8E);
        set_idt(22, (uint32_t)isr22, 0x08, 0x8E);
        set_idt(23, (uint32_t)isr23, 0x08, 0x8E);
        set_idt(24, (uint32_t)isr24, 0x08, 0x8E);
        set_idt(25, (uint32_t)isr25, 0x08, 0x8E);
        set_idt(26, (uint32_t)isr26, 0x08, 0x8E);
        set_idt(27, (uint32_t)isr27, 0x08, 0x8E);
        set_idt(28, (uint32_t)isr28, 0x08, 0x8E);
        set_idt(29, (uint32_t)isr29, 0x08, 0x8E);
        set_idt(30, (uint32_t)isr30, 0x08, 0x8E);
        set_idt(31, (uint32_t)isr31, 0x08, 0x8E);
}


/*  All of our Exception handling Interrupt Service Routines will
 *  point to this function. This will tell us what exception has
 *  happened! Right now, we simply halt the system by hitting an
 *  endless loop. All ISRs disable interrupts while they are being
 *  serviced as a 'locking' mechanism to prevent an IRQ from
 *  happening and messing up kernel data structures 
 */
void fault_handler(struct regs *r)
{
    if (r->int_no < 32) {

        print(exception_messages[r->int_no]);
        print("Exception. System Halted! Please turn off your computer.");
        __asm__ __volatile__("hlt");

    }
}


