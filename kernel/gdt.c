#include "types.h"
#include "gdt.h"


/*  Sets up a descriptor in the Global Descriptor Table.
 *  Protected Mode relies on the GDT. Mostly kernels will
 *  probably set the start of the address space to 0 and allow
 *  the whole memory space to be used. 
 */
struct gdt_entry gdt[3];
struct gdt_ptr gp;
extern void gdt_flush();


void gdt_set_gate(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
        /* Setup the descriptor base address */
        gdt[num].base_low = (base & 0xFFFF);
        gdt[num].base_middle = (base >> 16) & 0xFF;
        gdt[num].base_high = (base >> 24) & 0xFF;

        /* Setup the descriptor limits */
        gdt[num].limit_low = (limit & 0xFFFF);
        gdt[num].granularity = ((limit >> 16) & 0x0F);

        /* Finally, set up the granularity and access flags */
        gdt[num].granularity |= (gran & 0xF0);
        gdt[num].access = access;
}


/*  Should be called by main. This will setup the special GDT
 *  pointer, set up the first 3 entries in our GDT, and then
 *  finally call gdt_flush() in our assembler file in order
 *  to tell the processor where the new GDT is and update the
 *  new segment registers 
 */
void gdt_install()
{
        /* Setup the GDT pointer and limit */
        gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
        gp.bases = (uint32_t) &gdt;

        /* Our NULL descriptor */
        gdt_set_gate(0, 0, 0, 0, 0);

        /*  The second entry is our Code Segment. The base address
         *  is 0, the limit is 4GBytes, it uses 4KByte granularity,
         *  uses 32-bit opcodes, and is a Code Segment descriptor.
         */
        gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

        /*  The third entry is our Data Segment. It's EXACTLY the
         *  same as our code segment, but the descriptor type in
         *  this entry's access byte says it's a Data Segment 
         */
        gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);


        /* Flush out the old GDT and install the new changes! */
        gdt_flush();
}


