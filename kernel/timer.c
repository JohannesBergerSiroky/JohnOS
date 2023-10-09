#include "types.h"
#include "system.h"
#include "isr.h"
#include "timer.h"
#include "irq.h"


void timer_phase(int hz)
{
        /* Calculate the divisor */
        int divisor = 1193180 / hz;

        /* Set the command byte to 0x36 */
        outportb(0x43, 0x36); 

        /* Set low byte of divisor */
        outportb(0x40, divisor & 0xFF);
  
        /* Set high byte of divisor */
        outportb(0x40, divisor >> 8);  
  
}


/*  Handles the timer. In this case, it's very simple: We
 *  increment the 'timer_ticks' variable every time the
 *  timer fires. By default, the timer fires 18.222 times
 *  per second.
 */
void timer_handler(struct regs* r)
{
    /* Increment our 'tick count' */
    timer_ticks++;
}


/* Sets up the system clock by installing the timer handler
 *  into IRQ0 
 */
void timer_install()
{
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
}

void set_timer_variable()
{
        timer_ticks = 0;

}


/* Pauses execution of all other things in the process or thread.
 * The smallest amount of time to be paused is 1/100 seconds.
 * For a paus of 1/100 seconds one can give the number of one
 * as the parameter to this function.
 */
void kernel_delay_100(uint32_t k_time)
{
        asm("cli");
        timer_phase(100);
        timer_install();
        asm("sti");
        timer_ticks = 0;
        while(timer_ticks < k_time);

        asm("cli");
        irq_uninstall_handler(0);
        timer_phase(18);

}



