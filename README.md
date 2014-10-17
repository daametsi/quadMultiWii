quadMultiWii
============

Install Edimax wifi

http://embeddedprogrammer.blogspot.com/2013/01/beaglebone-using-usb-wifi-dongle-to.html


Install stuff:

$ sudo apt-get install gcc make 


Disable HDMI cape etc.

I. locate uEnv.txt enter '${EDITOR} ${DIR}/uEnv.txt'

eg. $ nano /boot/uEnv.txt

II. modify contents search for 'optargs' and insert/add the following: 

optargs=quiet drm.debug=7 capemgr.disable_partno=BB-BONELT-HDMI,BB-BONELT-HDMIN

III. verify enter 'cat /sys/devices/bone_capemgr.*/slots' all disabled devices should
be missing an L in the following sequence "P-O-L" and should appear something like this "P-O--"

Pull project from repo:

  $ cd
  
  $ mkdir ./quad
  
  $ cd ./quad
  
  $ git init
  
  $ git remote add origin ${THIS_REPO}
  
  $ git pull origin master

Navigate to pinmux:

  $ cd pinmux

Make and apply cape profile:

  $ dtc -O dtb -o BB-BONE-PRU-00A0.dtbo -b 0 -@ BB-BONE-PRU-00A0.dts
  
  $ cp ./BB-BONE-PRU-00A0.dtbo /lib/firmware
  
  $ echo BB-BONE-PRU > /sys/devices/bone_capemgr.9/slots

Check slots:
  
  cat /sys/devices/bone.capemgr.9/slots
  
%%Add Device Tree Overlays to initrd%%

Create /etc/initramfs-tools/hooks/dtbo:

    #!/bin/sh
    
    set -e
    
    . /usr/share/initramfs-tools/hook-functions
    
    # Copy Device Tree fragments
    mkdir -p "${DESTDIR}/lib/firmware"
    cp -p /lib/firmware/*.dtbo "${DESTDIR}/lib/firmware/"

Make executable

  $ sudo chmod +x /etc/initramfs-tools/hooks/dtbo
  Backup initrd:
  
  $ sudo cp /boot/uboot/initrd.img /boot/uboot/initrd.img.bak
  Create/update initramfs:
  
  $ sudo /opt/scripts/tools/update_initrd.sh
  
Reboot

  $ reboot
  
Install PRU:

http://analogdigitallab.org/articles/beaglebone-black-introduction-pru-icss








