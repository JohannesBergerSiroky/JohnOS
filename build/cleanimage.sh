#sudo rm /mnt/boot/grub/grub.cfg
#sudo rm /mnt/boot/grub/device.map
#sudo rm /mnt/boot/kernel.bin


sudo rm -r /mnt/docsa
sudo rm -r /mnt/boot/grub
sudo rm -r /mnt/boot
sudo umount /mnt
 
# Unmap the image after having used it with Qemu
sudo kpartx -d /home/johannes/Documents/jkghkj/build/johnos_kernel_usb.img
sleep 2
sudo rm /home/johannes/Documents/jkghkj/build/johnos_kernel_usb.img
