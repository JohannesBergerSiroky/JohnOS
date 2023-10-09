Welcome to JohannOS.



This single task kernel will run an Enhanced Host Controller which often handles USB 2.0.
It will also go through a FAT16 file system to look for files and folders. You can change the
names and the contents of the files in the buildimage.sh file. Sometimes the USB stack
will crash and display an error message. If this happens a lot then go to the device folder
and then the usb folder. Look for the usb_mass_storage.c file and look for the set_addr
function. Here you can change the parameter of the kernel_delay function. You can set
the parameter to 10, for example. The same goes for the set_config function. You can also
change the parameters in the 3 instances of the kernel_delay function in the 
ehci_transfer_bulkstorage_data function. Here you can change the values to higher values.
You can double the values, for example. In the confim_data function you can look for the 
timer_ticks function and also set the parameter to a higher value.

This kernel has some other USB drivers which you can experiment with. I only got the XHCI
driver to work in a modified version of Bochs emulator in which it showed sectors in a
virtual USB pen drive. I could get a descriptor from the UHCI driver and also from the
OHCI driver using Qemu. The network driver is unfinished and the Ahci driver is a stub, so
it won't work much. The EHCI driver is the only USB driver that worked on real hardware.
I think that the computer which was used was from 2006-2008, and I guess it was a 
Lenovo or a HP computer.
Moreover, my testing of the kernel heap is almost finished and there is a multitasking
stub which might work in it's own ways. However those two files will be changed later.
The ehci linked list file is finished and can be tested after the function kmem_allocate
has been fully tested. 
 
To build:

You can build this OS only in a Linux environment.
Go to the build folder where the build_kernel.sh file resides, open it, and
make the folders fit your computer setup.
Have the terminal running in that folder and type ./build_kernel.sh.
After this, run link_to_kernel.sh to activate the linker.


To build and to emulate a USB flash memory with the OS on it in qemu, 
do this:

Go to the folder where buildimage.sh resides
Open it and change the folders so that they fit your computer setup.
Also open cleanimage.sh and runimageonqemu.sh so that they fit your setup.
Type sudo -s or log in as superuser somehow
Type ./buildimage.sh in the terminal and wait a few seconds
Type ./runimageonqemu.sh
Note: you may want to change some parameters in the runimageonqemu.sh file,
such as the -m option which defines how many megabytes of memory
the qemu should emulate.

After you are finished with qemu and the operating system, please type
./cleanimage.sh to make sure to delete all files and loops associated with it.
You may want to have the terminal opened and type ./runimageonqemu.sh again to 
run the OS again, before typing ./cleanimage.sh. 
clean.sh is used to clean up and quit.
To run the OS again: simply repeat the abovementioned process.


To run the OS on a real USB flash memory, do this:

Go to the build folder where buildimageonrealusb.sh resides
Make the USB file fit your computer setup. For example sdb can be
sdc on your system.
Log in as superuser.
Insert a USB flash drive.
Delete the partition(s) and the partition table. Formatting is not needed.
Type ./buildimageonrealusb.sh and wait.
The USB flash drive should be ready to use by now.
Note: the file system is FAT16 and this should not be changed. So far the OS only suppors FAT16.

If booting does not work you could change some parameters in the grub.cfg-file at /boot/grub
The partition table msdos for example, could be changed to msdos0 or msdos2 depending
on how the BIOS finds it. You may encounter and error and may have to change the msdosx value and
build all over again. But if you encounter an error, it is recommended to enter the grub command shell
and type 'ls', and figure out what the value of the x in msdosx should be for the OS.
Type 'c', in the grub-menu to enter the grub command shell.

You could also type, in the command shell:
set root=(hd0,msdosx) or set root=(hd0, 1)
multiboot /boot/kernel.bin
boot
Note: It is likely that x = 1. So hd0,msdos1.

