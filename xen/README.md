# Xen Support

This guide explains how to boot Linux 6.1.y as Xen dom0 on the Chromebook xe303c12, aka Snow and configure and start a very basic domU guest. Xen support requires a Linux kernel with support for runing as dom0 on the Xen hypervisor. For more information, please refer to the help pages at https://xenproject.org.

## Prerequisites
This guide presumes the Chromebook Snow is already configured to boot Linux 5.4.257 with kvm support and Devuan 5 from an SD card as described in the top-level README of this repository.

The partiton table should look like:
```
user@devuan-bunsen ~ % sudo gdisk -l /dev/mmcblk1
GPT fdisk (gdisk) version 1.0.9

Partition table scan:
  MBR: protective
  BSD: not present
  APM: not present
  GPT: present

Found valid GPT with protective MBR; using GPT.
Disk /dev/mmcblk1: 249737216 sectors, 119.1 GiB
Sector size (logical/physical): 512/512 bytes
Disk identifier (GUID): 812B6AA8-B586-4B26-B8D2-08CEE675890A
Partition table holds up to 128 entries
Main partition table begins at sector 2 and ends at sector 33
First usable sector is 34, last usable sector is 249737182
Partitions will be aligned on 2048-sector boundaries
Total free space is 23410654 sectors (11.2 GiB)

Number  Start (sector)    End (sector)  Size       Code  Name
   1            2048           34815   16.0 MiB    7F00
   2           34816           67583   16.0 MiB    7F00
   3           67584         1116159   512.0 MiB   8300
   4         1116160        21528542   9.7 GiB     8300
user@devuan-bunsen ~ %
```
The fourth partition might be much larger than shown here, but the other 3 partitions should have all the settings shown here, and the fourth partiton should start at sector 1116160 and have Code 8300 just as the third partition does. If the fourth partition is smaller in size than the one shown here, expand it to at least the size shown above (9.7 GB) before trying the first method below of overwriting partitions 3 and 4 with new filesystem images that are provided in the links in this README file. After installing the new filesystem images, use resize2fs to expand the filesystem in partition 4 to the full size of the fourth partition, if the fourth partition is larger than the size of 9.7 GB shown above.

On the first partition is the u-boot image that enables hypervisor mode, the second partition is unused, the third partition is mounted at /boot, and the fourth partition is mounted as the root partition.

## Adding support for Xen as a new installation (destroys user data in the current installation)
This method of adding Xen support destroys the data on the root partition and the /boot partition and replaces it with new filesystem images that include both the 5.4.257-chromarietto-exy kernel with kvm support and the 6.1.61-stb-xen-cbe+ kernel with Xen support. Make backups of any data on the root partition or the /boot partition that you want to keep before proceeding with this method.

These filesystem images are created from the Devuan 5 image here (the same source for Devuan 5 as in the top-level README):

