# Makefile for in place build of BIRD
# (c) 1999--2000 Martin Mares <mj@ucw.cz>

ifeq ($(DIR_LINUX),)
include ../../linux-2.6.30/.config
else
include $(DIR_LINUX)/.config
endif

objdir=obj

all depend tags install install-docs:
	$(MAKE) -C $(objdir) $@

docs userdocs progdocs:
	$(MAKE) -C doc $@

romfs:
	$(ROMFSINST) bird /bin/bird6
	$(ROMFSINST) birdcl /bin/birdcl6
	$(ROMFSINST) bird6.conf /etc/bird6.conf

clean:
	$(MAKE) -C $(objdir) clean
	find . -name "*~" -or -name "*.[oa]" -or -name "\#*\#" -or -name TAGS -or -name core -or -name depend -or -name ".#*" | xargs rm -f

distclean: clean
	$(MAKE) -C doc distclean
	rm -rf $(objdir) autom4te.cache
	rm -f config.* sysdep/autoconf.h sysdep/paths.h

