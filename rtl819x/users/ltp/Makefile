#
# Realtek Semidonductor Corp.
#

CFLAGS+= -Os

export CFLAGS

all:
	cd lib; make; cd ..
	cd testcases/kernel; make; cd ..

romfs:
	cd testcases/kernel; make romfs; cd ..

clean:
	cd lib; make clean; cd ..
	cd testcases/kernel; make clean; cd ..
