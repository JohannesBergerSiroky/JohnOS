#ifndef GDT_H
#define GDT_H

struct gdt_entry {


        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_middle;
        uint8_t access;
        uint8_t granularity;
        uint8_t base_high;


} __attribute__((packed));

/* Special pointer which includes the limit: The max bytes taken up by the GDT, minus 1. Again, this NEEDS to be packed */
struct gdt_ptr {


        uint16_t limit;
        uint32_t bases;


} __attribute__((packed));

/* Our GDT, with 3 entries, and finally our special GDT pointer */


/* This will be a function in start.asm. We use this to properly reload the new segment registers */
void gdt_set_gate(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void gdt_install();

#endif





