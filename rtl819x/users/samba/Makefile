#
# Realtek Semiconductor Corp.
#
# Tony Wu (tonywu@realtek.com)
# Jan. 10, 2011
#

DIR_USERS := ..
include $(DIR_USERS)/.config

ifeq ($(CONFIG_APP_SAMBA-3.0.24),y)
DIR_SOURCE := samba-3.0.24
else
ifeq ($(CONFIG_APP_SAMBA-3.2.15),y)
DIR_SOURCE := samba-3.2.15
endif
endif

all: samba

samba:
	make -C $(DIR_SOURCE)/source

romfs:
	$(ROMFSINST) $(DIR_SOURCE)/source/bin/smbd /bin/smbd
	$(ROMFSINST) $(DIR_SOURCE)/source/bin/smbpasswd /bin/smbpasswd
	$(ROMFSINST) $(DIR_SOURCE)/source/bin/nmbd /bin/nmbd
	$(ROMFSINST) $(DIR_SOURCE)/source/codepages/lowcase.dat /etc/samba/lowcase.dat
	$(ROMFSINST) $(DIR_SOURCE)/source/codepages/upcase.dat /etc/samba/upcase.dat
	$(ROMFSINST) $(DIR_SOURCE)/source/codepages/valid.dat /etc/samba/valid.dat
	$(ROMFSINST) smbpasswd /etc/samba/smbpasswd
	$(ROMFSINST) smb.conf /etc/samba/smb.conf
clean:
	make -C $(DIR_SOURCE)/source clean
