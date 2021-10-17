#CC = msdk-linux-gcc
#CFLAGS = -fPIC -Wall -W
CFLAGS = -Os -pipe -mno-branch-likely -mips32 -mtune=mips32 -fno-caller-saves   -msoft-float   -Wall -W -D_GNU_SOURCE

AR = ar
RM = rm -f

SOURCES = cluster.c
OBJS = cluster.o

all: lib.a

lib.a: $(OBJS)
	$(AR) crs lib.a $(OBJS)

dep: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) >> Makefile

clean:
	$(RM) $(OBJS) lib.a

distclean:
	$(RM) Makefile

# automatically generated dependencies follow
