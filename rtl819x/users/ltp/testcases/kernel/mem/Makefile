#SUBDIRS = hugetlb mem mmapstress mtest01 mtest05 mtest06 mtest07 page shmt vmtests
SUBDIRS = mem mmapstress mtest01 mtest05 mtest06 mtest07 page shmt vmtests

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

romfs:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i romfs ; done
	$(ROMFSINST) /bin/memtest.sh

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
