/* This file contains functions for multitasking functionality.
 * suggestion: try to make an uint32_teger as third argument,  
 * instead of 3 pointer to Register argument for task_switch
 */

#include "types.h"
#include "mem.h"
#include "multitasking.h"
#include "graphics/screen.h"

/* Creates a new task and the entrypoint is the address
 * of the function to bind the task to.
 */
struct Tasks* init_a_task(struct Tasks* temptask, void (*entry_point)())
{
        temptask = (struct Tasks*)kmem_4k_allocate();
        temptask->regs = (struct Registers*)kmem_4k_allocate();
    	temptask->regs->r_eax = 0;
        temptask->regs->r_ebx = 0;
        temptask->regs->r_ecx = 0;
        temptask->regs->r_edx = 0;
        temptask->regs->r_esi = 0;
        temptask->regs->r_edi = 0;
        temptask->regs->r_esp = (volatile uint32_t)kmem_4k_allocate;
        temptask->regs->r_esp += 512;
        temptask->regs->r_esp &= 0xfffffff0;
        temptask->regs->r_esp -= 64;
        temptask->regs->r_ebp = temptask->regs->r_esp;
        temptask->regs->r_eip = (volatile uint32_t)entry_point;
        print("\n the entry point is: ");
        print_hex(temptask->regs->r_eip);
        temptask->regs->r_eflags = get_eflags();
        temptask->topstack = temptask->regs->r_esp;
        temptask->saved_func = (volatile uint32_t)temptask->regs->r_eip;
        temptask->next = 0;
        return temptask;
}


/* Returns the EFLAGS register. */
uint32_t get_eflags()
{
         asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(temp_eflags)::"%eax");       
         return temp_eflags;
}

/* Creates two tasks and makes them flip back and forth
 * once the scheduler is running.
 */
void init_tasks()
{
       maintask = init_a_task(maintask, scheduler_task);
       othertask = init_a_task(othertask, task_2);
       maintask->next = othertask;
       othertask->next = maintask;
       runningtask = maintask;
}

/* This will make the screen print the numbers one and two. */
void scheduler_task()
{
        print("1");
        lasttask = runningtask;
        runningtask = runningtask->next;
        task_switch(lasttask->regs, runningtask->regs);

        /* this can be saved in yet another entry in the struct */
        lasttask->regs->r_eip = (volatile uint32_t)lasttask->saved_func; 
        lasttask->regs->r_esp = lasttask->topstack;
        lasttask->regs->r_ebp = lasttask->regs->r_esp;
}

void task_1()
{
        print("\ninside task 1");
        scheduler_task();
}

/* This is the other task. */
void task_2()
{
        print("2");
        scheduler_task(); 
        old_return_address = (volatile uint32_t*)scheduler_task;

        asm("movl (old_return_address), %ecx");
        asm("pushl %ecx");
        asm("ret");
}

/* Tests the scheduler. */
void trying_multitasking()
{
        print("\nTask action commencing:\n");

        for(uint32_t i = 0; i < 25; i++) {
                scheduler_task();
        }

        print("\nBack in main task again\n");
}
