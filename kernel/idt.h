#ifndef IDT_H
#define IDT_H



struct idt_gate {


        uint16_t low_offset;
        uint16_t selector;
        uint8_t always0;
        uint8_t flags;
        uint16_t high_offset;


} __attribute__((packed));

struct idt_ptr {

        uint16_t size;
        uint32_t base;


} __attribute__((packed));


extern void idt_flush();
void set_idt(uint8_t n, uint32_t offset, uint16_t sel, uint8_t flags);
void idt_install();


#endif


