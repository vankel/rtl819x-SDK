# Makefile for Unix/Xenix rz and sz programs
# Some targets may not be up to date
#CC=cc
OFLAG= -O

all:image

clean:
	rm -f *.o image_upgrade

image:
	$(CC) $(OFLAG) image_upgrade.c -o image_upgrade

romfs:
	$(ROMFSINST)  image_upgrade /bin/image_upgrade 
	$(ROMFSINST)  upImage /bin/upImage
