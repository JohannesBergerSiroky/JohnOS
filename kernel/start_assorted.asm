; boot.s -- Kernel start location. Also defines multiboot header.
; Based on Bran's kernel development tutorial file start.asm

MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot num value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table.
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)


[BITS 32]

[GLOBAL mboot]
[EXTERN code]
[EXTERN bss]
[EXTERN end]

mboot:
        dd  MBOOT_HEADER_MAGIC
        dd  MBOOT_HEADER_FLAGS
        dd  MBOOT_CHECKSUM
        dd  mboot
        dd  code
        dd  bss
        dd  end
        dd  start

[GLOBAL start]
[EXTERN main]

;Calls the main function
start:
        push    ebx
        cli
        call main
        jmp $

global intflag
global gps_flag; getportstatusflag
global task_switch
global old_return_address
global old_return_value

global gdt_flush     ; Allows the C code to link to this
extern gp            ; Says that '_gp' is in another file

; Loads the the GDT.
; This will set up our new segment registers. We need to do
; something special in order to set CS. We do what is called a
; far jump. A jump that includes a segment as well as an offset.
; This is declared in C as 'extern void gdt_flush();'
gdt_flush:
        lgdt [gp]        ; Load the GDT with our '_gp' which is a special pointer
        mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        jmp 0x08:flush2   ; 0x08 (abstracted) is the offset to our code segment: Far jump!


flush2:
        ret               ; Returns back to the C code!


global idt_flush
extern iptr

;flushes the idt
idt_flush:
        lidt [iptr]
        ret

;Adding ISRs below
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

%macro ISR_NOCODE 1
	isr%1:
		cli
		push byte 0
		push byte %1
		jmp isr_common_stub
%endmacro

%macro ISR_CODE 1
	isr%1:
		cli
		push byte %1
		jmp isr_common_stub
%endmacro

;Divide By Zero Exception
ISR_NOCODE 0

;Debug Exception
ISR_NOCODE 1
	
;Non Maskable Interrrupt Exception
ISR_NOCODE 2

;Breakpoint Exception
ISR_NOCODE 3
	
;Into Detected Overflow Exception
ISR_NOCODE 4
	
;Out of Bounds Exception
ISR_NOCODE 5

;Invalid Opcode Exception
ISR_NOCODE 6

;No Coprocessor Exception
ISR_NOCODE 7

;Double Fault Exception
ISR_CODE 8

;Coprocessor Segment Overrun Exception
ISR_NOCODE 9

;Bad TSS Exception
ISR_CODE 10

;Segment Not Present Exception
ISR_CODE 11

;Stack Fault Exception
ISR_CODE 12

;General Protection Fault Exception
ISR_CODE 13

;Page Fault Exception
ISR_CODE 14

;Unknown Interrupt Exception
ISR_NOCODE 15

;Coprocessor Fault Exception
ISR_NOCODE 16

;Alignment Check Exception
ISR_NOCODE 17

;Machine Check Exception
ISR_NOCODE 18

;Reserved
ISR_NOCODE 19

;Reserved
ISR_NOCODE 20

;Reserved
ISR_NOCODE 21

;Reserved
ISR_NOCODE 22

;Reserved
ISR_NOCODE 23

;Reserved
ISR_NOCODE 24

;Reserved
ISR_NOCODE 25

;Reserved
ISR_NOCODE 26

;Reserved
ISR_NOCODE 27

;Reserved
ISR_NOCODE 28

;Reserved
ISR_NOCODE 29

;Reserved
ISR_NOCODE 30

;Reserved
ISR_NOCODE 31

extern fault_handler

; We call a C function in here. We need to let the assembler know
; that '_fault_handler' exists in another file


; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
        pusha
        push ds
        push es
        push fs
        push gs
        mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov eax, esp   ; Push us the stack
        push eax
        mov eax, fault_handler
        call eax       ; A special call, preserves the 'eip' register
        pop eax
        pop gs
        pop fs
        pop es
        pop ds
        popa
        add esp, 8     ; Cleans up the pushed error code and pushed ISR number
        iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

;Adding IRQs below.

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; 32: IRQ0
irq0:
        cli
        push byte 0    ; Note that these don't push an error code on the stack:
                       ; We need to push a dummy error code
        push byte 32
        jmp irq_common_stub

; 33: IRQ1
irq1:
        cli
        push byte 0    
        push byte 33
        jmp irq_common_stub

; 34: IRQ2
irq2:
        cli
        push byte 0    
        push byte 34
        jmp irq_common_stub

; 35: IRQ3
irq3:
        cli
        push byte 0    
        push byte 35
        jmp irq_common_stub

; 36: IRQ4
irq4:
        cli
        push byte 0    
        push byte 36
        jmp irq_common_stub

; 37: IRQ5
irq5:
        cli
        push byte 0    
        push byte 37
        jmp irq_common_stub

; 38: IRQ1
irq6:
    cli
    push byte 0    
    push byte 38
    jmp irq_common_stub

; 39: IRQ2
irq7:
        cli
        push byte 0    
        push byte 39
        jmp irq_common_stub

