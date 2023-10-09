
sudo qemu-system-i386 -m 512 -drive file=johannos_kernel_usb.img,id=usbstick,if=none,format=raw -usb -device usb-ehci,id=ehci -device usb-storage,bus=ehci.0,drive=usbstick

