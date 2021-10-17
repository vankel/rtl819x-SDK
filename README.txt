		REALTEK 11nRouter SDK(based on linux-3.10)- v3.4.7.3
		----------------------------------------------------

Package List
============
  1. rtl819x.tar.gz                        - containing the source code of linux-3.10 v3.4.7.3 sdk
  2. rtl819x-bootcode-SDK-v3.4.7.3.tar.gz
  3. README.txt                            - this file
  4. INSTALL.txt                           - how to build code 
  5. Document.tar.gz                       - containing the documents for this SDK
  6. image/tar.gz                          - containning the images of each kind of combination.
	 - The images is specially builded for release.
	 - It's combines the default configuration with firmware in order to avoid the MIB conflicts.
	image/fw_96E_92E.bin                - gateway image for 8196E+92E
    image/fw_98C_92E_8812.bin    	    - gateway image for 8198C+8812+92E
	image/fw_98CD_92E_8812.bin          - gateway image for 8198CD+8812+92E
	image/fw_98CS_92E_8812.bin          - gateway image for 8198CS+8812+92E
    image/fw_98C_nand.bin               - gateway image for 8198C+8812+92E Nand flash

    image/96E_92E_nfjrom                - MP image for 8196E+92E
	image/98C_92E_8812_nfjrom           - MP image for 8198C+8812+92E

	image/boot_96e.bin                  - bootloader for 8196e+92e
	image/boot_98c.bin                  - bootloader for 8198C+8812+92E
    image/boot_98c_nand.bin             - bootloader for 8198C NAND Flash

Environment
===========
  Fedora 9, Ubuntu 8.10/9.10 are recommended


Install the linux-2.6 sdk package
=================================
  1. Copy 'rtl819x.tar.gz' to a file directory on a Linux PC
  2. Type 'tar -zxvf rtl819x.tar.gz' to extract the package
 
Install the bootcode package
============================= 
  Type 'tar -zxvf rtl819x-bootcode-SDK-v3.4.7.3.tar.gz' to extract the package 

build the linux kernel/rootfs/bootcode
======================================
  follow the INSTALL.txt file
