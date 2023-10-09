#ifndef SYSTEM_H
#define SYSTEM_H


extern volatile uint32_t intflag;
extern uint32_t os_on_system;

uint8_t inportb (uint16_t _port);
void outportb (uint16_t _port, uint8_t _data);

void outb_p (uint8_t value, uint16_t port);
uint32_t inportl(uint16_t port);

void outportl(uint16_t port, uint32_t data);
uint16_t inportw(uint16_t port);
void outportw(uint16_t port, uint16_t data);


#endif
