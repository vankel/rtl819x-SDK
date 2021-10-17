
DEBUG=1

ifdef DEBUG
CXXFLAGS ?= -g -DDEBUG
else
CXXFLAGS ?= -O3
endif

OBJS     = src/logger.o src/ndppd.o src/iface.o src/proxy.o src/address.o \
           src/rule.o src/session.o src/conf.o src/route.o

all: ndppd 

ndppd: ${OBJS}
	${CXX} -o ndppd ${LDFLAGS} ${LIBS} ${OBJS}

.cc.o:
	${CXX} -c $(CXXFLAGS) -o $@ $<

romfs:
	$(ROMFSINST) ndppd /bin/ndppd
	$(ROMFSINST) ndppd.conf /etc/ndppd.conf

clean:
	rm -f ndppd  ${OBJS}