https://drive.google.com/u/0/uc?id=1-obTOWKIbgRjQZDjjQazbOtPFHboeA8N&export=download
(source : https://github.com/quarkscript/linux-armv7-xe303c12-only)

For this method, do not use the xe303c12_vubu_xfce_1882M_btrfs_5.15.79.img directly. Instead, download these two image files derived from it:

1. https://drive.google.com/file/d/1ybbzb7SOArC8vd7W7muv1IAXpT-FBoPE/view?usp=drive_link

2. https://drive.google.com/file/d/1nD5T--3oPeFBRAR_EekHoVK9oK_Xrdwl/view?usp=drive_link

The two files are xe303c12_vubu-xen-BOOT3.img.gz and xe303c12_vubu-xen-ROOT4.img.gz

They correspond to the third and fourth partition of the SD card after it is installed with Devuan 5 and the Linux version 5.4.257-chromarietto-exy kernel with support for kvm on the Chromebook Snow.

This method overwrites the filesystems in the third and fourth partitions with the filesystem images in these two files, so this is another warning to backup any data on the third or fourth partition of the SD card that needs to be preserved.

Either remove the SD card from the Chromebook and plug it into another computer where its raw partitions can be accessed directly, or if the Chromebook Snow is configured to be able to boot into Chrome OS Developer Mode, boot into Chrome OS developer mode (Ctrl + d) and then when Chrome OS developer mode is fully loaded, go to the terminal command line interface (Ctrl-Alt->) and first check if either /dev/mmcblk1p3 or /dev/mmcblk1p4 is mounted and if so, unmount them both. Then copy the xe303c12_vubu-xen-BOOT3.img.gz and xe303c12_vubu-xen-ROOT4.img.gz files to the device where the SD card is installed. If using Chrome OS developer mode, access to downloaded files is in the /home/chronos/Downloads folder. 

3. Overwrite partition 3 and partition 4 with the downloaded images :

**Warning! - make sure the parameters of=/dev/mmcblk1p3 and of=/dev/mmcblk1p4 are the actual partitions on the SD card that the Chromebook boots from. A mistake here will cause data loss. You have been warned.**
```
localhost ~/Downloads $ gzip -c -d xe303c12_vubu-xen-BOOT3.img.gz | sudo dd of=/dev/mmcblk1p3 bs=1M
localhost ~/Downloads $ gzip -c -d xe303c12_vubu-xen-ROOT4.img.gz | sudo dd of=/dev/mmcblk1p4 bs=1M
```
Now the SD card should be prepared to boot into either the normal 5.4.257-chromarietto-exy system or into Xen and dom0 with Linux version 6.1.59, following step 8 in the section below, near the bottom of this page, on adding Xen support as an upgrade.

4. Since the updated Linux kernel version 6.1.61 has a patch to fix a bug that causes a black screen on the display, download the 6.1.61 kernel from here :

https://github.com/mobile-virt/arm-legacy-kvm/releases/download/2023-11-04/linux-6.1.61-stb-xen-cbe+-arm.tar.gz

5. Install the 6.1.61-stb-xen-cbe+ kernel with the patch to fix the dark screen bug :

Replace all the Linux kernel 6.1.59-stb-xen-cbe+ files on the /dev/mmcblk1p3 and /dev/mmcblk1p4 partitions with corresponding files for Linux kernel version 6.1.61-stb-xen-cbe+. These are the zImage-6.1.59-stb-xen-cbe+ file, the config-6.1.59-stb-xen-cbe+ file, the exynos5250-snow-6.1.59-stb-xen-cbe+.dtb file, and the bootxen.scr file on partition 3 of the SD card, and the lib/modules/6.1.59-stb-xen-cbe+ directory and all files under that directory on partition 4 of the SD card, and they should be replaced with the corresponding files for kernel version 6.1.61-stb-xen-cbe+. Except for the bootxen.scr file, these files can be extracted from the linux-6.1.61-stb-xen-cbe+-arm.tar.gz tarball. Follow the steps in the section Adding support for Xen as an upgrade at steps 6 and 7 to create and install the bootxen.scr file for Linux version 6.1.61-stb-xen-cbd+ on partition 3 of the SD card. Optionally delete the files for the 6.1.59-stb-xen-cbd+ kernel.

## Creating the filesystem images (optional)
These images should be created as follows with a 6.1.61-stb-xen-cbe+ kernel with the patch that fixes the dark screen bug (the actual kernel on the images at the download links above is still 6.1.59) :

1. Complete the steps in the README of the top level of this repository to install Devuan 5 and the 5.4.257-chromarietto-exy kernel with support for kvm.

2. Boot into the Devuan 5 system, the initial password for user is 'q' (it should be changed!) and user is a member of the sudo group. Then run `sudo apt-get update && sudo apt-get upgrade` after configuring the system for network access and setting the correct date and time. More information about the lightweight Bunsen Labs system which uses the OpenBox Window manager that is used with this image is here : https://www.bunsenlabs.org/

3. There will be an error from apt-get at this point, the bunsen-conky package fails on the first try because it conflicts with the bunsen-configs package. Fortunately, this is easy to fix :

`user@devuan-bunsen ~ % sudo apt-get remove bunsen-configs`

This will take a little while, but it should now succeed with the upgrade.

Then after apt-get is happy again:
```
user@devuan-bunsen ~ % sudo apt-get install bunsen-configs
user@devuan-bunsen ~ % sudo apt-get upgrade
user@devuan-bunsen ~ % sudo apt-get dist-upgrade
user@devuan-bunsen ~ % sudo apt-get autoremove
```
4. Install the Xen software packages and optionally clean the package download cache :
```
user@devuan-bunsen ~ % sudo apt-get install xen-system-armhf
user@devuan-bunsen ~ % sudo apt-get clean
```
5. To boot Xen using u-boot, it is necessary to wrap the xen-4.17-armhf kernel in u-boot format with a LOADADDR set appropriately :
```
user@devuan-bunsen ~ % cd /boot
user@devuan-bunsen /boot % sudo mkimage -A arm -T kernel -C none -a 0x51004000 -e 0x51004000 -d xen-4.17-armhf xen-4.17-armhf-armmp-0x51004000.ub
```
6. Edit /var/lib/connman/settings - set WiFi enable to true

7. Check that /etc/fstab has the two mount points for / and /boot (/ is ext4 and /boot is ext2) and remove any entries that are no longer valid. It should like :
```
LABEL=ROOT /    ext4    defaults 0       0
LABEL=BOOT /boot    ext2    defaults 0       0
```
8. Install the Linux 6.1.61-stb-xen-cbe+ kernel with Xen support according to the instructions below for adding support for Xen as an upgrade, steps 1 and 2.

9. Some 6.1.y kernels look for init at /init instead of /sbin/init. So create a symbolic like as follows if it does not exist :
```
user@devuan-bunsen ~ % cd /
user@devuan-bunsen / % sudo ln -s sbin/init init
```
10. Follow the steps in the next section Adding support for Xen as an upgrade at steps 6 and 7 to create and install the bootxen.scr file in /boot. Only the bootxen.scr file needs to be installed on the image at /boot, the bootxen.source file is only necessary to create the binary bootxen.scr using mkimage and can remain in /home/user.

11. Shutdown the Chromebook and either reboot the Chromebook in Chrome OS mode or move the SD card to another computer to create the images with commands like this :
```
localhost ~/Downloads $ sudo dd if=/dev/mmcblk1p3 bs=1M | gzip - > xe303c12_vubu-xen-BOOT3.img.gz
localhost ~/Downloads $ sudo dd if=/dev/mmcblk1p4 bs=1M | gzip - > xe303c12_vubu-xen-ROOT4.img.gz
```
## Adding support for Xen as an upgrade (does not destroy the current installation)
To install a kernel with Xen support as an upgrade of the installation with support for kvm created by following the instructions on the main README of this repository, follow these steps on the Chromebook to be upgraded with Xen support :

1. Download the kernel from here : https://github.com/mobile-virt/arm-legacy-kvm/releases/download/2023-11-04/linux-6.1.61-stb-xen-cbe+-arm.tar.gz

2. Extract the necessary kernel files into the filesystem from the tar.gz kernel package :
```
user@devuan-bunsen ~ % cd /
user@devuan-bunsen / % sudo tar xfpz ~/Downloads/linux-6.1.61-stb-xen-cbe+-arm.tar.gz boot/zImage-6.1.61-stb-xen-cbe+
user@devuan-bunsen / % sudo tar xfpz ~/Downloads/linux-6.1.61-stb-xen-cbe+-arm.tar.gz lib/modules
user@devuan-bunsen / % sudo tar xfpz ~/Downloads/linux-6.1.61-stb-xen-cbe+-arm.tar.gz boot/config-6.1.61-stb-xen-cbe+
user@devuan-bunsen / % sudo tar xfpz ~/Downloads/linux-6.1.61-stb-xen-cbe+-arm.tar.gz boot/dtbs/6.1.61-stb-xen-cbe+/exynos5250-snow.dtb
user@devuan-bunsen / % sudo mv boot/dtbs/6.1.61-stb-xen-cbe+/exynos5250-snow.dtb boot/exynos5250-snow-6.1.61-stb-xen-cbe+.dtb
user@devuan-bunsen / % sudo rmdir boot/dtbs/6.1.61-stb-xen-cbe+
```
This next one is optional and is only needed if booting this 6.1.61 kernel without Xen is desired or can be done later :

`user@devuan-bunsen / % sudo tar xfpz ~/Downloads/linux-6.1.61-stb-xen-cbe+-arm.tar.gz boot/uImage-6.1.61-stb-xen-cbe+`

3. Next it is necessary to install the Xen system software packages and it is recommended to upgrade the packages :
```
user@devuan-bunsen / % sudo apt-get update
user@devuan-bunsen / % sudo apt-get install xen-system-armhf
user@devuan-bunsen / % sudo apt-get upgrade
user@devuan-bunsen / % sudo apt-get dist-upgrade
user@devuan-bunsen / % sudo apt-get clean && sudo apt-get autoremove
```
If apt-get returns an error with the bunsen-configs or bunsen-conky packages during the `apt-get upgrade` step, the fix is described above at step 3 in the section above on creating the filesystem images.
4. To boot Xen using u-boot, it is necessary to wrap the xen-4.17-armhf kernel in u-boot format with a LOADADDR set appropriately :
```
user@devuan-bunsen / % cd boot
user@devuan-bunsen /boot % sudo mkimage -A arm -T kernel -C none -a 0x51004000 -e 0x51004000 -d xen-4.17-armhf xen-4.17-armhf-armmp-0x51004000.ub
```
5. Some 6.1.y kernels looks for init at /init instead of /sbin/init. So create a symbolic like as follows if it does not exist :
```
user@devuan-bunsen /boot % cd /
user@devuan-bunsen / % sudo ln -s sbin/init init
```
6. Create the u-boot shell commands that will be used to boot Xen and dom0.

Create a file in a /home/user named bootxen.source with these contents :
```
mmc dev 1 && mmc rescan 1
ext2load mmc 1:3 0x42000000 zImage-6.1.61-stb-xen-cbe+
ext2load mmc 1:3 0x51000000 xen-4.17-armhf-armmp-0x51004000.ub
ext2load mmc 1:3 0x5ffec000 exynos5250-snow-6.1.61-stb-xen-cbe+.dtb
fdt addr 0x5ffec000
fdt resize 1024
fdt set /chosen \#address-cells <0x2>
fdt set /chosen \#size-cells <0x2>
fdt set /chosen xen,xen-bootargs "console=dtuart dtuart=serial0 dom0_mem=1G dom0_max_vcpus=2 bootscrub=0 vwfi=native"
fdt mknod /chosen dom0
fdt set /chosen/dom0 compatible  "xen,linux-zimage" "xen,multiboot-module" "multiboot,module"
fdt set /chosen/dom0 reg <0x0 0x42000000 0x0 0x7D7200 >
fdt set /chosen xen,dom0-bootargs "console=tty1 root=/dev/mmcblk1p4 rw rootwait clk_ignore_unused --no-log"
bootm 0x51000000 - 0x5ffec000
```
The hex value 0x7D7200 is the size of the zImage-6.1.61-stb-xen-cbe+ file, and that value is computed from the uboot-script-gen script available from here : https://gitlab.com/ViryaOS/imagebuilder

Please note that most of the other values in the script generated by the ViryaOS uboot-script-gen do not work correctly with the Chromebook Snow, but the script does correctly calculate the size of the dom0 Linux kernel image.

7. Create the binary script file that u-boot can load and install it in /boot :
```
user@devuan-bunsen ~ % mkimage -A arm -T script -C none -a 0x50000000 -e 0x50000000 -d bootxen.source bootxen.scr
user@devuan-bunsen ~ % sudo mv bootxen.scr /boot && sudo chown root:root /boot/bootxen.scr
```
8. To boot Xen and the Linux kernel as dom0 on the Chromebook :

Before the 3 second timout expires after rebooting or turning on the Chromebook, press a key to escape into the u-boot shell.

Then type these commands to boot Xen and Linux 6.1.61 as dom0 :
```
SMDK5250 # mmc dev 1
SMDK5250 # ext2load mmc 1:3 0x50000000 bootxen.scr; source 0x50000000
```
Expected result: The Chromebook will boot into the Devuan 5 Bunsen labs desktop and automatically log into the user account. Check to see that Xen is working by reproducing this result (initial password is 'q' and it should be changed):
```
user@devuan-bunsen ~ % sudo xl list
[sudo] password for user:
Name                                        ID   Mem VCPUs      State   Time(s)
Domain-0                                     0  1024     2     r-----     217.3
user@devuan-bunsen ~ %
```
Note that there is another minor bug, and it is that when running on Xen, at shutdown the power does not turn off automatically and it is necessary to hold the power button for several seconds for the power to turn off the Chromebook.

Other bugs: Most functions work, but sound does not with this kernel, and videos don't render well either with this kernel and video drivers.

## Starting a domU guest
On Xen, dom0 is usually both the management domain where the toolstack to manage guests is installed. Also, dom0 is usually the hardware domain that has privileged access to most of the hardware in the system, such as the real disk, wifi network interface, graphics display, etc. This system is configured so dom0 is both the privileged domain with direct access to the hardware and the management domain. Other domains are referred to as domU, the U stands for unprivileged and these unprivileged guests rely on paravirtualized (PV) drivers for disk and network access, video output, HID input devices, etc. The following steps describe how to configure and start a Debian domU guest.

1. Since the Chromebook Snow is a low-memory system with 2 GB of memory, first reduce the allocation of memory for dom0 from 1 GB to 768 MB. This setiing is the dom0_mem setting in bootxen.source as follows in the line that sets the Xen boot arguments in bootxen.source:
```
fdt set /chosen xen,xen-bootargs "console=dtuart dtuart=serial0 dom0_mem=768M dom0_max_vcpus=2 bootscrub=0 vwfi=native"
```
Then re-generate and install the updated bootxen.scr in /boot and reboot so dom0 will now have 768 MB allocated to it:
```
user@devuan-bunsen ~ % mkimage -A arm -T script -C none -a 0x50000000 -e 0x50000000 -d bootxen.source bootxen.scr
user@devuan-bunsen ~ % sudo mv bootxen.scr /boot && sudo chown root:root /boot/bootxen.scr
```
2. Create a file named debian.cfg in a directory to store domU configuration files with these contents :
```
kernel = '/data/kernels/zImage-6.1.61-stb-xen-cbe+'
memory = '1152'
name = 'debian'
vcpus = '2'
disk = [ '/data/debian.img,,xvda,w' ]
extra = 'console=hvc0 root=/dev/xvda rw init=/sbin/init xen-fbfront.video=24,1024,768'
```
The xen-fbfront.video setting is for the color depth and geometry of the virtual framebuffer (vfb) device.

3. The same kernel for dom0 can be used for the domU guest:
```
user@devuan-bunsen ~ % sudo mkdir -p /data/kernels
user@devuan-bunsen ~ % sudo cp -p /boot/zImage-6.1.61-stb-xen-cbe+ /data/kernels/zImage-6.1.61-stb-xen-cbe+
```
4. Obtain a useable root filesystem (rootfs) image file debian.img and install it at /data/debian.img
There are many ways to obtain the rootfs filesystem image. If a Debian type system is desired, debootstrap can be used to create the image as described here: https://wiki.debian.org/Debootstrap

It is important to note the base system on the rootfs must be compatible with the Debian armhf architecture to run it on the Chromebook Snow. The method of creating the guest disk image, debian.img, using debootstrap from a system that is already running on the Debian armhf architecture has been tested and it works on the Chromebook Snow.

Another possible way to obtain a useable debian.img, not tested, is to obtain a filesystem image from Debian installation sites and mirrors here https://deb.debian.org/debian/dists/bookworm/main/installer-armhf/current/images/hd-media/ and search the contents for the file with the root filesystem and copy the root filesystem into the filesystem image debian.img.

5. Try to start the guest:
```
user@devuan-bunsen ~ % sudo xl create debian.cfg -c
```
The -c option connects immediately to the guest console, so if the guest boots correctly the kernel log messages should appear and eventually a login prompt will appear.

6. Use the key sequence to escape from the guest terminal back to the dom0 terminal. The default is ctrl-], but it may be different for different keyboard layouts, so search the Internet for the correct key sequence for the keyboard in use.

