#include "types.h"
#include "system.h" 


/* Reads from a port and returns a byte value. */
uint8_t inportb (uint16_t _port)
{
        uint8_t rv;
        __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
        return rv;
}

/* Writes a byte sized value to a port. */
void outportb (uint16_t _port, uint8_t _data)
{
        __asm__ __volatile__ ("outb %%al, %%dx" : : "d" (_port), "a" (_data));
}

/* Writes a byte sized value to a port using 
 * another format.
 */
void outb_p(uint8_t value, uint16_t port)
{
        __asm__("outb %%al,%%dx\n" "\tjmp 1f\n" "1:\tjmp 1f\n" "1:":: "a"(value), "d"(port));
}

/* Reads from a port and returns a dword value. */
uint32_t inportl(uint16_t port)
{
        uint32_t result;
        __asm__ __volatile__("inl %%dx, %%eax" : "=a" (result) : "dN" (port));
        return result;
}

/* Writes a dword sized value to a port. */
void outportl(uint16_t port, uint32_t data)
{
        __asm__ __volatile__("outl %%eax, %%dx" : : "d" (port), "a" (data));
}

/* Reads from a port and returns a word value. */
uint16_t inportw(uint16_t port)
{
        uint16_t result;
           __asm__ __volatile__("inw %%dx, %%ax" : "=a" (result) : "dN" (port));
        return result;
}

/* Writes a word sized value to a port. */
void outportw(uint16_t port, uint16_t data)
{
        __asm__ __volatile__("outw %%ax, %%dx" : : "d" (port), "a" (data));
}
