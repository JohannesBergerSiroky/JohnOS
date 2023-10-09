nasm -f elf32 ../kernel/start_assorted.asm -o start_assorted.o

gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/kernel.c -o kernel.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/util.c -o util.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/graphics/screen.c -o screen.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/string.c -o string.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/system.c -o system.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/gdt.c -o gdt.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter

gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/idt.c -o idt.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/isr.c -o isr.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/irq.c -o irq.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/timer.c -o timer.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/mem.c -o mem.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/device.c -o device.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/kb.c -o kb.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/pci.c -o pci.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/usb/usb_ehci.c -o ehci.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/usb/usb_ehci_ll.c -o ehci_ll.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter

gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/usb/usb_hubs.c -o usb_hubs.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/usb/usb.c -o usb.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/usb/usb_mass_storage.c -o usb_mass_storage.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/fs/fat16.c -o fat16.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/usb/usb_uhci.c -o usb_uhci.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/usb/usb_ohci.c -o usb_ohci.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/network/using_ethernet.c -o using_ethernet.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/multitasking.c -o multitasking.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/power/using_acpi.c -o using_acpi.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter

gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/power/check_enable_acpi.c -o check_enable_acpi.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/power/check_shutdown_acpi.c -o check_shutdown_acpi.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/usb/usb_xhci_prints.c -o usb_xhci_prints.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/usb/usb_xhci.c -o usb_xhci.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/hdd/ahci.c -o ahci.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector -m32 -c ../kernel/device/hdd/ahci_prints.c -o ahci_prints.o -std=gnu99 -ffreestanding -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector  -m32 -c ../kernel/kernel_heap_alloc_nopaging.c -o kernel_heap_alloc_nopaging.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wno-unused-parameter
gcc -fno-omit-frame-pointer -fno-stack-protector  -m32 -c ../kernel/test_kernel_heap.c -o test_kernel_heap.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wno-unused-parameter




