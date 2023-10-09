# Create the actual disk image - 21MB
sudo dd if=/dev/zero of=/usr/local/share/bochs/usb.img bs=512 count=64260

 
# Make the partition table, partition and set it bootable.
sudo parted --script /usr/local/share/bochs/usb.img mklabel msdos mkpart p fat16 1 20 set 1 boot on
# Map the partitions from the image file
sudo kpartx -a /usr/local/share/bochs/usb.img
 
# sleep a sec, wait for kpartx to create the device nodes
sleep 1


sudo mkfs.msdos -F16 /dev/mapper/loop0p1
sudo mount /dev/mapper/loop0p1 /mnt


sudo mkdir -p /mnt/boot/grub
sudo cp /home/temploc/Documents/Brans/tmploc/boot/kernel.bin /mnt/boot
sudo cp /home/temploc/Documents/Brans/tmploc/boot/grub/grub.cfg /mnt/boot/grub

 
#sudo sync
# Use grub2-install to actually install Grub. The options are:
#   * No floppy polling.
#   * Use the device map we generated in the previous step.
#   * Include the basic set of modules we need in the Grub image.
#   * Install grub into the filesystem at our loopback mountpoint.
#   * Install the MBR to the loopback device itself.
#sudo grub-probe --target=fs mnt/boot/grub --
sudo grub-install --no-floppy --root-directory=/mnt --modules="normal part_msdos fat multiboot usb usbms" /usr/local/share/bochs/usb.img
sudo dd if=/usr/local/share/bochs/usb.img of=/usr/local/share/bochs/usb2.img bs=512 count=64260

