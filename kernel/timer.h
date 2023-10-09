#ifndef TIMER_H
#define TIMER_H


volatile uint32_t timer_ticks;
void timer_phase(int hz);
void timer_handler();
void timer_install();
void set_timer_variable();


void timer_wait(int32_t ticks);
void kernel_delay_100(uint32_t k_time);


#endif
