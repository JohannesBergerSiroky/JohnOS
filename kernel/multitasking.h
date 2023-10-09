#ifndef MULTITASKING_H
#define MULTITASKING_H


extern volatile uint32_t* old_return_address;
extern volatile uint32_t* old_return_value;

/* struct for the register data and a linked list for the tasks */
struct Registers {


        volatile uint32_t r_eax;
        volatile uint32_t r_ebx;
        volatile uint32_t r_ecx;
        volatile uint32_t r_edx;
        volatile uint32_t r_esi; 
        volatile uint32_t r_edi; 
        volatile uint32_t r_esp; 
        volatile uint32_t r_ebp; 
        volatile uint32_t r_eip;
        volatile uint32_t r_eflags;


} __attribute__((packed));

struct Tasks {


        struct Registers* regs;
        volatile uint32_t topstack;
        volatile uint32_t saved_func;
        struct Tasks* next;



} __attribute__((packed));

struct Tasks* maintask; 
struct Tasks* firsttask; 
struct Tasks* othertask; 
struct Tasks* lasttask;
struct Tasks* runningtask;
struct Tasks* sched_task;

struct Tasks* init_a_task(struct Tasks* temptask, void (*entry_point)());
extern void task_switch(struct Registers* task1, struct Registers* task2);


volatile uint32_t temp_eflags;
volatile uint32_t scheduler_eip;
void task_1();
void task_2();


uint32_t get_eflags();
void init_tasks();
void scheduler_task();
void task_1();
void trying_multitasking();


#endif
