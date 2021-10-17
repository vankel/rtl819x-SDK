#SUBDIRS = cfs-scheduler clisrv hyperthreading nptl process_stress pthreads sched_stress tool 
SUBDIRS = process_stress pthreads sched_stress tool 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

romfs:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i romfs ; done
	
clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done


