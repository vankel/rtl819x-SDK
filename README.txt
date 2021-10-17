		REALTEK 11nRouter SDK(based on linux-2.6.30)- v3.4.6.6
		------------------------------------------------------

Package List
============
  1. rtl819x.tar.gz  - containing the source code of linux-2.6 v3.4.6.6 sdk
  2. rtl819x-bootcode-SDK-v3.4.6.6-96d-96e.tar.gz
  3. rtl819x-bootcode-SDK-v3.4.6.6-97d_8367.tar.gz
  4. rtl819x-bootcode-SDK-v3.4.6.6-98-96c.tar.gz
  5. rtl819x-bootcode-SDK-v3.4.6.6-8881A.tar.gz
  6. README.txt      - this file
  7. INSTALL.txt     - how to build code 
  8. Document.tar.gz - containing the documents for this SDK
  9. image.tar.gz    - containning the images of each kind of combination.
	                 - The images is specially builded for release.
	 	             - It's combines the default configuration with firmware in order to avoid the MIB conflicts.
	image/fw_8881A_8367_92E.bin           - gateway image for 8881AB+8367+92E
	image/fw_8881A_88E.bin                - gateway image for 8881AQ+88E
	image/fw_8881A_92E.bin                - gateway image for 8881AQ+92E
	image/fw_8881A-selective.bin          - gateway image for 8881A Selectable
	image/fw_96E_92E.bin                  - gateway image for 8196E+92E
	image/fw_97D_8367_92C_8812.bin        - gateway image for 8197D+8812+8367+92C
	image/fw_97D_8367_92E_8812.bin        - gateway image for 8197D+8812+8367+92E
	image/fw_97D_8367_92E_HP_8812_HP.bin  - gateway image for 8197DN+8812_highpower+8367+92E_highpower
	image/fw_97DL_92E_8812AR-VN.bin	      - gateway image for 8197DL+8812AR_VN+92E
	image/fw_97DL_92E_8812.bin            - gateway image for 8197DL+8812+92E
	
	image/8881A_8367_92E_nfjrom 	      - MP image for 8881AB+8367+92E
	image/8881A_88E_nfjrom                - MP image for 8881AQ+88E
	image/8881A_92E_nfjrom                - MP image for 8881AQ+92E
	image/8881A-selective_nfjrom          - MP image for 8881A Selectable
	image/96E_92E_nfjrom                  - MP image for 8196E+92E
	image/97D_8367_92C_8812_nfjrom        - MP image for 8197D+8367+8812+92C
	image/97D_8367_92E_8812_nfjrom        - MP image for 8197D+8367+8812+92E
	image/97D_8367_92E_HP_8812_HP_nfjrom  - MP image for8197D+8367+8812_highpower+92E_highpower
	image/97DL_92E_8812AR-VN_nfjrom       - MP image for 8197DL+8812AR_VN+92E
	image/97DL_92E_8812_nfjrom            - MP image for 8197DL+8812+92E
	
	image/boot_97dl.bin                   - bootloader for 97dl
	image/boot_8196e.bin                  - bootloader for 8196e
	image/boot_8196eu.bin                 - bootloader for 8196eu
	image/boot_8197d_8367r.bin            - bootloader for 8197d+8367r
	image/boot_8881ab_8367r.bin           - bootloader for 8881ab+8367r
	image/boot_8881aq.bin                 - bootloader for 8881aq

Environment
===========
  Fedora 9, Ubuntu 8.10/9.10 are recommended

Install the linux-2.6 sdk package
=================================
  1. Copy 'rtl819x.tar.gz' to a file directory on a Linux PC
  2. Type 'tar -zxvf rtl819x.tar.gz' to extract the package
 
Install the bootcode package
============================ 
  Type 'tar -zxvf rtl819x-bootcode-SDK-v3.4.6-97d_8367r.tar.gz' to extract the package 

build the linux kernel/rootfs/bootcode
=============================
  follow the INSTALL.txt file
