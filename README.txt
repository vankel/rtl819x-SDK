        REALTEK 11nRouter SDK(based on linux-3.10)- v3.4.9.3
        ----------------------------------------------------

Package List
============
  1. rtl819x.tar.gz                     - containing the source code of linux-3.10 v3.4.9.3 sdk
  2. rtl819x-bootcode-SDK-v3.4.9.3.tar.gz
  3. README.txt                         - this file
  4. INSTALL.txt                        - how to build code 
  5. Document.tar.gz                    - containing the documents for this SDK
  6. image.tar.gz                       - containning the images of each kind of combination.
	 - The images is specially builded for release.
         - It's combines the default configuration with firmware in order to avoid the MIB conflicts.
            image/fw_98C_8814_8194.bin    	    - gateway image for 8198C+8814+8194
            image/fw_98C_8814_HP_8194_HP.bin        - gateway image for 8198C+8814+8194 HP
	    image/98C_8814_8194_nfjrom    	    - MP image for 8198C+8814+8194
	    image/98C_8814_HP_8194_HP_nfjrom        - MP image for 8198C+8814+8194 HP
	    image/boot_98c.bin                      - bootloader for 8198C+8814+8194

Environment
===========
  Fedora 9, Ubuntu 8.10/9.10 are recommended

Install the linux-3.10 sdk package
==================================
  1. Copy 'rtl819x.tar.gz' to a file directory on a Linux PC
  2. Type 'tar -zxvf rtl819x.tar.gz' to extract the package
 
Install the bootcode package
============================
  Type 'tar -zxvf rtl819x-bootcode-SDK-v3.4.9.3.tar.gz' to extract the package 

build the linux kernel/rootfs/bootcode
======================================
  follow the INSTALL.txt file
