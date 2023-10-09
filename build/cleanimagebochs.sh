

#sudo rm /mnt/boot/grub/fonts/*
#sudo rm /mnt/boot/grub/i386-pc/*
#sudo rm /mnt/boot/grub/locale/*
#sudo rm /mnt/boot/grub/grubenv
#sudo rm /mnt/boot/grub/grub.cfg
#sudo rm /mnt/boot/kernel.bin
#sudo rm /mnt/boot/device.map
#sudo rm -d /mnt/boot/grub/fonts
#sudo rm -d /mnt/boot/grub/locale
#sudo rm -d /mnt/boot/grub/i386-pc

#sudo rm /dev/mapper/loop0p1
#sudo umount /dev/loop0p1
#sudo losetup -d /dev/loop0p1
#sudo losetup -d /dev/loop0

#sudo rm -d /mnt/boot/grub
#sudo rm -d /mnt/boot



sudo rm -r /mnt/boot/grub
sudo rm -r /mnt/boot


#sudo rm /mnt/FILE1.TXT
sudo umount /mnt
 
# Unmap the image after having used it with Qemu
sudo kpartx -d /usr/local/share/bochs/usb.img
sleep 2
sudo rm /usr/local/share/bochs/usb2.img
sudo rm /usr/local/share/bochs/usb.img
#sudo rm /usr/local/share/bochs/usb.img.lock
#losetup -d /dev/loop0
#sudo rm /mnt/boot/grub/grub.cfg
#sudo rm /mnt/boot/grub/device.map
#sudo rm /mnt/boot/kernel.bin

#sudo rm -d /mnt/boot/grub
#sudo rm -d /mnt/boot