7. Other useful xl commands: `sudo xl list` lists the guests that are on the system with some information about their state, `sudo xl shutdown < name | domID >` shuts down a running domain, and `sudo xl console < name | domID >` connects to a guest's console.

8. Consult the `xl.cfg(5)` man page to extend the capabilities of the guest, such as adding a PV network device with the vif `xl.cfg(5)` setting, or to add other images for swap space or for other filesystems that can be added to the guest using the disk `xl.cfg(5)` setting and adding the disk image to the /etc/fstab file.

9. Some capabilities, such as the vfb device, require the qemu device model, and if it is not installed this error is returned on Debian and Debian drivative systems :
```
user@devuan-bunsen ~ % sudo xl create debian.cfg
Parsing config from debian.cfg
libxl: error: libxl_dm.c:2899:libxl__spawn_local_dm: Domain 15:device model /usr/libexec/xen-qemu-system-i386 is not executable: No such file or directory
libxl: error: libxl_dm.c:2901:libxl__spawn_local_dm: Domain 15:Please install the qemu-system-xen package for this domain to work
libxl: error: libxl_dm.c:3151:device_model_spawn_outcome: Domain 15:(null): spawn failed (rc=-3)
libxl: error: libxl_dm.c:3371:device_model_postconfig_done: Domain 15:Post DM startup configs failed, rc=-3
libxl: error: libxl_create.c:1896:domcreate_devmodel_started: Domain 15:device model did not start: -3
libxl: error: libxl_domain.c:1183:libxl__destroy_domid: Domain 15:Non-existant domain
libxl: error: libxl_domain.c:1137:domain_destroy_callback: Domain 15:Unable to destroy guest
libxl: error: libxl_domain.c:1064:domain_destroy_cb: Domain 15:Destruction of domain failed
user@devuan-bunsen ~ %
```
Debian only the provides the qemu-system-xen package for the amd64 architecture, so a build for armhf is provided here :

https://github.com/mobile-virt/arm-xen-kvm/releases/download/2023-11-07-qemu/qemu-system-xen_7.2+dfsg-7+deb12u2_armhf.deb

The package has been tested to work with a setting like this in the `xl.cfg(5)` file:
```
vfb = [ 'type=vnc,vnclisten=0.0.0.0,vncdisplay=1' ]
```
### Copyright
Original (C) Copyright 2000 - 2012
Wolfgang Denk, DENX Software Engineering, wd@denx.de.

See file CREDITS for list of people who contributed to this
project.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston,
MA 02111-1307 USA
