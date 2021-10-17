CFLAGS += -Wall -O -g
LDLIBS := -lpthread

SRCS=$(wildcard *.c)
TARGETS=$(patsubst %.c,%,$(SRCS))

all: $(TARGETS)

install:
	@set -e; for i in $(TARGETS) ; do ln -f $$i ../../../bin/$$i ; done ;

romfs:
	for x in $(TARGETS); do $(ROMFSINST) /bin/$$x; done

clean:
	rm -f $(TARGETS)