; 40: IRQ3
irq8:
        cli
        push byte 0    
        push byte 40
        jmp irq_common_stub

; 41: IRQ4
irq9:
        cli
        push byte 0    
        push byte 41
        jmp irq_common_stub

; 42: IRQ5
irq10:
        cli
        push byte 0    
        push byte 42
        jmp irq_common_stub

; 43: IRQ1
irq11:
        cli
        push byte 0    
        push byte 43
        jmp irq_common_stub

; 44: IRQ2
irq12:
        cli
        push byte 0    
        push byte 44
        jmp irq_common_stub

; 45: IRQ3
irq13:
        cli
        push byte 0    
        push byte 45
        jmp irq_common_stub

; 46: IRQ4
irq14:
        cli
        push byte 0    
        push byte 46
        jmp irq_common_stub

; 47: IRQ5
irq15:
        cli
        push byte 0    
        push byte 47
        jmp irq_common_stub

extern irq_handler

; This is a stub that we have created for IRQ based ISRs. This calls
; '_irq_handler' in our C code. We need to create this in an 'irq.c'
irq_common_stub:
        pusha
        push ds
        push es
        push fs
        push gs
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov eax, esp ; esp is at what it is now. original - 44. for the handler to use
        push eax
        mov eax, irq_handler
        call eax
        pop eax ; in multitasking the esp will point to the other stack. return value of the handler. the pointer should be stored, of course before jumping to the other stack.
        pop gs
        pop fs
        pop es
        pop ds
        popa
        add esp, 8
        iret


old_return_address:
dd 0

old_return_value:
dd 0

; Attempts to perform a task switch.
; It switches between two tasks and saves
; the state of the task that was executed
; before the switch.
task_switch:
        pusha ; EAX, ECX, EDX, EBX, ESP (original value), EBP, ESI, and EDI.
        pushf ; EFLAGS
        ;mov ebp, esp
                                                                      ;44                                  44    40    36   32   28   24   20  16   12   8    4     0
        ;the stack now looks like this:                                                                    arg2, arg1, eip, eax, ecx, edx, ebx, esp, ebp, esi, edi, eflags] <---  esp
        ;[esp] is value of cr3 esp is the address of esp
        ;[esp + 4] is value of eflags. esp+4 is the address in which the value of eflags resides

        mov eax, [esp+40] ; this means that we take argument 1 (a memory address) and place that address into eax.
                          ;   4    8   12  16   20   24   28   32   36      40
        ;struct registers:eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags   ; eax gets read from memory first. and last cr3
        mov [eax + 4], ebx ; this means that we take the value of ebx (same value as the pushed ebx, now) and place it in the ebx spot for registers from.
        mov [eax + 8], ecx ; this means that we take the value of ecx and place it in the ecx spot for registers from. And so it goes on:
        mov [eax + 12], edx
        mov [eax + 16], esi
        mov [eax + 20], edi
        mov ebx, [esp + 12]
        mov [eax + 28], ebx ; ebp
        ;mov [eax + 24], ebx

        mov ebx, [esp + 32] ; [esp + 32] = eax

        mov [eax], ebx ; eax
        mov ebx, [esp + 36]
        mov [eax + 32], ebx; eip, now 0-20, 28, 32  has been taken care of. what is missing is 24 (esp), and 36 (eflags)

        mov ebx, [esp] ; eflags
        mov [eax + 36], ebx ; pushed eflags from the beginning of this callee func gets into eflags for the struct 

        mov edx, [esp + 16]
        mov [eax + 24], edx

        ;now time to take care of the other argument. which is the stack frame for function 2
        ;the stack frame looks the same, I guess: [etc..., argument 2, argument1, return value for caller function (old eip), eax, ecx, edx, ebx, esp, ebp, esi, edi, eflags]


        mov eax, [esp + 44] ; address for struct registers *to moves to eax


        ; this means that we now are in that stack frame for real
        ;push ecx ; push the return value for scheduler
        mov ebx, [eax + 4] ; now the argument values get moved into the registers instead of the other way around
        mov ecx, [eax + 8]
        mov edx, [eax + 12]
        mov esi, [eax + 16]
        mov edi, [eax + 20]
        mov ebp, [eax + 28] ; from ebp from struct to ebp
        push eax ; what gets pushed is argument 2 (pointer to registers to)
        mov eax, [eax + 36] ; eflags from struct registers to into eax
        push eax ; eflags from struct gets pushed
        popf ; eflags from struct into register eflags
        pop eax ; eax contains address again
        ;mov esp, [eax + 24]
        push eax ; push the address
        mov esp, [eax + 24] ; esp from struct into esp
        ;mov esp, ebp
        ; eax contains the address for argument 2
        mov eax, [eax + 32] ; eip from struct into eax. can look at this later to see if its needed
        sub esp, 4
        xchg [esp], eax ; at the beginning of the new stack frame there is the eip from struct registers to

        add esp, 4
        pop eax

        mov eax, [eax] ; eax from struct into eax
        sub esp, 8
        ret

gps_flag:
dd 0

intflag:
dd 0


SECTION .bss
        resb 8192               ; This reserves 8KBytes of memory here
sys_stack:
