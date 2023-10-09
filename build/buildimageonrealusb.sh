# Fill the first 20MB in the USB-stick with zeros:
sudo dd if=/dev/zero of=/dev/sdc bs=512 count=40960

  
 
# Make the partition table, partition, and set it (don't know if the usb-stick will be bootable by this, or the partition) bootable.
sudo parted --script /dev/sdc mklabel msdos mkpart p ext2 1 20 set 1 boot on

 
# sleep a sec
sleep 1
 
# Make an ext2 filesystem on the partition (sdc1). The rest (sdc) will have no filesystem 


sudo mkfs.ext2 /dev/sdc1

sudo mount /dev/sdc1 /mnt


sudo mkdir -p /mnt/boot/grub
sudo cp /home/temploc/Brans/tmploc/boot/kernel.bin /mnt/boot
sudo cp /home/temploc/Brans/tmploc/boot/grub/grub.cfg /mnt/boot/grub


#install grub on the 1st 1MB where there's no file system
sudo grub-install --no-floppy --root-directory=/mnt --modules="normal part_msdos ext2 multiboot" /dev/sdc


sudo sync
