#CC = msdk-linux-gcc
#CFLAGS = -fPIC -Wall -W
CFLAGS = -fPIC -Os -pipe -mno-branch-likely -mips32 -mtune=mips32 -fno-caller-saves   -msoft-float   -Wall -W -D_GNU_SOURCE

AR = ar
RM = rm -f
LEX = flex
YACC = bison -y

OBJS = lex.yy.o y.tab.o

all: lib.a

lib.a: $(OBJS)
	$(AR) crs lib.a $(OBJS)

y.tab.h y.tab.c: exports.y
	$(YACC) -d exports.y

y.tab.o: y.tab.c exports.h ../nfs.h ../mount.h ../daemon.h

lex.yy.c: exports.l
	$(LEX) exports.l

lex.yy.o: lex.yy.c y.tab.h ../nfs.h

dep:
	:

clean:
	$(RM) $(OBJS) lib.a
	$(RM) lex.yy.c y.tab.h y.tab.c

distclean:
	$(RM) Makefile
