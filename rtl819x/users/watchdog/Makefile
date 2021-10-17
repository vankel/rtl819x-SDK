CFLAGS += -Os
 
EXEC = watchdog
OBJS = watchdog.o
all: $(EXEC)
 
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS)  -o $@ $(OBJS) $(LDLIBS)  
 
romfs:
	$(ROMFSINST) /bin/$(EXEC)
 
clean:
	rm -f $(EXEC) *.o

