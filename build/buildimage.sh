# Create the actual disk image - 21MB
sudo dd if=/dev/zero of=/home/johannes/Documents/jkghkj/build/johnos_kernel_usb.img bs=512 count=64260

# Make the partition table, partition and set it bootable.
sudo parted --script /home/johannes/Documents/jkghkj/build/johnos_kernel_usb.img mklabel msdos mkpart p fat16 1 20 set 1 boot on

# Map the partitions from the image file
sudo kpartx -a /home/johannes/Documents/jkghkj/build/johnos_kernel_usb.img
 
# sleep a sec, wait for kpartx to create the device nodes
sleep 1
 
# Make a fat16 filesystem on the first partition.
sudo mkfs.msdos -F16 /dev/mapper/loop0p1

# Make the mount-point
sudo mount /dev/mapper/loop0p1 /mnt
 

sudo mkdir -p /mnt/boot/grub
sudo mkdir -p /mnt/docsa/docsb/docsc/docsd
sudo mkdir -p /mnt/docsa/docse

# Copy in the files from the staging directory
sudo cp /home/johannes/Documents/jkghkj/build/boot/kernel.bin /mnt/boot
sudo cp /home/johannes/Documents/jkghkj/build/boot/grub/grub.cfg /mnt/boot/grub


# Use grub2-install to actually install Grub. The options are:
#   * No floppy polling.
#   * Include the basic set of modules we need in the Grub image.
#   * Install grub into the filesystem at our loopback mountpoint.
#   * Install the MBR to the loopback device itself.
sudo grub-install --no-floppy --root-directory=/mnt --modules="normal part_msdos fat multiboot usb usbms" /home/johannes/Documents/jkghkj/build/johnos_kernel_usb.img

#Makes some files. You can change these files to anything less
#than 512 bytes. Also, only ASCII characters are supported.
#This kernel cannot support many folders yet. The tree array
#is only 4096 bytes. However, if you like to create new folders
#and files, please do it with care.
sudo echo "Hello." > /mnt/docsa/docsb/docsc/FILEA.TXT
sudo echo "Hello there." > /mnt/docsa/docsb/docsc/FILEB.TXT
sudo echo "Hi, I'm making the first files here." > /mnt/docsa/docsb/docsc/FILEC.TXT
sudo echo "It's going well." > /mnt/docsa/docsb/docsc/FILED.TXT
sudo echo "Soon finished with phase one". > /mnt/docsa/docsb/docsc/FILEE.TXT
sudo echo "There will be 4 phases I think." > /mnt/docsa/docsb/docsc/FILEF.TXT
sudo echo "The kernel is coming to town." > /mnt/docsa/docsb/docsc/FILEG.TXT
sudo echo "Say hi to this baby kernel." > /mnt/docsa/docsb/docsc/FILEH.TXT
sudo echo "This was supposed to be a long file. I'm trying to write something new here. This is a hobby project. It will be expanded." > /mnt/docsa/docse/FILEI.TXT
sudo echo "Thank you for your time." > /mnt/docsa/docsb/docsc/FILEJ.TXT
sudo echo "Please do other things, now." > /mnt/docsa/docsb/docsc/FILEK.TXT
sudo echo "Have a good day." > /mnt/docsa/docsb/docsc/FILEL.TXT
sudo sync
